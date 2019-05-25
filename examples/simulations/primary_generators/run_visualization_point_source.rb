#! /usr/bin/env ruby

require 'comptonsoft'

def run_simulation(num)
  energy = 122.1 # keV

  sim = ComptonSoft::Simulation.new
  sim.random_seed = 0
  sim.set_gdml "database/mass_model.gdml"
  sim.set_primary_generator :PointSourcePrimaryGen, {
    particle: "gamma",
    spectral_distribution: "gaussian",
    energy_mean: energy,
    energy_sigma: energy*0.01,
    position: vec(0.0, 0.0, 5.0),
    direction: vec(0.0, 0.0, -1.0),
    theta_min: 0.0,
    theta_max: 45.0*Math::PI/180.0,
  }

  sim.visualize(mode: 'OGLSQt')
  sim.run(num)
end

### main ###
num = 500
run_simulation(num)
