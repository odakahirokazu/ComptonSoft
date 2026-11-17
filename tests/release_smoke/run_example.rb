#!/usr/bin/env ruby

require 'comptonsoft'
require 'fileutils'

# Run an unchanged standard example with bounded statistics and isolated output.
example, output, events, threads, mode = ARGV
raise 'usage: run_example.rb EXAMPLE OUTPUT [EVENTS=1000] [THREADS=1] [standard|eventtree|raw]' unless example && output
raise 'unknown example' unless %w[pixel_detector stacked_pixel_detectors strip_detector].include?(example)

options = {
  output: File.expand_path(output), events: Integer(events || '1000'),
  threads: Integer(threads || '1'), mode: mode || 'standard'
}
raise 'unknown output mode' unless %w[standard eventtree raw].include?(options[:mode])
raise 'output already exists' if File.exist?(options[:output])
raise 'positive event and thread counts required' unless options[:events] > 0 && options[:threads] > 0
FileUtils.mkdir_p(File.dirname(options[:output]))

settings = Module.new do
  define_method(:run) do |_num, _display_period = nil|
    self.console = false
    self.output = options[:output]
    self.random_seed = 10
    self.num_threads = options[:threads]
    self.num_events_per_run = 200
    record_raw_hits if options[:mode] == 'raw'
    use_tree_format('eventtree', notice_undetected: true) unless options[:mode] == 'standard'
    write_parameters_to_json(options[:output] + '.parameters.json')
    super(options[:events], 10000)
    raise 'ANL did not finalize' unless @_anlapp_analysis_chain.finalization_done?
  end
end
ComptonSoft::Simulation.prepend(settings)

directory = File.expand_path("../../examples/simulations/#{example}", __dir__)
Dir.chdir(directory) { load './run_simulation.rb' }
