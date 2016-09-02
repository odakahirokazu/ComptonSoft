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
                    source_distant: true,
                    source_direction: vec(0.0, 0.0, 1.0))
    chain :ComptonModeFilter
    chain :HistogramEnergy2D
    with_parameters(number_of_bins: 720,
                    energy_min: 0.0,
                    energy_max: 720.0)
    chain :HistogramARM
    with_parameters(number_of_bins: 500,
                    range_min: -25.0,
                    range_max: 25.0)
    chain :WriteComptonEventTree
    chain :SaveData
    with_parameters(output: @output)
  end
end

a = MyApp.new
a.inputs = ["simulation.root"]
a.output = "compton.root"
a.run(:all, 10000)
