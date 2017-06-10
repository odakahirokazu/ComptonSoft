#!/usr/bin/env ruby

require "RubyROOT"

file_list = Dir.glob("sim_decay_CdTeWafer_*.root")
file_output = "spectrum.root"
exposure =  1000000.0

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
