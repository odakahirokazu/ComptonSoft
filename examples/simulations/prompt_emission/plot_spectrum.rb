#!/usr/bin/env ruby

require 'RubyROOT'
include RootApp

filename = "simulation.root"

c1 = Root::TCanvas.create
nbins = 1024; e0 = 0.0; e1 = 1024.0
s = Root::TH1I.create("spectrum", "spectrum", nbins, e0, e1)
Root::TFile.open(filename) do |tf|
  tf.Get("otree").read.each do |row|
    s.Fill row.energy
  end
end
s.Draw
c1.Update

run_app
