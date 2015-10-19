#! /usr/bin/env ruby

require 'comptonsoft/basic'

def run_simulation(num, random, output)
  energy = 511.0 # keV

  sim = ComptonSoft::Simulation.new
  sim.random_seed = random
  sim.use_gdml "database/mass_model.gdml"

  sim.set_primary_generator :PointSourcePrimaryGen, {
    particle: "gamma",
    photon_index: 0.0,
    energy_min: energy,
    energy_max: energy,
    position: vec(0.0, 0.0, 10.0),
    direction: vec(0.0, 0.0, -1.0),
    theta_min: 0.0,
    theta_max: 180.0*Math::PI/180.0,
  }

  sim.visualize(mode: 'OGLSQt')
  sim.run(num)
end

### main ###

num = 100
output = "vis.root"
random = 0
run_simulation(num, random, output)
