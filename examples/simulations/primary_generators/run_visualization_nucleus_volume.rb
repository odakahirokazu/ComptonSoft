#! /usr/bin/env ruby

require 'comptonsoft'

def run_simulation(num)
  sim = ComptonSoft::Simulation.new
  sim_num_events_per_run = 10
  sim.random_seed = 0
  sim.set_gdml "database/mass_model.gdml"
  sim.set_physics(radioactive_decay: true)
  sim.set_primary_generator :UniformVolumePrimaryGenerator, {
    particle: "nucleus",
    nucleus_atomic_number: 56,
    nucleus_mass_number: 133,
    nucleus_excitation_energy: 0.0,
    spectral_distribution: "mono",
    energy: 0.0,
    volume_hierarchy: ["CdTeDetector"]
  }

  sim.visualize(mode: 'OGLSQt')
  sim.run(num)
end

### main ###
run_simulation(1)
