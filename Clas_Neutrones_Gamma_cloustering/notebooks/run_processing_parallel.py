from __future__ import annotations

import os

from stilbene_psd.io import read_path_list
from stilbene_psd.parallel import run_parallel


def main():
    ruta = "/home/uri/Desktop/Dubna/Analisis_espectro/Picoscope_Stilbene/picoscope_stilbene_pu_c13/list_Stilbene_Rutas.txt"
    paths = read_path_list(ruta)

    workers = max(1, (os.cpu_count() or 2) - 1)

    cfg = {
        "csv_header": 1,
        "csv_col_t": "(ns)",
        "csv_col_v": "(mV)",
        "smooth_window": 31,
        "min_puntos_negativos": 50,
        "factor_umbral": 0.001,
        "min_amplitude_mV": 1.0,
    }

    df_good, df_bad = run_parallel(paths, max_workers=workers, cfg=cfg)

    print("Good:", len(df_good), "Bad:", len(df_bad))
    if len(df_bad) > 0 and "reject_reason" in df_bad.columns:
        print(df_bad["reject_reason"].value_counts().head(15))

    df_good.to_csv("features_good.csv", index=False)
    df_bad.to_csv("features_bad.csv", index=False)
    print("Saved features_good.csv and features_bad.csv")


if __name__ == "__main__":
    main()
