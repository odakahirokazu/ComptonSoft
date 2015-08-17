#! /usr/bin/env ruby

def run_simulation(num, random, output, activation_output)
  require 'ComptonSoftLib'

  energy = 150000.0 # keV

  sim = ComptonSoft::Simulation.new
  sim.output = output
  sim.random_seed = random
  sim.verbose = 1

  sim.detector_config = "database/detector_configuration.xml"
  sim.simulation_param = "database/simulation_parameters.xml"
  sim.detector_group = "database/detector_group.txt"
  sim.analysis_param = "database/analysis_parameters.xml"
  sim.use_gdml "database/mass_model.gdml"

  sim.set_physics(hadron_model: "BIC", radioactive_decay: true)

  sim.set_primary_generator :PlaneWavePrimaryGen, {
    particle: "proton",
    photon_index: 0.0,
    energy_min: energy,
    energy_max: energy,
    position: vec(0.0, 0.0, 10.0),
    direction: vec(0.0, 0.0, -1.0),
    radius: 0.5
  }

  sim.set_pickup_data :ActivationPickUpData, {
    output_filename_base: activation_output
  }

  sim.run(num)
end

### Main
require "run_parallel"

# sleep 4

num = 1000000
runs = (1..32).to_a

a = RunParallel.new
a.num_proc = 4
a.set_log{|run_id| "simulation_%03d.log" % run_id }
a.run1(runs) do |run_id|
  output = "simulation_%03d.root" % run_id
  activation_output = "activation_%03d" % run_id
  random = run_id
  run_simulation(num, random, output, activation_output)
end
