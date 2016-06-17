# comptonsoft/radioactivation.rb
#
# @author Hirokazu Odaka
#

module ComptonSoft
  class RIData
    def initialize(i, z, a, energy, value)
      @id = i
      @z = z
      @a = a
      @energy = energy
      @value = value
    end
    attr_reader :id, :z, :a, :energy, :value
    attr_writer :value

    def format()
      "%14d %3d %3d %15.9e %15d" % [@id, @z, @a, @energy, @value]
    end

    def format_for_summary_file()
      "Isotope "+format()
    end

    def format_for_cosima()
      id_cosima = @id/1000000000
      "RP %5d %15.9e %15d" % [id_cosima, @energy, @value]
    end
  end

  class ActivationSummary
    def initialize()
      @data = {}
      @num_events = 0
    end
    attr_accessor :num_events

    def cd(volume_id)
      @current_volume = @data[volume_id]
      unless @current_volume
        @data[volume_id] = {}
        @current_volume = @data[volume_id]
      end
    end

    def fill(ri_id, data)
      ri = @current_volume[ri_id]
      if ri
        ri.value += data.value
      else
        @current_volume[ri_id] = data
      end
    end

    def get_list(volume_id)
      cd(volume_id)
      @current_volume.values
    end

    def read(filename, as_rate: false)
      File::open(filename) do |fin|
        fin.each_line do |line|
          if line =~ /^NumberOfEvents\s+(\w+)/
            @num_events += $1.to_i
          elsif line =~ /^Volume\S+\s+(\S+)/
            volume_id = $1
            cd(volume_id)
          elsif line =~ /^Isotope/
            row_data = line.strip.split(/\s+/)
            ri_id = row_data[1].to_i
            ri_z = row_data[2].to_i
            ri_a = row_data[3].to_i
            ri_e = row_data[4].to_f
            if as_rate
              value = row_data[5].to_f
            else
              value = row_data[5].to_i
            end
            ri = RIData.new(ri_id, ri_z, ri_a, ri_e, value)
            fill(ri_id, ri)
          end
        end
      end
    end

    def write(filename)
      File.open(filename, 'w') do |fout|
        fout.puts "NumberOfEvents %d" % @num_events
        fout.puts ''
        @data.each do |volume_id, volume_data|
          fout.puts "Volume[.] #{volume_id}"
          ri_data = volume_data.values.sort{|a, b| a.id <=> b.id }
          ri_data.each do |ri|
            fout.puts ri.format_for_summary_file
          end
          fout.puts ''
        end
      end
    end

    def read_for_cosima(filename)
      File::open(filename) do |fin|
        fin.each_line do |line|
          if line =~ /^VN\s+(\S+)/
            volume_id = $1
            cd(volume_id)
          elsif line =~ /^RP/
            row_data = line.strip.split(/\s+/)
            cosima_id = row_data[1].to_i
            ri_z = cosima_id/1000
            ri_a = cosima_id%1000
            ri_e = row_data[2].to_f
            ri_id = cosima_id*1000000000+((ri_e*1000).to_i)%1000000000
            value = row_data[3].to_f
            ri = RIData.new(ri_id, ri_z, ri_a, ri_e, value)
            fill(ri_id, ri)
          end
        end
      end
    end

    def write_for_cosima(filename, time=1.0)
      File.open(filename, 'w') do |fout|
        fout.puts "# Cosima universal isotope store"
        fout.puts "# VN is followed by the volume name in which the isotope was produced"
        fout.puts "# RP is followed by the isotope ID (1000*Z+A), the excitation in keV, and a value (e.g. the number of produced isotopes, activation in Bq)"
        fout.puts ""
        fout.puts "TT #{time}"
        fout.puts ""

        @data.each do |volume_id, volume_data|
          fout.puts "VN "+volume_id
          ri_data = volume_data.values.sort{|a, b| a.id <=> b.id }
          ri_data.each do |ri|
            fout.puts ri.format_for_cosima
          end
        end
        fout.puts ""
        fout.puts "EN"
      end
    end
  end
end # module ComptonSoft
