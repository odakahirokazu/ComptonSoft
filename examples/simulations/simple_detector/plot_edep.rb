#!/usr/bin/env ruby

require "RubyROOT"
include RootApp

filename = "simulation_proton.root"
n = 256
x0 = 1000.0
x1 = 100000000.0
y0 = 10.0
y1 = 10000.0

c1 = Root::TCanvas.create

ax = (0..n).map{|i| x0*(x1/x0)**(i.to_f/n) }
ay = (0..n).map{|i| y0*(y1/y0)**(i.to_f/n) }
h = Root::TH2I.create("h", "h", n, Root::DoubleArray.from_list(ax), n, Root::DoubleArray.from_list(ay))

tf = Root::TFile.open(filename)
t = tf.Get "hittree"

t.read.each do |row|
  h.Fill(row.ini_energy, row.edep)
end

h.Draw("colz")
c1.SetLogx
c1.SetLogy
c1.Update
run_app
