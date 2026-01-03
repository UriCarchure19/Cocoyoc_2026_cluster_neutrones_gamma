import pandas as pd

from stilbene_psd.ml import KMeansClusterer, ModelSelector
from stilbene_psd.viz import ClusteringPlotter


def main():
    df = pd.read_csv("features_good.csv")

    # Core features for PSD clustering:
    # (you can also try using PSD + q_total instead of q_total + q_tail)
    feature_cols = ["q_total", "q_tail"]
    X = df[feature_cols].dropna().copy()

    # scale once for elbow/silhouette
    from sklearn.preprocessing import StandardScaler
    scaler = StandardScaler()
    X_scaled = scaler.fit_transform(X.values)

    selector = ModelSelector(random_state=42, n_init=10)
    k_range = range(1, 8)
    inertias = selector.elbow_inertias(X_scaled, k_range)
    sils = selector.silhouettes(X_scaled, ks=[2, 3, 4, 5])

    print("Silhouette:", sils)

    plotter = ClusteringPlotter()
    plotter.plot_elbow(k_range, inertias)

    # Choose k (e.g. 2)
    clusterer = KMeansClusterer(random_state=42, n_init=10)
    res = clusterer.fit_predict(X, n_clusters=2)

    df.loc[X.index, "cluster"] = res.labels

    # Stats
    stats = df.groupby("cluster")[feature_cols].agg(["mean", "std", "count"])
    print(stats)

    # Scatter
    plotter.plot_scatter(df.loc[X.index], "q_total", "q_tail", label_col="cluster")

    df.to_csv("features_with_clusters.csv", index=False)
    print("Saved features_with_clusters.csv")


if __name__ == "__main__":
    main()
