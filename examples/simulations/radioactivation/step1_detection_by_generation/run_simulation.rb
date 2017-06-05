#! /usr/bin/env ruby

require 'comptonsoft/basic'

def run_simulation(num, random, output)
  energy = 150000.0 # keV

  sim = ComptonSoft::Simulation.new
  sim.output = output
  sim.random_seed = random
  sim.verbose = 0
  sim.print_detector_info
  sim.set_database(detector_configuration: "../database/detector_configuration.xml",
                   detector_parameters: "../database/detector_parameters.xml")
  sim.set_gdml "../database/mass_model.gdml"
  sim.set_physics(physics_list: "QGSP_BIC_HP")

  sim.set_primary_generator :PlaneWavePrimaryGen, {
    particle: "proton",
    photon_index: 0.0,
    energy_min: energy,
    energy_max: energy,
    position: vec(0.0, 0.0, 10.0),
    direction: vec(0.0, 0.0, -1.0),
    radius: 2.0,
  }

  sim.set_pickup_data :ActivationPickUpData, {
    output_filename_base: output.sub(".root", ".act"),
    detection_by_generation: true,
    processes_to_detect: ["protonInelastic"],
  }

  sim.run(num)
end

### Main

num = 1000000
runs = (1..16).to_a

a = ANL::ParallelRun.new
a.num_processes = 4
a.set_log "simulation_%03d.log"
a.run(runs, testrun: false) do |run_id|
  output = "simulation_%03d.root" % run_id
  random = run_id
  run_simulation(num, random, output)
end
