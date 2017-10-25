#!/usr/bin/env ruby

require 'comptonsoft'

as = ComptonSoft::ActivationSummary.new
1.upto(16){|i| as.read("./simulation_%06d.act.summary.dat" % i) }
as.write("simulation.act.summary.dat")
