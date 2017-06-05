#!/usr/bin/env ruby
require "RubyROOT"
include RootApp

c1 = Root::TCanvas.create

f1 = Root::TFile.open("../step3/spectrum.root")
s1 = f1.Get("spectrum")
s1.Draw
s1.SetLineColor 1
s1.SetMarkerColor 1

f2 = Root::TFile.open("../straightforward_run/spectrum.root")
s2 = f2.Get("spectrum")
s2.Draw "same"
s2.SetLineColor 2
s2.SetMarkerColor 2

c1.Update
run_app
