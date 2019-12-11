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
      @num_pixels_x = 1
      @num_pixels_y = 1
    end

    attr_accessor :inputs, :output
    attr_accessor :pedestal_level, :event_threshold
    attr_accessor :split_threshold
    attr_accessor :hotpix_threshold
    attr_accessor :pedestal_file, :hotpix_file
    attr_accessor :num_pixels_x, :num_pixels_y

    def define_pixels(nx, ny)
      @num_pixels_x = nx
      @num_pixels_y = ny
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

    def setup()
      add_namespace ComptonSoft

      chain :ConstructFrame
      with_parameters(num_pixel_x: @num_pixels_x,
                      num_pixel_y: @num_pixels_y)

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

    def initialize()
      super
      @analysis_list = []
    end

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
      with_parameters(num_pixel_x: @num_pixels_x,
                      num_pixel_y: @num_pixels_y)

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
      with_parameters(num_pixel_x: @num_pixels_x,
                      num_pixel_y: @num_pixels_y)
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

  module XrayEventAnalysis
    class SpectrumAndPolarization
      include QuickLookBase

      def initialize()
        super
        @name = ""
        @event_selector = {}
        @spectrum_num_bins = 512
        @spectrum_min = 0.0
        @spectrum_max = 4096.0
        set_quick_look_canvas(600, 400)
      end
      attr_accessor :name, :event_selector

      def define_spectrum(num_bins, energy_min, energy_max)
        @spectrum_num_bins = num_bins
        @spectrum_min = energy_min
        @spectrum_max = energy_max
      end

      def insert_modules(app)
        app.chain :XrayEventSelection, "XrayEventSelection_Basic_#{@name}_1"
        event_selector_for_spectrum = @event_selector.clone
        event_selector_for_spectrum.delete(:sumPH_min)
        event_selector_for_spectrum.delete(:sumPH_max)
        app.with_parameters(**event_selector_for_spectrum)
        app.chain :HistogramXrayEventSpectrum, "HistogramXrayEventSpectrum_Basic_#{@name}_1"
        app.with_parameters(collection_module: "XrayEventSelection_Basic_#{@name}_1",
                            num_bins: @spectrum_num_bins,
                            energy_min: @spectrum_min,
                            energy_max: @spectrum_max)
        add_quick_look_module("HistogramXrayEventSpectrum_Basic_#{@name}_1")
        app.chain :XrayEventSelection, "XrayEventSelection_Basic_#{@name}_2"
        app.with_parameters(**@event_selector)
        app.chain :HistogramXrayEventAzimuthAngle, "HistogramXrayEventAzimuthAngle_Basic_#{@name}_2"
        app.with_parameters(collection_module: "XrayEventSelection_Basic_#{@name}_2")
        add_quick_look_module("HistogramXrayEventAzimuthAngle_Basic_#{@name}_2")
        append_quick_look(app)
      end
    end

    class CodedApertureImaging
      include QuickLookBase

      def initialize()
        super
        @name = ""
        @event_selector = {}
        set_quick_look_canvas(300, 300)
      end
      attr_accessor :name, :event_selector

      def define_pattern(nx, ny, pattern_file)
        @aperture_nx = nx
        @aperture_ny = ny
        @pattern_file = pattern_file
      end

      def define_encoded_image_shape(nx, ny, offset_x, offset_y)
        @encoded_image_num_pixels_x = nx
        @encoded_image_num_pixels_y = ny
        @encoded_image_offset_x = offset_x
        @encoded_image_offset_y = offset_y
       end

      def define_encoded_image_rotation(center_x, center_y, angle)
        @encoded_image_rotation_center_x = center_x
        @encoded_image_rotation_center_y = center_y
        @encoded_image_rotation_angle = angle
      end

      def define_decoded_image_shape(nx, ny)
        @decoded_image_pixel_x = nx
        @decoded_image_pixel_y = ny
      end

      def set_aperture_to_detector_relation(aperture_element_size, detector_element_size)
        @aperture_element_size = aperture_element_size
        @detector_element_size = detector_element_size
      end

      def insert_modules(app)
        app.chain :XrayEventSelection, "XrayEventSelection_CA_#{@name}"
        app.with_parameters(**@event_selector)
        app.chain :ExtractXrayEventImage, "ExtractXrayEventImage_CA_#{@name}"
        app.with_parameters(collection_module: "XrayEventSelection_CA_#{@name}",
                            num_x: @encoded_image_num_pixels_x,
                            num_y: @encoded_image_num_pixels_y,
                            new_origin_x: @encoded_image_offset_x,
                            new_origin_y: @encoded_image_offset_y,
                            rotation_angle: @encoded_image_rotation_angle,
                            image_center_x: @encoded_image_rotation_center_x,
                            image_center_y: @encoded_image_rotation_center_y)
        add_quick_look_module("ExtractXrayEventImage_CA_#{@name}")
        app.chain :ProcessCodedAperture, "ProcessCodedAperture_CA_#{@name}"
        app.with_parameters(num_encoded_image_x: @encoded_image_num_pixels_x,
                            num_encoded_image_y: @encoded_image_num_pixels_y,
                            num_mask_x: @aperture_nx,
                            num_mask_y: @aperture_ny,
                            detector_element_size: @detector_element_size,
                            mask_element_size: @aperture_element_size,
                            pattern_file: @pattern_file,
                            image_owner_module: "ExtractXrayEventImage_CA_#{@name}")
        add_quick_look_module("ProcessCodedAperture_CA_#{@name}")
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
        set_quick_look_canvas(600, 600)
      end
      attr_accessor :name, :event_selector

      def define_image(num_x, num_y)
        @nx = num_x
        @ny = num_y
      end

      def insert_modules(app)
        app.chain :XrayEventSelection, "XrayEventSelection_Image_#{@name}"
        app.with_parameters(**@event_selector)
        app.chain :ExtractXrayEventImage, "ExtractXrayEventImage_Image_#{@name}"
        app.with_parameters(collection_module: "XrayEventSelection_Image_#{@name}",
                            num_x: @nx,
                            num_y: @ny)
        add_quick_look_module("ExtractXrayEventImage_Image_#{@name}")
        append_quick_look(app)
      end
    end
  end
end # module ComptonSoft
