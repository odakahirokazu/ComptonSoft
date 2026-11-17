#!/usr/bin/env ruby

require 'comptonsoft'

input, output = ARGV
raise 'usage: rewrite_without_initial_info.rb INPUT OUTPUT' unless input && output
raise 'output already exists' if File.exist?(output)

app = ANL::AnalysisChain.new
app.console = false
app.add_namespace ComptonSoft
app.chain :SaveData
app.with_parameters(output: File.expand_path(output))
app.chain :CSHitCollection
app.chain :ReadEventTree
app.with_parameters(file_list: [File.expand_path(input)])
app.chain :WriteHitTree
app.chain :WriteEventTree
app.with_parameters(notice_undetected: true)
app.run(:all)
raise 'ANL did not finalize' unless app.finalization_done?
