#!/usr/bin/env ruby

require 'comptonsoft'

class AssignEventTime < ANL::ANLApp
  attr_accessor :inputs, :output
  attr_accessor :time_start, :count_rate

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ReadEventTree
    with_parameters(file_list: @inputs)
    chain :AssignTime
    with_parameters(number_of_events: -1,
                    time_start: @time_start,
                    count_rate: @count_rate,
                    sort: true,
                    random_seed: 0,
                    randomize_time: true)
    chain :WriteEventTree
    chain :SaveData
    with_parameters(output: @output)
  end
end

class DefineFrames < ANL::ANLApp
  attr_accessor :inputs, :output
  attr_accessor :time_start, :frame_exposure

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ReadEventTree
    with_parameters(file_list: @inputs)
    chain :DefineFrame
    with_parameters(time_start: @time_start,
                    frame_exposure: @frame_exposure)
    chain :WriteHitTree
    chain :SaveData
    with_parameters(output: @output)
  end
end

class AnalyzeFrameData < ANL::ANLApp
  attr_accessor :inputs, :output
  attr_accessor :event_threshold

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :XrayEventCollection
    chain :ReadHitTree
    with_parameters(file_list: @inputs, trust_num_hits: false)
    chain :ConstructSXIFrame
    with_parameters(num_pixels_x: 992, num_pixels_y: 992)
    chain :FillFrame
    with_parameters(offset_x: 0, offset_y: 0)
    chain :AnalyzeFrame
    with_parameters(event_threshold: 1.0, split_threshold: 0.05)
    chain :WriteXrayEventTree
    chain :SaveData
    with_parameters(output: @output)
  end
end


### Main

time_start = 0.0
frame_exposure = 1.0
count_rate = 10.0
id = ARGV[0]
input_file = "simulation.root"
filename_base = input_file.gsub(".root", "")

a1 = AssignEventTime.new
a1.inputs = ["#{input_file}"]
a1.output = "#{filename_base}_time.root"
a1.count_rate = count_rate
a1.time_start = time_start
a1.run(:all, 100000)

a2 = DefineFrames.new
a2.inputs = [a1.output]
a2.output = "#{filename_base}_frame.root"
a2.time_start = time_start
a2.frame_exposure = frame_exposure
a2.run(:all, 100000)

a3 = AnalyzeFrameData.new
a3.inputs = [a2.output]
a3.output = "#{filename_base}_xetree.root"
a3.run(:all, 1000)
