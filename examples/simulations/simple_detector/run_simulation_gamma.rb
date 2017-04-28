#! /usr/bin/env ruby

require 'comptonsoft/basic'

def run_simulation(num, random, output)
  sim = ComptonSoft::Simulation.new
  sim.output = output
  sim.random_seed = random
  sim.verbose = 0
  sim.print_detector_info
  sim.set_database(detector_configuration: "database/detector_configuration.xml",
                   detector_parameters: "database/detector_parameters.xml")
  sim.set_gdml "database/mass_model.gdml"
  sim.set_physics(hadron_hp: false, cut_value: 0.001)

  sim.set_primary_generator :PlaneWavePrimaryGen, {
    particle: "gamma",
    photon_index: 1.0,
    energy_min: 1.0,
    energy_max: 100.0,
    position: vec(0.0, 0.0, 1.0),
    direction: vec(0.0, 0.0, -1.0),
    radius: 0.0,
  }

  sim.run(num)
end

### main ###
num = 10000000
output = "simulation_gamma.root"
random = 0
run_simulation(num, random, output)
