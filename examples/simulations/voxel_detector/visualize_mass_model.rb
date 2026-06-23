#! /usr/bin/env ruby

require 'comptonsoft'

def run_simulation(num)
  energy = 1332.5 # keV

  sim = ComptonSoft::Simulation.new
  sim.num_events_per_run = 50
  sim.random_seed = 0
  sim.set_gdml "database/mass_model.gdml"
  sim.set_primary_generator :PlaneWavePrimaryGenerator, {
    particle: "gamma",
    photon_index: 0.0,
    energy_min: energy,
    energy_max: energy,
    position: vec(0.0, 0.0, 50.0),
    direction: vec(0.0, 0.0, -1.0),
    radius: 10.0
  }

  sim.visualize(mode: 'OGLSQt')
  sim.run(num)
end

### main ###
run_simulation(1)
