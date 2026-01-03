from __future__ import annotations
from typing import Protocol, Optional, Iterable, Tuple
import numpy as np
from .domain import Waveform, ProcessedWaveform, WaveformFeatures, ValidationResult


class IWaveformLoader(Protocol):
    def load(self, event_id: int, path: str) -> Waveform: ...


class IPreprocessor(Protocol):
    def process(self, w: Waveform) -> ProcessedWaveform: ...


class IOnsetDetector(Protocol):
    def detect(self, pw: ProcessedWaveform) -> Optional[float]:
        """Return onset time t0 (ns) or None."""
        ...


class ICrossingTimeEstimator(Protocol):
    def crossing_time(self, t: np.ndarray, v: np.ndarray, target: float) -> Optional[float]: ...


class IIntegrator(Protocol):
    def integrate(self, t: np.ndarray, v: np.ndarray) -> float: ...


class IFeatureExtractor(Protocol):
    def extract(self, pw: ProcessedWaveform) -> WaveformFeatures: ...


class IQualityGate(Protocol):
    def check(self, pw: ProcessedWaveform, feat: WaveformFeatures) -> ValidationResult: ...
