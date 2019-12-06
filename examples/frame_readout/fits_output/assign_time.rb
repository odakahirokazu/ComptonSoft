#!/usr/bin/env ruby

require 'comptonsoft'

### Main
filename_base = "xray_ccd_crab"
time_start = 1000.0
rate = 200.0
frame_exposure = 4.0

a1 = ComptonSoft::AssignEventTime.new
a1.inputs = ["#{filename_base}.root"]
a1.output = "#{filename_base}_time.root"
a1.set_time_assignment(time_start: time_start,
                       count_rate: rate,
                       sort: true)
a1.run(:all, 1000)

a2 = ComptonSoft::DefineFrames.new
a2.inputs = [a1.output]
a2.output = "#{filename_base}_frame.root"
a2.time_start = time_start
a2.frame_exposure = frame_exposure
a2.run(:all, 1000)
