#!/usr/bin/env ruby

require 'comptonsoft/decayChainCalculator'

production_file = "../step1/simulation.act.summary.dat"
time_profile_file = "time_profile.dat"
output_file = "decay_rates.dat"
measurement_start_time = 199950.0 # second
measurement_duration = 28911.0 # second

calculator = DecayChainCalculator::RIDecayCalculation.new
calculator.setVerboseLevel 3
calculator.setInputFiles production_file, time_profile_file
calculator.addMeasurementWindow measurement_start_time, measurement_start_time+measurement_duration
calculator.setOutputFile output_file

calculator.run
