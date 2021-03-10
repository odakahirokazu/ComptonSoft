#!/usr/bin/env ruby

require 'comptonsoft'

class DecodeImage < ANL::ANLApp
  attr_accessor :inputs, :output
  attr_accessor :pixelx, :pixely
  attr_accessor :pattern_file
  
  def setup()
      add_namespace ComptonSoft
      arcsecond = 1.0/3600.0
      
      chain :XrayEventCollection
      chain :ConstructFrame
      with_parameters(num_pixels_x: @pixelx, num_pixels_y: @pixely)
      chain :ReadXrayEventTree
      with_parameters(file_list: @inputs)
      chain :ExtractXrayEventImage, "ExtractXrayEventImage"
      with_parameters(num_x: @pixelx, num_y: @pixely, image_center_x: 496.0, image_center_y: 496.0, rotation_angle: 0.0, new_origin_x: 0.0, new_origin_y: 0.0)
      chain :ProcessCodedAperture, "ProcessCodedAperture"
      with_parameters(
        aperture_num_x: 31, aperture_num_y: 31,
        sky_num_x: 101, sky_num_y: 101,
        detector_element_size: 2.5E-4, aperture_element_size: 80.0E-4,
        detector_to_aperture_distance: 22.3,
        sky_fov_x: 1000.0*arcsecond, sky_fov_y: 1000.0*arcsecond,
        decoding_mode: 1, num_decoding_iterations: 1,
        detector_roll_angle: 0.0,
        aperture_roll_angle: 0.0,
        pattern_file: @pattern_file,
        image_owner_module: "ExtractXrayEventImage",
        output_name: "decoded_image.png",
        output_angle_unit: "arcsecond")
      chain :SaveData
      with_parameters(output: @output)
  end
end

      


### Main
pixel = 992
pattern_file = "aperture_patterns/RANDOM_pattern-ComptonSoft2D_A.txt"

a1 = DecodeImage.new
a1.pixelx = pixel
a1.pixely = pixel
a1.inputs = ["simulation_xetree.root"]
a1.output = "image.root"
a1.pattern_file = pattern_file
a1.run(:all, 100)
