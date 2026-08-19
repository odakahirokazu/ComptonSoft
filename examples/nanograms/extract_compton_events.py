#!/usr/bin/env python

from __future__ import annotations

from pathlib import Path
import yaml

import anlnext
import comptonsoft as cs


SCRIPT_DIR = Path(__file__).resolve().parent
PARAMETER_FILE = SCRIPT_DIR / "metadata/parfile_NanoGRAMS.yaml"


def run_path(value: str) -> Path:
    path = Path(value)
    return path if path.is_absolute() else SCRIPT_DIR / path


def reconstruct_events(
    input_file: Path,
    output_file: Path,
    source_position: list[float],
    reconstruction_method: str,
) -> None:
    chain = anlnext.AnalysisChain()
    chain.console = False
    chain.chain(cs.CSHitCollection)
    chain.chain(cs.ConstructDetector).with_parameters(
        {
            "detector_configuration": str(SCRIPT_DIR / "database/detector_configuration.xml"),
            "verbose_level": 1,
        }
    )
    chain.chain(cs.ReadHitTree).with_parameters({"file_list": [str(input_file)]})
    chain.chain(cs.EventReconstruction).with_parameters(
        {
            "reconstruction_method": reconstruction_method,
            "source_distant": False,
            "parameter_file": str(PARAMETER_FILE),
        }
    ).with_setter(
        lambda module: module.set_parameter("source_position", *source_position)
    )
    chain.chain(cs.WriteComptonEventTree)
    chain.chain(cs.SaveData).with_parameters({"output": str(output_file)})
    chain.run(-1)


if __name__ == "__main__":
    with PARAMETER_FILE.open() as parameter_stream:
        config = yaml.safe_load(parameter_stream)

    extraction = config["extract_compton_events"]
    input_dir  = run_path(extraction["input_dir"])
    output_dir = run_path(extraction["output_dir"])
    output_dir.mkdir(parents=True, exist_ok=True)
    reconstruction_method = extraction["reconstruction_method"]

    source_position = extraction["source_position"]
    input_filename  = "hittree_Na22Test.root"
    output_filename = "comptontree_Na22Test.root"

    for tag in extraction["data_group_list"]:
        reconstruct_events(
            input_file=input_dir / input_filename,
            output_file=output_dir / output_filename,
            source_position=source_position,
            reconstruction_method=reconstruction_method,
        )
