#!/usr/bin/env ruby

require 'comptonsoft/basic'

class Reprocess < ANL::ANLApp
  attr_accessor :input_files, :output_file

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ConstructDetector
    with_parameters(detector_configuration: "../step1/database/detector_configuration.xml",
                    verbose_level: 1)
    chain :ReadHitTree
    with_parameters(file_list: @input_files)
    chain :EventReconstruction
    with_parameters(max_hits: 1,
                    reconstruction_method: "photoabsorption",
                    source_distant: true,
                    source_direction: vec(0.0, 0.0, 1.0))
    chain :HistogramEnergySpectrum
    with_parameters(number_of_bins: 512,
                    energy_min: 0.0,
                    energy_max: 512.0,
                    event_selections: ["HitPattern:1hit", "HitPattern:2hit"])
    chain :WriteHitTree
    chain :SaveData
    with_parameters(output: @output_file)
  end
end


### Main

file_list = Dir::glob("sim_decay_CdTeWafer_*.root")
app = Reprocess.new
app.input_files = file_list
app.output_file = "sim_decay_CdTeWafer_reprocessed.root"
app.run(:all, 1000000)
