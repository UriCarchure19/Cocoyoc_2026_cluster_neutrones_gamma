from __future__ import annotations
from dataclasses import dataclass, field
from typing import Optional, Dict, Any
import numpy as np


@dataclass(frozen=True)
class Waveform:
    """Raw waveform for one event."""
    event_id: int
    path: str
    t_ns: np.ndarray
    v_mV: np.ndarray
    meta: Dict[str, Any] = field(default_factory=dict)


@dataclass
class ProcessedWaveform:
    """Waveform after preprocessing (smoothing, derivative, cleaned NaNs)."""
    raw: Waveform
    t_ns: np.ndarray
    v_mV: np.ndarray
    v_smooth_mV: np.ndarray
    derivative: np.ndarray


@dataclass
class ValidationResult:
    passed: bool
    reason: Optional[str] = None


@dataclass
class WaveformFeatures:
    event_id: int
    path: str

    baseline_mV: float = np.nan
    amplitude_mV: float = np.nan

    # times
    t_min_ns: float = np.nan
    t0_ns: float = np.nan  # onset (descenso real)
    t10_ns: float = np.nan
    t90_ns: float = np.nan
    rise_time_ns: float = np.nan

    t90_f_ns: float = np.nan
    t10_f_ns: float = np.nan
    fall_time_ns: float = np.nan

    # charge / PSD
    q_total: float = np.nan
    q_tail: float = np.nan
    psd: float = np.nan

    valid: bool = False
    invalid_reason: Optional[str] = None

    def to_dict(self) -> dict:
        d = self.__dict__.copy()
        return d
