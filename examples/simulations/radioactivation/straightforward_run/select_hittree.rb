#!/usr/bin/env ruby

require "RubyROOT"

file_range = 1..128
file_output = "simulation_selected.root"

t_irradiation = (1020.0+13860.0)/2
t_start = 199950.0 - t_irradiation
t_end = t_start + 28911.0
t_addition = 10000.0
t_start_shifted = t_start - t_addition/2
t_end_shifted = t_end + t_addition/2

chain = Root::TChain.new("hittree", "hittree")
file_range.each{|i| chain.AddFile("simulation_%03d.root"%i) }
selection = "#{t_start_shifted} <= real_time && real_time < #{t_end_shifted}"

Root::TFile.open(file_output, "recreate") do |fout|
  tree_selected = chain.CopyTree(selection)
  tree_selected.SetDirectory fout
  tree_selected.Write
end
