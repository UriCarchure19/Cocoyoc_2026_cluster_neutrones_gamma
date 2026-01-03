from __future__ import annotations
from dataclasses import dataclass
import numpy as np
import pandas as pd
from .domain import Waveform, ProcessedWaveform
from .interfaces import IPreprocessor


@dataclass
class StilbenePreprocessor(IPreprocessor):
    """Numeric coercion already done in loader; this focuses on smoothing + derivative."""
    smooth_window: int = 31

    def process(self, w: Waveform) -> ProcessedWaveform:
        # Put into DataFrame for rolling mean (centered)
        df = pd.DataFrame({"t": w.t_ns, "v": w.v_mV})
        df["v_smooth"] = df["v"].rolling(window=self.smooth_window, center=True).mean()

        df_clean = df.dropna(subset=["v_smooth"]).copy()

        t = df_clean["t"].to_numpy(dtype=float)
        v = df_clean["v"].to_numpy(dtype=float)
        v_s = df_clean["v_smooth"].to_numpy(dtype=float)

        # derivative dv/dt on smoothed signal
        der = np.gradient(v_s, t)

        return ProcessedWaveform(
            raw=w,
            t_ns=t,
            v_mV=v,
            v_smooth_mV=v_s,
            derivative=der,
        )
