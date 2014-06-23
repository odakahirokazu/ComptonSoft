#!/usr/bin/env ruby

require 'ComptonSoftLib'

class MyApp < ANL::ANLApp
  attr_accessor :inputs, :output

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ReadHitTree
    with_parameters("Hit tree file" => @inputs)
    chain :EventReconstruction
    with_parameters("Detector group file" => "database/detector_group.txt",
                    "Maximum hit number to analyze" => 2,
                    "Cut by total energy?" => false,
                    "Lower energy range" => 0.0,
                    "Upper energy range" => 10000.0,
                    "Source distant?" => true,
                    "Source direction x" => 0.0,
                    "Source direction y" => 0.0,
                    "Source direction z" => 1.0)
    chain :ComptonModeFilter
    chain :Edep2DMap
    with_parameters("Number of bins" => 720,
                    "Energy min" => 0.0,
                    "Energy max" => 720.0)
    chain :CalcARM
    with_parameters("Number of bins" => 500,
                    "Range min" => -25.0,
                    "Range max" => 25.0)
    chain :ComptonTree
    with_parameters("Save detector detail?" => false)
    chain :SaveData
    with_parameters("Output file" => @output)
  end
end

a = MyApp.new
a.inputs = ["simulation_000.root"]
a.output = "compton.root"
a.run(:all, 10000)
