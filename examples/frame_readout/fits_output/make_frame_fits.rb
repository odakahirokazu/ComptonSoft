#!/usr/bin/env ruby

require 'comptonsoft'

class MakeFrameFITSFiles < ANL::ANLApp
  attr_accessor :inputs, :output

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ReadHitTree
    with_parameters(file_list: @inputs,
                    trust_num_hits: false)
    chain :MakeFrameFITS
    with_parameters(filename_base: @output, num_pixelx: 640, num_pixely: 640)
  end
end

a = MakeFrameFITSFiles.new
a.inputs = ["xray_ccd_crab_frame.root"]
a.output = "xray_ccd_crab"
a.run(:all, 1)
