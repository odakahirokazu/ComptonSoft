#! /usr/bin/env ruby

require 'comptonsoft'

def run_simulation(num)
  energy = 100000.0 # keV

  sim = ComptonSoft::Simulation.new
  sim.random_seed = 0
  sim.set_gdml "database/mass_model.gdml"
  sim.set_primary_generator :PlaneWavePrimaryGen, {
    particle: "nucleus",
    nucleus_atomic_number: 6,
    nucleus_mass_number: 12,
    nucleus_excitation_energy: 0.0,
    spectral_distribution: "gaussian",
    energy_mean: energy,
    energy_sigma: energy*0.01,
    position: vec(0.0, 0.0, 10.0),
    direction: vec(0.0, 0.0, -1.0),
    radius: 1.0
  }

  sim.visualize(mode: 'OGLSQt')
  sim.run(num)
end

### main ###
num = 20
run_simulation(num)
