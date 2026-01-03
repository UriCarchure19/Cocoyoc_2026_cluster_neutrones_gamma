import matplotlib.pyplot as plt
plt.rcParams['axes.unicode_minus'] = False
plt.rcParams['font.family'] = 'DejaVu Sans'

from stilbene_psd.io import read_path_list, CSVWaveformLoader
from stilbene_psd.preprocess import StilbenePreprocessor
from stilbene_psd.features import (
    DerivativeOnsetDetector,
    LinearInterpolationCrossingEstimator,
    TrapezoidalIntegrator,
    StilbeneFeatureExtractor,
)
from stilbene_psd.pipeline import ProcessingPipeline, BasicQualityGate


def main():
    ruta = "/home/uri/Desktop/Dubna/Analisis_espectro/Picoscope_Stilbene/picoscope_stilbene_pu_c13/list_Stilbene_Rutas.txt"
    paths = read_path_list(ruta)

    loader = CSVWaveformLoader(header=1, col_t="(ns)", col_v="(mV)")
    pre = StilbenePreprocessor(smooth_window=31)

    onset = DerivativeOnsetDetector(min_puntos_negativos=50, factor_umbral=0.001)
    crossing = LinearInterpolationCrossingEstimator()
    integrator = TrapezoidalIntegrator()

    extractor = StilbeneFeatureExtractor(onset_detector=onset, crossing=crossing, integrator=integrator)
    gate = BasicQualityGate(min_amplitude_mV=1.0)

    pipe = ProcessingPipeline(loader=loader, preprocessor=pre, extractor=extractor, gate=gate)

    df_good, df_bad = pipe.run(paths)

    print("Good:", len(df_good), "Bad:", len(df_bad))
    print(df_bad["reject_reason"].value_counts().head(15))

    # Save for later
    df_good.to_csv("features_good.csv", index=False)
    df_bad.to_csv("features_bad.csv", index=False)

    print("Saved features_good.csv and features_bad.csv")


if __name__ == "__main__":
    main()
