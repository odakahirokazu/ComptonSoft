#!/usr/bin/env ruby

require 'comptonsoft/radioactivation'

as = ComptonSoft::ActivationSummary.new
1.upto(16){|i| as.read("./activation_%03d_summary.txt" % i) }
as.write("activation_summary.txt")
