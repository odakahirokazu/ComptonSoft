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


  # Simulation application class inherited from ANLApp
  #
  # @author Yuto Ichinohe, Hirokazu Odaka
  #
  class Simulation < ANL::ANLApp
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
      @basic_mode = false

      ### Modules
      @geometry = nil
      @primary_generator = nil
      @pickup_data = ANL::ModuleInitializer.new(:StandardPickUpData)
      @simx = nil
      @vis = nil

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

    # Set geometry builder to this simulator.
    # This method should be called before running this simulation.
    # @param [Symbol] mod symbol of a primary generator module
    # @param [Hash] params parameter list of this module
    def set_geometry(mod, params={})
      @geometry = ANL::ModuleInitializer.new(mod, nil, param)
    end

    # Use GDML file for geometry building.
    # When you use GDML, you need to call this method instead of set_geometry().
    # @param [String] gdml_file GDML file name
    # @param [Bool] validate Validate the GDML file?
    def use_gdml(gdml_file, validate=true)
      @geometry = ANL::ModuleInitializer.new(:ReadGDML)
      @geometry.with_parameters("Detector geometry file" => gdml_file,
                                "Validate GDML?" => validate)
    end

    # Set primary generator to this simulator.
    # This method should be called before running this simulation.
    # @param [Symbol] mod symbol of a primary generator module
    # @param [Hash] params parameter list of this module
    def set_primary_generator(mod, params)
      @primary_generator = ANL::ModuleInitializer.new(mod, nil, params)
      if mod.to_s.include?("Pol")
        @physics += " PC"
      end
    end

    # Set pickup data module to this simulator.
    # @param [Symbol] mod symbol of a pickup data module
    # @param [Hash] params parameter list of this module
    def set_pickup_data(mod, params={})
      @pickup_data = ANL::ModuleInitializer.new(mod, nil, params)
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

    def visualize(params={})
      self.thread_mode = false
      @vis = ANL::ModuleInitializer.new(:VisualizeG4Geom, nil, params)
    end

    def basic_mode_on()
      @basic_mode = true
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

      unless @basic_mode
        chain :SaveData
        with_parameters("Output file" => @output)
      end
      chain :CSHitCollection

      chain :ConstructDetector_Sim
      with_parameters("Detector configuration file" => @detector_config,
                      "Simulation parameter file" => @simulation_param)

      if @channel_table
        chain :ConstructChannelTable
        with_parameters("Detector channel table file" => @channel_table)
      end

      chain :SetNoiseLevel
      with_parameters("Set by file?" => false)

      chain_with_parameters @geometry

      chain :AHStandardANLPhysicsList
      with_parameters("Physics option" => @physics,
                      "Cut value" => @cut_value)
      push_simx()

      chain_with_parameters @primary_generator

      chain :Geant4Body
      with_parameters("Random engine" => "MTwistEngine",
                      "Random initialization mode" => 1,
                      "Random seed" => @random_seed,
                      "Random initial status file" => @output.gsub(/.root/, "")+"_seed_I.txt",
                      "Random final status file" => @output.gsub(/.root/, "")+"_seed_F.txt",
                      "Verbose level" => @verbose)

      chain :MakeDetectorHit

      chain :EventSelection
      with_parameters("Detector group file" => @detector_group,
                      "Remove veto events?" => @enable_veto,
                      "Range of fluorescence lines" => @fluorescence_range)

      if @compton_mode
        chain :EventReconstruction
        with_parameters("Detector group file" => @detector_group,
                        "Maximum hit number to analyze" => 3,
                        "Cut by total energy?" => false,
                        "Lower energy range" => 0.0,
                        "Upper energy range" => 10000.0,
                        "Source distant?" => true,
                        "Source direction x" => 0.0,
                        "Source direction y" => 0.0,
                        "Source direction z" => 1.0)
      end

      chain :HitTree_Sim unless @basic_mode
      chain_with_parameters @pickup_data

      if @vis
        chain_with_parameters @vis
      end

      if @analysis_param
        load_analysis_param(@analysis_param)
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
          insert_map "Noise level map", name, {
            "Detector type" => type,
            "Noise parameter 00" => get_noise_level.("param00"),
            "Noise parameter 01" => get_noise_level.("param01"),
            "Noise parameter 02" => get_noise_level.("param02"),
            "Noise parameter 10" => get_noise_level.("param10"),
            "Noise parameter 11" => get_noise_level.("param11"),
            "Noise parameter 12" => get_noise_level.("param12"),
          }
        end

        analysis = elem.elements["analysis"]
        if analysis
          m1 = {
            "Detector type" => type,
            "Analysis mode" => analysis.elements["mode"].text.to_i,
          }
          if n = analysis.elements["threshold"]; m1["Threshold"] = n.text.to_f; end
          if n = analysis.elements["threshold_cathode"]; m1["Threshold (cathode)"] = n.text.to_f; end
          if n = analysis.elements["threshold_anode"]; m1["Threshold (anode)"] = n.text.to_f; end

          set_parameters :MakeDetectorHit
          insert_map "Analysis map", name, m1
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
        "File name" => self.output.sub(".root", "")+"_incident_photons.fits",
        "Area" => @area,
      }
    end
  end
end # module ComptonSoft
