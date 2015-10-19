#!/usr/bin/env ruby

require 'comptonsoft/basic'

Degree = Math::PI / 180.0

class MyApp < ANL::ANLApp
  attr_accessor :inputs, :output

  def setup()
    add_namespace ComptonSoft

    chain :ConstructDetector
    with_parameters(detector_configuration: "database/detector_configuration.xml")
    chain :ReadComptonEventTree
    with_parameters(file_list: inputs)
    chain :HistogramAzimuthAngle
    with_parameters(number_of_bins: 72)
    chain :SaveData
    with_parameters(output: output)
  end
end

app = MyApp.new
app.inputs = ["compton_cut.root"]
app.output = "compton_phi.root"
app.run(:all, 10000)
