#!/usr/bin/env ruby

require "RubyROOT"

file_range = 1..100
file_output = "simulation_selected.root"

t_irradiation = 0.0
t_start = 100000.0 - t_irradiation
t_end = t_start + 100000.0
t_addition = 0.0
t_start_broad = t_start - t_addition/2
t_end_broad = t_end + t_addition/2

chain = Root::TChain.new("hittree", "hittree")
file_range.each{|i| chain.AddFile("simulation_%06d.root"%i) }
selection = "#{t_start_broad} <= real_time && real_time < #{t_end_broad}"

Root::TFile.open(file_output, "recreate") do |fout|
  tree_selected = chain.CopyTree(selection)
  tree_selected.SetDirectory fout
  tree_selected.Write
end
