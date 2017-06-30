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

  module DetectorType
    Pad = 1
    DoubleSidedStrip = 2
    Scintillator = 3
  end

  # Simulation application class inherited from ANLApp
  #
  # @author Hirokazu Odaka
  # @author Yuto Ichinohe
  # @date 2016-08-25 (latest) | H. Odaka
  #
  class Simulation < ANL::ANLApp
    def initialize()
      ### Basic settings
      @setup_mode = :normal

      ### Input files
      @detector_configuration = nil # "detector_configuration.xml"
      @detector_parameters = nil    # "detector_parameters.xml"
      @channel_map = nil            # "channel_map.xml"
      @channel_properties = nil     # "channel_properties.xml"

      ### Output settings
      @output = "output.root"
      @detector_info_verbose_level = 0

      ### Geant4 settings
      @random_seed = 0
      @verbose = 0

      ### Modules
      @make_detector_hits_module = :MakeDetectorHits
      @write_tree_module = :WriteHitTree

      ### SimX
      @simx_seetings = nil

      super
    end

    ### Basic settings
    attr_accessor :setup_mode

    ### Output files
    attr_accessor :output

    ### Geant4 settings
    attr_accessor :random_seed, :verbose

    ### ANL module setup.
    define_setup_module("geometry")
    define_setup_module("physics", :PhysicsListManager)
    define_setup_module("primary_generator")
    define_setup_module("user_action")
    define_setup_module("pickup_data", array: true)
    define_setup_module("event_selection")
    define_setup_module("visualization", :VisualizeG4Geom)
    define_setup_module("fits_output")

    # Set database files
    def set_database(detector_configuration:,
                     detector_parameters:,
                     channel_map: nil,
                     channel_properties: nil)
      @detector_configuration = detector_configuration
      @detector_parameters = detector_parameters
      @channel_map = channel_map
      @channel_properties = channel_properties
    end

    # Enable detector info print
    def print_detector_info(level=1)
      @detector_info_verbose_level = level
    end

    # Enable event tree output instead of hit tree
    def use_tree_format(format)
      if format == "hittree" or format == "hit tree"
        @write_tree_module = :WriteHitTree
      elsif format == "eventtree" or format == "event tree"
        @write_tree_module = :WriteEventTree
      else
        raise "Unknown tree format: #{format}"
      end
    end

    # Enable visualization.
    #
    def visualize(params={})
      self.thread_mode = false
      set_visualization()
      with(params)
      self.setup_mode = :minimal
    end

    # Set GDML file for geometry building.
    # When you use GDML, you need to call this method instead of set_geometry().
    # @param [String] gdml_file GDML file name
    # @param [Bool] validate Validate the GDML file?
    def set_gdml(gdml_file, validate=false)
      set_geometry :ReadGDML
      with(file: gdml_file, validate: validate)
    end

    def enable_timing_process(b=true)
      if b
        @make_detector_hits_module = :MakeDetectorHitsWithTimingProcess
      else
        @make_detector_hits_module = :MakeDetectorHits
      end
    end

    def setup_simx()
      @simx_seetings = SimxSettings.new
      @simx_seetings.setup_simx(@output.sub('.root', ''))
    end

    def setup()
      case @setup_mode
      when :minimal
        setup_minimal()
      when :normal
        setup_normal()
      end
    end

    def setup_normal()
      add_namespace ComptonSoft

      unless module_of_user_action()
        set_user_action :StandardUserActionAssembly
      end

      unless module_of_event_selection()
        set_event_selection :EventSelection
      end

      chain :SaveData
      with_parameters(output: @output)

      chain :CSHitCollection
      chain :ConstructDetectorForSimulation
      with_parameters(detector_configuration: @detector_configuration,
                      detector_parameters: @detector_parameters,
                      verbose_level: @detector_info_verbose_level)

      if @channel_map
        chain :ConstructChannelMap
        with_parameters(filename: @channel_map)
      end

      if @channel_properties
        chain :SetChannelProperties
        with_parameters(filename: @channel_properties)
      end

      chain_with_parameters module_of_geometry

      unless module_of_physics()
        set_physics()
      end
      chain_with_parameters module_of_physics

      if @simx_seetings
        chain :SimXIF
      end

      chain_with_parameters module_of_primary_generator
      chain :Geant4Body
      with_parameters(random_engine: "MTwistEngine",
                      random_initialization_mode: 1,
                      random_seed: @random_seed,
                      output_random_status: true,
                      random_initial_status_file: @output.sub(/.root/, "")+"_seed_i.dat",
                      random_final_status_file: @output.sub(/.root/, "")+"_seed_f.dat",
                      verbose: @verbose)

      chain @make_detector_hits_module

      chain_with_parameters module_of_event_selection

      chain @write_tree_module

      chain_with_parameters module_of_user_action

      if pickup_list = module_list_of_pickup_data
        pickup_list.each{|m| chain_with_parameters(m) }
      end

      if fits_output = module_of_fits_output
        chain_with_parameters fits_output
      end

      if vis = module_of_visualization
        chain_with_parameters vis
      end
    end

    def setup_minimal()
      add_namespace ComptonSoft

      unless module_of_user_action()
        set_user_action :StandardUserActionAssembly
      end

      chain_with_parameters module_of_geometry

      unless module_of_physics()
        set_physics()
      end
      chain_with_parameters module_of_physics

      chain_with_parameters module_of_primary_generator

      chain :Geant4Body
      with_parameters(random_engine: "MTwistEngine",
                      random_initialization_mode: 1,
                      random_seed: @random_seed,
                      output_random_status: false,
                      verbose: @verbose)

      chain_with_parameters module_of_user_action

      if pickup_list = module_list_of_pickup_data
        pickup_list.each{|m| chain_with_parameters(m) }
      end

      if vis = module_of_visualization
        chain_with_parameters vis
      end
    end
  end

  # SimX setting
  #
  class SimxSettings
    attr_accessor :pointing_ra, :pointing_dec, :exposure
    attr_accessor :flux
    attr_accessor :source_type, :source_point_ra, :source_point_dec
    attr_accessor :source_image_file
    attr_accessor :source_spectrum_type, :source_spectrum_file, :mono_energy
    attr_accessor :instrument_name, :filter_name
    attr_accessor :scale_background

    def initialize()
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
    end

    def setup_simx(output_filename)
      soft = 'simx'
      pset_command soft, 'OutputFileName', output_filename
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
    end

    def pset_command(soft, key, value)
      system "pset #{soft} #{key}=#{value}"
    end
    private :pset_command
  end

  # SimX interface application to generate input file for raytracing code
  #
  class Simx2Xrrt < Simulation
    def initialize()
      super
      @area = PI * (22.6**2 - 5.65**2)
    end
    attr_writer :area

    def setup()
      chain :SimXIF
      chain :OutputSimXPrimaries
      with_parameters(filename: self.output.sub(".root", "")+"_incident_photons.fits",
                      area: @area)
    end
  end

  # Simulation class for observation
  #
  class SimulationOfObservedSystem < Simulation
    def initialize()
      super
      self.setup_mode = :minimal
      @record_primaries = true
      @particle_selection = []
    end

    attr_accessor :record_primaries, :particle_selection

    def observe_gamma_emissions()
      @record_primaries = false
      @particle_selection = [22] # gamma: 22
    end

    def setup()
      add_pickup_data :ObservationPickUpData, {
        record_primaries: @record_primaries,
        particle_selection: @particle_selection
      }

      super
      chain(:WriteObservationTree)
      chain(:SaveData)
      with_parameters(output: self.output)
    end
  end
end # module ComptonSoft
