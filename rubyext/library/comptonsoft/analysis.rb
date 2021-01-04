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
      @byte_order = true
      @odd_row_pixel_shift = 0
      @start_position = 0
      @read_direction_x = false
      @pedestal_level = 0.0
      @event_threshold = 100.0
      @split_threshold = 10.0
      @pedestal_file = nil
      @channel_properties_list = [] # "channel_properties.xml"
      @num_pixels_x = 1
      @num_pixels_y = 1
    end

    attr_accessor :inputs, :output
    attr_accessor :byte_order, :odd_row_pixel_shift, :start_position, :read_direction_x
    attr_accessor :pedestal_level, :event_threshold
    attr_accessor :split_threshold
    attr_accessor :pedestal_file
    attr_accessor :num_pixels_x, :num_pixels_y

    def define_pixels(nx, ny)
      @num_pixels_x = nx
      @num_pixels_y = ny
    end

    def add_channel_properties(filename)
      @channel_properties_list << filename
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

  module QuickLookBase
    def initialize()
      super
      @_quicklookbase_module_list = []
      @_quicklookbase_canvas_width = 600
      @_quicklookbase_canvas_height = 400
      @_quicklookbase_period = 1
      @_quicklookbase_phase = 0
    end

    def set_quick_look(collection, directory, document, block_name)
      @_quicklookbase_collection = collection
      @_quicklookbase_directory = directory
      @_quicklookbase_document = document
      @_quicklookbase_block_name = block_name
    end

    def set_quick_look_period(period, phase=0)
      @_quicklookbase_period = period
      @_quicklookbase_phase = phase
    end

    def set_quick_look_canvas(width, height)
      @_quicklookbase_canvas_width = width
      @_quicklookbase_canvas_height = height
    end

    def add_quick_look_module(m)
      @_quicklookbase_module_list << m
    end

    def append_quick_look(app)
      if @_quicklookbase_collection
        app.chain :PushToQuickLookDB, @_quicklookbase_block_name
        app.with_parameters(module_list: @_quicklookbase_module_list,
                            canvas_width: @_quicklookbase_canvas_width,
                            canvas_height: @_quicklookbase_canvas_height,
                            collection: @_quicklookbase_collection,
                            directory: @_quicklookbase_directory,
                            document: @_quicklookbase_document,
                            period: @_quicklookbase_period,
                            phase: @_quicklookbase_phase)
      end
    end
  end

  class DarkFrameAnalyzer < ANL::ANLApp
    include FrameAnalyzerBase
    include DirectoryInput

    def initialize()
      super
      @bad_pixels_file = "bad_pixels.xml"
      @append_pedestal_histogram_modules = nil
      @analysis_list = []
    end
    attr_accessor :bad_pixels_file

    begin
      require "HSQuickLook"
      define_setup_module("mdb_client", HSQuickLook::MongoDBClient)
    rescue LoadError
      define_setup_module("mdb_client")
    end

    def set_statistics_exclusion_numbers(low, high)
      @num_exclusion_low = low
      @num_exclusion_high = high
    end

    def add_analysis(a)
      @analysis_list << a
    end

    def set_good_pixel_conditions(mean_min, mean_max, sigma_min, sigma_max)
      @good_pixel_mean_min = mean_min
      @good_pixel_mean_max = mean_max
      @good_pixel_sigma_min = sigma_min
      @good_pixel_sigma_max = sigma_max
    end

    def save_pedestal_histograms(mean_num_bins, mean_min, mean_max,
                                 sigma_num_bins, sigma_min, sigma_max,
                                 filename: nil)
      @output = filename if filename
      @append_pedestal_histogram_modules = lambda do |app|
        app.chain :HistogramFramePedestal
        app.with_parameters(mean_num_bins: mean_num_bins,
                            mean_min: mean_min,
                            mean_max: mean_max,
                            sigma_num_bins: sigma_num_bins,
                            sigma_min: sigma_min,
                            sigma_max: sigma_max)
      end
    end

    def setup()
      add_namespace ComptonSoft

      if module_of_mdb_client()
        chain_with_parameters module_of_mdb_client
      end

      chain :ConstructFrame
      with_parameters(num_pixels_x: @num_pixels_x,
                      num_pixels_y: @num_pixels_y)

      if @inputs.empty?
        chain_directory_input(self)
      end

      chain :LoadFrame
      with_parameters(files: @inputs,
                      byte_order: @byte_order,
                      odd_row_pixel_shift: @odd_row_pixel_shift,
                      start_position: @start_position,
                      read_direction_x: @read_direction_x)
      chain :AnalyzeDarkFrame
      with_parameters(pedestal_level: @pedestal_level,
                      num_exclusion_low: @num_exclusion_low,
                      num_exclusion_high: @num_exclusion_high,
                      good_pixel_mean_min: @good_pixel_mean_min,
                      good_pixel_mean_max: @good_pixel_mean_max,
                      good_pixel_sigma_min: @good_pixel_sigma_min,
                      good_pixel_sigma_max: @good_pixel_sigma_max)
      chain :WritePedestals
      with_parameters(filename: @pedestal_file)
      chain :WriteBadPixels
      with_parameters(filename: @bad_pixels_file)

      @analysis_list.each do |a|
        a.insert_modules(self)
      end

      if @append_pedestal_histogram_modules
        @append_pedestal_histogram_modules.(self)
      end

      chain :SaveData
      with_parameters(output: @output)
    end
  end

  class XrayEventAnalyzer < ANL::ANLApp
    include FrameAnalyzerBase
    include DirectoryInput

    def initialize()
      super
      @event_size = 5
      @trim_size = 0
      @analysis_list = []
    end
    attr_accessor :event_size, :trim_size

    begin
      require "HSQuickLook"
      define_setup_module("mdb_client", HSQuickLook::MongoDBClient)
    rescue LoadError
      define_setup_module("mdb_client")
    end

    def append_analysis_modules()
    end

    def add_analysis(a)
      @analysis_list << a
    end

    def setup()
      add_namespace ComptonSoft

      if module_of_mdb_client()
        chain_with_parameters module_of_mdb_client
      end

      chain :XrayEventCollection
      chain :ConstructFrame
      with_parameters(num_pixels_x: @num_pixels_x,
                      num_pixels_y: @num_pixels_y)

      @channel_properties_list.each_with_index do |channel_properties, i|
        chain :SetChannelProperties, "SetChannelProperties_#{i}"
        with_parameters(filename: channel_properties)
      end

      if @inputs.empty?
        chain_directory_input(self)
      end

      chain :LoadFrame
      with_parameters(files: @inputs,
                      byte_order: @byte_order,
                      odd_row_pixel_shift: @odd_row_pixel_shift,
                      start_position: @start_position,
                      read_direction_x: @read_direction_x)
      chain :AnalyzeFrame
      with_parameters(pedestal_level: @pedestal_level,
                      event_threshold: @event_threshold,
                      split_threshold: @split_threshold,
                      event_size: @event_size,
                      trim_size: @trim_size,
                      gain_correction: false)

      if @pedestal_file
        chain :SetPedestals
        with_parameters(filename: @pedestal_file)
      end

      chain :WriteXrayEventTree

      @analysis_list.each do |a|
        a.insert_modules(self)
      end

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
      with_parameters(num_pixels_x: @num_pixels_x,
                      num_pixels_y: @num_pixels_y)
      chain :FillFrame
      chain :AnalyzeFrame
      with_parameters(pedestal_level: @pedestal_level,
                      event_threshold: @event_threshold,
                      split_threshold: @split_threshold,
                      event_size: 5,
                      trim_size: 0,
                      gain_correction: false)
      chain :WriteXrayEventTree

      append_analysis_modules()

      chain :SaveData
      with_parameters(output: @output)
    end
  end

  module XrayDataEvaluationItems
    class SpectrumAndPolarization
      include QuickLookBase

      def initialize()
        super
        @name = ""
        @event_selector = {}
        @spectrum_num_bins = 512
        @spectrum_min = 0.0
        @spectrum_max = 4096.0
        @angle_num_bins = 40
        @angle_min = -210.0
        @angle_max = 210.0
        set_quick_look_canvas(600, 400)
      end
      attr_accessor :name, :event_selector

      def define_spectrum(num_bins, energy_min, energy_max)
        @spectrum_num_bins = num_bins
        @spectrum_min = energy_min
        @spectrum_max = energy_max
      end

      def define_angle(num_bins, angle_min, angle_max)
        @angle_num_bins = num_bins
        @angle_min = angle_min
        @angle_max = angle_max
      end

      def insert_modules(app)
        app.chain :XrayEventSelection, "XrayEventSelection_Basic_#{@name}_1"
        event_selector_for_spectrum = @event_selector.clone
        event_selector_for_spectrum.delete(:sumPH_min)
        event_selector_for_spectrum.delete(:sumPH_max)
        app.with_parameters() do |m|
          event_selector_for_spectrum.each do |k, v|
            m.add_condition(k.to_s, v)
          end
        end
        app.chain :HistogramXrayEventSpectrum, "HistogramXrayEventSpectrum_Basic_#{@name}_1"
        app.with_parameters(collection_module: "XrayEventSelection_Basic_#{@name}_1",
                            num_bins: @spectrum_num_bins,
                            energy_min: @spectrum_min,
                            energy_max: @spectrum_max)
        add_quick_look_module("HistogramXrayEventSpectrum_Basic_#{@name}_1")
        app.chain :XrayEventSelection, "XrayEventSelection_Basic_#{@name}_2"
        app.with_parameters() do |m|
          event_selector_for_spectrum.each do |k, v|
            m.add_condition(k.to_s, v)
          end
        end
        app.chain :HistogramXrayEventAzimuthAngle, "HistogramXrayEventAzimuthAngle_Basic_#{@name}_2"
        app.with_parameters(collection_module: "XrayEventSelection_Basic_#{@name}_2",
                            num_bins: @angle_num_bins,
                            angle_min: @angle_min,
                            angle_max: @angle_max)
        add_quick_look_module("HistogramXrayEventAzimuthAngle_Basic_#{@name}_2")
        append_quick_look(app)
      end
    end

    class EventProperties
      include QuickLookBase

      def initialize()
        super
        @name = ""
        @num_bins = 30
        @weight_min = 0.5
        @weight_max = 29.5
        @event_selector = {}
        set_quick_look_canvas(600, 400)
      end
      attr_accessor :name, :event_selector

      def define_weight(num_bins, weight_min, weight_max)
        @num_bins = num_bins
        @weight_min = weight_min
        @weight_max = weight_max
      end

      def insert_modules(app)
        app.chain :XrayEventSelection, "XrayEventSelection_Properties_#{@name}"
        app.with_parameters() do |m|
          event_selector.each do |k, v|
            m.add_condition(k.to_s, v)
          end
        end
        app.chain :HistogramXrayEventProperties, "HistogramXrayEventProperties_Basic_#{@name}"
        app.with_parameters(collection_module: "XrayEventSelection_Properties_#{@name}",
                            num_bins: @num_bins,
                            weight_min: @weight_min,
                            weight_max: @weight_max)
        add_quick_look_module("HistogramXrayEventProperties_Basic_#{@name}")
        append_quick_look(app)
      end
    end

    class SensorImage
      include QuickLookBase

      def initialize()
        super
        @name = ""
        @event_selector = {}
        @nx = 1024
        @ny = 1024
        @output_name = "image"
        set_quick_look_canvas(600, 600)
      end
      attr_accessor :name, :event_selector, :output_name

      def define_image(num_x, num_y)
        @nx = num_x
        @ny = num_y
      end

      def insert_modules(app)
        app.chain :XrayEventSelection, "XrayEventSelection_Image_#{@name}"
        app.with_parameters() do |m|
          event_selector.each do |k, v|
            m.add_condition(k.to_s, v)
          end
        end
        app.chain :ExtractXrayEventImage, "ExtractXrayEventImage_Image_#{@name}"
        app.with_parameters(collection_module: "XrayEventSelection_Image_#{@name}",
                            num_x: @nx,
                            num_y: @ny,
                            output_name: @output_name)
        add_quick_look_module("ExtractXrayEventImage_Image_#{@name}")
        append_quick_look(app)
      end
    end

    class RawPixelImage
      include QuickLookBase

      def initialize()
        super
        @name = ""
        @rebin_x = 1
        @rebin_y = 1
        @output = "image"
        set_quick_look_canvas(400, 400)
      end
      attr_accessor :name, :output

      def define_rebin(rebin_x, rebin_y)
        @rebin_x = rebin_x
        @rebin_y = rebin_y
      end

      def insert_modules(app)
        app.chain :HistogramRawFrameImage, "HistogramRawFrameImage_#{@name}_1"
        app.with_parameters(rebin_x: @rebin_x,
                            rebin_y: @rebin_y)
        add_quick_look_module("HistogramRawFrameImage_#{@name}_1")
        append_quick_look(app)
      end
    end

    class RawPixelProperties
      include QuickLookBase

      def initialize()
        super
        @name = ""
        @mean_num_bins = 100
        @mean_min = 0.0
        @mean_max = 400.0
        @sigma_num_bins = 100
        @sigma_min = 0.0
        @sigma_max = 40.0
        @output = nil
        set_quick_look_canvas(600, 400)
      end
      attr_accessor :name, :output

      def define_mean(num_bins, min, max)
        @mean_num_bins = num_bins
        @mean_min = min
        @mean_max = max
      end

      def define_sigma(num_bins, min, max)
        @sigma_num_bins = num_bins
        @sigma_min = min
        @sigma_max = max
      end

      def insert_modules(app)
        app.chain :HistogramFramePedestal, "HistogramFramePedestal_Dark_#{@name}_1"
        app.with_parameters(mean_num_bins: @mean_num_bins,
                            mean_min: @mean_min,
                            mean_max: @mean_max,
                            sigma_num_bins: @sigma_num_bins,
                            sigma_min: @sigma_min,
                            sigma_max: @sigma_max)
        add_quick_look_module("HistogramFramePedestal_Dark_#{@name}_1")
        append_quick_look(app)
      end
    end
  end
end # module ComptonSoft
