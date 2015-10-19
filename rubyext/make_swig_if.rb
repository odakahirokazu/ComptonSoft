#!/usr/bin/env ruby

require 'ANLLib'

name = 'comptonSoft'
namespace = 'comptonsoft'

clMod = [
  ANL::SWIGClass.new("ConstructDetector"),
  ANL::SWIGClass.new("ConstructDetectorForSimulation"),
  ANL::SWIGClass.new("VCSModule"),
  ANL::SWIGClass.new("CSHitCollection"),
  ANL::SWIGClass.new("ConstructChannelMap"),
  ANL::SWIGClass.new("SetNoiseLevels"),
  ANL::SWIGClass.new("SetBadChannels"),
  ANL::SWIGClass.new("SetChannelsInfo"),
  ANL::SWIGClass.new("SelectHits"),
  ANL::SWIGClass.new("AnalyzeHits"),
  ANL::SWIGClass.new("MakeDetectorHits"),
  ANL::SWIGClass.new("MakeRawHits"),
  ANL::SWIGClass.new("MakeDetectorHitsTR"),
  ANL::SWIGClass.new("ApplyEPICompensation"),
  ANL::SWIGClass.new("EventSelection"),
  ANL::SWIGClass.new("EventReconstruction"),
  ANL::SWIGClass.new("HXIEventSelection"),
  ANL::SWIGClass.new("SaveData"),
  ANL::SWIGClass.new("ReadDataFile"),
  ANL::SWIGClass.new("ReadDataFile_VME3"),
  ANL::SWIGClass.new("ReadDataFile_SpW2"),
  ANL::SWIGClass.new("ReadDataFile_NT"),
  ANL::SWIGClass.new("ReadDataFile_NB0"),
  ANL::SWIGClass.new("CorrectPHA"),
  ANL::SWIGClass.new("RecalculateEPI"),
  ANL::SWIGClass.new("CalculatePedestalLevels"),
  ANL::SWIGClass.new("InitialConditionFilter"),
  ANL::SWIGClass.new("RecalculateSimulationNoise"),
  ANL::SWIGClass.new("ComptonModeFilter"),
  ANL::SWIGClass.new("SelectFullDeposit"),
  ANL::SWIGClass.new("ComptonEventFilter", true),
  ANL::SWIGClass.new("SelectTime"),
  ANL::SWIGClass.new("WriteHitTree"),
  ANL::SWIGClass.new("ReadHitTree"),
  ANL::SWIGClass.new("ReadHitTreeAsRawHits"),
  ANL::SWIGClass.new("ReadHitTreeAsDetectorHits"),
  ANL::SWIGClass.new("WriteEventTree"),
  ANL::SWIGClass.new("ReadEventTree"),
  ANL::SWIGClass.new("ReadEventTreeAsRawHits"),
  ANL::SWIGClass.new("ReadEventTreeAsDetectorHits"),
  ANL::SWIGClass.new("WriteComptonEventTree"),
  ANL::SWIGClass.new("ReadComptonEventTree"),
  ANL::SWIGClass.new("HistogramPHA"),
  ANL::SWIGClass.new("HistogramEnergySpectrum"),
  ANL::SWIGClass.new("HistogramEnergy2D"),
  ANL::SWIGClass.new("HistogramARM"),
  ANL::SWIGClass.new("HistogramARMByPositionMeasurement"),
  ANL::SWIGClass.new("HistogramAzimuthAngle"),
  ANL::SWIGClass.new("ResponseMatrix"),
  ANL::SWIGClass.new("BackProjection"),
  ANL::SWIGClass.new("BackProjectionSky"),
  ANL::SWIGClass.new("EfficiencyMapSky"),
  ANL::SWIGClass.new("QuickAnalysisForDSD"),
]

clSim = [# primary generator
  ANL::SWIGClass.new("AHRayTracingPrimaryGen", false, 'USE_SIMX'),
  ANL::SWIGClass.new("SimXPrimaryGen", false, 'USE_SIMX'),
  ANL::SWIGClass.new("AHRadiationBackgroundPrimaryGen"),
  # pick up data
  ANL::SWIGClass.new("RDPickUpData"),
  ANL::SWIGClass.new("ActivationPickUpData"),
  ANL::SWIGClass.new("AHStandardPickUpData", false, 'USE_SIMX'),
  ANL::SWIGClass.new("KillStepPickUpData"),
  ANL::SWIGClass.new("MaterialSamplePickUpData"),
  ANL::SWIGClass.new("ScatteringPickUpData"),
  # physics list
  ANL::SWIGClass.new("PhysicsListManager"),
  # others
  ANL::SWIGClass.new("SimXIF", false, 'USE_SIMX'),
  ANL::SWIGClass.new("GenerateSimXEvent", false, 'USE_SIMX'),
  ANL::SWIGClass.new("OutputSimXPrimaries", false, 'USE_SIMX'),
  ANL::SWIGClass.new("AssignG4CopyNumber"),
  ANL::SWIGClass.new("InitialParticleTree"),
  ANL::SWIGClass.new("DumpMass"),
]

classList = clMod + clSim
classList.each{|s|
  s.include_path = ["../source/modules/include",
                    "../source/simulation/include"]
}

m = ANL::SWIGModule.new(name, classList, namespace)
m.includeFiles = ['class_list_anlGeant4.hh', 'comptonsoft_basic_classes.hh']
m.importModules = ['ANL.i', 'anlGeant4.i']
m.includeModules = []

if ARGV.include? '-l'
  m.print_class_list
else
  m.print_interface
end
