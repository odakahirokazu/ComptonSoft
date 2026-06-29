#! /usr/bin/env ruby

require "optparse"
require 'comptonsoft'

def run_simulation(num, random, output, ri, volume_path)
  sim = ComptonSoft::Simulation.new
  sim.output = output
  sim.random_seed = random
  sim.verbose = 0
  sim.print_detector_info
  sim.set_database(detector_configuration: "../database/detector_configuration.xml",
                   detector_parameters: "../database/detector_parameters.xml")
  sim.set_gdml "../database/mass_model.gdml"
  sim.set_physics(hadron_model: "BIC",
                  hadron_hp: true,
                  radioactive_decay: true)

  sim.set_primary_generator :UniformVolumePrimaryGenerator, {
    particle: "nucleus",
    nucleus_atomic_number: ri.z,
    nucleus_mass_number: ri.a,
    nucleus_excitation_energy: ri.energy,
    nucleus_floating_level: ri.floating_level,
    volume_hierarchy: volume_path.split('/').drop(1),
    spectral_distribution: "mono",
    energy: 0.0,
  }

  sim.set_user_action :RadioactiveDecayUserActionAssembly, {
    termination_time: 1.0e-6,
    radioative_decay_process_name: "RadioactiveDecay"
  }

  sim.enable_timing_process()

  sim.set_event_selection :EventSelection, {
    discard_time_group_nonzero: true
  }

  sim.console = false
  sim.run(num)
end

### Main
options = {}
OptionParser.new do |opt|
  opt.banner = "Usage: run_simulation_rd_step3.rb [options]"
  opt.on("-n", "--number NUMBER", Integer, "Number of events") do |v|
    options[:number] = v
  end

  opt.on("-r", "--random RANDOM", Integer, "Random seed") do |v|
    options[:random] = v
  end

  opt.on("-o", "--output FILE", "Output file") do |v|
    options[:output] = v
  end

  opt.on("-i", "--isotope ID", Integer, "Isotope ID") do |v|
    options[:isotope] = v
  end

  opt.on("-v", "--volume VOLUME_PATH", "Volume path") do |v|
    options[:volume] = v
  end
end.parse!

p options

num = options[:number]
random = options[:random]
output = options[:output]
ri = ComptonSoft::RIData.from_isotope_id(options[:isotope])
volume_path = options[:volume]

run_simulation(num, random, output, ri, volume_path)
