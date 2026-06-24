#!/usr/bin/env python3

import argparse
from pathlib import Path

import numpy as np
import uproot
import matplotlib.pyplot as plt


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("directory", help="Directory containing ROOT files")
    parser.add_argument("--tree", default="hittree")
    parser.add_argument("--branch", default="energy")
    parser.add_argument("--bins", type=int, default=512)
    parser.add_argument("--emin", type=float, default=0.0)
    parser.add_argument("--emax", type=float, default=512.0)
    parser.add_argument("--output", default="spectrum.pdf")
    parser.add_argument("--logy", action="store_true", default=True)
    args = parser.parse_args()

    root_files = sorted(Path(args.directory).glob("sim_*.root"))
    if not root_files:
        raise RuntimeError(f"No ROOT files found in {args.directory}")

    energies = []

    for path in root_files:
        try:
            with uproot.open(path) as f:
                tree = f[args.tree]
                e = tree[args.branch].array(library="np")
                energies.append(e)
                print(f"Read {len(e):8d} entries from {path.name}")
        except Exception as exc:
            print(f"Skip {path.name}: {exc}")

    if not energies:
        raise RuntimeError("No energy data were read.")

    energy = np.concatenate(energies)

    hist_range = None
    if args.emin is not None or args.emax is not None:
        emin = args.emin if args.emin is not None else np.nanmin(energy)
        emax = args.emax if args.emax is not None else np.nanmax(energy)
        hist_range = (emin, emax)

    counts, edges = np.histogram(
        energy,
        bins=args.bins,
        range=hist_range,
    )

    centers = 0.5 * (edges[:-1] + edges[1:])

    plt.figure(figsize=(7, 5))
    plt.step(centers, counts, where="mid", linewidth=1.0)
    plt.xlabel("Energy")
    plt.ylabel("Counts")
    plt.grid(True, alpha=0.3)

    if args.logy:
        plt.yscale("log")
        plt.ylim(bottom=0.5)

    plt.tight_layout()
    plt.savefig(args.output)
    print(f"Saved: {args.output}")


if __name__ == "__main__":
    main()
    
