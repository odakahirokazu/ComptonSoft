#! /usr/bin/env ruby
require 'comptonsoft'
require 'csv'

def get_energy_photons_array(filename)
    array = CSV.read(filename)
    #model_array = array.map{|a,b,c,d,e|[a.to_f, b.to_f, c.to_f, d.to_f, e.to_f]}
    model_array = array.map{|a,b,c|[a.to_f, b.to_f, c.to_f]}
    numbin = model_array.size
    energy_min = model_array[0][0]-model_array[0][1]
    energy_array = []
    num_loop_energy = numbin+1
    num_loop_energy.times do |i|
        if i==0 then
            energy = model_array[i][0]-model_array[i][1]
        elsif i==numbin then
            energy = model_array[i-1][0]+model_array[i-1][1]
        else
            energy = 0.5*(model_array[i-1][0]+model_array[i][0])
        end
        energy_array << energy
    end
    photons_array = []
    numbin.times do |i|
        de = energy_array[i+1]-energy_array[i]
        photons = model_array[i][2]*de
        photons_array << photons
    end
    return energy_array, photons_array
end

    
def run_simulation(num, random, output)
    energy_array, photons_array = get_energy_photons_array("../tools/lif_yreg.txt")
    sim = ComptonSoft::Simulation.new
    sim.output = output
    sim.random_seed = random
    sim.verbose = 0
    sim.print_detector_info
    sim.set_database(detector_configuration: "database/detector_configuration.xml",
                   detector_parameters: "database/detector_parameters.xml")
    sim.set_gdml "database/mass_model.gdml"
    sim.set_physics(hadron_hp: false, cut_value: 0.0001,
                  customized_em: true, electron_range_ratio: 0.01, electron_final_range: 1.0e-5)
    sim.set_primary_generator :AEObservationPrimaryGen, {
        particle: "gamma",
        spectral_distribution: "histogram",
        energy_array: energy_array,
        photons_array: photons_array,
        position_offset: vec(0.0, 0.0, 1.0),
        arf_filename: "../tools/flat_dummy.arf",
        pixel_size: 0.048,#mm
        num_pixel_x: 320,
        num_pixel_y: 640,
        exposure: 21890.0,
        use_flux: false,
    }
    sim.use_tree_format("eventtree", notice_undetected: true)
  # sim.visualize
    sim.run(num)
end

### main ###
num = -1
random = 0
output = "events.root"
run_simulation(num, random, output)

