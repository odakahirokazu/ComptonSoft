#! /usr/bin/env ruby

require 'comptonsoft'
require 'MyGeometry'

def run_simulation(num)
  energy = 661.7 # keV

  sim = ComptonSoft::Simulation.new
  sim.random_seed = 0
  sim.set_geometry MyGeometry::MyMainGeometry
  sim.set_primary_generator :PlaneWavePrimaryGen, {
    particle: "gamma",
    photon_index: 0.0,
    energy_min: energy,
    energy_max: energy,
    position: vec(0.0, 0.0, 10.0),
    direction: vec(0.0, 0.0, -1.0),
    radius: 0.5
  }

  sim.visualize(mode: 'OGLSQt')
  sim.run(num)
end

### main ###
num = 3
run_simulation(num)
