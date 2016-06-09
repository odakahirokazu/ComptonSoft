#!/usr/bin/env ruby

require "RubyROOT"

filename = ARGV[0]

Root::TFile.open("spectrum.root", "recreate") do |fout|
  spectrum = Root::TH1D.create("spectrum", "spectrum", 512, 0.0, 512.0)
  Root::TFile.open(filename) do |fin|
    tree = fin.Get("hittree")
    tree.read.each do |row|
      spectrum.Fill(row.energy)
    end
  end
  fout.Write
  # spectrum.SetDirectory fout
  # spectrum.Write
end
