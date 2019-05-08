#!/usr/bin/env ruby

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
  sim.set_physics(hadron_hp: false, cut_value: 0.0001,
                  customized_em: true, electron_range_ratio: 0.01, electron_final_range: 1.0e-5)
  sim.set_primary_generator :GaussianBeamPrimaryGen, {
    particle: "gamma",
    photon_index: 2.1,
    energy_min: 0.4,
    energy_max: 12.0,
    position: vec(0.0, 0.0, 1.0),
    direction: vec(0.0, 0.0, -1.0),
    radial_sigma: 0.0808, # corresponds to 30 arcsec
  }
  # sim.visualize
  sim.run(num)
end

### main ###
num = 10000
output = "HitomiCCD_Gaussian_Crab.root"
random = 0
run_simulation(num, random, output)
