# NanoGRAMS config_pipeline.yaml reference

This file summarizes the parameters read by `NanoGRAMSHitExtraction` and
`NanoGRAMSCalibration`.

## Example

```yaml
general:
  efield_v_cm: 500
  temperature_k: 87.0

light:
  event_selection_mode: veto_only
  waveform_analysis: average
  general_analysis_channels: [4, 6, 5, 7]
  pileup_analysis_channels: [4]
  light_gamma_thr_mV: 20.0
  light_cosmic_thr_mV: 250.0
  delay_counts: 60
  pre_roi_window_us: 1.0
  post_roi_window_us: 5.0
  out_roi_peak_thr_mV: 15.0

charge:
  noise_th_kev: 18.0
  clustering_pix_range: [1, 3]
  spread_thr_kev: 7.0
  drift_time_max_us: 150.0
  noise_th_for_noisy_pixel_kev: 20.0
  core_exclude_pix:
    0: [0, 63]
    1: [0, 63]
    2: [0, 63]
    3: [0, 63]

calibration:
  energy:
    gain_info_file: subproducts/gainFuncParam3d.hdf5
    q_to_kev_spline_file: subproducts/QvsEkeV_spline.root
    max_time_us: 67.0
    tp_channel: 17
    ccal: 8
  position:
    anode_pos_z_cm: 5.0
```

## general

| Key | Type/range | Meaning |
| --- | --- | --- |
| `efield_v_cm` | positive number | Electric field in V/cm. Used for drift velocity and charge-to-energy calibration. |
| `temperature_k` | positive number | LAr temperature in K. Used for drift velocity and temperature correction. |

## light

| Key | Type/range | Meaning |
| --- | --- | --- |
| `event_selection_mode` | `gamma_required`, `veto_only`, or `disabled` | Controls how light waveforms enter event selection. |
| `waveform_analysis` | `average` or `channel` | `average` uses the average waveform within each analysis-channel group; `channel` checks each channel independently. |
| `general_analysis_channels` | list of DPP channel IDs, normally 0-7 | Channels used for ROI light-peak analysis, including gamma and cosmic classification. |
| `pileup_analysis_channels` | list of DPP channel IDs, normally 0-7 | Channels used for pre-ROI and post-ROI pileup analysis. Channels may overlap with `general_analysis_channels`. |
| `light_gamma_thr_mV` | number, mV | ROI peak threshold for gamma selection when `event_selection_mode` is `gamma_required`. |
| `light_cosmic_thr_mV` | number, mV | ROI peak threshold above which the event is classified as cosmic. |
| `delay_counts` | non-negative integer | Trigger delay count. One count corresponds to 8 waveform time bins, so the physical delay depends on each channel's `wave_compress`. |
| `pre_roi_window_us` | non-negative number, us | Time window before the ROI. Peaks outside the ROI can be pileup. |
| `post_roi_window_us` | non-negative number, us | Time window after the ROI. Peaks outside the ROI can be pileup. |
| `out_roi_peak_thr_mV` | number, mV | Common threshold for pre-ROI and post-ROI pileup peaks. |

### `event_selection_mode`

| Word | Behavior |
| --- | --- |
| `gamma_required` | Light waveform must pass the gamma threshold, and charge clustering must also pass. |
| `veto_only` | Charge clustering defines gamma candidates; light is used only to veto cosmic and pileup events. This is closest to the old `run_nhit.py` workflow. |
| `disabled` | Light waveform is not used in event selection. |

The legacy boolean `use_for_event_selection` is still accepted. `true` maps to
`gamma_required`, and `false` maps to `disabled`. New configs should prefer
`event_selection_mode`.

When `waveform_analysis` is `average`, all enabled channels inside
`general_analysis_channels` must have the same `wave_compress`, and all enabled
channels inside `pileup_analysis_channels` must also have the same
`wave_compress`. The two groups may have different `wave_compress` values if
they do not share channels. If the two groups share any channel, the common
`wave_compress` value of the two groups must match.

## charge

| Key | Type/range | Meaning |
| --- | --- | --- |
| `noise_th_kev` | number, keV | Minimum calibrated energy required for the largest deposit pixel to become the core seed. FEC hits with core energy at or below this value are rejected. |
| `clustering_pix_range` | `[min, max]`, integers | Allowed number of pixels in one clustered FEC hit. `max >= 3` enables diagonal neighbors. |
| `spread_thr_kev` | number, keV | Neighbor-pixel threshold for absorbing pixels into the cluster around a valid core pixel. |
| `drift_time_max_us` | positive number, us | FECs with drift time at or above this value are treated as time-up and are not accepted as gamma hits. |
| `noise_th_for_noisy_pixel_kev` | number, keV | Veto threshold for known noisy pixels listed in `core_exclude_pix`. If one of those pixels exceeds this value, the FEC hit is rejected. |
| `core_exclude_pix` | map from FEC ID to pixel list or token | Known noisy pixels. They cannot become the core seed, and they veto the FEC hit when above `noise_th_for_noisy_pixel_kev`. |

### `core_exclude_pix`

The keys are FEC IDs `0`, `1`, `2`, and `3`. Each value can be one of:

| Form | Meaning |
| --- | --- |
| `[0, 63]` | Treat the listed channels as known noisy pixels. |
| `peripheral` | Treat all peripheral pixels in that FEC section as known noisy pixels. |
| `[peripheral, 12, 34]` | Treat all peripheral pixels plus explicit channels as known noisy pixels. |

These pixels cannot become core seeds. If any of them exceeds
`noise_th_for_noisy_pixel_kev`, the FEC hit is rejected.

## calibration.energy

| Key | Type/range | Meaning |
| --- | --- | --- |
| `gain_info_file` | path string | HDF5 file containing gain matrix parameters. Relative paths are resolved from the config file directory. |
| `q_to_kev_spline_file` | path string | ROOT file containing the charge-to-energy spline. Relative paths are resolved from the config file directory. |
| `max_time_us` | positive number, us | Maximum drift time used in z-position conversion during calibration. |
| `tp_channel` | integer channel ID | Test-pulse channel used for temperature/gain correction. |
| `ccal` | integer | Test-pulse calibration capacitance setting. |

Test-pulse gain values are supplied from Ruby parameters, not from this YAML:
use either `gain_tp_file` for time interpolation from CSV or `gain_tp_hash` for
fixed FEC gains. The same Ruby parameter should also be passed to
`NanoGRAMSHitExtraction` so the keV-based hit selection and final calibration
use the same gain correction.

## calibration.position

| Key | Type/range | Meaning |
| --- | --- | --- |
| `anode_pos_z_cm` | number, cm | Anode z position used for calibrated hit positions. |

## parfile_NanoGRAMS.yaml

These parameters are read by `EventReconstruction` when
`reconstruction_method: "NanoGRAMS"` is used.

```yaml
incident_energy_candidates: [1173.2, 1332.5]
energy_correction_factor: [1.0, 1.0, 1.0, 1.0]
required_minimum_energy_deposit_in_higher_hit: 530.0
```

| Key | Type/range | Meaning |
| --- | --- | --- |
| `incident_energy_candidates` | list of positive numbers, keV | Candidate incident gamma-ray energies used in NanoGRAMS Compton reconstruction. |
| `energy_correction_factor` | length-4 list of positive numbers | FEC0-FEC3 multiplicative factors applied to each hit energy during NanoGRAMS event reconstruction. The input hittree energy is not modified. |
| `required_minimum_energy_deposit_in_higher_hit` | non-negative number, keV | For 2-hit NanoGRAMS reconstruction, reject the event when both corrected hit energies are at or below this value. Use `0.0` to disable this cut. |

## Legacy names

Old charge-threshold aliases are not accepted. Use one explicit key for each function:

| Function | Key |
| --- | --- |
| Core seed threshold | `charge.noise_th_kev` |
| Spread-pixel threshold | `charge.spread_thr_kev` |
| Known noisy-pixel veto threshold | `charge.noise_th_for_noisy_pixel_kev` |
| Cluster pixel count range | `charge.clustering_pix_range` |
| Known noisy pixel list | `charge.core_exclude_pix` |
| `light.use_for_event_selection` | `light.event_selection_mode` |
