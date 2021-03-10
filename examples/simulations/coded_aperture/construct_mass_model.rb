#!/usr/bin/env ruby

require "rexml/document"

def read_aperture_pattern(filename)
  arr = []
  File.open(filename, mode="rt") do |f|
    f.each_line do |line|
      s = line.strip.split(/\s/).map!(&:to_i)
      arr << s
    end
  end
  return arr
end

def set_position(aperture_pattern, aperture_element_size)
  num_aperture_x = aperture_pattern.size()
  num_aperture_y = aperture_pattern[0].size()
  arr = []
  x0 = aperture_element_size*num_aperture_x*0.5
  y0 = aperture_element_size*num_aperture_y*0.5
  aperture_pattern.each_with_index do |line, i|
    now = []
    line.each_with_index do |val, j|
      x = i*aperture_element_size - x0
      y = j*aperture_element_size - y0
      x *= -1.0
      now << [x, y]
    end
    arr << now
  end
  return arr
end

def make_mask(mask_volume, aperture_pattern, aperture_position)
  aperture_pattern.each_with_index do |line, i|
    line.each_with_index do |val, j|
      if val==0 then
        next
      end
      x = aperture_position[i][j][0]
      y = aperture_position[i][j][1]
      physvol = REXML::Element.new("physvol")
      physvol_name = "Hole_#{i}_#{j}"
      physvol.add_attributes([["name", physvol_name]])
      volumeref = REXML::Element.new("volumeref")
      position = REXML::Element.new("position")
      volumeref.add_attributes([["ref", "Hole.log"]])
      position_name = physvol_name + ".pos"
      position.add_attributes([["name", position_name], ["unit", "mm"], ["x", x], ["y", y], ["z", 0.0]])
      physvol.add_element(volumeref)
      physvol.add_element(position)
      mask_volume.add_element(physvol)
    end
  end
end

def make_gdml_file(output_file, aperture_pattern_file, aperture_element_size)
  doc = REXML::Document.new(File.new("database/mass_model_base.gdml"))
  doc_mask = REXML::Document.new(File.new("database/mass_model_base_mask.gdml"))
  doc_world = REXML::Document.new(File.new("database/mass_model_base_world.gdml"))

  aperture_pattern = read_aperture_pattern(aperture_pattern_file)

  structure = doc.elements["gdml/structure"]
  mask_volume = doc_mask.elements["volume"]
  world_volume = doc_world.elements["volume"]

  aperture_position = set_position(aperture_pattern, aperture_element_size)
  make_mask(mask_volume, aperture_pattern, aperture_position)

  structure.add_element(mask_volume)
  structure.add_element(world_volume)

  File.open(output_file, 'w') do |f|
    doc.write(f, indent=2)
  end
end


### Main
ids = ["A", "B", "C", "D", "E", "F", "G", "H"]
um = 1.0E-3
aperture_element_size = 80.0*um
ids.each_with_index do |id, i|
  pattern_file = "aperture_patterns/RANDOM_pattern-ComptonSoft2D_#{id}.txt"
  output_file = "mass_models/mass_model_#{id}.gdml"
  make_gdml_file(output_file, pattern_file, aperture_element_size)
end
