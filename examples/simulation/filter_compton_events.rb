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
    with_parameters(detector_group: "database/detector_group.txt",
                    maximum_hits_for_analysis: 2,
                    total_energy_cut: false,
                    source_distant: true,
                    source_direction_x: 0.0,
                    source_direction_y: 0.0,
                    source_direction_z: 1.0,
                    file_list: [@file])
    chain :ComptonEventFilter
    with_parameters(hit_patterns: ["Si-CdTe"]) do |m|
      m.define_condition
      m.add_condition("E1+E2", @emin, @emax)
      m.add_condition("E1", @emin_si, @emax_si)
      m.add_condition("E2", @emin_cdte, @emax_cdte)
      m.add_condition("theta K", @thetamin, @thetamax)
    end
    chain :ComptonTree
    with_parameters(record_detector_details: false)
    chain :SaveData
    with_parameters(output: file.sub(".root", "_cut.root"))
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
