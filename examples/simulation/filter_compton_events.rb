#!/usr/bin/env ruby
require 'ComptonSoftLib'

Degree = Math::PI / 180.0



class MyApp < ANL::ANLApp
  attr_accessor :file
  attr_accessor :emin, :emax, :thetamin, :thetamax, :emin_si, :emax_si, :emin_cdte, :emax_cdte

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ReadComptonTree
    with_parameters("Detector group file" => "database/detector_group.txt",
                    "Maximum hit number to analyze" => 2,
                    "Cut by total energy?" => false,
                    "Source distant?" => true,
                    "Source direction x" => 0.0,
                    "Source direction y" => 0.0,
                    "Source direction z" => 1.0,
                    "Compton event tree file" => [@file])
    chain :ComptonEventFilter
    with_parameters("Hit patterns" => ["Si-CdTe"]) do |m|
      m.define_condition
      m.add_condition("E1+E2", @emin, @emax)
      m.add_condition("E1", @emin_si, @emax_si)
      m.add_condition("E2", @emin_cdte, @emax_cdte)
      m.add_condition("theta K", @thetamin, @thetamax)
    end
    chain :ComptonTree
    with_parameters("Save detector detail?" => false)
    chain :SaveData
    with_parameters("Output file" => file.sub(".root", "_cut.root"))
  end
end

app = MyApp.new
app.file = "compton.root"
energy = 661.7 # keV
erange = 5.0 # keV
app.emin = energy - erange
app.emax = energy + erange
app.thetamin = 5.0 * Degree
app.thetamax = 175.0 * Degree
app.emin_si = 50.0
app.emax_si = 480.0
app.emin_cdte = 178.0
app.emax_cdte = app.emax
app.run(-1, 10000)
