from __future__ import annotations
from dataclasses import dataclass
from typing import List, Tuple
import pandas as pd

from .interfaces import IWaveformLoader, IPreprocessor, IFeatureExtractor, IQualityGate
from .domain import WaveformFeatures, ValidationResult


@dataclass
class BasicQualityGate(IQualityGate):
    """Simple gate: keep only if feat.valid True and key fields finite."""
    min_amplitude_mV: float = 1.0

    def check(self, pw, feat: WaveformFeatures) -> ValidationResult:
        if not feat.valid:
            return ValidationResult(False, feat.invalid_reason or "invalid")

        if not (feat.amplitude_mV >= self.min_amplitude_mV):
            return ValidationResult(False, "amplitude_too_small")

        # require integrals
        if pd.isna(feat.q_total) or pd.isna(feat.q_tail):
            return ValidationResult(False, "missing_integrals")

        return ValidationResult(True, None)


@dataclass
class ProcessingPipeline:
    loader: IWaveformLoader
    preprocessor: IPreprocessor
    extractor: IFeatureExtractor
    gate: IQualityGate

    def run(self, paths: List[str]) -> Tuple[pd.DataFrame, pd.DataFrame]:
        """
        Returns:
          df_good: features for good events
          df_bad:  features for rejected events with reasons
        """
        good: list[dict] = []
        bad: list[dict] = []

        for event_id, path in enumerate(paths):
            try:
                w = self.loader.load(event_id=event_id, path=path)
                pw = self.preprocessor.process(w)
                feat = self.extractor.extract(pw)
                verdict = self.gate.check(pw, feat)

                row = feat.to_dict()
                row["passed"] = verdict.passed
                row["reject_reason"] = verdict.reason

                if verdict.passed:
                    good.append(row)
                else:
                    bad.append(row)

            except Exception as e:
                bad.append({
                    "event_id": event_id,
                    "path": path,
                    "passed": False,
                    "reject_reason": f"exception:{type(e).__name__}:{e}",
                })

        df_good = pd.DataFrame(good)
        df_bad = pd.DataFrame(bad)
        return df_good, df_bad
