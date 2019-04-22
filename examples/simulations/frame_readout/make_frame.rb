#!/usr/bin/env ruby

require 'comptonsoft'

class MyApp < ANL::ANLApp
  attr_accessor :inputs, :output
  attr_accessor :time_start, :frame_exposure

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ReadHitTree
    with_parameters(file_list: @inputs,
                    trust_num_hits: false)
    chain :MakeFrame
    with_parameters(filename_base: "test", num_pixelx: 16, num_pixely: 16)
    chain :WriteHitTree
    chain :SaveData
    with_parameters(output: @output)
  end
end

a = MyApp.new
a.inputs = ["simulation_frame.root"]
a.output = "simulation_frame2.root"
a.time_start = 1000.0
a.frame_exposure = 4.0
a.run(:all, 1)
