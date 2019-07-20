#!/usr/bin/env ruby

require 'comptonsoft'

Degree = Math::PI / 180.0

class MyApp < ANL::ANLApp
  attr_accessor :inputs, :output

  def setup()
    add_namespace ComptonSoft

    chain :ConstructDetector
    with_parameters(detector_configuration: "database/detector_configuration.xml")
    chain :ReadComptonEventTree
    with_parameters(file_list: inputs)
    chain :BackProjectionSky
    with_parameters(x_min: -60.0,
                    x_max: +60.0,
                    y_min: -60.0,
                    y_max: +60.0)
    chain :SaveData
    with_parameters(output: output)
  end
end

app = MyApp.new
app.inputs = ["compton_cut.root"]
app.output = "compton_image.root"
app.run(:all, 1)
