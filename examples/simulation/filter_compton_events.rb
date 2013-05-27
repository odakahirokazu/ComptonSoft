#!/usr/bin/env ruby
require 'ANLLib'

require 'cslib'

Degree = Math::PI / 180.0

energy = 661.7 # keV
erange = 5.0 # keV
emin = energy - erange
emax = energy + erange
thetamin = 5.0 * Degree
thetamax = 175.0 * Degree
emin_si = 50.0
emax_si = 480.0
emin_cdte = 178.0
emax_cdte = emax

anl = ANLApp.new
anl.chain :CSHitCollection
anl.chain :ReadComptonTree
anl.chain :ComptonEventFilter
anl.chain :ComptonTree
anl.chain :SaveData

file = "compton.root"

anl.set_parameters :ReadComptonTree, {
  "Detector group file" => "database/detector_group.txt",
  "Maximum hit number to analyze" => 2,
  "Cut by total energy?" => false,
  "Source distant?" => true,
  "Source direction x" => 0.0,
  "Source direction y" => 0.0,
  "Source direction z" => 1.0,
  "Compton event tree file" => [file],
}

anl.set_parameters :ComptonEventFilter, {
  "Hit pattern" => "Si-CdTe",
} do |m|
  m.define_condition
  m.add_condition("E1+E2", emin, emax)
  m.add_condition("E1", emin_si, emax_si)
  m.add_condition("E2", emin_cdte, emax_cdte)
  m.add_condition("theta K", thetamin, thetamax)
end

anl.set_parameters :ComptonTree, {
  "Save detector detail?" => false,
}

anl.set_parameters :SaveData, {
  "Output file" => file.sub(".root", "_cut.root"),
}

anl.run(-1, 10000)
