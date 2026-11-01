#!/usr/bin/env ruby

require 'comptonsoft'
require 'fileutils'
require 'csv'

# Light (scintillation) analysis of DPP-only data (dpp_list_*.root = dpplist tree of nanograms-readout).
# The waveforms are treated in the same way as those of the tpctree, and the results of
# NanoGRAMSAnalyzeLight are written into the quicklook tree:
#   light_integrated_charge [e], light_peak_value [mV], wave_compress, waveform [mV]
# The charge branches (adu_cmn_sub, hit_*, ...) are empty, and event_type is not meaningful
# without the charge data (most events are TimeUp; Cosmic/PileUp are given by the light flags).
#
# usage:
#   ruby light_analysis_dpp.rb                         # all the data in the data groups
#   TIME_ID=20260625_2000_20 ruby light_analysis_dpp.rb  # only one data

# the original config and the copy in the output directory (the copy is actually used)
CONFIG_FILE = "metadata/config_pipeline.yaml"
$config_file = CONFIG_FILE

class NanoGRAMSLightAnalysisDpp < ANL::ANLApp
  attr_accessor :dpplist_files, :quicklook_file
  attr_accessor :dpp_config_file, :save_waveforms

  def setup
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ConstructDetector
    with_parameters(detector_configuration: "database/detector_configuration.xml",
                    detector_parameters: "database/detector_parameters.xml")

    # yaml configuration (light analysis parameters, analysis channels)
    # if dpp_config_file is empty, config_dpp.yaml in the directory of each dpplist file is read
    chain :NanoGRAMSLoadConfig
    with_parameters(config_file: $config_file,
                    dpp_config_file: @dpp_config_file || "")

    # read dpplist: entries with the same trigger ID -> one event; light waveforms -> LightData
    chain :NanoGRAMSReadDppEvents
    with_parameters(dpplist_files: @dpplist_files,
                    tree_name: "dpplist")

    # light: waveform corrections (light.pedestal_correction etc. in the yaml),
    # then judgement (LightGamma/Cosmic/Pileup) and the integrated charge (photon count)
    chain :NanoGRAMSCorrectLightWaveform
    chain :NanoGRAMSAnalyzeLight

    # all the events are written
    chain :NanoGRAMSQuickLookWriter
    with_parameters(quicklook_file: @quicklook_file,
                    event_types: [],
                    num_hits: -1,
                    save_waveforms: @save_waveforms)
  end
end


### main ###

data_root_dir = "data"
data_group_list = ["Na22Test"]
only_time_id = ENV["TIME_ID"]&.strip

# output directory named by the start time of the analysis; the config and the log are also put there
run_start_time = Time.now
run_id = run_start_time.strftime("%Y%m%d_%H%M%S")
outdir_parent = File.expand_path(File.join("products/light_analysis_dpp", run_id), __dir__)
FileUtils.mkdir_p(outdir_parent)

config_source = File.expand_path(CONFIG_FILE, __dir__)
raise "config file not found: #{config_source}" unless File.exist?(config_source)
$config_file = File.join(outdir_parent, File.basename(config_source))
FileUtils.cp(config_source, $config_file)

# output to both the terminal and the log file (redirect the file descriptors to catch the C++ output)
log_path = File.join(outdir_parent, "run.log")
tee = IO.popen(["tee", log_path], "w")
STDOUT.reopen(tee)
STDERR.reopen(tee)
STDOUT.sync = true
STDERR.sync = true

puts "[run] start: #{run_start_time}"
puts "[run] output directory: #{outdir_parent}"

def event_dir_name(time_id)
  time_id.strip.tr("/", "_")
end

def dpp_data_dir(data_root_dir, time_id)
  flat_dir = File.join(data_root_dir, event_dir_name(time_id))
  return flat_dir if Dir.exist?(flat_dir)

  # 20260626_0320_47 -> 20260626/0320_47
  match = time_id.strip.match(/\A(\d{8})_(\d{4}_\d{2})\z/)
  return File.join(data_root_dir, match[1], match[2]) if match

  File.join(data_root_dir, *time_id.strip.split("/"))
end

data_group_list.each do |tag|
  filename = File.expand_path("metadata/data_group/data_group_#{tag}.csv", __dir__)
  CSV.foreach(filename, headers: true) do |row|
    time_id = row["time"]
    next if only_time_id && event_dir_name(time_id) != event_dir_name(only_time_id)

    data_dir = dpp_data_dir(data_root_dir, time_id)
    dpplist_files = Dir.glob(File.join(data_dir, "dpp_list_*.root")).sort
    if dpplist_files.empty?
      warn "[skip] no dpp_list files: #{data_dir}"
      next
    end

    outdir = File.join(outdir_parent, event_dir_name(time_id))
    FileUtils.mkdir_p(outdir)

    puts "[run] #{event_dir_name(time_id)}: #{dpplist_files.size} dpp_list files"
    a = NanoGRAMSLightAnalysisDpp.new
    a.dpplist_files = dpplist_files
    a.quicklook_file = File.join(outdir, "quicklook.root")
    a.save_waveforms = true
    a.run(:all, 100000)
  end
end
