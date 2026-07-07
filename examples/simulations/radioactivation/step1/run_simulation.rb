#! /usr/bin/env ruby

require 'anlnext'
require 'comptonsoft'

def run_simulation(num, random, output)
  energy = 150000.0 # keV

  sim = ComptonSoft::RadioactivationSimulationStep1.new
  sim.output = output
  sim.random_seed = random
  sim.verbose = 0
  sim.print_detector_info
  sim.set_database(detector_configuration: "../database/detector_configuration.xml",
                   detector_parameters: "../database/detector_parameters.xml")
  sim.set_gdml "../database/mass_model.gdml"
  sim.set_physics(physics_list: "QGSP_BIC_HP_RD")

  sim.set_primary_generator :PlaneWavePrimaryGenerator, {
    particle: "proton",
    photon_index: 0.0,
    energy_min: energy,
    energy_max: energy,
    position: vec(0.0, 0.0, 10.0),
    direction: vec(0.0, 0.0, -1.0),
    radius: 2.0,
  }

  sim.run(num)
end

### Main

num = 10000000
output = "simulation.root"
random = 12345
run_simulation(num, random, output)
