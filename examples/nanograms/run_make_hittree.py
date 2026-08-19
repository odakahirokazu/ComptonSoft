#!/usr/bin/env python

from __future__ import annotations

import csv
import re
import subprocess
import sys
import yaml
from datetime import datetime, timezone
from pathlib import Path

import anlnext
import comptonsoft as cs


SCRIPT_DIR       = Path(__file__).resolve().parent
CONFIG_FILE      = SCRIPT_DIR / "metadata/config_pipeline.yaml"
RUN_ID_EPOCH     = datetime(2025, 1, 1, tzinfo=timezone.utc)
TPC_FILE_PATTERN = re.compile(r"tpc_data_?(\d+)\.root\Z")
TIME_ID_PATTERN  = re.compile(r"\A(\d{4})(\d{2})(\d{2})_(\d{2})(\d{2})_(\d{2})\Z")


def run_path(value: str) -> Path:
    path = Path(value)
    return path if path.is_absolute() else SCRIPT_DIR / path


def event_dir_name(time_id: str) -> str:
    return time_id.strip().replace("/", "_")


def run_id_from_time_id(time_id: str) -> int:
    match = TIME_ID_PATTERN.fullmatch(event_dir_name(time_id))
    if not match:
        raise RuntimeError(f"Invalid time_id for run_id: {time_id}")
    event_time = datetime(*(int(value) for value in match.groups()), tzinfo=timezone.utc)
    return int((event_time - RUN_ID_EPOCH).total_seconds())


def run_data_reduction(
    config_file: Path,
    dpp_config_file: Path,
    tpc_tree_files: list[Path],
    gain_tp_file: Path,
    gain_cache_seconds: float,
    output_flush_entries: int,
    hittree_file: Path,
    quicklook_file: Path,
    quicklook_event_types: list[str],
    quicklook_num_hits: int,
    quicklook_save_waveforms: bool,
    run_id: int,
) -> None:
    chain = anlnext.AnalysisChain()
    chain.console = False
    chain.chain(cs.NanoGRAMSReadTPCEvents).with_parameters(
        {
            "config_file": str(config_file),
            "dpp_config_file": str(dpp_config_file),
            "tpctree_files": [str(path) for path in tpc_tree_files],
            "gain_tp_file": str(gain_tp_file),
            "gain_cache_seconds": gain_cache_seconds,
            "run_id": run_id,
        }
    )
    chain.chain(cs.NanoGRAMSWriteHitTree).with_parameters(
        {
            "hittree_file": str(hittree_file),
            "output_flush_entries": output_flush_entries,
        }
    )
    # Comment out this block when quicklook output is not needed.
    chain.chain(cs.NanoGRAMSQuickLookWriter).with_parameters(
        {
            "quicklook_file": str(quicklook_file),
            "event_types": quicklook_event_types,
            "num_hits": quicklook_num_hits,
            "save_waveforms": quicklook_save_waveforms,
            "output_flush_entries": output_flush_entries,
        }
    )
    chain.run(-1)



if __name__ == "__main__":

    with CONFIG_FILE.open() as config_stream:
        config            = yaml.safe_load(config_stream)
    clustering            = config["clustering"]
    data_root_dir         = run_path(clustering["data_root_dir"])
    outdir_parent         = run_path(clustering["outdir_parent"])
    gain_tp_file          = run_path(clustering["gain_tp_file"])
    gain_cache_seconds    = float(clustering["gain_cache_seconds"])
    output_flush_entries  = int(clustering["output_flush_entries"])
    quicklook_event_types = clustering["quicklook_event_types"]
    quicklook_num_hits    = int(clustering["quicklook_num_hits"])
    quicklook_save_waveforms = bool(clustering["quicklook_save_waveforms"])

    data_dir     = Path("data")
    time_id      = "20260625_2000_20"

    input_files  = sorted((data_dir / time_id).glob("tpc_data*.root"))
    config_path  = data_dir / time_id / "config_dpp.yaml"
    outdir       = outdir_parent / time_id

    run_data_reduction(
        config_file=CONFIG_FILE,
        dpp_config_file=config_path,
        tpc_tree_files=input_files,
        gain_tp_file=gain_tp_file,
        gain_cache_seconds=gain_cache_seconds,
        output_flush_entries=output_flush_entries,
        hittree_file=outdir / "hittree.root",
        quicklook_file=outdir / "quicklook.root",
        quicklook_event_types=quicklook_event_types,
        quicklook_num_hits=quicklook_num_hits,
        quicklook_save_waveforms=quicklook_save_waveforms,
        run_id=run_id_from_time_id(time_id),
    )
