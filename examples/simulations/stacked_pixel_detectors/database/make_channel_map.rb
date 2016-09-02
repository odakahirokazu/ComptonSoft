#!/usr/bin/env ruby

# author: Hirokazu Odaka
# 2016-08-25 | for ComptonSoft version 5

require 'csv'
require 'builder'

def detector_node(xml, detector_prefix)
  xml.detector(prefix: detector_prefix,
               type: "2DPixel",
               num_sections: 4,
               num_channels: 64,
               num_x: 16,
               num_y: 16) do
    n = 64
    n.times{|i| xml.channel(section: 0, index: i, x: (15-i/8), y: (8+i%8))  }
    n.times{|i| xml.channel(section: 1, index: i, x: (7-i%8),  y: (15-i/8)) }
    n.times{|i| xml.channel(section: 2, index: i, x: (0+i/8),  y: (7-i%8))  }
    n.times{|i| xml.channel(section: 3, index: i, x: (8+i%8),  y: (0+i/8))  }
  end
end


x = Builder::XmlMarkup.new(:target=>STDOUT, :indent=>2)
x.instruct!
x.instruct!(:"xml-stylesheet",
  :type => "text/xsl",
  :href =>"https://raw.githubusercontent.com/odakahirokazu/ComptonSoft/master/xmlstyle/channel_map_v4.xsl"
  )
x.channel_map {
  x.name "Compton Camera"
  x.comment "2016-08-25 | Hirokazu Odaka"
  detector_node(x, "SiPad")
  detector_node(x, "CdTePad")
}
