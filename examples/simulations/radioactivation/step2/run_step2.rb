#!/usr/bin/env ruby

# require 'comptonsoft/radioactivation'

production_file = "../step1/simulation.act.summary.dat"
time_profile_file = "time_profile.dat"

#####################################
## measurement
## time origin 2011-05-18 00:00:00
## start 2011-05-20 07:32:30
## end   2011-05-20 15:34:21
#####################################
measurement_start_time = 199950.0 # second
measurement_duration = 28911.0 # second

output_file = "decay_rates.dat"

com = "activation_step2 #{production_file} #{time_profile_file} #{measurement_start_time} #{measurement_duration} #{output_file}"
system com
