#!/usr/bin/env ruby

require "RubyROOT"

file_list = ["simulation_selected.root"]
file_output = "spectrum.root"
scale = (4563862.9*(13860.0-1020.0))/(128*1000000)
exposure =  (28911.0 + 10000.0)/scale

tree = Root::TChain.new("hittree", "hittree")
file_list.each do |file|
  tree.AddFile file
end

Root::TFile.open(file_output, "recreate") do |fout|
  spectrum = Root::TH1D.create("spectrum", "spectrum", 512, 0.0, 512.0)
  tree.read.each do |row|
    spectrum.Fill(row.energy)
  end
  spectrum.Sumw2
  spectrum.Scale(1.0/exposure)
  fout.Write
end
