from __future__ import annotations
from dataclasses import dataclass
import pandas as pd
import numpy as np
from .domain import Waveform
from .interfaces import IWaveformLoader


@dataclass
class CSVWaveformLoader(IWaveformLoader):
    header: int = 1
    col_t: str = "(ns)"
    col_v: str = "(mV)"
    max_inf_allowed: int = 50 #parametro configurable de no. infinitos

    INF_STRINGS = {"-∞", "−∞", "-inf", "-Inf", "inf", "∞"}

    def load(self, event_id: int, path: str) -> Waveform:
        df = pd.read_csv(path, header=self.header)

        # ---- 1) contar infinitos en la columna de voltaje (raw strings)
        col_v_raw = df[self.col_v].astype(str)
        count_inf=col_v_raw.isin(self.INF_STRINGS).sum()

        if count_inf > self.max_inf_allowed:
            raise ValueError(
                f"too_many_infinite_values_in_voltage: {count_inf} > {self.max_inf_allowed}"
            )

        # ---- 2) conversión segura a numérico
        t = pd.to_numeric(df[self.col_t], errors="coerce").to_numpy()
        v = pd.to_numeric(df[self.col_v], errors="coerce").to_numpy()

        # drop NaN pairs (keep alignment)
        mask = np.isfinite(t) & np.isfinite(v)
        t = t[mask]
        v = v[mask]

        return Waveform(
            event_id=event_id,
            path=path,
            t_ns=t.astype(float),
            v_mV=v.astype(float),
            meta={"header": self.header, "col_t": self.col_t, "col_v": self.col_v, 
                  "count_inf": int(count_inf),
                  },
        )


def read_path_list(txt_path: str) -> list[str]:
    """Reads your list_Stilbene_Rutas.txt (one path per line)."""
    df = pd.read_csv(txt_path, header=None)
    return df.iloc[:, 0].astype(str).tolist()
