#! /usr/bin/env ruby

def run_simulation(num, random, output)
  require 'ComptonSoftLib'

  energy = 661.7 # keV

  sim = ComptonSoft::Simulation.new
  sim.output = output
  sim.random_seed = random

  sim.detector_config = "database/detector_configuration.xml"
  sim.simulation_param = "database/simulation_parameters.xml"
  sim.detector_group = "database/detector_group.txt"
  sim.analysis_param = "database/analysis_parameters.xml"
  sim.use_gdml "database/mass_model.gdml"

  sim.set_primary_generator :PlaneWavePrimaryGen, {
    "Particle name" => "gamma",
    "Photon index" => 0.0,
    "Energy min" => energy,
    "Energy max" => energy,
    "Center position" => vec(0.0, 0.0, 10.0),
    "Direction" => vec(0.0, 0.0, -1.0),
    "Radius" => 10.0,
  }

  sim.thread_mode = false
  sim.visualize(Mode: 'OGLSQt')
  sim.run(num)
end

### main ###

num = 100
output = "vis.root"
random = 0
run_simulation(num, random, output)
