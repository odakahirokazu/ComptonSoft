#! /usr/bin/env ruby

require 'comptonsoft/basic'

def run_simulation(num)
  energy = 122.1 # keV

  sim = ComptonSoft::Simulation.new
  sim.random_seed = 0
  sim.set_gdml "database/mass_model.gdml"
  sim.set_physics(radioactive_decay: true)
  sim.set_primary_generator :NucleusPrimaryGen, {
    atomic_number: 56,
    mass_number: 133,
    energy: 0.0,
    position: vec(0.0, 0.0, 5.0),
  }

  sim.visualize(mode: 'OGLSQt')
  sim.run(num)
end

### main ###
num = 500
run_simulation(num)
