from __future__ import annotations
from dataclasses import dataclass
from typing import Iterable, Dict, List, Tuple
import numpy as np
import pandas as pd
from sklearn.preprocessing import StandardScaler
from sklearn.cluster import KMeans
from sklearn.metrics import silhouette_score


@dataclass
class KMeansClusteringResult:
    labels: np.ndarray
    scaler: StandardScaler
    model: KMeans
    X_scaled: np.ndarray


@dataclass
class KMeansClusterer:
    random_state: int = 42
    n_init: int = 10

    def fit_predict(self, X: pd.DataFrame, n_clusters: int) -> KMeansClusteringResult:
        scaler = StandardScaler()
        X_scaled = scaler.fit_transform(X.values)

        model = KMeans(n_clusters=n_clusters, random_state=self.random_state, n_init=self.n_init)
        labels = model.fit_predict(X_scaled)

        return KMeansClusteringResult(labels=labels, scaler=scaler, model=model, X_scaled=X_scaled)


@dataclass
class ModelSelector:
    random_state: int = 42
    n_init: int = 10

    def elbow_inertias(self, X_scaled: np.ndarray, k_range: range) -> List[float]:
        inertias = []
        for k in k_range:
            model = KMeans(n_clusters=k, random_state=self.random_state, n_init=self.n_init)
            model.fit(X_scaled)
            inertias.append(float(model.inertia_))
        return inertias

    def silhouettes(self, X_scaled: np.ndarray, ks: Iterable[int]) -> Dict[int, float]:
        out = {}
        for k in ks:
            if k <= 1 or k >= len(X_scaled):
                continue
            model = KMeans(n_clusters=k, random_state=self.random_state, n_init=self.n_init)
            labels = model.fit_predict(X_scaled)
            out[k] = float(silhouette_score(X_scaled, labels))
        return out
