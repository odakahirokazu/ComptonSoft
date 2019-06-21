#!/usr/bin/env ruby

require 'comptonsoft'

class AssignEventTime < ANL::ANLApp
  attr_accessor :inputs, :output

  def set_time_info(t0, rate)
    @time_start = t0
    @rate = rate
  end

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ReadEventTree
    with_parameters(file_list: @inputs)
    chain :AssignTime
    with_parameters(number_of_events: -1,
                    time_start: @time_start,
                    count_rate: @rate,
                    sort: true)
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

### Main
filename_base = "xray_ccd_crab"
time_start = 1000.0
rate = 200.0
frame_exposure = 4.0

a1 = AssignEventTime.new
a1.inputs = ["#{filename_base}.root"]
a1.output = "#{filename_base}_time.root"
a1.set_time_info(time_start, rate)
a1.run(:all, 1000)

a2 = DefineFrames.new
a2.inputs = [a1.output]
a2.output = "#{filename_base}_frame.root"
a2.time_start = time_start
a2.frame_exposure = frame_exposure
a2.run(:all, 1000)
