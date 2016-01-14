#!/usr/bin/env ruby

require 'comptonsoft/basic'

class MyApp < ANL::ANLApp
  attr_accessor :inputs, :output

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ConstructDetector
    with_parameters(detector_configuration: "database/detector_configuration.xml",
                    verbose_level: 1)
    chain :ReadHitTree
    with_parameters(file_list: @inputs)
    chain :EventReconstruction
    with_parameters(max_hits: 2,
                    reconstruction_method: "photoabsorption",
                    source_distant: true,
                    source_direction: vec(0.0, 0.0, 1.0))
    chain :WriteComptonEventTree
    chain :ResponseMatrix
    with_parameters(number_of_bins: 256,
                    energy_min: 0.5,
                    energy_max: 102.9,
                    event_selections: ["HitPattern:1hit", "HitPattern:2hit", "HitPattern:S", "HitPattern:C"])
    chain :SaveData
    with_parameters(output: @output)
  end
end

a = MyApp.new
a.inputs = ["simulation_000.root"]
a.output = "simulation_reconstructed.root"
a.run(:all, 10000)
