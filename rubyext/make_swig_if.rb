#!/usr/bin/env ruby
require 'ANLSwigClass'

name = 'comptonSoft'
namespace = 'comptonsoft'

clMod = [
  ANLSwigClass.new("ConstructDetector"),
  ANLSwigClass.new("ConstructDetectorForSimulation"),
  ANLSwigClass.new("VCSModule"),
  ANLSwigClass.new("CSHitCollection"),
  ANLSwigClass.new("ConstructChannelMap"),
  ANLSwigClass.new("DefineBadChannels"),
  ANLSwigClass.new("SetNoiseLevels"),
  ANLSwigClass.new("SetChannelsInfo"),
  ANLSwigClass.new("SelectHits"),
  ANLSwigClass.new("AnalyzeHits"),
  ANLSwigClass.new("MakeDetectorHits"),
  ANLSwigClass.new("MakeRawHits"),
  ANLSwigClass.new("MakeDetectorHitsTR"),
  ANLSwigClass.new("ApplyEPICompensation"),
  ANLSwigClass.new("DetectorGroupManager"),
  ANLSwigClass.new("EventSelection"),
  ANLSwigClass.new("EventReconstruction"),
  ANLSwigClass.new("HXIEventSelection"),
  ANLSwigClass.new("SaveData"),
  ANLSwigClass.new("ReadDataFile"),
  ANLSwigClass.new("ReadDataFile_VME3"),
  ANLSwigClass.new("ReadDataFile_SpW2"),
  ANLSwigClass.new("ReadDataFile_NT"),
  ANLSwigClass.new("ReadDataFile_NB0"),
  ANLSwigClass.new("CorrectPHA"),
  ANLSwigClass.new("RecalculateEPI"),
  ANLSwigClass.new("CalculatePedestalLevels"),
  ANLSwigClass.new("InitialConditionFilter"),
  ANLSwigClass.new("RecalculateSimulationNoise"),
  ANLSwigClass.new("ComptonModeFilter"),
  ANLSwigClass.new("SelectFullDeposit"),
  ANLSwigClass.new("ComptonEventFilter", true),
  ANLSwigClass.new("SelectTime"),
  ANLSwigClass.new("WriteHitTree"),
  ANLSwigClass.new("ReadHitTree"),
  ANLSwigClass.new("ReadRawHitTree"),
  ANLSwigClass.new("ReadDetectorHitTree"),
  ANLSwigClass.new("WriteComptonEventTree"),
  ANLSwigClass.new("ReadComptonEventTree"),
  ANLSwigClass.new("HistogramPHA"),
  ANLSwigClass.new("HistogramEnergySpectrum"),
  ANLSwigClass.new("HistogramEnergy2D"),
  ANLSwigClass.new("HistogramARM"),
  ANLSwigClass.new("HistogramAzimuthAngle"),
]

clSim = [# primary generator
  ANLSwigClass.new('PlaneWaveRectanglePrimaryGen'),
  ANLSwigClass.new('IsotropicPrimaryGen'),
  ANLSwigClass.new('NucleusPrimaryGen'),
  ANLSwigClass.new('NucleusPrimaryGenInVolume'),
  ANLSwigClass.new('AHRayTracingPrimaryGen', false, 'USE_SIMX'),
  ANLSwigClass.new('SimXPrimaryGen', false, 'USE_SIMX'),
  ANLSwigClass.new('AHRadiationBackgroundPrimaryGen'),
  # pick up data
  ANLSwigClass.new('RDPickUpData'),
  ANLSwigClass.new('ActivationPickUpData'),
  ANLSwigClass.new('AHStandardPickUpData', false, 'USE_SIMX'),
  ANLSwigClass.new('KillStepPickUpData'),
  ANLSwigClass.new('MaterialSamplePickUpData'),
  ANLSwigClass.new('ScatteringPickUpData'),
  # physics list
  ANLSwigClass.new('PhysicsListManager'),
  # others
  ANLSwigClass.new('SimXIF', false, 'USE_SIMX'),
  ANLSwigClass.new('GenerateSimXEvent', false, 'USE_SIMX'),
  ANLSwigClass.new('OutputSimXPrimaries', false, 'USE_SIMX'),
  ANLSwigClass.new('AssignG4CopyNumber'),
  ANLSwigClass.new('InitialParticleTree'),
  ANLSwigClass.new('DumpMass'),
]

classList = clMod + clSim
classList.each{|s|
  s.include_path = ["../source/modules/include",
                    "../source/simulation/include"]
}

m = ANLSwigModule.new(name, classList, namespace)
m.includeFiles = ['class_list_anlGeant4.hh', 'comptonsoft_basic_classes.hh']
m.importModules = ['ANL.i', 'anlGeant4.i']
m.includeModules = []

if ARGV.include? '-l'
  m.print_class_list
else
  m.print_interface
end
