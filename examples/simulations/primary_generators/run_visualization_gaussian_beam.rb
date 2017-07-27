#! /usr/bin/env ruby

require 'comptonsoft'

def run_simulation(num)
  energy = 122.1 # keV

  sim = ComptonSoft::Simulation.new
  sim.random_seed = 0
  sim.set_gdml "database/mass_model.gdml"
  sim.set_primary_generator :GaussianBeamPrimaryGen, {
    particle: "gamma",
    spectral_distribution: "mono",
    energy_mean: energy,
    energy_sigma: 0.0,
    position: vec(0.0, 0.0, 10.0),
    direction: vec(0.0, 0.0, -1.0),
    radial_sigma: 0.5,
  }

  sim.visualize(mode: 'OGLSQt')
  sim.run(num)
end

### main ###
num = 200
run_simulation(num)
