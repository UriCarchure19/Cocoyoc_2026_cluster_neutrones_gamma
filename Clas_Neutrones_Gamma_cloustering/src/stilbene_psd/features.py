from __future__ import annotations
from dataclasses import dataclass
from typing import Optional
import numpy as np

from .domain import ProcessedWaveform, WaveformFeatures, ValidationResult
from .interfaces import IOnsetDetector, ICrossingTimeEstimator, IIntegrator, IFeatureExtractor


@dataclass
class DerivativeOnsetDetector(IOnsetDetector):
    min_puntos_negativos: int = 50
    factor_umbral: float = 0.001

    def detect(self, pw: ProcessedWaveform) -> Optional[float]:
        der = pw.derivative
        if der.size < self.min_puntos_negativos + 5:
            return None

        umbral = self.factor_umbral * np.max(np.abs(der))
        if not np.isfinite(umbral) or umbral == 0:
            return None

        # candidates: shift(1) > +umbral and current < -umbral
        # emulate shift(1) with roll; ignore first element
        prev = np.empty_like(der)
        prev[0] = der[0]
        prev[1:] = der[:-1]

        candidatos = np.where((prev > umbral) & (der < -umbral))[0]
        if candidatos.size == 0:
            return None

        for idx in candidatos:
            if idx + self.min_puntos_negativos < der.size:
                tramo = der[idx: idx + self.min_puntos_negativos]
                if np.all(tramo < -umbral):
                    return float(pw.t_ns[idx])
        return None


@dataclass
class LinearInterpolationCrossingEstimator(ICrossingTimeEstimator):
    def crossing_time(self, t: np.ndarray, v: np.ndarray, target: float) -> Optional[float]:
        if t.size < 2:
            return None
        s = v - target
        # find sign change
        changes = np.where(np.sign(s[:-1]) != np.sign(s[1:]))[0]
        if changes.size == 0:
            return None
        j = int(changes[0])
        # linear interpolation: v[j] -> v[j+1]
        return float(np.interp(target, [v[j], v[j + 1]], [t[j], t[j + 1]]))


@dataclass
class TrapezoidalIntegrator(IIntegrator):
    """Your integral (with sign convention like your original)."""
    def integrate(self, t: np.ndarray, v: np.ndarray) -> float:
        if t.size < 2 or v.size < 2 or t.size != v.size:
            return 0.0
        # replicate your sign convention: v_prom = -(v[i]+v[i-1])/2
        dt = np.diff(t)
        v_prom = -(v[1:] + v[:-1]) / 2.0
        return float(np.sum(v_prom * dt))




@dataclass
class StilbeneFeatureExtractor(IFeatureExtractor):
    onset_detector: IOnsetDetector
    crossing: ICrossingTimeEstimator
    integrator: IIntegrator

    def extract(self, pw: ProcessedWaveform) -> WaveformFeatures:
        w = pw.raw
        feat = WaveformFeatures(event_id=w.event_id, path=w.path)

        # We need min on RAW (unsmoothed) usually; but pw is already cleaned range.
        # We'll compute min based on raw waveform for baseline window selection.
        t_raw = w.t_ns
        v_raw = w.v_mV

        if t_raw.size < 10:
            feat.valid = False
            feat.invalid_reason = "too_few_points_raw"
            return feat

        idx_min = int(np.argmin(v_raw))
        feat.t_min_ns = float(t_raw[idx_min])

        # baseline: median before min (as you did)
        if idx_min < 5:
            feat.valid = False
            feat.invalid_reason = "min_too_early_for_baseline"
            return feat

        baseline = float(np.median(v_raw[:idx_min]))
        amplitude = float(abs(baseline - np.min(v_raw)))
        feat.baseline_mV = baseline
        feat.amplitude_mV = amplitude

        # onset detection on processed waveform (smoothed + derivative)
        t0 = self.onset_detector.detect(pw)
        if t0 is None:
            feat.valid = False
            feat.invalid_reason = "no_onset_found"
            return feat
        feat.t0_ns = float(t0)

        # build rise segment between t0 and tmin using pw arrays (smoothed)
        # find indices closest to times
        t_proc = pw.t_ns
        v_s = pw.v_smooth_mV

        idx0 = int(np.argmin(np.abs(t_proc - t0)))
        idxm = int(np.argmin(np.abs(t_proc - feat.t_min_ns)))
        if idx0 == idxm:
            feat.valid = False
            feat.invalid_reason = "degenerate_rise_segment"
            return feat

        lo, hi = (idx0, idxm) if idx0 < idxm else (idxm, idx0)
        t_seg = t_proc[lo:hi+1]
        v_seg = v_s[lo:hi+1]
        # ensure time ordering
        order = np.argsort(t_seg)
        t_seg = t_seg[order]
        v_seg = v_seg[order]

        # thresholds for negative pulse
        v10 = baseline - 0.10 * amplitude
        v90 = baseline - 0.90 * amplitude

        t10 = self.crossing.crossing_time(t_seg, v_seg, v10)
        t90 = self.crossing.crossing_time(t_seg, v_seg, v90)
        if (t10 is None) or (t90 is None) or (t90 <= t10):
            feat.valid = False
            feat.invalid_reason = "no_t10_t90_rise"
            return feat

        feat.t10_ns = float(t10)
        feat.t90_ns = float(t90)
        feat.rise_time_ns = float(t90 - t10)

        # fall segment: from min forward (on processed arrays)
        t_f = t_proc[idxm:]
        v_f = v_s[idxm:]
        if t_f.size < 5:
            feat.valid = False
            feat.invalid_reason = "too_few_points_fall_segment"
            return feat

        t90_f = self.crossing.crossing_time(t_f, v_f, v90)
        t10_f = self.crossing.crossing_time(t_f, v_f, v10)
        if (t90_f is None) or (t10_f is None) or (t10_f <= t90_f):
            feat.valid = False
            feat.invalid_reason = "no_t90_t10_fall"
            return feat

        feat.t90_f_ns = float(t90_f)
        feat.t10_f_ns = float(t10_f)
        feat.fall_time_ns = float(t10_f - t90_f)

        # integrals on raw (as you did)
        q_total = self.integrator.integrate(t_raw, v_raw)

        # Q_tail from minimum index to end (raw)
        t_tail = t_raw[idx_min:]
        v_tail = v_raw[idx_min:]
        q_tail = self.integrator.integrate(t_tail, v_tail)

        feat.q_total = float(q_total)
        feat.q_tail = float(q_tail)
        feat.psd = float(q_tail / q_total) if (q_total not in (0.0, -0.0) and np.isfinite(q_total)) else np.nan

        feat.valid = True
        feat.invalid_reason = None
        return feat
