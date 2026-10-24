# NanoGRAMS 解析・シミュレーション マニュアル

対象ブランチ: `feature/pipeline_nanograms_merge`（2026-10 時点のコードに基づく）

このドキュメントは、ComptonSoft 上の NanoGRAMS（LArTPC, 16×16 ピクセルアノード + SiPM 光読み出し）の
実データ解析パイプラインとシミュレーションについて、以下をまとめたものです。

1. パラメータの意味と、**YAML / XML / ANL パラメータ**のどこで何を指定するか
2. どのような解析が、どの順番で走るか
3. HitTree・QuickLook Tree に書き込まれる値の定義と単位

> 記述はすべてソースコードから読み取ったものです。挙動が変わったら該当ソースを優先してください。
> 主なソース:
> - コア: `source/core/framework/src/RealDetectorUnitNanoGRAMS.cc`, `NanoGRAMSMultiChannelData.cc`, `NanoGRAMSTemperatureCorrection.cc`, `NanoGRAMSChargeToEnergySpline.cc`, `NanoGRAMSChannelMap.cc`, `LightData.cc`
> - アプリケーション: `source/applications/missions/nanograms/src/`
> - シミュレーション: `source/core/detector_models/src/SimDetectorUnitNanoGRAMS.cc`

---

## 1. 全体像

### 1.1 検出器モデル

| 項目 | 内容 |
|---|---|
| 検出器ユニット | 実データ: `RealDetectorUnitNanoGRAMS`（`RealDetectorUnitLArTPCPixel` の派生）、シミュレーション: `SimDetectorUnitNanoGRAMS` |
| 検出器タイプ | XML の `type="NanoGRAMS"`（`DetectorType::NanoGRAMS = 7`） |
| 1 検出器 | アノード全体（16×16 ピクセル） |
| セクション | section = FEC（VATA）。4 FEC × 64 ch で**コード内に固定**。XML の `<sections>` は無視される |
| チャンネルマップ | `NanoGRAMSChannelMap.hh` に固定。FEC ごとに 8×8 の区画で、配置（ピクセル原点）は FEC0:(0,0), FEC1:(8,0), FEC2:(8,8), FEC3:(0,8) |
| 光データ | `LightData` を 8 個（index = DPP チャンネル 0–7）登録 |
| MCD | `NanoGRAMSMultiChannelData`（FEC ごとに 1 つ） |

### 1.2 設定の置き場所の原則

| 置き場所 | 何を置くか | 例 |
|---|---|---|
| **YAML**（`config_pipeline.yaml`） | 解析方針・キャリブレーション入力。キャンペーンごとに変える値。`NanoGRAMSLoadConfig` が読み、**実データとシミュレーションで共通**に使う | 最大ドリフト時間、電場、光解析/波形補正、クラスタリング閾値、除外ピクセル、ゲインファイル |
| **XML**（`detector_configuration*.xml`, `detector_parameters*.xml`） | 検出器の形状・読み出し構成・再構成モード・物理定数。シミュレーションでは検出器応答（物理過程）のパラメータもここ | 形状、ピクセル数・ピッチ、readout module、`reconstruction mode`、`clustering_range`、`w_ion`、`recombination_correction` |
| **ANL パラメータ**（Ruby/Python のチェーンスクリプト） | 入出力ファイルパス、モジュール固有の動作オプション | tpctree ファイル、出力ファイル、テストパルス表、QuickLook に書くイベント種別 |
| **DPP 設定 YAML**（`config_dpp.yaml`） | 光波形のデータ形式（ファイルごとに変わり得る） | `savefile.listwave_delay.value` |
| **コード内固定値** | ハードウェア固有の定数 | チャンネルマップ、4 FEC×64 ch、8 DPP ch、ADC→mV 変換、TI 周期など（§2.5） |

---

## 2. パラメータ

### 2.1 YAML: パイプライン設定（`examples/nanograms/metadata/config_pipeline.yaml`）

ANL パラメータ `NanoGRAMSLoadConfig.config_file` で指定します。読み込みは `NanoGRAMSConfig.cc`（`readConfig`）と
`NanoGRAMSCalibrationData.cc`（`readCalibrationConfig`）。「必須」は、キーが無いと例外で止まるものです。
シミュレーションでも同じファイルを使います。シミュレーションで有効なキーは §5.2 にまとめています。

#### `general`

| キー | 単位 | 必須 | 意味・使われ方 |
|---|---|---|---|
| `drift_time_max_us` | µs | 必須 | 最大ドリフト時間 t_max。次の 3 か所で使われる: (1) 深さの換算 z = SizeZ·(0.5 − t/t_max)、(2) クラスタの TimeUp 判定の上限（t ≥ t_max で `NanoGRAMSTimeUp`）、(3) 光解析の pre-ROI 開始 −t_max と post-ROI 終了 +t_max |
| `efield_v_cm` | V/cm | 必須 | ドリフト電場。再結合補正（モード 4）の電荷→エネルギー変換スプラインに使う。**整数として読まれる**（`as<int>`）ので小数は書かないこと |
| `temperature_k` | K | 必須 | 読み込んで表示するだけで、現状は解析に使われない |

#### `light`（光解析・波形補正）

| キー | 単位 | 必須 | 既定値 | 意味 |
|---|---|---|---|---|
| `event_selection_mode` | – | – | `gamma_required` | 光をイベント選別にどう使うか。`gamma_required`: LightCosmic/LightPileup を除外し、さらに LightGamma を必須にする。`veto_only`: LightCosmic/LightPileup のみ除外。`disabled`: 光を選別に使わない |
| `use_for_event_selection` | bool | – | – | 旧キー。true → `gamma_required`、false → `disabled`。`event_selection_mode` があればそちらが優先 |
| `waveform_analysis` | – | – | `average` | チャンネル群の扱い。`average`: 波形をサンプルごとに平均してからピークを探す（群内で `wave_compress` が同じである必要あり）。`each_channel`: チャンネルごとにピークを探して最大値を取る |
| `general_analysis_channels` | DPP ch のリスト | 必須 | – | γ/宇宙線判定と光量（PHA）に使う DPP チャンネル（0–7） |
| `pileup_analysis_channels` | DPP ch のリスト | 必須 | – | パイルアップ判定に使う DPP チャンネル |
| `light_gamma_thr_mV` | mV | 必須 | – | ROI 内ピーク > この値 → `LightGamma` |
| `light_cosmic_thr_mV` | mV | 必須 | – | ROI 内ピーク > この値 → `LightCosmic` |
| `pre_roi_window_us` | µs | 必須 | – | ROI = [−pre, +post)（トリガー時刻 = 0） |
| `post_roi_window_us` | µs | 必須 | – | 同上 |
| `out_roi_peak_thr_mV` | mV | 必須 | – | pileup チャンネルの pre-ROI または post-ROI のピーク > この値 → `LightPileup` |
| `light_gain_mode` | – | 必須 | – | 積分値の変換方法。`direct`: 積分値 [V·ns] を `light_direct_gain` で割り、光電子数（p.e.）にする。`amp_property`: TIA の帰還抵抗と出力インピーダンスから入力電荷に換算する（電子数） |
| `light_direct_gain` | mV·ns / p.e. | 必須 | – | `direct` モードの 1 p.e. あたりの波形積分値 |
| `transimpedance_feedback_resistance_ohm` | Ω | 必須 | – | `amp_property` モードの TIA 帰還抵抗 R_f（正の値） |
| `output_impedance_ohm` | Ω | 必須 | – | `amp_property` モードの TIA 出力インピーダンス R_out（0 以上） |
| `sipm_gain` | – | 必須 | – | SiPM ゲイン。`amp_property` モードで photon_count = light_pha / sipm_gain に使う |
| `light_gain_correction` | `{DPP ch: 係数}` | 必須 | 全 ch 1 | チャンネルごとの光量補正係数。general チャンネルの積分値に掛ける |
| `pedestal_correction` | bool | – | false | 波形のペデスタル差し引きを行うか |
| `pedestal_method` | – | – | `value_range` | `value_range`: 値が [min, max] の範囲にあるサンプルの平均。`time_window`: 時間窓内のサンプルの平均 |
| `pedestal_range_min` / `pedestal_range_max` | **ADC** | – | −100 / 0 | `value_range` の範囲。内部で電圧に換算して比較する |
| `pedestal_time_window_us` | µs `[start, stop]` | `time_window` のとき必須 | – | `time_window` の時間窓（トリガー = 0） |
| `digitizer_offset_correction` | bool | – | false | インターリーブ ADC の位相ごとのオフセット補正 |
| `digitizer_offset_range_start_index` / `..._stop_index` | サンプル番号 | – | 0 / 0 | オフセット推定に使うサンプル範囲 [start, stop) |
| `fft_filter` | bool | – | false | FFT バンドパスフィルタ |
| `fft_low_frequency` / `fft_high_frequency` | GHz（1/ns） | – | 0 / 0 | FFT フィルタの通過帯域 |

#### `charge`（電荷・クラスタリング）

| キー | 単位 | 必須 | 意味 |
|---|---|---|---|
| `noise_th_kev` | keV | 必須 | クラスタのコア閾値（`ClusteringEnergyThreshold`）。XML の `clustering_energy_threshold` を**上書き**する。ExcludedCore 判定と MultipleClustersInFEC 判定にも使う |
| `spread_thr_kev` | keV | 必須 | ヒット閾値かつクラスタへの併合閾値（`ClusteringSplitThreshold`）。XML の `clustering_split_threshold` を上書きする。MCD のヒット閾値にもなる |
| `clustering_pix_range` | `[min, max]` ピクセル数 | 必須 | クラスタのピクセル数の許容範囲。範囲外なら `NanoGRAMSPixelCountOutOfRange` |
| `core_exclude_pix` | `{FEC: [ch...]}` | 必須 | コアにしてはいけない VATA ch（0–63）。`peripheral` と書くとその FEC の外周ピクセル群を指定できる（リスト中に混ぜても可） |
| `cross_fec_merge_drift_time_tolerance_us` | µs | – | 異なる FEC のピクセルを同じクラスタに併合してよいドリフト時間差の上限。省略または負の値なら FEC をまたいだ併合をしない |

> 上の 2 つの閾値は、**再結合補正前**の「電荷相当エネルギー」（電荷 × W_ion）に対する値です。
> 比較に使うのは lower-mean CMN ベースの選別用 EPI（`EPIForSelection`、§3.3）です。

#### `calibration`

| キー | 単位 | 必須 | 意味 |
|---|---|---|---|
| `energy.gain_info_file` | パス | 必須 | ゲイン関数の HDF5 ファイル。相対パスは YAML ファイルの場所が基準。中身は `/FEC{n}/ADC2C`（64 ch × 4 の 3 次多項式係数、高次から: p0·x³+p1·x²+p2·x+p3、ADC→電荷 [C]）と `/FEC{n}/ccal2ADC`（テストパルス量 → ADC） |
| `energy.tp_channel` | ch | 必須 | 温度補正の基準に使うテストパルスのチャンネル |
| `energy.ccal` | – | 必須 | 温度補正の基準に使うテストパルス量。基準 ADC = ccal2ADC[tp_channel](ccal) |
| `energy.max_time_us` | µs | – | **読まれていない**。最大ドリフト時間は `general.drift_time_max_us` が使われる |
| `energy.q_to_kev_spline_file` | パス | – | **読まれていない**。電荷→エネルギーのスプラインはコード内の再結合モデルから作られる（§3.3 ⑨） |
| `position.anode_pos_z_cm` | cm | 必須 | 読み込むが、現状は使われない |

#### 同じディレクトリにある他の YAML

| ファイル | 使う側 | 内容 |
|---|---|---|
| `metadata/parfile_NanoGRAMS.yaml` | `EventReconstruction`（Compton 再構成、`extract_compton_events.py`） | `known_initial_gammaray_energy`, `energy_correction_factors`（FEC ごと。ヒットの `readout_module` = FEC を前提にする）, `required_minimum_energy_deposit_in_higher_hit` など。ヒット作成のパイプラインでは使わない |
| `metadata/config_merge_file.yaml` | hittree を結合する補助スクリプト | data group ごとの hittree 結合設定 |

### 2.2 YAML: DPP 設定（`config_dpp.yaml`）

光波形の時間原点を決めるためだけに読みます。

| キー | 単位 | 意味 |
|---|---|---|
| `savefile.listwave_delay.value` | 整数 × 8 ch | DPP チャンネルごとの波形の遅延。波形の開始時刻 = −delay × 8 × (サンプル間隔)。トリガー時刻が t = 0 になるよう揃える |

ANL パラメータ `NanoGRAMSLoadConfig.dpp_config_file` で指定すると全ファイル共通で使います。
空にすると、**tpctree ファイルごとに**同じディレクトリの `config_dpp.yaml` を読みます。

### 2.3 XML: 実データ用（`examples/nanograms/database/*_nanograms.xml`）

#### `detector_configuration_nanograms.xml`

| 要素 | 値（例） | 意味 |
|---|---|---|
| `<detector type="NanoGRAMS" name="NanoGRAMS:0">` | – | `RealDetectorUnitNanoGRAMS` を生成する。name の `:` より前（`NanoGRAMS`）がプレフィックス |
| `<geometry x y z>` | 5.12, 5.12, 10.0 cm | 有感領域。z（SizeZ）は深さ換算に使われる |
| `<pixel number_x number_y size_x size_y>` | 16, 16, 0.32, 0.32 cm | **16×16 にすること**（チャンネルマップが固定のため） |
| `<position>`, `<*axis_direction>` | – | 検出器の配置。HitTree の global 座標に反映される |
| `<sections>` | 不要 | 書いても無視される |
| `<readout><module id="n"><section detector_id="0" section="n"/>` | n = 0–3 | readout module ID = FEC にしておく。HitTree の `readout_module` になり、Compton 再構成の FEC ごとのエネルギー補正係数で使われる |

#### `detector_parameters_nanograms.xml`

`<detector_set prefix="NanoGRAMS" type="NanoGRAMS">` の `<parameters>` 内に書きます。

| 要素/属性 | 単位 | 意味 |
|---|---|---|
| `reconstruction mode` | – | 0: 再構成しない（検出ヒットをそのまま出力。クラスタリングも再結合補正もなし）。5: NanoGRAMS 独自のクラスタリングを行う。1, 3, 4: RealDetectorUnitLArTPCPixel と同じ汎用のクラスタリング（1: 隣接するピクセルを併合、3: 1 と同じで接触条件なし、4: しきい値によるクラスタリング）。ピクセルとクラスタの対応（`ClusterCorrespondence`）は作られず、NanoGRAMS のクラスタフラグも付かない（すべてガンマ線のクラスタとして扱われる）。2: クラスタリングなし（ピクセルごと） |
| `reconstruction clustering_range` | ピクセル | コアから |Δx|, |Δy| ≤ range のピクセルを併合する（斜めを含む）。**XML でのみ指定**（指定しないと VRealDetectorUnit の既定値 2） |
| `reconstruction clustering_energy_threshold`, `clustering_split_threshold` | keV | `NanoGRAMSLoadConfig` を使うと YAML（`charge.noise_th_kev`, `spread_thr_kev`）で上書きされる（実データ・シミュレーションとも） |
| `reconstruction w_ion` | eV | 電離の W 値（既定 23.6 eV）。EPI = 電子数 × W_ion に使う。MCD にも伝わる |
| `reconstruction recombination_correction` | – | 0: 補正なし、1: 光を使う ((EPI/W_ion + photon_count) × W_exc)、2: 補正ファイル、3: 1 と 2 の併用、**4: NanoGRAMS 既定**（電場依存の電荷→エネルギースプライン） |
| `reconstruction w_exc` | eV | モード 1, 3 で使う |
| `<recombination photon_efficiency>` | – | 光検出効率（既定 1）。photon_count はこの値で割られる |

### 2.4 ANL パラメータ（実データのチェーン）

例: `examples/nanograms/data_reduction_with_light_correction.rb`

| モジュール | パラメータ | 型・既定値 | 意味 |
|---|---|---|---|
| `ConstructDetector` | `detector_configuration`, `detector_parameters` | パス | §2.3 の XML |
| `NanoGRAMSLoadConfig` | `config_file` | パス | §2.1 のパイプライン YAML |
| | `dpp_config_file` | パス（空 = ファイルごとに自動） | §2.2 |
| | `gain_tp_file` | パス（空可） | 温度補正用のテストパルス ADC の時系列 CSV（§3.3 ④）。相対パスは YAML の場所が基準 |
| | `gain_tp_hash` | `{"0"または"FEC0" => ADC, ...}` | テストパルス ADC の固定値（4 FEC すべて正の値が必要）。`gain_tp_file` が空のときだけ使う |
| `NanoGRAMSReadTPCEvents` | `tpctree_files` | パスのリスト | 入力 tpctree（ROOT, tree 名 `tpctree`）。順に読む |
| | `run_id` | int, 0 | 保持するだけで、**HitTree の runid には反映されない**（§4.1） |
| `CorrectPHA` | `pedestal_level` | `"0"` | ペデスタル補正なし（**`"0"` 固定**） |
| | `CMN_estimation` | `2` | メディアン CMN（**2 固定**。lower mean も同時に計算する） |
| | `gain_function` | `"1"` | ROOT ファイルを読まず、LoadConfig が設定したゲイン関数を使う（**`"1"` 固定**） |
| `SelectHits` | `analysis_map` | – | 現状のコードでは**適用されない**（`setAnalysisParameters()` がどこからも呼ばれていない）。再構成モードは XML、閾値は YAML で決まる |
| `NanoGRAMSQuickLookWriter` | `quicklook_file` | `"quicklook.root"` | 出力ファイル |
| | `detector_id` | 0 | 対象の検出器 ID |
| | `event_types` | 空 = 全種別 | 書き込むイベント種別（§4.2 の名前で指定） |
| | `num_hits` | −1 = 制限なし | 選別を通ったクラスタ数がちょうどこの値のイベントだけ書く |
| | `save_waveforms` | true | 光波形のブランチを書くか |
| `WriteHitTree` | – | – | `CSHitCollection` のヒットを `hittree` に書く |
| `SaveData` | `output` | パス | HitTree の出力ファイル |

### 2.5 コード内の固定値

| 値 | 定義場所 | 内容 |
|---|---|---|
| ADC → mV | `Config::adc2mv` | 1000/8192 mV/ADC（デジタイザ） |
| TI | `RealDetectorUnitNanoGRAMS` | 32 bit, 1 tick = 160 ns（約 687 s で一周する） |
| ドリフト時間 | `NanoGRAMSConstants.hh` | t = カウント × 10 ns + 0.68 µs（DPP の応答時間） |
| lower-mean CMN | `NanoGRAMSMultiChannelData` | 下位 10 チャンネルの平均 |
| 使えるデータ | `isTPCDataUsable` | tpctree の `error_flags` が 0 または 4 のイベントだけ使う |
| デジタイザ入力インピーダンス | `NanoGRAMSAnalyzeLight.cc` | 50 Ω（`amp_property` の換算に使う） |
| 電荷→エネルギースプライン | `NanoGRAMSChargeToEnergySpline.cc` | W_ion 23.6 eV, W_q 19.5 eV, N_ex/N_i 0.21 などの定数を持つ再結合モデル。1 keV–10 MeV を対数で 99 点にとり、範囲外はクランプする |

---

## 3. 実データ解析の流れ

### 3.1 モジュールチェーン

```
CSHitCollection
ConstructDetector             … XML から RealDetectorUnitNanoGRAMS を構築（4 MCD, 8 LightData, 固定チャンネルマップ）
NanoGRAMSLoadConfig           … YAML/DPP設定/ゲインHDF5/テストパルス表を読み、検出器ユニットとMCDに値を設定
NanoGRAMSReadTPCEvents        … tpctree を 1 エントリ読み、生データをユニット/MCD/LightData に詰める
NanoGRAMSCorrectLightWaveform … 光波形の補正（ペデスタル → デジタイザオフセット → FFT）
NanoGRAMSAnalyzeLight         … 光の判定フラグ、積分光量、ピーク値
CorrectPHA                    … CMN(メディアン/lower mean) → 温度補正 → EPI
SelectHits                    … ヒット選択 → クラスタリング → フラグ → 再結合補正
NanoGRAMSQuickLookWriter      … (任意) 選別前の全イベントを QuickLook に書く
NanoGRAMSSelectEvents         … 選別。残るヒットが無いイベントは AS_SKIP
WriteHitTree                  … HitTree に書く
SaveData
```

順番の制約:
- `NanoGRAMSLoadConfig` は `ConstructDetector` の後、それを使うモジュールの前に置く。
- `NanoGRAMSAnalyzeLight` は `SelectHits` の前に置く（光量をヒットに載せるため）。
- `NanoGRAMSQuickLookWriter` は `SelectHits` の後、`NanoGRAMSSelectEvents` の前に置く（除外されるイベントも書くため）。

### 3.2 初期化（`NanoGRAMSLoadConfig::mod_initialize`）

1. パイプライン YAML（§2.1）を読む。`dpp_config_file` があれば読む。
2. ゲイン HDF5 から FEC ごとに `ADC2C`（64 ch の 3 次式）を読み、各 MCD のゲイン関数に設定する。
3. 温度補正（`NanoGRAMSTemperatureCorrection`）を作る:
   - 基準 ADC = `ccal2ADC[tp_channel]` を `ccal` で評価した値（FEC ごと）
   - 測定 ADC = `gain_tp_file` の時系列（線形補間）、または `gain_tp_hash` の固定値
4. 検出器ユニットに次を設定する: 最大ドリフト時間、電場（→スプライン生成）、温度補正、
   クラスタ閾値（`noise_th_kev` / `spread_thr_kev`）、FEC 間併合の許容差、TimeUp 上限（= `drift_time_max_us`）、
   ピクセル数範囲、除外ピクセル。各 MCD のヒット閾値は `spread_thr_kev`。

### 3.3 イベントごとの処理

#### ① 読み込み（`NanoGRAMSReadTPCEvents`）

- ファイルを開くとき: DPP 設定を読み（未指定の場合）、tpctree の 1 エントリ目の `registered` から波形スロットと DPP ch の対応を決め、
  LightData のレイアウトを設定する（サンプル数、サンプル間隔 = `wave_compress` ns、開始時刻 = −delay×8×間隔）。
  general/pileup のどちらにも含まれないチャンネルと、記録されていないチャンネルは invalid になる。
- エントリごと:
  - `error_flags` ∉ {0, 4} → **AS_SKIP**（以降のモジュールは走らず、HitTree にも QuickLook にも出ない）
  - unixtime: 4 FEC のうち最初の 0 でない値
  - MCD: 64 ch の生 ADC。ユニット: FEC ごとの生 TI、生ドリフト時間カウント、ドリフト時間 [µs]
  - LightData: `RawWaveform` = ADC × adc2mv（電圧）。`Waveform` も同じ値で初期化する

#### ② 光波形補正（`NanoGRAMSCorrectLightWaveform`）

valid なチャンネルの `Waveform` をその場で補正する（`RawWaveform` は変えない）。
1. ペデスタル（`pedestal_correction`）: `value_range` / `time_window` で推定して差し引く
2. デジタイザオフセット（`digitizer_offset_correction`）: 位相数 = max(1, 8 / wave_compress)。指定サンプル範囲で位相ごとの平均を取り、差し引く
3. FFT バンドパス（`fft_filter`）

#### ③ 光解析（`NanoGRAMSAnalyzeLight`）

時間窓（トリガー = 0）: pre-ROI [−t_max, −pre)、ROI [−pre, +post)、post-ROI [+post, +t_max)

| 処理 | 対象チャンネル | 結果 |
|---|---|---|
| ROI 内ピーク > `light_gamma_thr_mV` | general | イベントフラグ `LightGamma` |
| ROI 内ピーク > `light_cosmic_thr_mV` | general | イベントフラグ `LightCosmic` |
| pre-ROI か post-ROI のピーク > `out_roi_peak_thr_mV` | pileup | イベントフラグ `LightPileup` |
| ROI 積分（ベースライン = pre-ROI の平均）× `light_gain_correction[ch]` | general | `LightIntegratedCharge(ch)`。その和が `LightPHA` |
| ROI 積分（ゲイン補正なし） | pileup のみのチャンネル | `LightIntegratedCharge(ch)`（LightPHA には含めない） |
| ROI 内ピーク値 | general | `LightPeakValue(ch)` |
| photon count | – | `direct`: LightPHA。`amp_property`: LightPHA / `sipm_gain` |

積分値の換算:
- `direct`: ∫(V − baseline)dt / `light_direct_gain` → **光電子数 [p.e.]**
- `amp_property`: ∫(V − baseline)dt × (R_out + 50 Ω) / 50 Ω / R_f → TIA 入力電荷 → **電子数 [e]**

#### ④ 電荷の補正（`CorrectPHA` + `NanoGRAMSMultiChannelData`）

FEC（MCD）ごとに:
1. PHA = 生 ADC（ペデスタル補正なし）
2. CMN = 有効な 64 ch の**メディアン**（偶数個なので中央 2 つの平均）。同時に **lower mean**（下位 10 ch の平均）も計算する
3. PHA = 生 ADC − メディアン
4. **温度補正**: PHA × f。f = 基準 ADC / 測定テストパルス ADC（イベントの unixtime で補間）
   - テストパルス情報が無ければ f = 1
   - `gain_tp_file` の時間範囲外のイベントは f = NaN になり、EPI も NaN になる（**ヒットが出ない**）
   - CSV の `time_id`（`YYYYMMDD_HHMM_SS`）は `mktime` で**ローカルタイムゾーン**として解釈される
5. **EPI** = (g(PHA) − g(0)) / e × W_ion（g = ADC2C の 3 次式、単位 C）。PHA ≤ 0 や電荷 ≤ 0 なら 0
6. **選別用 EPI**（`EPIForSelection`）: (生 ADC − lower mean) × f を同じ式で変換したもの

#### ⑤ ヒット選択（`RealDetectorUnitNanoGRAMS::selectHits`）

1. TI の一周補正: 前のイベントからの unixtime の経過時間に最も近い周回数を選ぶ（unixtime が無いときは TI が減ったら 1 周とみなす）
2. 絶対時刻 = 最初のイベントの unixtime + (TI − 最初の TI) × 160 ns（FEC ごと）
3. MCD のヒット: `EPIForSelection ≥ spread_thr_kev` のチャンネル
4. 各ヒットに、選別用 EPI、TI、LightPHA、PhotonCount を載せる
5. **ExcludedCore**: いずれかの FEC で選別用 EPI が最大のチャンネルが `core_exclude_pix` に含まれ、かつその値が `noise_th_kev` を超えていたらイベントフラグを立てる

#### ⑥ 位置

- x, y: ピクセル中心
- z（局所）= SizeZ × (0.5 − t_drift(FEC) / t_max)。アノード（t = 0）が局所 z = +SizeZ/2。**同じ FEC のヒットはすべて同じ z になる**（ドリフト時間が FEC ごとに 1 つのため）

#### ⑦ クラスタリング（`clusterForNanoGRAMS`、モード 1/3/4）

1. 候補 = 選別用 EPI ≥ `spread_thr_kev`。コア（シード）= 選別用 EPI > `noise_th_kev` で、除外ピクセルでないもの
2. シードを選別用 EPI の降順に処理し、未使用の候補のうちコアから |Δx|, |Δy| ≤ `clustering_range` のものを併合する
   - 範囲はコアからのみ測る（併合したピクセルからさらに広げない）
   - 異なる FEC のピクセルは、許容差 ≥ 0 かつ |Δt_drift| ≤ 許容差 のときだけ併合する
3. 併合後のクラスタ: チャンネル・FEC・rawpha はコアのもの。PHA・EPI・選別用 EPI は和。位置は EPI 重み付き平均。`AdjacentClustered` フラグ
4. **どのシードにも併合されなかった候補は捨てられる**（出力されない）

#### ⑧ クラスタフラグ（`setClusterFlags`。この時点ではヒットを消さない）

| フラグ | 値 | 条件 |
|---|---|---|
| `NanoGRAMSTimeUp` | 0x200000 | コアの FEC のドリフト時間が NaN、または ≥ `drift_time_max_us` |
| `NanoGRAMSPixelCountOutOfRange` | 0x400000 | ピクセル数が `clustering_pix_range` の範囲外 |
| `NanoGRAMSCollinear` | 0x800000 | 3 ピクセルが一直線に並ぶ |
| `NanoGRAMSMultipleClustersInFEC` | 0x1000000 | 同じ FEC に、このクラスタに含まれず除外ピクセルでもない、選別用 EPI > `noise_th_kev` のピクセルがある |

#### ⑨ エネルギー

1. `EnergyCharge` ← EPI（再結合補正前の電荷相当エネルギー、クラスタの和）
2. photon count ÷ photon_efficiency
3. 再結合補正（モード 4）: Q = EPI / W_ion × e [C] → E = spline(Q) [keV]。EPI ← E
4. `Energy` ← EPI

#### ⑩ 選別（`NanoGRAMSSelectEvents`）

検出器単位で、次のいずれかに当たればその検出器のヒットをすべて除外する:
- `ExcludedCore`（モードに関係なく）
- `event_selection_mode` ≠ `disabled` で、`LightCosmic` または `LightPileup`
- `gamma_required` で、`LightGamma` が無い

残ったヒットのうち、§3.3 ⑧ の 4 つのフラグのどれかを持つクラスタを除外する。
残るヒットが 0 個なら **AS_SKIP**（HitTree に書かない）。

---

## 4. 出力ファイル

### 4.1 HitTree（`hittree`、`WriteHitTree`）

1 エントリ = 1 ヒット（実データでは**選別を通ったクラスタ**）。同じイベントのヒットは連続して並ぶ。

| ブランチ | 型 | 単位 | NanoGRAMS 実データでの定義 |
|---|---|---|---|
| `runid` | I | – | **0 固定**（`InitialInformation` が無いため。`run_id` パラメータは反映されない） |
| `eventid` | I | – | ANL のループ番号（0 始まり）。スキップされたイベントも数えるので、複数ファイルを通した tpctree の通し番号に一致し、QuickLook の `raw_event_id` と対応する |
| `ihit` | S | – | イベント内のヒット番号 |
| `num_hits` | I | – | イベント内のヒット（クラスタ）数 |
| `ti` | L | 160 ns tick | コアの FEC の、一周補正済み TI |
| `instrument` | S | – | 0 |
| `detector` | S | – | 検出器 ID |
| `det_section` | S | – | コアの FEC（0–3） |
| `readout_module` | S | – | XML の readout module ID（= FEC にしておく） |
| `section` | S | – | readout module 内のセクション（0） |
| `channel` | S | – | コアの VATA チャンネル（0–63） |
| `pixelx`, `pixely` | S | – | ピクセル座標（0–15）。基本的にはコアのピクセル（EPI 重みで切り替わることがある） |
| `pixelz` | S | – | −1 |
| `rawpha` | I | ADC | コアピクセルの生 ADC |
| `pha` | F | ADC | クラスタの PHA の和（メディアン CMN を引き、温度補正したもの） |
| `epi` | F | keV | **再結合補正後**のエネルギー（モード 4 では `energy` と同じ） |
| `epi_error` | F | keV | EPI の誤差（実データでは通常 0） |
| `light_pha` | F | p.e.（`direct`）/ e（`amp_property`） | イベントの光量: general チャンネルの ROI 積分 × ゲイン補正の和。全ヒットで同じ値 |
| `light_pha_error` | F | – | 0 |
| `photon_count` | F | p.e. | `direct`: light_pha、`amp_property`: light_pha / sipm_gain。さらに photon_efficiency で割る |
| `photon_count_error` | F | – | 0 |
| `flag_data` | l | – | MCD のフラグ（通常 0） |
| `flags` | l | bit | `PrioritySide`(0x8)、`AdjacentClustered`(0x10)、§3.3 ⑧ の NanoGRAMS フラグなど。選別後は NanoGRAMS の除外フラグは立っていない |
| `trackid`, `particle`, `real_time`, `time_trig`, `time_group`, `real_pos*`, `edep`, `process` | | | シミュレーション用（実データでは既定値） |
| `echarge` | F | keV | **再結合補正前**の電荷相当エネルギー（電荷 × W_ion）のクラスタ和 |
| `energy` | F | keV | 再構成エネルギー（= `epi`） |
| `energy_error` | F | keV | = `epi_error` |
| `posx`, `posy`, `posz` | F | cm | 世界座標（XML の配置を反映）。x, y は EPI 重み付き、z はドリフト時間から（§3.3 ⑥） |
| `pos*_error` | F | cm | ピクセルピッチ / √12 など、VRealDetectorUnit の位置誤差 |
| `local_posx/y/z` | F | cm | 検出器の局所座標（中心が原点、アノード側が +z） |
| `local_pos*_error` | F | cm | 同上の誤差 |
| `time` | D | s | 絶対時刻 = unixtime₀ + (TI − TI₀) × 160 ns（コアの FEC） |
| `time_error` | D | s | 0 |
| `grade` | I | – | 0 |

> HitTree に残るのは選別後のヒットだけです。除外されたイベントを見るには QuickLook を使います。

### 4.2 QuickLook Tree（`tpcquicklook`、`NanoGRAMSQuickLookWriter`）

1 エントリ = 1 イベント（`SelectHits` の後・選別の前）。`event_types` と `num_hits` で書くイベントを絞れる。
`error_flags` でスキップされたイベントは書かれない。

#### `event_type` の分類（上から順に最初に当てはまったもの）

| 値 | 名前（`event_types` での指定名） | 条件 |
|---|---|---|
| 5 | ExcludedCore (`excluded`) | `ExcludedCore` フラグ |
| 1 | Gamma (`gamma`) | 選別（§3.3 ⑩）を通るクラスタが 1 つ以上ある（= HitTree に出るイベント） |
| 2 | Cosmic (`cosmic`) | 光を使うモードで `LightCosmic` |
| 3 | PileUp (`pileup`) | 光を使うモードで `LightPileup` |
| 4 | TimeUp (`timeup`) | 全 FEC のドリフト時間が NaN または ≥ t_max |
| 7 | LightNotGamma (`lightnotgamma`) | `gamma_required` で `LightGamma` が無い |
| 6 | NoCluster (`nocluster`) | クラスタが 0 個 |
| 8 | RejPixelCount (`rej_pixelcount`) | 全クラスタが除外され、いずれかに PixelCountOutOfRange |
| 9 | RejCollinear (`rej_collinear`) | 同上、Collinear |
| 10 | RejMultiCluster (`rej_multicluster`) | 同上、MultipleClustersInFEC |
| 11 | RejTimeUp (`rej_timeup`) | 同上、TimeUp |
| 0 | Other (`other`) | 上のどれでもない |
| −1 | Error (`error`) | 定義はあるが、書かれることはない |

#### ブランチ

| ブランチ | 型 | 単位 | 定義 |
|---|---|---|---|
| `raw_event_id` | L | – | tpctree の通し番号（複数ファイルを通して 0 始まり）。HitTree の `eventid` と対応 |
| `event_type` | S | – | 上の表 |
| `cmn_method` | S | – | Cosmic なら 1、それ以外 0（記録用。計算は常にメディアン CMN） |
| `adu_cmn_sub[4][64]` | F | ADC | 生 ADC − メディアン CMN（**温度補正前**） |
| `energy_cmn_sub[4][64]` | F | keV | MCD の EPI（メディアン CMN ベース、温度補正後、電荷 × W_ion、**再結合補正前**）。0 以下や無効なチャンネルは 0 |
| `cmn[4]` | F | ADC | FEC ごとのメディアン CMN |
| `ti[4]` | i | 160 ns tick | **生**の TI カウンタ（一周補正なし） |
| `drift_time[4]` | i | 10 ns clock | **生**のドリフト時間カウンタ。µs への換算: ×0.01 + 0.68 |
| `wave_compress[8]` | s | ns | DPP ch ごとのサンプル間隔（invalid なチャンネルは 0） |
| `light_integrated_charge[8]` | D | p.e.（`direct`）/ e（`amp_property`） | DPP ch ごとの ROI 積分値。general ch はゲイン補正込み、pileup のみの ch は補正なし、それ以外の ch は 0。pre-ROI にサンプルが無いと NaN |
| `light_peak_value[8]` | D | mV | 補正後の波形の ROI 内ピーク（general ch のみ。それ以外は 0） |
| `waveform_len` | I | – | 波形のサンプル数（`save_waveforms` のときだけ） |
| `waveform_num_channels` | I | – | 書いたチャンネル数 n |
| `waveform_dpp_ch[n]` | S | – | 各スロットの DPP ch |
| `waveform[n][len]` | F | mV | **補正後**の光波形（`LightData::Waveform`）。時刻は wave_compress と DPP の遅延から求める（トリガー = 0） |
| `hit_pixel_fec` | vector&lt;short&gt; | – | 選別を通ったクラスタを構成するピクセルの FEC |
| `hit_pixel_ch` | vector&lt;short&gt; | – | 同 VATA ch |
| `hit_pixel_adu` | vector&lt;float&gt; | ADC | 同 生 ADC − メディアン CMN |
| `hit_pixel_energy` | vector&lt;float&gt; | keV | 同 ピクセルの EPI（電荷 × W_ion、再結合補正前） |
| `hit_pixel_cluster_id` | vector&lt;short&gt; | – | このイベントで書いたクラスタの中での番号（0 始まり） |
| `hit_num_pixels` | vector&lt;short&gt; | – | クラスタごとのピクセル数 |
| `rej_hit_*` | 同上 | 同上 | 自身のフラグ（§3.3 ⑧）で除外されたクラスタ。検出器単位で除外されたイベント（Cosmic など）の、フラグの無いクラスタはどちらにも入らない |
| `rej_cluster_type` | vector&lt;uint&gt; | bit | 除外されたクラスタごとの理由。bit0: TimeUp, bit1: PixelCountOutOfRange, bit2: Collinear, bit3: MultipleClustersInFEC |

---

## 5. シミュレーション

### 5.1 指定方法

シミュレーションでも実データと同じく **XML・パイプライン YAML・ANL パラメータ**の 3 つで指定し、
YAML は `NanoGRAMSLoadConfig` で読み込みます（XML の例は `examples/simulations/NanoGRAMS/database/`）。

- 実行は `ComptonSoft::Simulation`（`set_database`, `set_gdml`, `set_primary_generator` など。例: `examples/simulations/LArTPC/run_simulation.rb`）
- 検出器の構築は `ConstructDetectorForSimulation`。`NanoGRAMSLoadConfig` はこれより**後**に chain する
  （LoadConfig は `mod_initialize` で検出器ユニットに値を設定するため）。`Simulation` クラスに LoadConfig を差し込む口は、
  現状のチェーン定義（`bindings/ruby/library/comptonsoft/simulation.rb`）にはまだ用意されていない
- 検出器は `SimDetectorUnitNanoGRAMS`（`RealDetectorUnitNanoGRAMS` + `LArTPCDeviceSimulation`）。
  `RealDetectorUnitNanoGRAMS` の派生なので、LoadConfig の設定処理がそのまま適用される
- セクション・チャンネルマップの固定や再構成処理（クラスタリング・フラグ・再結合補正）は実データと同じ

`NanoGRAMSLoadConfig` の ANL パラメータのうち、シミュレーションで必要なのは `config_file` だけです。
`dpp_config_file`, `gain_tp_file`, `gain_tp_hash` は不要です（指定しないと温度補正係数 = 1 の警告が出るだけ）。

### 5.2 パイプライン YAML のうちシミュレーションで有効なもの

シミュレーションのヒットは MCD（ADC→電荷の変換や CMN）を通らず、`MakeDetectorHits` が
`makeDetectorHits()`（検出器応答）と `reconstructHits()`（再構成）を呼ぶだけです。`selectHits()` も光の解析も走りません。
そのため YAML のうち有効なのは再構成に関わるキーだけです。

| キー | シミュレーションでの扱い |
|---|---|
| `general.drift_time_max_us` | **有効**。深さの換算の t_max と TimeUp の上限。XML から決まる値（SizeZ / `drift_velocity`）を**上書きする**ので、両者を一致させること（ずれると再構成の z が歪む） |
| `general.efield_v_cm` | **有効**。再結合補正モード 4 のスプライン |
| `charge.noise_th_kev`, `spread_thr_kev` | **有効**。XML の `clustering_energy_threshold` / `clustering_split_threshold` を上書きする |
| `charge.clustering_pix_range`, `cross_fec_merge_drift_time_tolerance_us` | **有効** |
| `charge.core_exclude_pix` | シードにしない画素・MultipleClustersInFEC の判定には**有効**。ただし ExcludedCore のイベントフラグは `selectHits()` で立てるので、シミュレーションでは**立たない** |
| `light.event_selection_mode` | `NanoGRAMSSelectEvents` を使う場合に有効。光のフラグはシミュレーションでは立たないので、`gamma_required` にすると全イベントが除外される。`disabled` か `veto_only` を使う |
| `light` のその他のキー | 使われない（ただし必須キーは書いておく必要がある） |
| `calibration.energy.gain_info_file`, `tp_channel`, `ccal` | 使われないが、**ファイルの読み込みは行われる**ので、存在する HDF5 を指定する必要がある |
| `general.temperature_k`, `calibration.position.anode_pos_z_cm` | 使われない（必須キー） |

### 5.3 `detector_parameters.xml` の主なパラメータ

| 要素/属性 | 単位 | 意味 |
|---|---|---|
| `<upside anode readout>` | – | 読み出し面の向き |
| `<quenching factor>` | – | 原子核反跳のエネルギーに掛ける係数 |
| `<temperature value>` | K | 温度 |
| `<charge_collection mode>` + `<mutau electron hole>` | cm²/V | 電荷収集の計算モード（2, 3 は重み付けポテンシャル・CCE マップを作る）と μτ |
| `<diffusion mode>` + `<coefficient transverse longitudinal>` | cm²/s | 拡散のモードと係数 |
| `<drift_velocity value>` | cm/s | ドリフト速度。t_max = SizeZ / v |
| `<efield bias>` | V | バイアス電圧。シミュレーションの再結合では E = bias / 厚さ |
| `<recombination filename photon_efficiency>` | – | 再結合モデルの XML（`recombination.xml`: Birks / ModifiedBox）と光検出効率 |
| `<channel_properties><noise_level param0 param1 param2>` | keV | EPI のノイズ: σ² = p0² + p1²·E + p2²·E²（E は keV） |
| `<channel_properties><threshold value>` | keV | EPI に対する閾値 |
| `<reconstruction ...>` | – | 実データと同じ（§2.3）。`clustering_energy_threshold` / `clustering_split_threshold` は YAML で上書きされる |

`<efield bias>` はシミュレーションの再結合（検出器応答）に、YAML の `efield_v_cm` は再構成の補正に使われます。
同じ電場になるよう両方をそろえてください。

### 5.4 処理の流れ

1. Geant4 のステップ → 生ヒット
2. 再結合モデルで電荷（`echarge`）と光子数を計算（中性粒子・dE/dx が無効なときは再結合なし）、クエンチング、電荷収集効率
3. 拡散で周辺ピクセルに分配
4. EPI = echarge + ガウスノイズ。閾値未満は捨てる
5. 光子数: 検出器内の全ヒットの和にノイズを加え、全ヒットに同じ値を設定する
6. 再構成（`SimDetectorUnitNanoGRAMS::reconstruct`）
   - 選別用 EPI = EPI（lower mean は無い）
   - FEC ごとのドリフト時間 = その FEC で**最もアノードに近い（z 最大の）ヒット**の (SizeZ/2 − z) / v
   - 以降は実データと同じ（§3.3 ⑥–⑨）。そのため **z は FEC ごとに 1 つ**になり、`echarge` は再構成時に EPI（ノイズ込み）で上書きされる

### 5.5 シミュレーションでの注意

- **ピクセルは 16×16 にすること**。チャンネルマップが 16×16 に固定されており、範囲外のピクセルの参照は配列の範囲外アクセスになる。
  `examples/simulations/NanoGRAMS` の XML（100×100 ピクセル、`<sections num_channels="100000">`）は現状のコードと合っていない。
- `recombination_correction="4"` は電場の設定が必要で、電場は `NanoGRAMSLoadConfig` からしか設定されない。
  LoadConfig を chain せずにモード 4 を使うと例外になる。
- QuickLook と光の判定（`NanoGRAMSAnalyzeLight` など）は実データ専用。シミュレーションの `light_pha` は 0。
- シミュレーションのヒットには TI・絶対時刻（§3.3 ⑤）が付かない（`selectHits()` を通らないため）。

---

## 6. 既知の問題・注意点（2026-10 時点）

| 対象 | 内容 |
|---|---|
| `examples/nanograms/data_reduction_with_light_correction.rb` | `detector_configuration.xml` / `detector_parameters.xml`（2DPixel 用）を読んでいる。NanoGRAMS 用の `*_nanograms.xml` を使う必要がある。また main 部で未定義の `MyAppDataReduction` を生成しており、`config_file` などのアトリビュートも設定していない |
| `examples/nanograms/run_make_hittree.py` | 旧 API（`NanoGRAMSWriteHitTree`、`NanoGRAMSReadTPCEvents` に `config_file` などを渡す形）のままで、現状のモジュール構成では動かない |
| `SelectHits.analysis_map` | 値が適用されない（§2.4） |
| `NanoGRAMSReadTPCEvents.run_id` | HitTree の `runid` に反映されない（常に 0） |
| `NanoGRAMSQuickLookTreeIO.hh` のコメント | `light_integrated_charge` の単位を [C] としているが、実際は p.e. または電子数（§4.2） |
| YAML の読まれないキー | `calibration.energy.max_time_us`, `calibration.energy.q_to_kev_spline_file`（読まれない）、`general.temperature_k`, `calibration.position.anode_pos_z_cm`（読むが使わない） |
| `general.efield_v_cm` | 整数として読まれる |
| テストパルス表 | 時間範囲外のイベントでは温度補正係数が NaN になり、ヒットが出ない。`time_id` はローカルタイムとして解釈される |
| シミュレーションでの `NanoGRAMSLoadConfig` | `Simulation` クラスに差し込む口が無い。また、使わない光・キャリブレーションのキーやゲイン HDF5 も必須になっている（§5.2） |
| `examples/simulations/NanoGRAMS` | 100×100 ピクセルの XML のまま。NanoGRAMS 用の実行スクリプトとパイプライン YAML も無い |
