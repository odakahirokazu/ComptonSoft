#!/usr/bin/env python

from __future__ import annotations

import csv
import subprocess
from pathlib import Path

import yaml


def read_time_ids(csv_path: Path, time_column: str) -> list[str]:
    with csv_path.open(newline="") as f:
        return [row[time_column].strip() for row in csv.DictReader(f) if row[time_column].strip()]


def event_dir_name(time_id: str) -> str:
    return time_id.strip().replace("/", "_")


def natural_sort_key(path: Path) -> list[int | str]:
    parts: list[int | str] = []
    text = path.name
    number = ""
    for char in text:
        if char.isdigit():
            number += char
        else:
            if number:
                parts.append(int(number))
                number = ""
            parts.append(char)
    if number:
        parts.append(int(number))
    return parts


def input_path(products_root: Path, time_id: str, hittree_name: str) -> Path:
    return products_root / event_dir_name(time_id) / hittree_name


def run_path(run_dir: Path, value: str) -> Path:
    path = Path(value)
    return path if path.is_absolute() else run_dir / path


def merge_hittrees(
    *,
    time_ids: list[str],
    products_root: Path,
    hittree_name: str,
    output_path: Path,
) -> int:
    input_files = sorted(
        [input_path(products_root, time_id, hittree_name) for time_id in time_ids],
        key=natural_sort_key,
    )
    missing = [path for path in input_files if not path.exists()]
    if missing:
        raise FileNotFoundError("\n".join(str(path) for path in missing))

    output_path.parent.mkdir(parents=True, exist_ok=True)
    xargs_input = "".join(f"{path}\0" for path in input_files)
    command     = ["xargs", "-0", "hadd", "-f", str(output_path)]
    result      = subprocess.run(command, input=xargs_input, text=True, capture_output=True)
    if result.returncode != 0:
        raise RuntimeError(
            f"Failed to merge hittrees into {output_path}\n"
            f"{result.stdout}{result.stderr}"
        )
    return len(input_files)


if __name__ == "__main__":
    run_dir = Path(__file__).resolve().parent
    config_path = run_dir / "metadata/config_merge_file.yaml"
    with config_path.open() as config_stream:
        config = yaml.safe_load(config_stream)["merge_hittree"]

    products_root = run_path(run_dir, config["products_root"])
    data_group_dir = run_path(run_dir, config["data_group_dir"])
    output_dir = run_path(run_dir, config["output_dir"])
    output_name_pattern = config["output_name_pattern"]
    hittree_name = config["hittree_name"]
    time_column = config["time_column"]

    for data_group in config["data_group_list"]:
        csv_path = data_group_dir / f"data_group_{data_group}.csv"
        output_path = output_dir / output_name_pattern.format(data_group=data_group)
        time_ids = read_time_ids(csv_path, time_column)
        n_files = merge_hittrees(
            time_ids=time_ids,
            products_root=products_root,
            hittree_name=hittree_name,
            output_path=output_path,
        )
        print(f"{n_files} files -> {output_path} (hadd)")
