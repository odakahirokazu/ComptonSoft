#!/usr/bin/env ruby

require 'comptonsoft'

class AssignEventTime < ANL::ANLApp
  attr_accessor :inputs, :output

  def set_time_info(t0, exposure)
    @time_start = t0
    @exposure = exposure
  end

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ReadEventTree
    with_parameters(file_list: @inputs)
    chain :AssignTime
    with_parameters(number_of_events: -1,
                    time_start: @time_start,
                    exposure: @exposure,
                    sort: true,
                    random_seed: 0)
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
  attr_accessor :event_threshold, :split_threshold
  attr_accessor :offset_x, :offset_y

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :XrayEventCollection
    chain :ReadHitTree
    with_parameters(file_list: @inputs, trust_num_hits: false)
    chain :ConstructSXIFrame
    with_parameters(num_pixels_x: 320, num_pixels_y: 640, detector_list: [1])
    chain :FillFrame
    with_parameters(offset_x: @offset_x, offset_y: @offset_y)
    chain :AnalyzeFrame
    with_parameters(event_threshold: @event_threshold, split_threshold: @split_threshold)
    chain :WriteXrayEventTree
    chain :SaveData
    with_parameters(output: @output)
  end
end



### Main
time_start = 0.0
input_file = "events.root"
exposure = 65536.0#2636.7#371.2#1889.2#2189.2#1813.6#2189.0#21890.0
frame_exposure = 4.0
filename_base = input_file.gsub(".root","")

a1 = AssignEventTime.new
a1.inputs = ["#{input_file}"]
a1.output = "#{filename_base}_time.root"
a1.set_time_info(time_start, exposure)
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
a3.event_threshold = 0.24
a3.split_threshold = 0.09
a3.offset_x = 0
a3.offset_y = 0
a3.run(:all, 1000)

#system("rm -rf #{a1.output}")
#system("rm -rf #{a2.output}")
