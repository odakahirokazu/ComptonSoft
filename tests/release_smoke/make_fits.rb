#!/usr/bin/env ruby

require 'comptonsoft'

input, output_base = ARGV
raise 'usage: make_fits.rb INPUT OUTPUT_BASE' unless input && output_base
raise 'output already exists' unless Dir.glob(output_base + '_Frame*.fits').empty?

app = ANL::AnalysisChain.new
app.console = false
app.add_namespace ComptonSoft
app.chain :CSHitCollection
app.chain :ReadEventTree
app.with_parameters(file_list: [File.expand_path(input)])
app.chain :MakeFrameFITS
app.with_parameters(filename_base: File.expand_path(output_base),
                    num_pixelx: 16, num_pixely: 16)
app.run(:all)
raise 'ANL did not finalize' unless app.finalization_done?
