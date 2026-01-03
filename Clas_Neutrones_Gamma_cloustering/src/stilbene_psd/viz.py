from __future__ import annotations
from dataclasses import dataclass
from typing import List, Optional, Dict
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


@dataclass
class ClusteringPlotter:
    def plot_elbow(self, k_range: range, inertias: List[float]) -> None:
        plt.figure(figsize=(7, 5))
        plt.plot(list(k_range), inertias, marker="o")
        plt.xlabel("Número de clusters (k)")
        plt.ylabel("Inercia")
        plt.title("Método del codo para elegir k")
        plt.grid(True)
        plt.tight_layout()
        plt.show()

    def plot_scatter(
        self,
        df: pd.DataFrame,
        xcol: str,
        ycol: str,
        label_col: str = "cluster",
        title: str | None = None,
        xlabel: str | None = None,
        ylabel: str | None = None,
        figsize: tuple = (8, 6),
        point_size: int = 35,
        alpha: float = 0.8,
        save: str | None =None,
    ) -> None:

        plt.figure(figsize=figsize)

        sc = plt.scatter(
            df[xcol],
            df[ycol],
            c=df[label_col],
            s=point_size,
            alpha=alpha
        )

        # Si el usuario no pasa etiquetas, usamos defaults
        plt.xlabel(xlabel if xlabel is not None else xcol)
        plt.ylabel(ylabel if ylabel is not None else ycol)

        plt.title(title if title is not None else f"Clusters: {xcol} vs {ycol}")

        handles, labels = sc.legend_elements(prop="colors", alpha=alpha)
        plt.legend(handles, labels, title="Cluster")

        plt.grid(True)
        plt.tight_layout()
        if save is not None:
            plt.savefig(save)
        else:
            print("Plot not saved (no path provided).")

        # MOSTRAR DESPUÉS
        plt.show()

        # CERRAR FIGURA (muy importante en loops)
        plt.close()



@dataclass
class WaveformPlotter:
    """Optional: plot waveforms for debugging (single event)."""
    def plot_event(self, t: np.ndarray, v: np.ndarray, t_s: np.ndarray, v_s: np.ndarray,
                   baseline: float, v10: float, v90: float,
                   t0: Optional[float] = None, t10: Optional[float] = None, t90: Optional[float] = None,
                   t90_f: Optional[float] = None, t10_f: Optional[float] = None,
                   title: str = "Waveform") -> None:
        plt.figure(figsize=(10, 6))
        plt.plot(t, v, alpha=0.35, label="original")
        plt.plot(t_s, v_s, linewidth=2, label="smooth")

        plt.axhline(baseline, linestyle="--", linewidth=2, label=f"baseline={baseline:.2f}")
        plt.axhline(v10, linestyle="-", alpha=0.7, label=f"V10={v10:.2f}")
        plt.axhline(v90, linestyle="-", alpha=0.7, label=f"V90={v90:.2f}")

        if t0 is not None:
            plt.axvline(t0, linestyle="--", alpha=0.7, label=f"t0={t0:.2f}")
        if t10 is not None:
            plt.axvline(t10, linestyle=":", alpha=0.8, label=f"t10={t10:.2f}")
        if t90 is not None:
            plt.axvline(t90, linestyle=":", alpha=0.8, label=f"t90={t90:.2f}")
        if t90_f is not None:
            plt.axvline(t90_f, linestyle=":", alpha=0.8, label=f"t90_f={t90_f:.2f}")
        if t10_f is not None:
            plt.axvline(t10_f, linestyle=":", alpha=0.8, label=f"t10_f={t10_f:.2f}")

        plt.xlabel("Tiempo (ns)")
        plt.ylabel("Voltaje (mV)")
        plt.title(title)
        plt.legend()
        plt.grid(True)
        plt.tight_layout()
        plt.show()
