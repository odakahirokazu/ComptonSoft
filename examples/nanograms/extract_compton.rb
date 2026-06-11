#!/usr/bin/env ruby

require 'comptonsoft'

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
    with_parameters(reconstruction_method: "NanoGRAMS",
                    source_distant: false,
                    source_position: vec(50.0, 0.0, 0.0),
                    parameter_file: "parfile_NanoGRAMS.yaml")
    chain :WriteComptonEventTree
    chain :SaveData
    with_parameters(output: @output)
  end
end


### main ###
data_directory = "data/2025_10_01_02_33_05"

a = MyApp.new
a.inputs = ["#{data_directory}/hittree.root"]
a.output = "#{data_directory}/cetree.root"
 
a.run(:all, 1000)
