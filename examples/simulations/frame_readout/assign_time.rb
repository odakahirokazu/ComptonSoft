#!/usr/bin/env ruby

require 'comptonsoft'

class AssignEventTime < ANL::ANLApp
  attr_accessor :inputs, :output

  def set_time_info(n, t0, t1)
    @num_events = n
    @time_start = t0
    @time_end = t1
  end

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ReadHitTree
    with_parameters(file_list: @inputs)
    chain :AssignTime
    with_parameters(number_of_events: @num_events,
                    time_start: @time_start,
                    time_end: @time_end)
    chain :WriteHitTree
    chain :SaveData
    with_parameters(output: @output)
  end
end

a = AssignEventTime.new
a.inputs = ["simulation.root"]
a.output = "simulation_time.root"
a.set_time_info(1000, 1000.0, 1020.0)
a.run(:all, 1000)
