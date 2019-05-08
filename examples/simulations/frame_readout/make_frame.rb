#!/usr/bin/env ruby

require 'comptonsoft'

class MyApp < ANL::ANLApp
  attr_accessor :inputs

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ReadHitTree
    with_parameters(file_list: @inputs,
                    trust_num_hits: false)
    chain :MakeFrame
    with_parameters(filename_base: "HitomiCCD_Gaussian_20mCrab", num_pixelx: 640, num_pixely: 640)
  end
end

a = MyApp.new
a.inputs = ["HitomiCCD_Gaussian_Crab_frame.root"]
a.run(:all, 1)
