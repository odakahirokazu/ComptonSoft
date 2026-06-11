#!/usr/bin/env ruby

require 'comptonsoft'

class MyApp < ANL::ANLApp
  attr_accessor :tpc_tree_file, :hittree_file
  attr_accessor :quicklook_file, :gain_tp_hash

  def setup
    add_namespace ComptonSoft

    chain :NanoGRAMSHitExtraction
    with_parameters(
      config_file:     "config_pipeline.yaml",
      tpctree_file:    @tpc_tree_file,
      quicklook_file:  @quicklook_file,
    )

    chain :NanoGRAMSCalibration
    with_parameters(
      hittree_file: @hittree_file,
      gain_tp_hash: @gain_tp_hash
    )
  end
end


### main ###
data_directory = "data/2025_10_01_02_33_05"
gain_tp_hash = { 
  "0" => 189.104, 
  "1" => 275.546, 
  "2" => 201.062, 
  "3" => 193.468 
}

a = MyApp.new
a.gain_tp_hash   = gain_tp_hash
a.tpc_tree_file  = "#{data_directory}/tpc_data.root"
a.hittree_file   = "#{data_directory}/hittree.root"
a.quicklook_file = "#{data_directory}/quicklook_tree.root"

a.run(:all, 1000)
