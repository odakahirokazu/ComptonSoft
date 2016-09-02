#!/usr/bin/env ruby

# author: Hirokazu Odaka
# 2016-08-25 | for ComptonSoft version 5

require 'csv'
require 'builder'

def detector_node(xml, detector_prefix)
  xml.detector(prefix: detector_prefix,
               type: "2DStrip",
               num_sections: 4,
               num_channels: 64,
               num_x: 16,
               num_y: 16) do
    n = 64
    n.times{|i| xml.channel(section: 0, index: i, x: -1,   y: 127-i) }
    n.times{|i| xml.channel(section: 1, index: i, x: -1,   y: 63-i)  }
    n.times{|i| xml.channel(section: 2, index: i, x: i,    y: -1)    }
    n.times{|i| xml.channel(section: 3, index: i, x: 64+i, y: -1)    }
  end
end


x = Builder::XmlMarkup.new(:target=>STDOUT, :indent=>2)
x.instruct!
x.instruct!(:"xml-stylesheet",
  :type => "text/xsl",
  :href =>"https://raw.githubusercontent.com/odakahirokazu/ComptonSoft/master/xmlstyle/channel_map_v4.xsl"
  )
x.channel_map {
  x.name "CdTe strip detector"
  x.comment "2016-08-25 | Hirokazu Odaka"
  detector_node(x, "Sensor")
}
