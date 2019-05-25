#! /usr/bin/env ruby

require 'comptonsoft'

def run_simulation(num, random, output)
  energy = 150000.0 # keV

  sim = ComptonSoft::SimulationOfObservedSystem.new
  sim.output = output
  sim.random_seed = random
  sim.set_gdml "database/mass_model.gdml"
  sim.set_primary_generator :GaussianBeamPrimaryGen, {
    particle: "proton",
    spectral_distribution: "gaussian",
    energy_mean: energy,
    energy_sigma: energy*0.01,
    position: vec(0.0, 0.0, 10.0),
    direction: vec(0.0, 0.0, -1.0),
    radial_sigma: 0.25,
  }

  sim.observe_gamma_emissions()
  # sim.visualize(mode: 'OGLSQt')
  sim.run(num)
end

### main ###
num = 100000
output = "simulation.root"
random = 0
run_simulation(num, random, output)
