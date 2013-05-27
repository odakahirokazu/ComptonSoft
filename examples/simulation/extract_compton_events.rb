#!/usr/bin/env ruby

require 'cslib'
include ComptonSoft

infiles = ["simulation_000.root"]
outfile = "compton.root"

anl = ANLApp.new
anl.chain :CSHitCollection
anl.chain :ReadHitTree
anl.chain :EventReconstruction
anl.chain :ComptonModeFilter
anl.chain :Edep2DMap
anl.chain :CalcARM
anl.chain :ComptonTree
anl.chain :SaveData

anl.set_parameters :ReadHitTree, {
  "Hit tree file" => infiles,
}

anl.set_parameters :EventReconstruction, {
  "Detector group file" => "database/detector_group.txt",
  "Maximum hit number to analyze" => 2,
  "Cut by total energy?" => false,
  "Lower energy range" => 0.0,
  "Upper energy range" => 10000.0,
  "Source distant?" => true,
  "Source direction x" => 0.0,
  "Source direction y" => 0.0,
  "Source direction z" => 1.0,
}

anl.set_parameters :Edep2DMap, {
  "Number of bins" => 720,
  "Energy min" => 0.0,
  "Energy max" => 720.0,
}

anl.set_parameters :CalcARM, {
  "Number of bins" => 500,
  "Range min" => -25.0,
  "Range max" => 25.0,
}

anl.set_parameters :ComptonTree, {
  "Save detector detail?" => false,
}

anl.set_parameters :SaveData, {
  "Output file" => outfile,
}

anl.run(-1, 10000)
