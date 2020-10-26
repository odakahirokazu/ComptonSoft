#!/usr/bin/env ruby

require 'anlnext'

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
  ANL::SWIGClass.new("SetChannelProperties"),
  ANL::SWIGClass.new("SelectHits"),
  ANL::SWIGClass.new("AnalyzeHits"),
  ANL::SWIGClass.new("MakeDetectorHits"),
  ANL::SWIGClass.new("MakeRawHits"),
  ANL::SWIGClass.new("MakeDetectorHitsWithTimingProcess"),
  ANL::SWIGClass.new("ApplyEPICompensation"),
  ANL::SWIGClass.new("EventSelection"),
  ANL::SWIGClass.new("EventReconstruction"),
  ANL::SWIGClass.new("HXIEventSelection"),
  ANL::SWIGClass.new("CreateRootFile"),
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
  ANL::SWIGClass.new("WeightByInitialDirection"),
  ANL::SWIGClass.new("RecalculateSimulationNoise"),
  ANL::SWIGClass.new("ComptonModeFilter"),
  ANL::SWIGClass.new("SelectFullDeposit"),
  ANL::SWIGClass.new("ComptonEventFilter", true),
  ANL::SWIGClass.new("SelectEventsOnFocalPlane"),
  ANL::SWIGClass.new("SelectTime"),
  ANL::SWIGClass.new("FilterByGoodTimeIntervals"),
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
  ANL::SWIGClass.new("HistogramEnergy1D"),
  ANL::SWIGClass.new("HistogramEnergy2D"),
  ANL::SWIGClass.new("HistogramARM"),
  ANL::SWIGClass.new("HistogramARMByPositionMeasurement"),
  ANL::SWIGClass.new("HistogramAzimuthAngle"),
  ANL::SWIGClass.new("Histogram2DDeltaEnergyWithARM"),
  ANL::SWIGClass.new("ResponseMatrix"),
  ANL::SWIGClass.new("BackProjection"),
  ANL::SWIGClass.new("BackProjectionSky"),
  ANL::SWIGClass.new("EfficiencyMapSky"),
  ANL::SWIGClass.new("QuickAnalysisForDSD"),
  ANL::SWIGClass.new("AssignTime"),
  ANL::SWIGClass.new("DefineFrame"),
  ANL::SWIGClass.new("MakeFrameFITS", false, 'USE_FITSIO'),
  ANL::SWIGClass.new("ConstructFrame"),
  ANL::SWIGClass.new("ConstructSXIFrame", false, 'USE_FITSIO'),
  ANL::SWIGClass.new("FillFrame"),
  ANL::SWIGClass.new("LoadFrame"),
  ANL::SWIGClass.new("XrayEventCollection"),
  ANL::SWIGClass.new("XrayEventSelection"),
  ANL::SWIGClass.new("AnalyzeFrame"),
  ANL::SWIGClass.new("WriteXrayEventTree"),
  ANL::SWIGClass.new("ReadXrayEventTree"),
  ANL::SWIGClass.new("SortEventTreeWithTime"),
  ANL::SWIGClass.new("SetPedestals", false, 'USE_FITSIO'),
  ANL::SWIGClass.new("WritePedestals", false, 'USE_FITSIO'),
  ANL::SWIGClass.new("WriteBadPixels"),
  ANL::SWIGClass.new("AnalyzeDarkFrame"),
  ANL::SWIGClass.new("LoadRootFrame"),
  ANL::SWIGClass.new("DetectBadFrames"),
  ANL::SWIGClass.new("SetBadFrames"),
  ANL::SWIGClass.new("SetPedestalsByMedian"),
  ANL::SWIGClass.new("HistogramFramePedestal"),
  ANL::SWIGClass.new("HistogramFramePedestalMean"),
  ANL::SWIGClass.new("HistogramFramePedestalSigma"),
  ANL::SWIGClass.new("HistogramXrayEventSpectrum"),
  ANL::SWIGClass.new("HistogramXrayEventAzimuthAngle"),
  ANL::SWIGClass.new("HistogramXrayEventWeight"),
  ANL::SWIGClass.new("ExtractXrayEventImage"),
  ANL::SWIGClass.new("ProcessCodedAperture"),
  ANL::SWIGClass.new("PushToQuickLookDB", false, "USE_HSQUICKLOOK"),
  ANL::SWIGClass.new("GetInputFilesFromDirectory"),
  ANL::SWIGClass.new("SelectEventsWithDetectorSpectrum"),
  ANL::SWIGClass.new("AssignSXIGrade"),
  ANL::SWIGClass.new("AEAttitudeCorrection", false, "USE_FITSIO"),
  ANL::SWIGClass.new("SelectEventsWithCelestialSpectrum", false, 'USE_FITSIO'),
  ANL::SWIGClass.new("AEAssignWeightWithResponseMatrix", false, 'USE_FITSIO'),
  ANL::SWIGClass.new("ExtractPhotoelectronTrajectory"),
]

clSim = [
  # primary generator
  ANL::SWIGClass.new("AHRayTracingPrimaryGen", false, 'USE_FITSIO'),
  ANL::SWIGClass.new("SimXPrimaryGen", false, 'USE_SIMX'),
  ANL::SWIGClass.new("AHRadiationBackgroundPrimaryGen"),
  ANL::SWIGClass.new("AEObservationPrimaryGen", false, 'USE_FITSIO'),
  ANL::SWIGClass.new("CelestialSourcePrimaryGen", false, 'USE_FITSIO'),
  # user action assembly
  ANL::SWIGClass.new("RadioactiveDecayUserActionAssembly"),
  ANL::SWIGClass.new("ActivationUserActionAssembly"),
  ANL::SWIGClass.new("AHStandardUserActionAssembly", false, 'USE_SIMX'),
  ANL::SWIGClass.new("SampleOpticalDepth"),
  ANL::SWIGClass.new("ScatteringPickUpData"),
  ANL::SWIGClass.new("ObservationPickUpData"),
  # physics list
  ANL::SWIGClass.new("PhysicsListManager"),
  # others
  ANL::SWIGClass.new("SimXIF", false, 'USE_SIMX'),
  ANL::SWIGClass.new("GenerateSimXEvent", false, 'USE_SIMX'),
  ANL::SWIGClass.new("OutputSimXPrimaries", false, 'USE_SIMX'),
  ANL::SWIGClass.new("AssignG4CopyNumber"),
  ANL::SWIGClass.new("InitialParticleTree"),
  ANL::SWIGClass.new("DumpMass"),
  ANL::SWIGClass.new("WriteObservationTree"),
]

clAH = [
  ANL::SWIGClass.new("SimulateCXBShieldPlate"),
  ANL::SWIGClass.new("RescaleSimulationNoiseOfSGDSiUntriggered"),
  ANL::SWIGClass.new("UniformlyRandomizeEPI"),
  ANL::SWIGClass.new("ReadSGDEventFITS", false, 'USE_FITSIO'),
  ANL::SWIGClass.new("WriteSGDEventFITS", false, 'USE_FITSIO'),
  ANL::SWIGClass.new("ReadHXIEventFITS", false, 'USE_FITSIO'),
  ANL::SWIGClass.new("WriteHXIEventFITS", false, 'USE_FITSIO'),
  ANL::SWIGClass.new("FilterByGoodTimeIntervalsForSGD", false, 'USE_FITSIO'),
  ANL::SWIGClass.new("FilterByGoodTimeIntervalsForHXI", false, 'USE_FITSIO'),
]

classList = clMod + clSim + clAH
classList.each{|s|
  s.include_path = ["../../source/modules/include",
    "../../source/simulation/include",
    "../../source/astroh/include",
  ]
}

m = ANL::SWIGModule.new(name, classList, namespace)
m.includeFiles = ['class_list_anlGeant4.hh', 'comptonsoft_basic_classes.hh']
m.importModules = [
  {name: 'anlnext/ANL', file: 'anlnext/ruby/ANL.i'},
  {name: 'anlgeant4/anlGeant4', file: 'anlGeant4.i'},
]
m.includeModules = []

if ARGV.include? '-l'
  m.print_class_list
elsif ARGV.include? '-d'
  m.print_make_doc
else
  m.print_interface
end
