#! /usr/bin/env ruby

require 'cslib'

vis = Visualization.new
vis.use_gdml "database/mass_model.gdml"
vis.run
