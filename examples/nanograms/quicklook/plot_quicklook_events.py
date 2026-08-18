#!/usr/bin/env python3
"""Plot NanoGRAMS quicklook TTree entries.

The input file is expected to contain the optional NanoGRAMSReadTPCEvents
``tpcquicklook`` tree with branches such as ``energy_cmn_sub[4][64]``,
``drift_time[4]``, and ``waveform[8][N]``.
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path

import numpy as np


PLOT_NUM_ALL = np.asarray(
    [
        [
            0, 8, 23, 24, 39, 40, 55, 63,
            1, 9, 22, 25, 38, 41, 54, 62,
            2, 10, 21, 26, 37, 42, 53, 61,
            3, 11, 20, 27, 36, 43, 52, 60,
            4, 12, 19, 28, 35, 44, 51, 59,
            5, 13, 18, 29, 34, 45, 50, 58,
            6, 14, 17, 30, 33, 46, 49, 57,
            7, 15, 16, 31, 32, 47, 48, 56,
        ],
        [
            63, 62, 61, 60, 59, 58, 57, 56,
            55, 54, 53, 52, 51, 50, 49, 48,
            40, 41, 42, 43, 44, 45, 46, 47,
            39, 38, 37, 36, 35, 34, 33, 32,
            24, 25, 26, 27, 28, 29, 30, 31,
            23, 22, 21, 20, 19, 18, 17, 16,
            8, 9, 10, 11, 12, 13, 14, 15,
            0, 1, 2, 3, 4, 5, 6, 7,
        ],
        [
            56, 48, 47, 32, 31, 16, 15, 7,
            57, 49, 46, 33, 30, 17, 14, 6,
            58, 50, 45, 34, 29, 18, 13, 5,
            59, 51, 44, 35, 28, 19, 12, 4,
            60, 52, 43, 36, 27, 20, 11, 3,
            61, 53, 42, 37, 26, 21, 10, 2,
            62, 54, 41, 38, 25, 22, 9, 1,
            63, 55, 40, 39, 24, 23, 8, 0,
        ],
        [
            7, 6, 5, 4, 3, 2, 1, 0,
            15, 14, 13, 12, 11, 10, 9, 8,
            16, 17, 18, 19, 20, 21, 22, 23,
            31, 30, 29, 28, 27, 26, 25, 24,
            32, 33, 34, 35, 36, 37, 38, 39,
            47, 46, 45, 44, 43, 42, 41, 40,
            48, 49, 50, 51, 52, 53, 54, 55,
            56, 57, 58, 59, 60, 61, 62, 63,
        ],
    ],
    dtype=int,
).reshape(4, 8, 8)

EVENT_TYPE_LABELS = {
    -1: "error",
    0: "other",
    1: "gamma",
    2: "cosmic",
    3: "pileup",
    4: "time_up",
}


@dataclass(frozen=True)
class PlotConfig:
    quicklook_file: str
    tree: str
    outdir: str
    entries: list[int] | None
    max_events: int | None
    event_types: list[int] | None
    light_channels: list[int]
    adc_to_mv: float
    waveform_dt_mode: str
    waveform_time_range_us: tuple[float, float]
    baseline_samples: int
    charge_vmin: float | None
    charge_vmax: float | None
    charge_scale: str
    charge_percentile_vmax: float
    drift_count_to_us: float
    drift_offset_us: float
    max_drift_time_us: float
    draw_channel_numbers: bool
    draw_pixel_grid: bool
    figure_dpi: int
    cmap: str


    #def parse_args() -> argparse.Namespace:
    #    parser = argparse.ArgumentParser(description="Plot NanoGRAMS tpcquicklook entries.")
    #    parser.add_argument("config", help="YAML config file.")
    #    return parser.parse_args()


def nested_get(config: dict, path: str, default=None):
    node = config
    for key in path.split("."):
        if not isinstance(node, dict) or key not in node:
            return default
        node = node[key]
    return node


def resolve_config_path(config_path: Path, value: str | Path) -> str:
    path = Path(value).expanduser()
    if path.is_absolute():
        return str(path)
    return str(config_path.parent / path)


def optional_int_list(value, name: str) -> list[int] | None:
    if value is None:
        return None
    if isinstance(value, str) and value.lower() in ("all", "none", ""):
        return None
    if isinstance(value, int):
        return [value]
    if isinstance(value, list):
        return [int(v) for v in value]
    raise TypeError(f"{name} must be an integer, list of integers, or null.")


def float_pair(value, name: str) -> tuple[float, float]:
    if not isinstance(value, list) or len(value) != 2:
        raise TypeError(f"{name} must be a two-element list.")
    return float(value[0]), float(value[1])


def load_config(path: str | Path) -> PlotConfig:
    import yaml

    config_path = Path(path).expanduser()
    with config_path.open() as f:
        config = yaml.safe_load(f) or {}

    quicklook_file = nested_get(config, "input.quicklook_file")
    if quicklook_file is None:
        raise KeyError("Missing required config value: input.quicklook_file")

    entries = optional_int_list(nested_get(config, "selection.entries"), "selection.entries")
    event_types = optional_int_list(nested_get(config, "selection.event_types"), "selection.event_types")
    light_channels = optional_int_list(nested_get(config, "light.channels", [4, 6, 5, 7]), "light.channels")
    if light_channels is None:
        light_channels = [4, 6, 5, 7]

    charge_vmin = nested_get(config, "charge.vmin")
    charge_vmax = nested_get(config, "charge.vmax")
    max_drift_time_us = nested_get(config, "drift.max_drift_time_us",
                                   nested_get(config, "drift.max_time_us", 150.0))

    return PlotConfig(
        quicklook_file=resolve_config_path(config_path, quicklook_file),
        tree=str(nested_get(config, "input.tree", "tpcquicklook")),
        outdir=resolve_config_path(config_path, nested_get(config, "output.outdir", "quicklook_plots")),
        entries=entries,
        max_events=nested_get(config, "selection.max_events"),
        event_types=event_types,
        light_channels=light_channels,
        adc_to_mv=float(nested_get(config, "light.adc_to_mv", 1000.0 / 8192.0)),
        waveform_dt_mode=str(nested_get(config, "light.dt_mode", "pow2_ns")),
        waveform_time_range_us=float_pair(nested_get(config, "light.time_range_us", [-1.0, 4.0]),
                                          "light.time_range_us"),
        baseline_samples=int(nested_get(config, "light.baseline_samples", 100)),
        charge_vmin=float(charge_vmin) if charge_vmin is not None else None,
        charge_vmax=float(charge_vmax) if charge_vmax is not None else None,
        charge_scale=str(nested_get(config, "charge.scale", "max")),
        charge_percentile_vmax=float(nested_get(config, "charge.percentile_vmax", 99.0)),
        drift_count_to_us=float(nested_get(config, "drift.count_to_us", 0.01)),
        drift_offset_us=float(nested_get(config, "drift.offset_us", 0.68)),
        max_drift_time_us=float(max_drift_time_us),
        draw_channel_numbers=bool(nested_get(config, "charge.draw_channel_numbers", False)),
        draw_pixel_grid=bool(nested_get(config, "charge.draw_pixel_grid", True)),
        figure_dpi=int(nested_get(config, "output.dpi", 160)),
        cmap=str(nested_get(config, "charge.cmap", "viridis")),
    )


def open_tree(root_file: str | Path, tree_name: str):
    import uproot

    root = uproot.open(root_file)
    if tree_name in root:
        return root[tree_name]
    candidates = [key for key in root.keys() if key.split(";")[0] == tree_name]
    if candidates:
        return root[candidates[0]]
    raise KeyError(f"TTree '{tree_name}' not found. Available keys: {', '.join(root.keys())}")


def select_entries(tree, cfg: PlotConfig) -> list[int]:
    n_entries = int(tree.num_entries)
    if cfg.entries is None:
        entries = list(range(n_entries))
    else:
        entries = [entry for entry in cfg.entries if 0 <= entry < n_entries]

    if cfg.event_types is not None:
        event_type = tree["event_type"].array(library="np")
        allowed = set(cfg.event_types)
        entries = [entry for entry in entries if int(event_type[entry]) in allowed]

    if cfg.max_events is not None:
        entries = entries[: int(cfg.max_events)]

    return entries


def fec_charge_panel(charge_values: np.ndarray, fec: int) -> np.ndarray:
    image = np.full((8, 8), np.nan, dtype=float)
    for row in range(8):
        for col in range(8):
            ch = PLOT_NUM_ALL[fec, row, col]
            image[row, col] = charge_values[fec, ch]
    return image


def fec_channel_number_panel(fec: int) -> np.ndarray:
    return PLOT_NUM_ALL[fec]


def charge_color_limits(charge_values: np.ndarray, cfg: PlotConfig) -> tuple[float, float]:
    finite_charge = charge_values[np.isfinite(charge_values)]
    vmin = cfg.charge_vmin if cfg.charge_vmin is not None else 0.0
    vmax = cfg.charge_vmax
    if vmax is None:
        scale = cfg.charge_scale.lower()
        if finite_charge.size == 0:
            vmax = vmin + 1.0
        elif scale == "max":
            vmax = float(np.max(finite_charge))
        elif scale == "percentile":
            vmax = float(np.percentile(finite_charge, cfg.charge_percentile_vmax))
        else:
            raise ValueError("charge.scale must be max or percentile.")

    if vmax <= vmin:
        vmax = vmin + 1.0
    return vmin, vmax


def drift_time_us(drift_count: float, cfg: PlotConfig) -> float:
    return drift_count * cfg.drift_count_to_us + cfg.drift_offset_us


def drift_time_label(drift_count: float, cfg: PlotConfig) -> str:
    drift_us = drift_time_us(drift_count, cfg)
    if not np.isfinite(drift_us) or drift_us >= cfg.max_drift_time_us:
        return "No detection"
    return f"{drift_us:.2f} us"


def panel_extent(row_index: int, col_index: int, gap: float) -> tuple[float, float, float, float]:
    half_size = 2.56
    half_gap = 0.5 * gap
    x0, x1 = (-half_size, -half_gap) if col_index == 0 else (half_gap, half_size)
    y0, y1 = (-half_size, -half_gap) if row_index == 1 else (half_gap, half_size)
    return x0, x1, y0, y1


def waveform_dt_us(wave_compress: int, cfg: PlotConfig) -> float:
    if cfg.waveform_dt_mode == "pow2_ns":
        return (2.0 ** int(wave_compress)) * 1.0e-3
    if cfg.waveform_dt_mode == "value_ns":
        return float(wave_compress) * 1.0e-3
    if cfg.waveform_dt_mode == "fixed_32ns":
        return 32.0e-3
    raise ValueError("light.dt_mode must be pow2_ns, value_ns, or fixed_32ns.")


def baseline_subtracted_waveform(waveform: np.ndarray, cfg: PlotConfig) -> np.ndarray:
    voltage = waveform.astype(float) * cfg.adc_to_mv
    n_base = min(cfg.baseline_samples, voltage.size)
    if n_base > 0:
        voltage = voltage - np.median(voltage[:n_base])
    return voltage


def strongest_light_channel(waveforms: np.ndarray, cfg: PlotConfig) -> int:
    best_ch = cfg.light_channels[0]
    best_peak = -np.inf
    for ch in cfg.light_channels:
        if ch < 0 or ch >= waveforms.shape[0]:
            continue
        voltage = baseline_subtracted_waveform(waveforms[ch], cfg)
        if not np.any(np.isfinite(voltage)):
            continue
        peak = np.nanmax(voltage)
        if peak > best_peak:
            best_peak = peak
            best_ch = ch
    return best_ch


def expand_waveforms_by_dpp_channel(waveforms: np.ndarray,
                                    registered: np.ndarray,
                                    waveform_dpp_ch: np.ndarray,
                                    waveform_len: int) -> np.ndarray:
    expanded = np.full((registered.size, waveform_len), np.nan, dtype=float)
    if waveform_dpp_ch.size == waveforms.shape[0]:
        for waveform_slot, dpp_ch in enumerate(waveform_dpp_ch):
            if 0 <= dpp_ch < registered.size:
                expanded[int(dpp_ch)] = waveforms[waveform_slot]
        return expanded

    if waveforms.shape[0] == registered.size:
        return waveforms.astype(float, copy=False)

    waveform_slot = 0
    for dpp_ch, is_registered in enumerate(registered):
        if is_registered:
            if waveform_slot >= waveforms.shape[0]:
                break
            expanded[dpp_ch] = waveforms[waveform_slot]
            waveform_slot += 1
    return expanded


def plot_entry(entry: int, arrays: dict[str, np.ndarray], cfg: PlotConfig, outdir: Path) -> None:
    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    from matplotlib.colors import Normalize

    raw_event_id = int(arrays["raw_event_id"][entry])
    event_type = int(arrays["event_type"][entry])
    cmn_method = int(arrays["cmn_method"][entry])
    waveform_len = int(arrays["waveform_len"][entry])
    waveform_num_channels = int(arrays.get("waveform_num_channels", [8])[entry])
    if "energy_cmn_sub" in arrays:
        charge_values = np.asarray(arrays["energy_cmn_sub"][entry], dtype=float).reshape(4, 64)
        charge_label = "Energy (keV)"
    else:
        charge_values = np.asarray(arrays["adu_cmn_sub"][entry], dtype=float).reshape(4, 64)
        charge_label = "ADU - CMN"
    drift_counts = np.asarray(arrays["drift_time"][entry], dtype=float).reshape(4)
    registered = np.asarray(arrays["registered"][entry], dtype=bool).reshape(8)
    waveform_dpp_ch = np.asarray(arrays.get("waveform_dpp_ch", []), dtype=np.int16)
    if waveform_dpp_ch.size > 0:
        waveform_dpp_ch = np.asarray(arrays["waveform_dpp_ch"][entry], dtype=np.int16)
    waveforms_flat = np.asarray(arrays["waveform"][entry]).reshape(waveform_num_channels,
                                                                   waveform_len)
    waveforms = expand_waveforms_by_dpp_channel(waveforms_flat,
                                                registered,
                                                waveform_dpp_ch,
                                                waveform_len)
    wave_compress = np.asarray(arrays["wave_compress"][entry]).reshape(8)

    vmin, vmax = charge_color_limits(charge_values, cfg)
    norm = Normalize(vmin=vmin, vmax=vmax)

    fig = plt.figure(figsize=(6.0, 7.2), constrained_layout=True)
    gs = fig.add_gridspec(2, 1, height_ratios=[1.35, 0.82])
    ax_charge = fig.add_subplot(gs[0])
    ax_wave = fig.add_subplot(gs[1])

    fig.suptitle(
        f"entry={entry}, raw_event_id={raw_event_id}, "
        f"type={EVENT_TYPE_LABELS.get(event_type, event_type)}, cmn_method={cmn_method}"
    )

    fec_layout = [
        (3, 0, 0),
        (2, 0, 1),
        (0, 1, 0),
        (1, 1, 1),
    ]
    im = None
    charge_gap = 0.03
    ax_charge.set_facecolor("white")
    for fec, row_index, col_index in fec_layout:
        extent = panel_extent(row_index, col_index, charge_gap)
        im = ax_charge.imshow(
            fec_charge_panel(charge_values, fec),
            origin="upper",
            extent=extent,
            cmap=cfg.cmap,
            norm=norm,
            interpolation="nearest",
        )

        if cfg.draw_pixel_grid:
            x0, x1, y0, y1 = extent
            for i in range(9):
                x = x0 + i * (x1 - x0) / 8.0
                y = y0 + i * (y1 - y0) / 8.0
                ax_charge.axhline(y, xmin=(x0 + 2.56) / 5.12, xmax=(x1 + 2.56) / 5.12,
                                  color="black", alpha=0.18, lw=0.5)
                ax_charge.axvline(x, ymin=(y0 + 2.56) / 5.12, ymax=(y1 + 2.56) / 5.12,
                                  color="black", alpha=0.18, lw=0.5)

        x0, x1, y0, y1 = extent
        ax_charge.text(
            x0 + 0.06 * (x1 - x0),
            y1 - 0.06 * (y1 - y0),
            drift_time_label(drift_counts[fec], cfg),
            color="white",
            fontsize=8,
            ha="left",
            va="top",
            bbox={"facecolor": "black", "alpha": 0.25, "edgecolor": "none", "pad": 1.5},
        )

        if cfg.draw_channel_numbers:
            ch_image = fec_channel_number_panel(fec)
            xs = np.linspace(x0 + (x1 - x0) / 16.0, x1 - (x1 - x0) / 16.0, 8)
            ys = np.linspace(y1 - (y1 - y0) / 16.0, y0 + (y1 - y0) / 16.0, 8)
            for pixel_row, y in enumerate(ys):
                for pixel_col, x in enumerate(xs):
                    ax_charge.text(x, y, str(ch_image[pixel_row, pixel_col]),
                                   ha="center", va="center", fontsize=6,
                                   color="white", alpha=0.7)

    ax_charge.set_aspect("equal", adjustable="box")
    ax_charge.set_xlim(-2.56, 2.56)
    ax_charge.set_ylim(-2.56, 2.56)
    ax_charge.set_xticks([-2.56, 0.0, 2.56])
    ax_charge.set_yticks([-2.56, 0.0, 2.56])
    ax_charge.set_xlabel("X (cm)")
    ax_charge.set_ylabel("Y (cm)")

    cbar = fig.colorbar(im, ax=ax_charge, pad=0.02, shrink=0.96)
    cbar.set_label(charge_label)

    peak_ch = strongest_light_channel(waveforms, cfg)
    peak_voltage = baseline_subtracted_waveform(waveforms[peak_ch], cfg)
    peak_index = int(np.nanargmax(peak_voltage)) if np.any(np.isfinite(peak_voltage)) else 0

    average_waveforms = []
    reference_dt_us = waveform_dt_us(int(wave_compress[peak_ch]), cfg)
    common_time_us = np.arange(cfg.waveform_time_range_us[0],
                               cfg.waveform_time_range_us[1] + 0.5 * reference_dt_us,
                               reference_dt_us)

    for ch in cfg.light_channels:
        if ch < 0 or ch >= waveforms.shape[0]:
            continue
        voltage = baseline_subtracted_waveform(waveforms[ch], cfg)
        dt_us = waveform_dt_us(int(wave_compress[ch]), cfg)
        time_us = (np.arange(voltage.size) - peak_index) * dt_us
        keep = ((time_us >= cfg.waveform_time_range_us[0]) &
                (time_us <= cfg.waveform_time_range_us[1]))
        if np.count_nonzero(keep) >= 2:
            average_waveforms.append(
                np.interp(common_time_us,
                          time_us[keep],
                          voltage[keep],
                          left=np.nan,
                          right=np.nan)
            )
        label = f"ch{ch} (dt={dt_us * 1.0e3:g} ns)"
        lw = 1.2 if ch == peak_ch else 0.8
        alpha = 1.0 if ch == peak_ch else 0.55
        ax_wave.plot(time_us[keep], voltage[keep], lw=lw, alpha=alpha, label=label)

    if average_waveforms:
        stacked_waveforms = np.vstack(average_waveforms)
        valid_time = np.any(np.isfinite(stacked_waveforms), axis=0)
        average_voltage = np.full(common_time_us.shape, np.nan)
        average_voltage[valid_time] = np.nanmean(stacked_waveforms[:, valid_time], axis=0)
        ax_wave.plot(common_time_us[valid_time],
                     average_voltage[valid_time],
                     color="black",
                     lw=2.8,
                     alpha=0.95,
                     label="average")

    ax_wave.axvline(0.0, color="black", lw=0.8, alpha=0.5)
    ax_wave.set_xlim(*cfg.waveform_time_range_us)
    ax_wave.set_xlabel("Time from peak (us)")
    ax_wave.set_ylabel("Voltage (mV)")
    ax_wave.grid(True, alpha=0.35)
    ax_wave.legend(fontsize=8, ncol=2)

    event_label = EVENT_TYPE_LABELS.get(event_type, str(event_type))
    event_outdir = outdir / event_label
    event_outdir.mkdir(parents=True, exist_ok=True)
    fig.savefig(event_outdir / f"raw_{raw_event_id:06d}_{event_label}.png",
                dpi=cfg.figure_dpi)
    plt.close(fig)


def main() -> None:
    #args = parse_args()
    cfg    = load_config("config_plot.yaml")
    outdir = Path(cfg.outdir)
    outdir.mkdir(parents=True, exist_ok=True)
    for event_label in ("cosmic", "gamma", "pileup", "time_up", "other"):
        (outdir / event_label).mkdir(parents=True, exist_ok=True)

    tree = open_tree(cfg.quicklook_file, cfg.tree)
    entries = select_entries(tree, cfg)
    if not entries:
        print("No entries selected.")
        return

    tree_branches = set(tree.keys())
    charge_branch = ""
    if "energy_cmn_sub" in tree_branches:
        charge_branch = "energy_cmn_sub"
    elif "adu_cmn_sub" in tree_branches:
        charge_branch = "adu_cmn_sub"
    elif "adc_cmn_sub" in tree_branches:
        charge_branch = "adc_cmn_sub"
    else:
        raise KeyError("quicklook tree has neither energy_cmn_sub nor adu_cmn_sub.")
    branches = [
        "raw_event_id",
        "event_type",
        "cmn_method",
        "waveform_len",
        charge_branch,
        "drift_time",
        "registered",
        "waveform",
        "wave_compress",
    ]
    if "waveform_num_channels" in tree_branches:
        branches.append("waveform_num_channels")
    if "waveform_dpp_ch" in tree_branches:
        branches.append("waveform_dpp_ch")
    arrays = tree.arrays(branches, library="np")
    if "waveform_num_channels" not in arrays:
        arrays["waveform_num_channels"] = np.full(tree.num_entries, 8, dtype=np.int32)
    if charge_branch == "adc_cmn_sub":
        arrays["adu_cmn_sub"] = arrays[charge_branch]
    for index, entry in enumerate(entries, 1):
        plot_entry(entry, arrays, cfg, outdir)
        if index % 20 == 0 or index == len(entries):
            print(f"plotted {index}/{len(entries)} events")

    print(f"outputs = {outdir}")


if __name__ == "__main__":
    main()
