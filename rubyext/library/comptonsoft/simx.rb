# comptonsoft/simx.rb
#
# @author Hirokazu Odaka
#

require 'comptonsoft/simulation'

module ComptonSoft
  # SimX setting
  #
  class SimxSettings
    attr_accessor :pointing_ra, :pointing_dec, :exposure
    attr_accessor :flux
    attr_accessor :source_type, :source_point_ra, :source_point_dec
    attr_accessor :source_image_file
    attr_accessor :source_spectrum_type, :source_spectrum_file, :mono_energy
    attr_accessor :instrument_name, :filter_name
    attr_accessor :scale_background

    def initialize()
      @pointing_ra = 0.0
      @pointing_dec = 0.0
      @exposure = 100000.0
      @flux = 1.0e-9
      @source_type = 'Point'
      @source_point_ra = 0.0
      @source_point_dec = 0.0
      @source_image_file = 'image.fits'
      @source_spectrum_type = 'XSPEC_File'
      @source_spectrum_file = 'model.qdp'
      @mono_energy = 6.4
      @instrument_name = 'HXI'
      @filter_name = 'None'
      @scale_background = 0.0
    end

    def setup_simx(output_filename)
      soft = 'simx'
      pset_command soft, 'OutputFileName', output_filename
      pset_command soft, 'PointingRA', @pointing_ra.to_s
      pset_command soft, 'PointingDec', @pointing_dec.to_s
      pset_command soft, 'Exposure', @exposure.to_s
      pset_command soft, 'SourceFlux', @flux.to_s
      pset_command soft, 'SourceImageType', @source_type
      pset_command soft, 'SourcePointRA', @source_point_ra.to_s
      pset_command soft, 'SourcePointDec', @source_point_dec.to_s
      pset_command soft, 'SourceImageFile', @source_image_file
      pset_command soft, 'SourceSpectrumType', @source_spectrum_type
      pset_command soft, 'SourceSpectrumFile', @source_spectrum_file
      pset_command soft, 'MonoEnergy', @mono_energy.to_s
      pset_command soft, 'MissionName', 'Astro-H'
      pset_command soft, 'InstrumentName', @instrument_name
      pset_command soft, 'FilterName', @filter_name
      pset_command soft, 'ScaleBkgnd', @scale_background
    end

    def pset_command(soft, key, value)
      system "pset #{soft} #{key}=#{value}"
    end
    private :pset_command
  end

  # Monte-Carlo simulation using Simx IF
  #
  class SimulationWithSimxIF < Simulation
    def initialize()
      super
      @simx_seetings = SimxSettings.new
    end

    def output=(v)
      super
      @simx_seetings.setup_simx(@output.sub('.root', ''))
    end

    def setup()
      super
      insert_before(ComptoSoft::SimXIF.new, :SimXPrimaryGen)
    end
  end

  # SimX interface application to generate input file for raytracing code
  #
  class Simx2Xrrt < SimulationWithSimxIF
    def initialize()
      super
      @area = PI * (22.6**2 - 5.65**2)
    end
    attr_writer :area

    def setup()
      chain :SimXIF
      chain :OutputSimXPrimaries
      with_parameters(filename: self.output.sub(".root", "")+"_incident_photons.fits",
                      area: @area)
    end
  end
end # module ComptonSoft
