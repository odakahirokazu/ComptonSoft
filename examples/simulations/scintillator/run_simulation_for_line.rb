#! /usr/bin/env ruby
require 'comptonsoft'

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

  sim.set_primary_generator :PlaneWaveRectanglePrimaryGen, {
    particle: "gamma",
    spectral_distribution: "gaussian",
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
random = 0
output = "simulation_line.root"
run_simulation(num, random, output)
