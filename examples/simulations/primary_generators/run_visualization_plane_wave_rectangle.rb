#! /usr/bin/env ruby

require 'comptonsoft'

def run_simulation(num)
  sim = ComptonSoft::Simulation.new
  sim.random_seed = 0
  sim.set_gdml "database/mass_model.gdml"
  sim.set_primary_generator :PlaneWaveRectanglePrimaryGen, {
    particle: "gamma",
    photon_index: 1.5,
    energy_min: 10.0,
    energy_max: 1000.0,
    position: vec(0.0, 0.0, 10.0),
    direction: vec(0.0, 0.0, -1.0),
    size_x: 2.0,
    size_y: 0.5,
    roll_angle: 45.0*Math::PI/180.0,
  }

  sim.visualize(mode: 'OGLSQt')
  sim.run(num)
end

### main ###
num = 100
run_simulation(num)
