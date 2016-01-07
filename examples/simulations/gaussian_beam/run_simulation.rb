#! /usr/bin/env ruby
require 'comptonsoft/basic'

def run_simulation(num, random, output)
  energy = 150.0 # keV

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

  # sim.set_physics(hadron_hp: false, cut_value: 0.001)
  sim.set_physics(hadron_hp: false, cut_value: 0.001, polarization: true)

  sim.set_primary_generator :GaussianBeamPrimaryGen, {
    particle: "gamma",
    spectral_distribution: "mono",
    energy_mean: energy,
    energy_sigma: 0.0,
    position: vec(0.0, 0.0, 10.0),
    direction: vec(0.0, 0.0, -1.0),
    radial_sigma: 1.25,
  }

  sim.run(num)
end

### main ###

num = 100000
run_id = 0
output = "simulation_%03d.root" % run_id
random = run_id
run_simulation(num, random, output)
