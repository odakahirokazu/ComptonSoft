# comptonsoft/analysis.rb
#
# @author Hirokazu Odaka
#

module ComptonSoft
  module FrameAnalyzerBase
    def initialize()
      super
      @inputs = []
      @output = "output.root"
      @pedestal_level = 0.0
      @event_threshold = 100.0
      @split_threshold = 10.0
      @hotpix_threshold = 400.0
      @pedestal_file = "pedestals.fits"
      @hotpix_file = "hotpix.txt"
      @num_pixel_x = 1
      @num_pixel_y = 1
    end

    attr_accessor :inputs, :output
    attr_accessor :pedestal_level, :event_threshold
    attr_accessor :split_threshold
    attr_accessor :hotpix_threshold
    attr_accessor :pedestal_file, :hotpix_file
    attr_accessor :num_pixel_x, :num_pixel_y

    def define_pixels(num_x, num_y)
      @num_pixel_x = num_x
      @num_pixel_y = num_y
    end
  end

  module DirectoryInput
    def initialize()
      super
      @_directory_input_data_dir = "."
      @_directory_input_extenstion = ".raw"
      @_directory_input_delay = 5
      @_directory_input_wait = 30
    end

    def set_directory_input(data_dir,
                            extension: ".raw",
                            delay: 5,
                            wait: 30)
      @_directory_input_data_dir = data_dir
      @_directory_input_extenstion = extension
      @_directory_input_delay = delay
      @_directory_input_wait = wait
    end

    def chain_directory_input(app)
      app.chain :GetInputFilesFromDirectory
      app.with_parameters(reader_module: "LoadFrame",
                          directory: @_directory_input_data_dir,
                          extension: @_directory_input_extenstion,
                          delay: @_directory_input_delay,
                          wait: @_directory_input_wait)
    end
  end

  class DarkFrameAnalyzer < ANL::ANLApp
    include FrameAnalyzerBase
    include DirectoryInput

    def setup()
      add_namespace ComptonSoft

      chain :ConstructFrame
      with_parameters(num_pixel_x: @num_pixel_x,
                      num_pixel_y: @num_pixel_y)

      if @inputs.empty?
        chain_directory_input(self)
      end

      chain :LoadFrame
      with_parameters(files: @inputs)
      chain :AnalyzeDarkFrame
      with_parameters(pedestal_level: @pedestal_level,
                      event_threshold: @event_threshold,
                      hotpix_threshold: @hotpix_threshold)
      chain :MakePedestals
      with_parameters(filename: @pedestal_file)
      chain :MakeHotPixels
      with_parameters(filename: @hotpix_file)
    end
  end

  class XrayEventAnalyzer < ANL::ANLApp
    include FrameAnalyzerBase
    include DirectoryInput

    attr_writer :coded_apertures ## to be removed

    begin
      require "HSQuickLook"
      define_setup_module("mdb_client", HSQuickLook::MongoDBClient)
    rescue LoadError
      define_setup_module("mdb_client")
    end

    def append_analysis_modules()
    end

    def setup()
      add_namespace ComptonSoft

      if module_of_mdb_client()
        chain_with_parameters module_of_mdb_client
      end

      chain :XrayEventCollection
      chain :ConstructFrame
      with_parameters(num_pixel_x: @num_pixel_x,
                      num_pixel_y: @num_pixel_y)

      if @inputs.empty?
        chain_directory_input(self)
      end

      chain :LoadFrame
      with_parameters(files: @inputs)
      chain :AnalyzeFrame
      with_parameters(pedestal_level: @pedestal_level,
                      event_threshold: @event_threshold,
                      split_threshold: @split_threshold,
                      event_size: 5,
                      set_gain: false,
                      trim_size: 0)
      chain :SetPedestals
      with_parameters(filename: @pedestal_file)
      chain :SetHotPixels
      with_parameters(filename: @hotpix_file)
      chain :WriteXrayEventTree

      append_analysis_modules()

      chain :SaveData
      with_parameters(output: @output)
    end
  end

  class XrayEventAnalyzerFromSimulation < ANL::ANLApp
    include FrameAnalyzerBase

    def append_analysis_modules()
    end

    def setup()
      add_namespace ComptonSoft

      chain :CSHitCollection
      chain :ReadHitTree
      with_parameters(file_list: @inputs, trust_num_hits: false)

      chain :XrayEventCollection
      chain :ConstructFrame
      with_parameters(num_pixel_x: @num_pixel_x,
                      num_pixel_y: @num_pixel_y)
      chain :FillFrame
      chain :AnalyzeFrame
      with_parameters(pedestal_level: @pedestal_level,
                      event_threshold: @event_threshold,
                      split_threshold: @split_threshold,
                      event_size: 5,
                      set_gain: false,
                      trim_size: 0)
      chain :WriteXrayEventTree

      append_analysis_modules()

      chain :SaveData
      with_parameters(output: @output)
    end
  end
end # module ComptonSoft
