#!/usr/bin/env ruby

require 'ComptonSoftLib'

class MyApp < ANL::ANLApp
  attr_accessor :inputs, :output

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ReadHitTree
    with_parameters(file_list: @inputs)
    chain :EventReconstruction
    with_parameters(detector_group: "database/detector_group.txt",
                    maximum_hits_for_analysis: 2,
                    total_energy_cut: false,
                    energy_min: 0.0,
                    energy_max: 10000.0,
                    source_distant: true,
                    source_direction_x: 0.0,
                    source_direction_y: 0.0,
                    source_direction_z: 1.0)
    chain :ComptonModeFilter
    chain :Edep2DMap
    with_parameters(number_of_bins: 720,
                    energy_min: 0.0,
                    energy_max: 720.0)
    chain :CalcARM
    with_parameters(number_of_bins: 500,
                    range_min: -25.0,
                    range_max: 25.0)
    chain :ComptonTree
    with_parameters(record_detector_details: false)
    chain :SaveData
    with_parameters(output: @output)
  end
end

a = MyApp.new
a.inputs = ["simulation_000.root"]
a.output = "compton.root"
a.run(:all, 10000)
