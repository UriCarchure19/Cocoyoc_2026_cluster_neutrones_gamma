from __future__ import annotations
import os
from typing import Optional, Dict, Any, List, Tuple

import pandas as pd
from concurrent.futures import ProcessPoolExecutor, as_completed

from stilbene_psd.io import read_path_list, CSVWaveformLoader
from stilbene_psd.preprocess import StilbenePreprocessor
from stilbene_psd.features import (
    DerivativeOnsetDetector,
    LinearInterpolationCrossingEstimator,
    TrapezoidalIntegrator,
    StilbeneFeatureExtractor,
)
from stilbene_psd.pipeline import BasicQualityGate


def process_one(event_id: int, path: str) -> Dict[str, Any]:
    """
    Procesa 1 archivo y regresa un dict con features + passed/reject_reason.
    IMPORTANTE: creamos los objetos dentro del proceso para evitar problemas de pickle.
    """
    try:
        loader = CSVWaveformLoader(header=1, col_t="(ns)", col_v="(mV)")
        pre = StilbenePreprocessor(smooth_window=31)

        onset = DerivativeOnsetDetector(min_puntos_negativos=50, factor_umbral=0.001)
        crossing = LinearInterpolationCrossingEstimator()
        integrator = TrapezoidalIntegrator()

        extractor = StilbeneFeatureExtractor(onset_detector=onset, crossing=crossing, integrator=integrator)
        gate = BasicQualityGate(min_amplitude_mV=1.0)

        w = loader.load(event_id=event_id, path=path)
        pw = pre.process(w)
        feat = extractor.extract(pw)
        verdict = gate.check(pw, feat)

        row = feat.to_dict()
        row["passed"] = verdict.passed
        row["reject_reason"] = verdict.reason
        return row

    except Exception as e:
        return {
            "event_id": event_id,
            "path": path,
            "passed": False,
            "reject_reason": f"exception:{type(e).__name__}:{e}",
        }


def run_parallel(paths: List[str], max_workers: Optional[int] = None) -> Tuple[pd.DataFrame, pd.DataFrame]:
    results: List[Dict[str, Any]] = []

    with ProcessPoolExecutor(max_workers=max_workers) as ex:
        futures = [ex.submit(process_one, i, p) for i, p in enumerate(paths)]
        for fut in as_completed(futures):
            results.append(fut.result())

    df = pd.DataFrame(results)
    df_good = df[df["passed"] == True].copy()
    df_bad = df[df["passed"] == False].copy()
    return df_good, df_bad


def main():
    ruta = "/home/uri/Desktop/Dubna/Analisis_espectro/Picoscope_Stilbene/picoscope_stilbene_pu_c13/list_Stilbene_Rutas.txt"
    paths = read_path_list(ruta)

    # Usa todos los núcleos menos 1 (para no congelar tu PC)
    workers = max(1, (os.cpu_count() or 2) - 1)

    df_good, df_bad = run_parallel(paths, max_workers=workers)

    print("Good:", len(df_good), "Bad:", len(df_bad))
    if "reject_reason" in df_bad.columns:
        print(df_bad["reject_reason"].value_counts().head(15))

    df_good.to_csv("features_good.csv", index=False)
    df_bad.to_csv("features_bad.csv", index=False)
    print("Saved features_good.csv and features_bad.csv")


if __name__ == "__main__":
    main()
