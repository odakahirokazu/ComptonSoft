# comptonsoft/simulation.rb
#
# @author Hirokazu Odaka, Yuto Ichinohe
#

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
  # @date 2020-04-03 | H. Odaka | support multiple channel properties database
  #
  class Simulation < ANL::ANLApp
    def initialize()
      super

      ### Basic settings
      @minimal_mode = false

      ### Input files
      @detector_configuration = nil # "detector_configuration.xml"
      @detector_parameters = nil    # "detector_parameters.xml"
      @channel_map = nil            # "channel_map.xml"
      @channel_properties_list = [] # "channel_properties.xml"

      ### Output settings
      @output = "output.root"
      @detector_info_verbose_level = 0

      ### Geant4 settings
      @num_events_per_run = 1000
      @num_threads = 0
      @print_beamon_time = false
      @random_seed = 0
      @verbose = 0
      @store_trajectory = false
    end

    ### Basic settings
    attr_accessor :minimal_mode

    ### Output files
    attr_accessor :output

    ### Geant4 settings
    attr_accessor :num_events_per_run, :num_threads, :print_beamon_time, :random_seed, :verbose

    ### ANL module setup.
    define_setup_module("event_store")
    define_setup_module("geometry")
    define_setup_module("physics", :PhysicsListManager)
    define_setup_module("primary_generator")
    define_setup_module("user_action")
    define_setup_module("pickup_data", array: true)
    define_setup_module("make_detector_hits")
    define_setup_module("event_selection")
    define_setup_module("tree_output")
    define_setup_module("visualization", :VisualizeGeometry)
    define_setup_module("fits_output")

    # Set database files
    def set_database(detector_configuration:,
                     detector_parameters:,
                     channel_map: nil,
                     channel_properties: nil)
      @detector_configuration = detector_configuration
      @detector_parameters = detector_parameters
      @channel_map = channel_map
      if channel_properties
        @channel_properties_list << channel_properties
      end
    end

    def add_channel_properties(filename)
      @channel_properties_list << filename
    end

    # Enable detector info print
    def print_detector_info(level=1)
      @detector_info_verbose_level = level
    end

    # Enable event tree output instead of hit tree
    def use_tree_format(format, parameters={})
      if format == "hittree" or format == "hit tree"
        set_tree_output(:WriteHitTree, parameters)
      elsif format == "eventtree" or format == "event tree"
        set_tree_output(:WriteEventTree, parameters)
      else
        raise "Unknown tree format: #{format}"
      end
    end

    # Enable visualization.
    #
    def visualize(params={})
      self.console = false
      set_visualization()
      with(params)
      self.minimal_mode = true
      @store_trajectory = true
    end

    # Set GDML file for geometry building.
    # When you use GDML, you need to call this method instead of set_geometry().
    # @param [String] gdml_file GDML file name
    # @param [Bool] validate Validate the GDML file?
    def set_gdml(gdml_file, validate=false)
      set_geometry :ReadGDML
      with(filename: gdml_file, validate: validate)
    end

    def enable_timing_process()
      set_make_detector_hits :MakeDetectorHitsWithTimingProcess
    end

    def record_raw_hits()
      set_make_detector_hits :MakeRawHits
    end

    def setup()
      add_namespace ComptonSoft
      prepare_default_modules()
      setup_detector_construction() unless minimal_mode()
      setup_geant4()
      setup_postprocesses() unless minimal_mode()
    end

    def prepare_default_modules()
      set_event_store :CSEventStore unless module_of_event_store()
      set_physics unless module_of_physics()
      set_user_action :StandardUserActionAssembly unless module_of_user_action()
      set_make_detector_hits :MakeDetectorHits unless module_of_make_detector_hits()
      set_event_selection :EventSelection unless module_of_event_selection()
      set_tree_output(:WriteHitTree) unless module_of_tree_output()
    end

    def setup_detector_construction()
      chain :ConstructDetectorForSimulation
      with_parameters(detector_configuration: @detector_configuration,
                      detector_parameters: @detector_parameters,
                      verbose_level: @detector_info_verbose_level)

      if @channel_map
        chain :ConstructChannelMap
        with_parameters(filename: @channel_map)
      end

      @channel_properties_list.each_with_index do |channel_properties, i|
        chain :SetChannelProperties, "SetChannelProperties_#{i}"
        with_parameters(filename: channel_properties)
      end
    end

    def setup_geant4()
      chain_with_parameters module_of_event_store
      chain_with_parameters module_of_geometry
      chain_with_parameters module_of_physics
      chain_with_parameters module_of_primary_generator
      chain_with_parameters module_of_user_action

      if pickup_list = module_list_of_pickup_data
        pickup_list.each{|m| chain_with_parameters(m) }
      end

      chain :Geant4Body
      with_parameters(num_events: @num_events_per_run,
                      num_threads: @num_threads,
                      print_beamon_time: @print_beamon_time,
                      random_engine: "MixMaxRng",
                      random_seed: @random_seed,
                      verbose: @verbose,
                      store_trajectory: @store_trajectory)

      if vis = module_of_visualization
        chain_with_parameters vis
      end
    end

    def setup_postprocesses()
      chain :SaveData
      with_parameters(output: @output)

      chain :CSHitCollection

      chain_with_parameters module_of_make_detector_hits

      unless module_of_make_detector_hits.module_class == :MakeRawHits
        chain_with_parameters module_of_event_selection
      end

      chain_with_parameters module_of_tree_output

      if fits_output = module_of_fits_output
        chain_with_parameters fits_output
      end
    end
  end

  # Simulation class for observation
  #
  class SimulationOfObservedSystem < Simulation
    def initialize()
      super
      self.minimal_mode = true
      @record_primaries = true
      @particle_selection = []
    end

    attr_accessor :record_primaries, :particle_selection

    def observe_gamma_emissions()
      @record_primaries = false
      @particle_selection = [22] # gamma: 22
    end

    def setup()
      set_event_store :ObservationEventStore

      add_pickup_data :ObservationPickUpData, {
        record_primaries: @record_primaries,
        particle_selection: @particle_selection
      }

      super

      chain(:SaveData)
      with_parameters(output: self.output)

      chain(:WriteObservationTree)
    end
  end

  # Simulation class for radioactivation step 1
  #
  class RadioactivationSimulationStep1 < Simulation
    def setup()
      set_event_store :RadioactivationEventStore

      set_user_action :RadioactivationUserActionAssembly, {
        output_filename_base: output.sub(".root", ".act"),
        detection_by_generation: false,
        processes_to_detect: ["RadioactiveDecay"],
      }

      super
    end
  end

end # module ComptonSoft
