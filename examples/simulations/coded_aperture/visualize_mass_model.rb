#! /usr/bin/env ruby

require 'comptonsoft'

def run_simulation(num)
  energy = 16.0 # keV

  sim = ComptonSoft::Simulation.new
  sim.random_seed = 0
  sim.set_gdml "mass_models/mass_model_A.gdml"
  sim.set_primary_generator :PlaneWavePrimaryGen, {
    particle: "gamma",
    photon_index: 0.0,
    energy_min: energy,
    energy_max: energy,
    position: vec(0.0, 0.0, 30.0),
    direction: vec(0.0, 0.0, -1.0),
    radius: 1.0
  }

  sim.visualize(mode: 'OGLSQt')
  sim.run(num)
end

### main ###
num = 3
run_simulation(num)
