# comptonsoft/simulation.rb
#
# @author Hirokazu Odaka, Yuto Ichinohe
#

require 'comptonsoft/comptonSoft'
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

  module DetectorType
    Pad = 1
    DoubleSidedStrip = 2
    Scintillator = 3
  end

  # Simulation application class inherited from ANLApp
  #
  # @author Yuto Ichinohe, Hirokazu Odaka
  #
  class Simulation < ANL::ANLApp
    ### Mode
    attr_accessor :mode

    ### Input files
    attr_writer :detector_configuration, :channel_map
    attr_writer :simulation_parameters, :noise_levels, :bad_channels
    attr_writer :detector_info_verbose_level

    ### Output files
    attr_accessor :output

    ### Geant4 setting
    attr_writer :random_seed, :verbose

    ### Analysis parameters
    attr_writer :analysis_parameters
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
      @detector_configuration = "detector_configuration.xml"
      @channel_map = nil # "channel_map.xml"
      @simulation_parameters = "simulation_parameters.xml"
      @noise_levels = nil
      @bad_channels = nil
      @detector_info_verbose_level = 0
      @event_tree_enabled = false

      ### Output files
      @output = "output.root"

      ### Geant4 settings
      @random_seed = 0
      @verbose = 0

      ### analysis settings
      @analysis_parameters = nil # "analysis_parameters.xml"
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
    define_setup_module("physics", :PhysicsListManager, take_parameters: true)
    define_setup_module("primary_generator", take_parameters: true)
    define_setup_module("pickup_data", take_parameters: true)
    define_setup_module("visualization", :VisualizeG4Geom)
    define_setup_module("fits_output", take_parameters: true)

    # alias methods
    alias :detector_config= :"detector_configuration="
    alias :simulation_param= :"simulation_parameters="
    alias :analysis_param= :"analysis_parameters="

    # Enable detector info print
    def print_detector_info(level=1)
      @detector_info_verbose_level = level
    end

    # Enable event tree output instead of hit tree
    def use_event_tree(v=true)
      @event_tree_enabled = v
    end

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

      unless module_of_pickup_data()
        set_pickup_data :StandardPickUpData
      end

      if mode >= SimulationMode::Normal
        chain :SaveData
        with_parameters(output: @output)
      end

      if mode >= SimulationMode::Basic
        chain :CSHitCollection
        chain :ConstructDetectorForSimulation
        with_parameters(detector_configuration: @detector_configuration,
                        verbose_level: @detector_info_verbose_level,
                        simulation_parameters: @simulation_parameters)
        if @channel_map
          chain :ConstructChannelMap
          with_parameters(filename: @channel_map)
        end
        chain :SetNoiseLevels
        if @noise_levels
          with_parameters(filename: @noise_levels)
        end
        if @bad_channels
          chain :SetBadChannels
          with_parameters(filename: @bad_channels)
        end
      end

      chain_with_parameters module_of_geometry

      unless module_of_physics()
        set_physics()
      end
      chain_with_parameters module_of_physics

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
        chain :MakeDetectorHits
        chain :EventSelection
        with_parameters(enable_veto: @enable_veto)
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
          if @event_tree_enabled
            chain :WriteEventTree
          else
            chain :WriteHitTree
          end
        end
      end
      chain_with_parameters module_of_pickup_data

      if fits_output = module_of_fits_output
        chain_with_parameters fits_output
      end

      if vis = module_of_visualization
        chain_with_parameters vis
      end

      if mode >= SimulationMode::Normal
        if @analysis_parameters
          load_analysis_parameters(@analysis_parameters)
        end
      end
    end

    def load_analysis_parameters(file)
      xmldoc = REXML::Document.new(File.open(file))
      xmldoc.elements.each("analysis/detectors/detector") do |elem|
        prefix = elem.elements["prefix"].text
        type = elem.elements["type"].text.to_i

        noise_level = elem.elements["noiselevel"]
        if noise_level
          parameters = {
            detector_type: type
          }
          if type == DetectorType::DoubleSidedStrip
            parameters.merge!({
              cathode_noise_coefficient0: noise_level.elements["cathode_param0"].text.to_f,
              cathode_noise_coefficient1: noise_level.elements["cathode_param1"].text.to_f,
              cathode_noise_coefficient2: noise_level.elements["cathode_param2"].text.to_f,
              anode_noise_coefficient0: noise_level.elements["anode_param0"].text.to_f,
              anode_noise_coefficient1: noise_level.elements["anode_param1"].text.to_f,
              anode_noise_coefficient2: noise_level.elements["anode_param2"].text.to_f
            })
          else
            parameters.merge!({
              noise_coefficient0: noise_level.elements["param0"].text.to_f,
              noise_coefficient1: noise_level.elements["param1"].text.to_f,
              noise_coefficient2: noise_level.elements["param2"].text.to_f
            })
          end

          set_parameters :SetNoiseLevels
          insert_to_map "noise_level_map", prefix, parameters
        end

        reconstruction = elem.elements["reconstruction"]
        if reconstruction
          parameters = {
            detector_type: type,
            reconstruction_mode: reconstruction.elements["mode"].text.to_i,
          }
          if x = reconstruction.elements["threshold"]
            parameters[:threshold] = x.text.to_f
          end
          if x = reconstruction.elements["threshold_cathode"]
            parameters[:threshold_cathode] = x.text.to_f
          end
          if x = reconstruction.elements["threshold_anode"]
            parameters[:threshold_anode] = x.text.to_f
          end

          energy_consistency_check = reconstruction.elements["energy_consistency_check"]
          if energy_consistency_check
            lc0 = energy_consistency_check.elements["lower_function_c0"].text.to_f
            lc1 = energy_consistency_check.elements["lower_function_c1"].text.to_f
            uc0 = energy_consistency_check.elements["upper_function_c0"].text.to_f
            uc1 = energy_consistency_check.elements["upper_function_c1"].text.to_f
            parameters.merge!({
              lower_energy_consistency_check_function_c0: lc0,
              lower_energy_consistency_check_function_c1: lc1,
              upper_energy_consistency_check_function_c0: uc0,
              upper_energy_consistency_check_function_c1: uc1
            })
          end
          set_parameters :MakeDetectorHits
          insert_to_map "analysis_map", prefix, parameters
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
