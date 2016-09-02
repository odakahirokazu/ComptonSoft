#! /usr/bin/env ruby

require 'comptonsoft/basic'

def run_simulation(num)
  sim = ComptonSoft::Simulation.new
  sim.random_seed = 0
  sim.set_gdml "database/mass_model.gdml"
  sim.set_primary_generator :PrimaryGenUniformSourceInVolume, {
    particle: "gamma",
    spectral_distribution: "mono",
    energy_mean: 122.1,
    energy_sigma: 0.0,
    volume_hierarchy: ["CdTeDetector"]
  }

  sim.visualize(mode: 'OGLSQt')
  sim.run(num)
end

### main ###
num = 500
run_simulation(num)
