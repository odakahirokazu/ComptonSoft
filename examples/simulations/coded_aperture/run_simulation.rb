#! /usr/bin/env ruby
require 'comptonsoft'

def run_simulation(num, random, output)
  energy = 16.0 # keV

  sim = ComptonSoft::Simulation.new
  sim.output = output
  sim.random_seed = random
  sim.verbose = 0
  sim.print_detector_info
  sim.set_database(detector_configuration: "database/detector_configuration.xml",
                   detector_parameters: "database/detector_parameters.xml")
  sim.set_gdml "mass_models/mass_model_A.gdml"
  sim.set_physics(hadron_hp: false, cut_value: 0.001,
                  customized_em: true, electron_range_ratio: 0.01, electron_final_range: 1.0e-5, polarization: true)

  sim.set_primary_generator :PlaneWavePrimaryGen, {
    particle: "gamma",
    photon_index: 0.0,
    energy_min: energy,
    energy_max: energy,
    position: vec(0.0, 0.0, 26.0),
    direction: vec(0.0, 0.0, -1.0),
    polarization_vector: vec(1.0, 0.0, 0.0),
    degree_of_polarization: 1.0,
    radius: 0.2
  }

  #sim.visualize
  sim.use_tree_format("eventtree")
  sim.run(num)
end

### main ###
num = 100000
output = "simulation.root"
random = 0
run_simulation(num, random, output)
