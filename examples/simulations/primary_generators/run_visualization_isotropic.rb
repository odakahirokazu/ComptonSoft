#! /usr/bin/env ruby

require 'comptonsoft'

def run_simulation(num)
  sim = ComptonSoft::Simulation.new
  sim.num_events_per_run = 100
  sim.random_seed = 0
  sim.set_gdml "database/mass_model.gdml"
  sim.set_primary_generator :IsotropicPrimaryGen, {
    particle: "gamma",
    photon_index: 1.5,
    energy_min: 10.0,
    energy_max: 100.0,
    center_position: vec(0.0, 0.0, 0.0),
    center_direction: vec(0.0, 0.0, -1.0),
    radius: 4.0,
  }

  sim.visualize(mode: 'OGLSQt')
  sim.run(num)
end

### main ###
run_simulation(1)
