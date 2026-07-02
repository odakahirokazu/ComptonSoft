# comptonsoft/simulation_postprocessing.rb
#
# @author Hirokazu Odaka
#

module ComptonSoft
  module PostProcessingBase
    def initialize()
      super
      @inputs = []
      @output = "output.root"
    end

    attr_accessor :inputs, :output
  end

  class AssignEventTime < ANL::ANLApp
    include PostProcessingBase

    define_setup_module("time_assignment", :AssignTime)

    def setup()
      add_namespace ComptonSoft

      chain :CSHitCollection
      chain :ReadEventTree
      with_parameters(file_list: @inputs)
      chain_with_parameters module_of_time_assignment
      chain :WriteEventTree
      chain :SaveData
      with_parameters(output: @output)
    end
  end

  class DefineFrames < ANL::ANLApp
    include PostProcessingBase
    attr_accessor :time_start, :frame_exposure

    def setup()
      add_namespace ComptonSoft

      chain :CSHitCollection
      chain :ReadEventTree
      with_parameters(file_list: @inputs)
      chain :DefineFrame
      with_parameters(time_start: @time_start,
                      frame_exposure: @frame_exposure)
      chain :WriteHitTree
      chain :SaveData
      with_parameters(output: @output)
    end
  end

end # module ComptonSoft
