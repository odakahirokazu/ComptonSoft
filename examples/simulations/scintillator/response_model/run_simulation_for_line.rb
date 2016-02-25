#! /usr/bin/env ruby
require 'comptonsoft/basic'

def run_simulation(num, random, output)
  energy_min = 50.0 # keV
  energy_max = 20000.0 # keV

  sim = ComptonSoft::Simulation.new
  sim.output = output
  sim.random_seed = random
  sim.verbose = 0
  sim.print_detector_info
  # sim.use_event_tree

  sim.detector_config = "database/detector_configuration.xml"
  sim.simulation_param = "database/simulation_parameters.xml"
  sim.analysis_param = "database/analysis_parameters.xml"
  sim.use_gdml "database/mass_model.gdml"

  sim.set_physics(hadron_hp: false, cut_value: 0.001)

  sim.set_primary_generator :PlaneWaveRectanglePrimaryGen, {
    particle: "gamma",
    spectral_distribution: "mono",
    energy_mean: 2200.0,
    energy_sigma: 220.0,
    position: vec(0.0, 0.0, 10.0),
    direction: vec(0.0, 0.0, -1.0),
    size_x: 5.0,
    size_y: 5.0,
    roll_angle: 0.0,
  }

  sim.run(num)
end

### main ###

num = 1000000
runs = (1..4).to_a
a = ANL::ParallelRun.new
a.num_processes = 8
a.set_log "simulation_line_%03d.log"
a.run(runs) do |run_id|
  random = run_id
  output = "simulation_line_%03d.root" % run_id
  run_simulation(num, random, output)
end
