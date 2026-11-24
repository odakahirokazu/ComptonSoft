#!/usr/bin/env ruby
#
# Input: Compton event tree (e.g. compton.root made by
#        examples/simulations/stacked_pixel_detectors/extract_compton_events.rb)

require 'comptonsoft'

class LMMLEMApp < ANL::ANLApp
  attr_accessor :detector_configuration, :inputs
  attr_accessor :input_image, :output_image, :iteration

  def setup()
    add_namespace ComptonSoft

    chain :ConstructDetector
    with_parameters(detector_configuration: @detector_configuration)
    chain :ReadComptonEventTree
    with_parameters(file_list: @inputs)
    chain :ImageReconstructionModule
    with_parameters(nside: 32,
                    image_center_theta: 0.0, # degree
                    image_center_phi: 0.0,   # degree
                    image_yaxis_theta: 90.0, # degree
                    image_yaxis_phi: 90.0,   # degree
                    input_image: @input_image,
                    sensitivity_map: "",     # empty: uniform
                    output_image: @output_image,
                    iteration: @iteration) do |m|
      # parameters of the algorithm submodule (LMMLEM)
      algorithm = m.get_submodule("algorithm")
      algorithm.set_parameter("arm", 3.0) # degree (Float)
      algorithm.set_parameter("cutoff_sigma", 3.0)
    end
  end
end

### main ###

num_iterations = 10
image_name = lambda{|k| format("lmmlem_%03d.fits", k) }

(1..num_iterations).each do |k|
  app = LMMLEMApp.new
  app.detector_configuration = "database/detector_configuration.xml"
  app.inputs = ["compton.root"]
  app.input_image = (k == 1) ? "" : image_name.(k-1) # empty: uniform
  app.output_image = image_name.(k)
  app.iteration = k
  app.run(:all, 100000)
end
