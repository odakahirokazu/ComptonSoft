module ComptonSoft
  class XrayEventSelection
    alias_method :add_condition_original, :add_condition

    def add_condition(key, ranges)
      new_ranges = []
      if ranges[0].is_a? Array
        new_ranges = ranges
      else
        new_ranges << ranges
      end

      if new_ranges[0][0].is_a? Integer then
        v = new_ranges.map{|a| ANL::PairII.new(a[0], a[1]) }
        w = ANL::VectorPairII.new(v)
        add_condition_original(key, w)
      elsif new_ranges[0][0].is_a? Float then
        v = new_ranges.map{|a| ANL::PairDD.new(a[0], a[1]) }
        w = ANL::VectorPairDD.new(v)
        add_condition_original(key, w)
      else
        raise "XrayEventSelection#add_condition(): type invalid (Array of Array of *)."
      end
    end
  end
end
