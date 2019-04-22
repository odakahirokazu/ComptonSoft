#!/usr/bin/env ruby

require 'comptonsoft'

class MyApp < ANL::ANLApp
  attr_accessor :inputs, :output
  attr_accessor :time_start, :frame_exposure

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ReadHitTree
    with_parameters(file_list: @inputs)
    chain :DefineFrame
    with_parameters(time_start: @time_start,
                    frame_exposure: @frame_exposure)
    chain :WriteHitTree
    chain :SaveData
    with_parameters(output: @output)
  end
end

a = MyApp.new
a.inputs = ["simulation_time.root"]
a.output = "simulation_frame.root"
a.time_start = 1000.0
a.frame_exposure = 4.0
a.run(:all, 1000)
