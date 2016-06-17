#! /usr/bin/env ruby

require 'comptonsoft/basic'
require 'comptonsoft/radioactivation'

def run_simulation(num, random, output, ri, volume_path)
  sim = ComptonSoft::Simulation.new
  sim.output = output
  sim.random_seed = random

  sim.detector_config = "../step1/database/detector_configuration.xml"
  sim.simulation_param = "../step1/database/simulation_parameters.xml"
  sim.analysis_param = "../step1/database/analysis_parameters.xml"
  sim.use_gdml "../step1/database/mass_model.gdml", false

  sim.set_physics(hadron_model: "BIC",
                  hadron_hp: true,
                  radioactive_decay: true)

  sim.set_primary_generator :NucleusPrimaryGenInVolume, {
    atomic_number: ri.z,
    mass_number: ri.a,
    energy: ri.energy,
    volume_hierarchy: volume_path.split('/').drop(1),
  }

  sim.thread_mode = false
  sim.run(num)
end

def run_step3_simulations(volume_name, volume_path, ri_list)
  a = ANL::ParallelRun.new
  a.num_processes = 4
  a.set_log do |ri|
    "sim_decay_%s_%014d.log" % [volume_name, ri.id]
  end

  a.run(ri_list) do |ri|
    output = "sim_decay_%s_%014d.root" % [volume_name, ri.id]
    num = ri.value
    random = 0
    run_simulation(num, random, output, ri, volume_path)
  end
end

def print_run_list(ri_list, filename)
  File.open(filename, 'w') do |fout|
    ri_list.each{|ri| fout.puts "%14d %15d"%[ri.id, ri.value] }
  end
end

### Main

# sleep 4
decay_rates_file = "../step2/decay_rates.txt"
volume = 'CdTeWafer'
volume_path = '/World_log_PV/CdTeDetector'
time = 10000.0 # s
number_threshold = 100.0

as = ComptonSoft::ActivationSummary.new
as.read(decay_rates_file, as_rate: true)
ri_list = as.get_list(volume_path)
ri_list.each{|ri| ri.value = (ri.value*time).round }
ri_list.reject!{|ri| ri.value <= number_threshold }
ri_list.sort!{|a, b| a.value <=> b.value }

print_run_list(ri_list, "run_list.txt")
run_step3_simulations(volume, volume_path, ri_list)
