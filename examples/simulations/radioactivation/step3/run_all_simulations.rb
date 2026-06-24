#! /usr/bin/env ruby

require 'comptonsoft/radioactivation'

def run_step3_simulations(volume_name, volume_path, ri_list)
  ri_list.each do |ri|
    logfile = "simulation_decay_%s_%014d.log" % [volume_name, ri.id]
    output = "simulation_decay_%s_%014d.root" % [volume_name, ri.id]
    number = ri.value
    random = 0
    cmd = "ruby ./run_simulation.rb --number #{number} --random #{random} --output #{output} --isotope #{ri.id} --volume #{volume_path}"
    system cmd
  end
end

def make_ri_list(decay_rates_file, volume_path, time, number_threshold)
  summary = ComptonSoft::ActivationSummary.new
  summary.read(decay_rates_file, as_rate: true)
  ri_list = summary.get_list(volume_path)
  ri_list.each{|ri| ri.value = (ri.value*time).round }
  ri_list.reject!{|ri| ri.value < number_threshold }
  ri_list.sort!{|a, b| a.value <=> b.value }
  ri_list
end

def save_run_list(ri_list, filename)
  File.open(filename, 'w') do |fout|
    ri_list.each{|ri| fout.puts "%14d %15d"%[ri.id, ri.value] }
  end
end

### Parameters
decay_rates_file = "../step2/decay_rates.dat"
volume = 'CdTeWafer'
volume_path = '/World.log_PV/CdTeDetector'
time = 1000000.0 # s
number_threshold = 100.0

### Main
ri_list = make_ri_list(decay_rates_file, volume_path, time, number_threshold)
save_run_list(ri_list, "run_list.dat")
run_step3_simulations(volume, volume_path, ri_list)
