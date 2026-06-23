#! /usr/bin/env ruby

require 'comptonsoft'

def run_simulation(num)
  sim = ComptonSoft::Simulation.new
  sim.num_events_per_run = 50
  sim.random_seed = 0
  sim.set_gdml "database/mass_model.gdml"
  sim.set_primary_generator :UniformVolumePrimaryGenerator, {
    particle: "gamma",
    spectral_distribution: "mono",
    energy_mean: 122.1,
    volume_hierarchy: ["CdTeDetector"]
  }

  sim.visualize(mode: 'OGLSQt')
  sim.run(num)
end

### main ###
run_simulation(1)
