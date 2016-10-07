require 'rexml/document'
require 'rexml/formatters/pretty'

module ComptonSoft
  module Database
    class DetectorConfiguration
      class Detector
        attr_accessor :id, :name, :type
        attr_accessor :geometry_x, :geometry_y, :geometry_z
        attr_accessor :offset_x, :offset_y
        attr_accessor :pixel_number_x, :pixel_number_y
        attr_accessor :pixel_size_x, :pixel_size_y
        attr_accessor :position_x, :position_y, :position_z
        attr_accessor :xaxis_direction_x, :xaxis_direction_y, :xaxis_direction_z
        attr_accessor :yaxis_direction_x, :yaxis_direction_y, :yaxis_direction_z
        attr_accessor :energy_priority_electrode_side

        def initialize()
          @sections = []
        end

        def add_section(num_channels, electrode_side)
          @sections << [num_channels, electrode_side]
        end

        def each_section(&b)
          @sections.each(&b)
        end
      end

      class ReadoutModule
        attr_accessor :id

        def initialize()
          @sections = []
        end

        def add_section(detector_id, section)
          @sections << [detector_id, section]
        end

        def each_section(&b)
          @sections.each(&b)
        end
      end

      class Group
        attr_accessor :name, :ids
      end

      class HitPattern
        attr_accessor :name, :short_name, :bit, :groups
      end

      def initialize()
        @name = nil
        @comments = []
        @length_unit = nil
        @detectors = []
        @readout = []
        @groups = []
        @hit_patterns = []
      end
      attr_accessor :name

      def add_comment(s)
        @comments << s
      end

      def read(filename)
        xmldoc = REXML::Document.new(File.open(filename))
        root = xmldoc.elements["configuration"]
        @name = root.elements["name"].text
        root.elements.each("comment"){|e| @comments << e.text }
        @length_unit = root.elements["length_unit"].text
        root.elements.each("detectors/detector") do |e|
          d = Detector.new
          d.id = e.attributes["id"]
          d.type = e.attributes["type"]
          d.name = e.attributes["name"]
          d.geometry_x = e.elements["geometry/@x"]&.value
          d.geometry_y = e.elements["geometry/@y"]&.value
          d.geometry_z = e.elements["geometry/@z"]&.value
          d.offset_x = e.elements["offset/@x"]&.value
          d.offset_y = e.elements["offset/@y"]&.value
          d.pixel_number_x = e.elements["pixel/@number_x"]&.value
          d.pixel_number_y = e.elements["pixel/@number_y"]&.value
          d.pixel_size_x = e.elements["pixel/@size_x"]&.value
          d.pixel_size_y = e.elements["pixel/@size_y"]&.value
          d.position_x = e.elements["position/@x"]&.value
          d.position_y = e.elements["position/@y"]&.value
          d.position_z = e.elements["position/@z"]&.value
          d.xaxis_direction_x = e.elements["xaxis_direction/@x"]&.value
          d.xaxis_direction_y = e.elements["xaxis_direction/@y"]&.value
          d.xaxis_direction_z = e.elements["xaxis_direction/@z"]&.value
          d.yaxis_direction_x = e.elements["yaxis_direction/@x"]&.value
          d.yaxis_direction_y = e.elements["yaxis_direction/@y"]&.value
          d.yaxis_direction_z = e.elements["yaxis_direction/@z"]&.value
          d.energy_priority_electrode_side = e.elements["energy_priority/@electrode_side"]&.value
          e.elements.each("sections/section") do |ee|
            d.add_section(ee.attributes["num_channels"], ee.attributes["electrode_side"])
          end
          @detectors << d
        end
        root.elements.each("readout/module") do |e|
          r = ReadoutModule.new
          r.id = e.attributes["id"]
          e.elements.each("section") do |ee|
            r.add_section(ee.attributes["detector_id"], ee.attributes["section"])
          end
          @readout << r
        end
        root.elements.each("groups/group") do |e|
          g = Group.new
          g.name = e.attributes["name"]
          g.ids = []; e.elements.each("detector"){|ee| g.ids << ee.attributes["id"] }
          @groups << g
        end
        root.elements.each("groups/hit_pattern") do |e|
          h = HitPattern.new
          h.name = e.attributes["name"]
          h.short_name = e.attributes["short_name"]
          h.bit = e.attributes["bit"]
          h.groups = []; e.elements.each("group"){|ee| h.groups << ee.attributes["name"] }
          @hit_patterns << h
        end
      end

      def read_old(filename)
        xmldoc = REXML::Document.new(File.open(filename))
        root = xmldoc.elements["configuration"]
        @name = root.elements["name"].text
        root.elements.each("comment"){|e| @comments << e.text }
        @length_unit = root.elements["length_unit"].text
        root.elements.each("detectors/detector") do |e|
          d = Detector.new
          d.id = e.attributes["id"]
          d.type = e.elements["type"].text
          d.name = e.elements["name"].text
          d.geometry_x = e.elements["geometry/widthx"].text
          d.geometry_y = e.elements["geometry/widthy"].text
          d.geometry_z = e.elements["geometry/thickness"].text
          d.offset_x = e.elements["geometry/offsetx"].text
          d.offset_y = e.elements["geometry/offsety"].text
          d.pixel_number_x = e.elements["pixel/numx"].text
          d.pixel_number_y = e.elements["pixel/numy"].text
          d.pixel_size_x = e.elements["pixel/pitchx"].text
          d.pixel_size_y = e.elements["pixel/pitchy"].text
          d.position_x = e.elements["position/x"].text
          d.position_y = e.elements["position/y"].text
          d.position_z = e.elements["position/z"].text
          d.xaxis_direction_x = e.elements["direction_xaxis/x"].text
          d.xaxis_direction_y = e.elements["direction_xaxis/y"].text
          d.xaxis_direction_z = e.elements["direction_xaxis/z"].text
          d.yaxis_direction_x = e.elements["direction_yaxis/x"].text
          d.yaxis_direction_y = e.elements["direction_yaxis/y"].text
          d.yaxis_direction_z = e.elements["direction_yaxis/z"].text
          d.energy_priority_electrode_side = e.elements["energy_priority"]&.text
          e.elements.each("readout/section") do |ee|
            d.add_section(ee.attributes["num_channels"], ee.attributes["electrode_side"])
          end
          @detectors << d
        end
        root.elements.each("readout/module") do |e|
          r = ReadoutModule.new
          r.id = e.attributes["id"]
          e.elements.each("section") do |ee|
            r.add_section(ee.elements["detector"].attributes["id"], ee.elements["detector"].attributes["section"])
          end
          @readout << r
        end
        root.elements.each("groups/group") do |e|
          g = Group.new
          g.name = e.attributes["name"]
          g.ids = []; e.elements.each("detector"){|ee| g.ids << ee.attributes["id"] }
          @groups << g
        end
        root.elements.each("groups/hit_pattern") do |e|
          h = HitPattern.new
          h.name = e.attributes["name"]
          h.short_name = e.attributes["short_name"]
          h.bit = e.attributes["bit"]
          h.groups = []; e.elements.each("group"){|ee| h.groups << ee.attributes["name"] }
          @hit_patterns << h
        end
      end

      def write(filename)
        root = REXML::Element.new("configuration")
        root.add_element("name").add_text(@name)
        @comments.each{|c| root.add_element("comment").add_text(c) }
        root.add_element("length_unit").add_text(@length_unit)
        detectors_node = root.add_element("detectors")
        @detectors.each do |d|
          e = detectors_node.add_element("detector")
          e.add_attribute("id", d.id)
          e.add_attribute("type", d.type)
          e.add_attribute("name", d.name)
          ee = nil; make_ee = lambda{ ee ||= e.add_element("geometry") }
          if v=d.geometry_x; make_ee.call; ee.add_attribute("x", v); end
          if v=d.geometry_y; make_ee.call; ee.add_attribute("y", v); end
          if v=d.geometry_z; make_ee.call; ee.add_attribute("z", v); end
          ee = nil; make_ee = lambda{ ee ||= e.add_element("offset") }
          if v=d.offset_x; make_ee.call; ee.add_attribute("x", v); end
          if v=d.offset_y; make_ee.call; ee.add_attribute("y", v); end
          ee = nil; make_ee = lambda{ ee ||= e.add_element("pixel") }
          if v=d.pixel_number_x; make_ee.call; ee.add_attribute("number_x", v); end
          if v=d.pixel_number_y; make_ee.call; ee.add_attribute("number_y", v); end
          if v=d.pixel_size_x; make_ee.call; ee.add_attribute("size_x", v); end
          if v=d.pixel_size_y; make_ee.call; ee.add_attribute("size_y", v); end
          ee = nil; make_ee = lambda{ ee ||= e.add_element("position") }
          if v=d.position_x; make_ee.call; ee.add_attribute("x", v); end
          if v=d.position_y; make_ee.call; ee.add_attribute("y", v); end
          if v=d.position_z; make_ee.call; ee.add_attribute("z", v); end
          ee = nil; make_ee = lambda{ ee ||= e.add_element("xaxis_direction") }
          if v=d.xaxis_direction_x; make_ee.call; ee.add_attribute("x", v); end
          if v=d.xaxis_direction_y; make_ee.call; ee.add_attribute("y", v); end
          if v=d.xaxis_direction_z; make_ee.call; ee.add_attribute("z", v); end
          ee = nil; make_ee = lambda{ ee ||= e.add_element("yaxis_direction") }
          if v=d.yaxis_direction_x; make_ee.call; ee.add_attribute("x", v); end
          if v=d.yaxis_direction_y; make_ee.call; ee.add_attribute("y", v); end
          if v=d.yaxis_direction_z; make_ee.call; ee.add_attribute("z", v); end
          ee = nil; make_ee = lambda{ ee ||= e.add_element("energy_priority") }
          if v=d.energy_priority_electrode_side; make_ee.call; ee.add_attribute("electrode_side", v); end
          ee = nil; make_ee = lambda{ ee ||= e.add_element("sections") }
          d.each_section do |s|
            make_ee.call
            eee = ee.add_element("section")
            eee.add_attribute("num_channels", s[0])
            if v=s[1]; eee.add_attribute("electrode_side", v); end
          end
        end
        readout_node = root.add_element("readout")
        @readout.each do |r|
          e = readout_node.add_element("module")
          e.add_attributes({"id"=>r.id})
          r.each_section{|s| e.add_element("section", {"detector_id"=>s[0], "section"=>s[1]}) }
        end
        groups_node = root.add_element("groups")
        @groups.each do |g|
          e = groups_node.add_element("group")
          e.add_attributes("name"=>g.name)
          g.ids.each{|id| e.add_element("detector", {"id"=>id}) }
        end
        @hit_patterns.each do |h|
          e = groups_node.add_element("hit_pattern")
          e.add_attributes("name"=>h.name, "short_name"=>h.short_name, "bit"=>h.bit)
          h.groups.each{|name| e.add_element("group", {"name"=>name}) }
        end

        set_double_quote = lambda do |e|
          e.context ||= {}
          e.context[:attribute_quote] = :quote
          e.elements.each{|ee| set_double_quote.(ee) }
        end
        set_double_quote.(root)

        output = File.open(filename, 'w')
        output.puts '<?xml version="1.0" encoding="UTF-8"?>'
        output.puts '<?xml-stylesheet type="text/xsl" href="https://raw.githubusercontent.com/odakahirokazu/ComptonSoft/master/xmlstyle/detector_configuration_v4.xsl" ?>'
        output.puts ''
        formatter = REXML::Formatters::Pretty.new
        formatter.compact = true
        formatter.write(root, output)
      end
    end

    class ChannelMap
      class DetectorChannelMap
        attr_accessor :prefix, :type, :num_sections, :num_channels, :num_x, :num_y

        def initialize()
          @data = []
        end

        def set(section, channel, x, y)
          @data << [section, channel, x, y]
        end

        def each_channel(&b)
          @data.each(&b)
        end
      end

      def initialize()
        @detectors = []
        @name = nil
        @comments = []
      end
      attr_accessor :name

      def add_comment(s)
        @comments << s
      end

      def read(filename)
        xmldoc = REXML::Document.new(File.open(filename))
        root = xmldoc.elements["channel_map"]
        @name = root.elements["name"].text
        root.elements.each("comment"){|e| @comments << e.text }
        root.elements.each("detector") do |e|
          d = DetectorChannelMap.new
          d.prefix = e.attributes["prefix"]
          d.type = e.attributes["type"]
          d.num_sections = e.attributes["num_sections"]
          d.num_channels = e.attributes["num_channels"]
          d.num_x = e.attributes["num_x"]
          d.num_y = e.attributes["num_y"]
          e.elements.each("channel") do |ee|
            d.set(ee.attributes["section"],
                  ee.attributes["index"],
                  ee.attributes["x"],
                  ee.attributes["y"])
          end
          @detectors << d
        end
      end

      def read_old(filename)
        xmldoc = REXML::Document.new(File.open(filename))
        root = xmldoc.elements["channel_map"]
        @name = root.elements["name"].text
        root.elements.each("comment"){|e| @comments << e.text }
        root.elements.each("detector") do |e|
          d = DetectorChannelMap.new
          d.prefix = e.attributes["prefix"]
          d.type = e.attributes["type"]
          d.num_sections = e.attributes["num_sections"]
          d.num_channels = e.attributes["num_channels"]
          d.num_x = e.attributes["num_x"]
          d.num_y = e.attributes["num_y"]
          e.elements.each("section") do |ee|
            section = ee.attributes["id"]
            ee.elements.each("channel") do |eee|
              d.set(section,
                    eee.attributes["id"],
                    eee.elements["x"].text,
                    eee.elements["y"].text)
            end
          end
          @detectors << d
        end
      end

      def write(filename)
        root = REXML::Element.new("channel_map")
        root.add_element("name").add_text(@name)
        @comments.each{|c| root.add_element("comment").add_text(c) }
        @detectors.each do |d|
          e = root.add_element("detector")
          e.add_attribute("prefix", d.prefix)
          e.add_attribute("type", d.type)
          e.add_attribute("num_sections", d.num_sections)
          e.add_attribute("num_channels", d.num_channels)
          e.add_attribute("num_x", d.num_x)
          e.add_attribute("num_y", d.num_y)
          d.each_channel do |c|
            e.add_element("channel", {"section"=>c[0], "index"=>c[1], "x"=>c[2], "y"=>c[3]})
          end

          set_double_quote = lambda do |e|
            e.context ||= {}
            e.context[:attribute_quote] = :quote
            e.elements.each{|ee| set_double_quote.(ee) }
          end
          set_double_quote.(root)

          output = File.open(filename, 'w')
          output.puts '<?xml version="1.0" encoding="UTF-8"?>'
          output.puts '<?xml-stylesheet type="text/xsl" href="https://raw.githubusercontent.com/odakahirokazu/ComptonSoft/master/xmlstyle/channel_map_v4.xsl" ?>'
          output.puts ''
          formatter = REXML::Formatters::Pretty.new
          formatter.compact = true
          formatter.write(root, output)
        end
      end
    end

    class ChannelProperties
      attr_accessor :id
      attr_accessor :disable_status
      attr_accessor :trigger_discrimination_center, :trigger_discrimination_sigma
      attr_accessor :noise_level_param0, :noise_level_param1, :noise_level_param2
      attr_accessor :compensation_factor, :threshold_value

      def empty?()
        if @disable_status || @trigger_discrimination_center || @trigger_discrimination_sigma || @noise_level_param0 || @noise_level_param1 || @noise_level_param2 || @compensation_factor || @threshold_value
          return false
        end
        return true
      end

      def load(node)
        @disable_status = node.elements["disable/@status"]&.value
        @trigger_discrimination_center = node.elements["trigger_discrimination/@center"]&.value
        @trigger_discrimination_sigma = node.elements["trigger_discrimination/@sigma"]&.value
        @noise_level_param0 = node.elements["noise_level/@param0"]&.value
        @noise_level_param1 = node.elements["noise_level/@param1"]&.value
        @noise_level_param2 = node.elements["noise_level/@param2"]&.value
        @compensation_factor = node.elements["compensation/@factor"]&.value
        @threshold_value = node.elements["threshold/@value"]&.value
      end

      def output(node)
        if v=@id; node.add_attribute("id", v); end
        if v=@disable_status; node.add_element("disable", {"status"=>v}); end
        ee = nil; make_ee = lambda{ ee ||= node.add_element("trigger_discrimination") }
        if v=@trigger_discrimination_center; make_ee.call; ee.add_attribute("center", v); end
        if v=@trigger_discrimination_sigma; make_ee.call; ee.add_attribute("sigma", v); end
        ee = nil; make_ee = lambda{ ee ||= node.add_element("noise_level") }
        if v=@noise_level_param0; make_ee.call; ee.add_attribute("param0", v); end
        if v=@noise_level_param1; make_ee.call; ee.add_attribute("param1", v); end
        if v=@noise_level_param2; make_ee.call; ee.add_attribute("param2", v); end
        if v=@compensation_factor; node.add_element("compenstaion", {"factor"=>v}); end
        if v=@threshold_value; node.add_element("threshold", {"value"=>v}); end
      end
    end

    class ChannelPropertiesDB
      class Section
        attr_accessor :id, :all, :common, :data
        def initialize()
          @data = []
        end

        def [](i)
          @data[i]
        end
      end

      class SectionList
        attr_accessor :type, :id, :list
        def initialize()
          @list = []
        end

        def [](i)
          @list[i]
        end
      end

      def initialize()
        @name = nil
        @comments = []
        @data = []
      end
      attr_accessor :name, :comments, :data

      def [](i)
        @data[i]
      end

      def add_comment(s)
        @comments << s
      end

      def build(type, id, num_sections, num_channels, all_flag: nil)
        sl = SectionList.new
        @data << sl
        sl.type = type
        sl.id = id
        sl.list = []
        num_sections.times do |section_id|
          s = Section.new
          sl.list << s
          s.id = section_id
          if all_flag; s.all = all_flag; end
          s.common = ChannelProperties.new
          s.data = []
          num_channels.times do |i|
            c = ChannelProperties.new
            s.data << c
            c.id = i
          end
        end
      end

      def find_detector(id)
        i = @data.index{|sl| (sl.type==:detector && sl.id.to_i==id.to_i) }
        @data[i]
      end

      def find_readout_module(id)
        i = @data.index{|sl| (sl.type==:readout_module && sl.id.to_i==id.to_i) }
        @data[i]
      end

      def read(filename)
        xmldoc = REXML::Document.new(File.open(filename))
        root = xmldoc.elements["channel_properties"]
        @name = root.elements["name"].text
        root.elements.each("comment"){|e| @comments << e.text }
        root.elements.each("data/detector") do |e|
          sl = SectionList.new
          @data << sl
          sl.type = :detector
          sl.id = e.attributes["id"]
          e.elements.each("section") do |ee|
            s = Section.new
            sl.list << s
            s.id = ee.attributes["id"]
            s.all = ee.attributes["all"]
            ee.elements.each("channel") do |eee|
              c = ChannelProperties.new
              s.data << c
              c.id = eee.attributes["id"]
              c.load(eee)
            end
          end
        end
        root.elements.each("data/readout_module") do |e|
          sl = SectionList.new
          @data << sl
          sl.type = :readout_module
          sl.id = e.attributes["id"]
          e.elements.each("section") do |ee|
            s = Section.new
            sl.list << s
            s.id = ee.attributes["id"]
            s.all = ee.attributes["all"]
            ee.elements.each("channel") do |eee|
              c = ChannelProperties.new
              s.data << c
              c.id = eee.attributes["id"]
              c.load(eee)
            end
          end
        end
      end

      def read_noise_levels(filename)
        xmldoc = REXML::Document.new(File.open(filename))
        root = xmldoc.elements["noiselevels"]
        root.elements.each("data/detector") do |e|
          module_id = e.attributes["id"]
          m = find_detector(module_id)
          e.elements.each("section") do |ee|
            section_id = ee.attributes["id"]
            ee.elements.each("channel") do |eee|
              channel_id = eee.attributes["id"]
              c = m[section_id.to_i][channel_id.to_i]
              c.noise_level_param0 = eee.elements["param0"].text
              c.noise_level_param1 = eee.elements["param1"].text
              c.noise_level_param2 = eee.elements["param2"].text
            end
          end
        end
        root.elements.each("data/readout_module") do |e|
          module_id = e.attributes["id"]
          m = find_readout_module(module_id)
          e.elements.each("section") do |ee|
            section_id = ee.attributes["id"]
            ee.elements.each("channel") do |eee|
              channel_id = eee.attributes["id"]
              c = m[section_id.to_i][channel_id.to_i]
              c.noise_level_param0 = eee.elements["param0"].text
              c.noise_level_param1 = eee.elements["param1"].text
              c.noise_level_param2 = eee.elements["param2"].text
            end
          end
        end
      end

      def read_bad_channels(filename)
        xmldoc = REXML::Document.new(File.open(filename))
        root = xmldoc.elements["badchannels"]
        root.elements.each("data/detector") do |e|
          module_id = e.attributes["id"]
          m = find_detector(module_id)
          e.elements.each("section") do |ee|
            section_id = ee.attributes["id"]
            ee.elements.each("channel") do |eee|
              channel_id = eee.attributes["id"]
              c = m[section_id.to_i][channel_id.to_i]
              c.disable_status = eee.attributes["status"]
            end
          end
        end
        root.elements.each("data/readout_module") do |e|
          module_id = e.attributes["id"]
          m = find_readout_module(module_id)
          e.elements.each("section") do |ee|
            section_id = ee.attributes["id"]
            ee.elements.each("channel") do |eee|
              channel_id = eee.attributes["id"]
              c = m[section_id.to_i][channel_id.to_i]
              c.disable_status = eee.attributes["status"]
            end
          end
        end
      end

      def write(filename)
        root = REXML::Element.new("channel_properties")
        root.add_element("name").add_text(@name)
        @comments.each{|c| root.add_element("comment").add_text(c) }
        data_node = root.add_element("data")
        @data.each do |m|
          e = data_node.add_element(m.type.to_s)
          e.add_attribute("id", m.id)
          m.list.each do |s|
            ee = e.add_element("section")
            ee.add_attribute("id", s.id)
            if v=s.all; ee.add_attribute("all", v); end
            common_node = ee.add_element("common")
            s.data.each do |c|
              unless c.empty?
                eee =ee.add_element("channel", c.id)
                c.output(eee)
              end
            end
          end

          set_double_quote = lambda do |e|
            e.context ||= {}
            e.context[:attribute_quote] = :quote
            e.elements.each{|ee| set_double_quote.(ee) }
          end
          set_double_quote.(root)

          output = File.open(filename, 'w')
          output.puts '<?xml version="1.0" encoding="UTF-8"?>'
          output.puts '<?xml-stylesheet type="text/xsl" href="https://raw.githubusercontent.com/odakahirokazu/ComptonSoft/master/xmlstyle/channel_properties_v1.xsl" ?>'
          output.puts ''
          formatter = REXML::Formatters::Pretty.new
          formatter.compact = true
          formatter.write(root, output)
        end
      end
    end

    class DetectorParametersDB
      class DetectorSet
        attr_accessor :sensitive_detector
        attr_accessor :type, :name, :prefix
        attr_accessor :id_method, :layer_offset
        attr_accessor :common
        attr_accessor :detector_list

        def initialize()
          @common = nil
          @detector_list = []
        end

        def load(node)
          @type = node.attributes["type"]
          @name = node.attributes["name"]
          @prefix = node.attributes["prefix"]
          @id_method = node.elements["detector_list/@id_method"]&.value
          @layer_offset = node.elements["detector_list/@layer_offset"]&.value
          node.elements.each("detector_list/detector") do |e|
            d = Detector.new
            d.id = e&.attributes["id"]
            d.path = e&.attributes["path"]
            d.copyno = e&.attributes["copyno"]
            @detector_list << d
          end
        end
      end

      class Detector
        attr_accessor :id, :path, :copyno
      end

      def initialize()
        @name = nil
        @comments = []
        @auto_position_flag = nil
        @sensitive_detector_not_found_warning_flag = nil
        @root_file = nil
        @detector_sets = []
      end
      attr_accessor :name

      def add_comment(s)
        @comments << s
      end

      def read(filename)
        xmldoc = REXML::Document.new(File.open(filename))
        root = xmldoc.elements["detector_parameters"]
        @name = root.elements["name"].text
        root.elements.each("comment"){|e| @comments << e.text }
        @root_file = root.elements["root_file"]&.text
        @auto_position_flag = root.elements["auto_position/@flag"]&.value
        @sensitive_detector_not_found_warning_flag = root.elements["sensitive_detector_not_found_warning/@flag"]&.value
        root.elements.each("data/sensitive_detector") do |e|
          d = DetectorSet.new
          d.sensitive_detector = true
          d.load(e)
          @detector_sets << d
        end
        root.elements.each("data/detector_set") do |e|
          d = DetectorSet.new
          d.sensitive_detector = false
          d.load(e)
          @detector_sets << d
        end
      end

      def read_simulation_parameters(filename)
        xmldoc = REXML::Document.new(File.open(filename))
        root = xmldoc.elements["simulation"]
        @name = root.elements["name"].text
        root.elements.each("comment"){|e| @comments << e.text }
        @root_file = root.elements["cce_file"]&.text
        @auto_position_flag = root.elements["auto_position"] ? "1" : "0"
        @sensitive_detector_not_found_warning_flag = root.elements["sd_check"] ? "1" : "0"
        root.elements.each("sensitive_detectors/sensitive_detector") do |e|
          d = DetectorSet.new
          d.sensitive_detector = true
          d.type = e.elements["type"].text
          d.name = e.elements["name"].text
          d.id_method = e.elements["id_method"].text
          d.layer_offset = e.elements["layer_offset"].text
          e.elements.each("detector_list/detector") do |ee|
            dd = Detector.new
            dd.id = ee&.attributes["id"]
            dd.path = ee&.attributes["path"]
            dd.copyno = ee&.attributes["copyno"]
            d.detector_list << dd
          end
          @detector_sets << d
        end
      end

      def write(filename)
        root = REXML::Element.new("detector_parameters")
        root.add_element("name").add_text(@name)
        @comments.each{|c| root.add_element("comment").add_text(c) }
        if @root_file; root.add_element("root_file").add_text(@root_file); end
        if @auto_position_flag; root.add_element("auto_position").add_attribute("flag", @auto_position_flag); end
        if @sensitive_detector_not_found_warning_flag; root.add_element("sensitive_detector_not_found_warning").add_attribute("flag", @sensitive_detector_not_found_warning_flag); end
        data_node = root.add_element("data")
        @detector_sets.each do |d|
          if d.sensitive_detector
            e = data_node.add_element("sensitive_detector")
          else
            e = data_node.add_element("detector_set")
          end
          e.add_attribute("type", d.type)
          if v=d.name; e.add_attribute("name", v); end
          if v=d.prefix; e.add_attribute("prefix", v); end
          e.add_element("common")
          list_node = e.add_element("detector_list")
          if v=d.id_method; list_node.add_attribute("id_method", v); end
          if v=d.layer_offset; list_node.add_attribute("layer_offset", v); end
          d.detector_list.each do |dd|
            ee = list_node.add_element("detector")
            ee.add_attribute("id", dd.id)
            if v=dd.path; ee.add_attribute("path", v); end
            if v=dd.copyno; ee.add_attribute("copyno", v); end
          end
        end

        set_double_quote = lambda do |e|
          e.context ||= {}
          e.context[:attribute_quote] = :quote
          e.elements.each{|ee| set_double_quote.(ee) }
        end
        set_double_quote.(root)

        output = File.open(filename, 'w')
        output.puts '<?xml version="1.0" encoding="UTF-8"?>'
        output.puts '<?xml-stylesheet type="text/xsl" href="https://raw.githubusercontent.com/odakahirokazu/ComptonSoft/master/xmlstyle/detector_parameters_v1.xsl" ?>'
        output.puts ''
        formatter = REXML::Formatters::Pretty.new
        formatter.compact = true
        formatter.write(root, output)
      end
    end
  end # module database
end # module ComptonSoft
