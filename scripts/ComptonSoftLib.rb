# ComptonSoftLib.rb
#
# @author Hirokazu Odaka, Yuto Ichinohe
#

require 'comptonSoft'
require 'anlGeant4'
require 'ANLLib'

module ComptonSoft
  include AnlGeant4

  # Calculate noise parameter 1 from energy resolution ratio (Delta E/E) at energy E.
  #
  # @param [Float] energy energy [keV]
  # @param [Float] ratio ratio of energy resolution to energy, Delta E over E
  # @return [Float] noise parameter 1
  #
  def noise_param1(energy, ratio)
    ratio * Math.sqrt(energy)
  end
  module_function :noise_param1


  module SimulationMode
    Minimal = 0
    Basic = 1
    Normal = 2
  end


  # Simulation application class inherited from ANLApp
  #
  # @author Yuto Ichinohe, Hirokazu Odaka
  #
  class Simulation < ANL::ANLApp
    ### Mode
    attr_accessor :mode

    ### Input files
    attr_writer :detector_config, :channel_table, :detector_group
    attr_writer :simulation_param

    ### Output files
    attr_accessor :output

    ### Geant4 setting
    attr_writer :random_seed, :verbose, :physics, :cut_value

    ### Analysis parameters
    attr_writer :analysis_param
    attr_writer :enable_veto, :fluorescence_range

    ### SimX parameters
    attr_accessor :pointing_ra, :pointing_dec, :exposure
    attr_accessor :flux
    attr_accessor :source_type, :source_point_ra, :source_point_dec
    attr_accessor :source_image_file
    attr_accessor :source_spectrum_type, :source_spectrum_file, :mono_energy
    attr_accessor :instrument_name, :filter_name
    attr_accessor :scale_background

    def initialize()
      ### mode
      @mode = SimulationMode::Normal

      ### Modules
      @simx = nil

      ### Input files
      @detector_config = "detector_config.xml"
      @channel_table = nil # "channel_table.xml"
      @simulation_param = "simulation_param.xml"
      @detector_group = '0' # "detector_group.txt"

      ### Output files
      @output = "output.root"

      ### Geant4 settings
      @random_seed = 0
      @verbose = 0
      @physics = ""
      @cut_value = 0.0001

      ### analysis settings
      @analysis_param = nil # "analysis_param.xml"
      @enable_veto = true
      @fluorescence_range = 1.5 # keV
      @compton_mode = false

      ### simx settings
      @pointing_ra = 0.0
      @pointing_dec = 0.0
      @exposure = 100000.0
      @flux = 1.0e-9
      @source_type = 'Point'
      @source_point_ra = 0.0
      @source_point_dec = 0.0
      @source_image_file = 'image.fits'
      @source_spectrum_type = 'XSPEC_File'
      @source_spectrum_file = 'model.qdp'
      @mono_energy = 6.4
      @instrument_name = 'HXI'
      @filter_name = 'None'
      @scale_background = 0.0

      super
    end

    ### ANL module setup.
    define_setup_module("geometry", take_parameters: true)
    define_setup_module("primary_generator", take_parameters: true)
    define_setup_module("pickup_data", take_parameters: true)
    define_setup_module("visualization", :VisualizeG4Geom)

    # Enable visualization.
    #
    def visualize(params={})
      self.thread_mode = false
      self.mode = SimulationMode::Minimal
      set_visualization()
      with(params)
    end

    # Use GDML file for geometry building.
    # When you use GDML, you need to call this method instead of set_geometry().
    # @param [String] gdml_file GDML file name
    # @param [Bool] validate Validate the GDML file?
    def use_gdml(gdml_file, validate=true)
      set_geometry :ReadGDML
      with(file: gdml_file, validate: validate)
    end

    def simx_on(simx=nil)
      if simx
        @simx = simx
      else
        @simx = ComptonSoft::SimXIF.new
      end
      return @simx
    end

    def get_simx_module() @simx end

    def compton_mode_on()
      @compton_mode = true
    end

    def setup_simx()
      soft = 'simx'
      pset_command soft, 'OutputFileName', @output.sub('.root', '')
      pset_command soft, 'PointingRA', @pointing_ra.to_s
      pset_command soft, 'PointingDec', @pointing_dec.to_s
      pset_command soft, 'Exposure', @exposure.to_s
      pset_command soft, 'SourceFlux', @flux.to_s
      pset_command soft, 'SourceImageType', @source_type
      pset_command soft, 'SourcePointRA', @source_point_ra.to_s
      pset_command soft, 'SourcePointDec', @source_point_dec.to_s
      pset_command soft, 'SourceImageFile', @source_image_file
      pset_command soft, 'SourceSpectrumType', @source_spectrum_type
      pset_command soft, 'SourceSpectrumFile', @source_spectrum_file
      pset_command soft, 'MonoEnergy', @mono_energy.to_s
      pset_command soft, 'MissionName', 'Astro-H'
      pset_command soft, 'InstrumentName', @instrument_name
      pset_command soft, 'FilterName', @filter_name
      pset_command soft, 'ScaleBkgnd', @scale_background
      simx_on @simx
    end

    def push_simx()
      push @simx if @simx
    end

    def setup()
      add_namespace ComptonSoft
      set_pickup_data :StandardPickUpData

      if mode >= SimulationMode::Normal
        chain :SaveData
        with_parameters(output: @output)
      end

      if mode >= SimulationMode::Basic
        chain :CSHitCollection
        chain :ConstructDetector_Sim
        with_parameters(detector_configuration: @detector_config,
                        simulation_parameters: @simulation_param)
        if @channel_table
          chain :ConstructChannelTable
          with_parameters(filename: @channel_table)
        end
        chain :SetNoiseLevel
      end

      chain_with_parameters module_of_geometry
      chain :AHStandardANLPhysicsList
      with_parameters(physics_option: @physics,
                      cut_value: @cut_value)
      push_simx()
      chain_with_parameters module_of_primary_generator
      chain :Geant4Body
      with_parameters(random_engine: "MTwistEngine",
                      random_initialization_mode: 1,
                      random_seed: @random_seed,
                      random_initial_status_file: @output.gsub(/.root/, "")+"_seed_I.txt",
                      random_final_status_file: @output.gsub(/.root/, "")+"_seed_F.txt",
                      verbose: @verbose)

      if mode >= SimulationMode::Basic
        chain :MakeDetectorHit
        chain :EventSelection
        with_parameters(detector_group: @detector_group,
                        apply_veto: @enable_veto,
                        tolerance_for_fluorescence_lines: @fluorescence_range)
        if @compton_mode
          chain :EventReconstruction
          with_parameters(detector_group: @detector_group,
                          maximum_hits_for_analysis: 3,
                          total_energy_cut: false,
                          energy_min: 0.0,
                          energy_max: 10000.0,
                          source_distant: true,
                          source_direction_x: 0.0,
                          source_direction_y: 0.0,
                          source_direction_z: 1.0)
        end
        if mode >= SimulationMode::Normal
          chain :HitTree_Sim
        end
      end
      chain_with_parameters module_of_pickup_data

      if vis = module_of_visualization
        chain_with_parameters vis
      end

      if mode >= SimulationMode::Normal
        if @analysis_param
          load_analysis_param(@analysis_param)
        end
      end
    end

    def load_analysis_param(file)
      xmldoc = REXML::Document.new(File.open(file))
      xmldoc.elements.each("analysis/detectors/detector") do |elem|
        name = elem.elements["name"].text
        type = elem.elements["type"].text.to_i

        noise_level = elem.elements["noiselevel"]
        if noise_level
          get_noise_level = lambda do |key|
            if n = noise_level.elements[key]
              n.text.to_f
            else
              0.0
            end
          end
          set_parameters :SetNoiseLevel
          insert_map "noise_level_map", name, {
            detector_type: type,
            noise_coefficient_00: get_noise_level.("param00"),
            noise_coefficient_01: get_noise_level.("param01"),
            noise_coefficient_02: get_noise_level.("param02"),
            noise_coefficient_10: get_noise_level.("param10"),
            noise_coefficient_11: get_noise_level.("param11"),
            noise_coefficient_12: get_noise_level.("param12")
          }
        end

        analysis = elem.elements["analysis"]
        if analysis
          m1 = {
            detector_type: type,
            analysis_mode: analysis.elements["mode"].text.to_i,
          }
          if n = analysis.elements["threshold"]; m1["threshold"] = n.text.to_f; end
          if n = analysis.elements["threshold_cathode"]; m1["threshold_cathode"] = n.text.to_f; end
          if n = analysis.elements["threshold_anode"]; m1["threshold_anode"] = n.text.to_f; end

          set_parameters :MakeDetectorHit
          insert_map "analysis_map", name, m1
        end
      end
    end

    private

    def pset_command(soft, key, value)
      system "pset #{soft} #{key}=#{value}"
    end

  end


  # SimX interface application to generate input file for raytracing code
  #
  class Simx2Xrrt < Simulation
    def initialize
      super
      @area = PI * (22.6**2 - 5.65**2),
      simx_on()
    end

    attr_writer :area

    def setup()
      push_simx()
      chain :OutputSimXPrimaries

      set_parameters :OutputSimXPrimaries, {
        filename: self.output.sub(".root", "")+"_incident_photons.fits",
        area: @area,
      }
    end
  end
end # module ComptonSoft
