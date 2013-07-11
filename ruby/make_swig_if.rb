#!/usr/bin/env ruby
require 'ANLSwigClass'

name = 'comptonSoft'
namespace = 'comptonsoft'

clSim = [# primary generator
         ANLSwigClass.new('PlaneWavePrimaryGen'),
         ANLSwigClass.new('PlaneWavePrimaryGenPol', false, 'CS_BASIC2'),
         ANLSwigClass.new('PlaneWaveRectanglePrimaryGen'),
         ANLSwigClass.new('PlaneWaveRectanglePrimaryGenPol', false, 'CS_BASIC2'),
         ANLSwigClass.new('PointSourcePrimaryGen'),
         ANLSwigClass.new('IsotropicPrimaryGen'),
         ANLSwigClass.new('PrimaryGenInVolume', false, 'CS_BASIC2'),
         ANLSwigClass.new('NucleusPrimaryGen'),
         ANLSwigClass.new('NucleusPrimaryGenInVolume', false, 'CS_BASIC2'),
         ANLSwigClass.new('AHRayTracingPrimaryGen', false, 'USE_SIMX'),
         ANLSwigClass.new('SimXPrimaryGen', false, 'USE_SIMX'),
         ANLSwigClass.new('SpenvisIsotropicPrimaryGen'),
         ANLSwigClass.new('AHRadiationBackgroundPrimaryGen', false, 'CS_BASIC2'),
         # pick up data
         ANLSwigClass.new('RDPickUpData'),
         ANLSwigClass.new('ActivationPickUpData'),
         ANLSwigClass.new('AHStandardPickUpData', false, 'USE_SIMX'),
         ANLSwigClass.new('KillStepPickUpData', false, 'CS_BASIC2'),
         ANLSwigClass.new('MaterialSamplePickUpData', false, 'CS_BASIC2'),
         ANLSwigClass.new('ScatteringPickUpData', false, 'CS_BASIC2'),
         # physics list
         ANLSwigClass.new('AHStandardANLPhysicsList'),
         # others
         ANLSwigClass.new('SimXIF', false, 'USE_SIMX'),
         ANLSwigClass.new('GenerateSimXEvent', false, 'USE_SIMX'),
         ANLSwigClass.new('OutputSimXPrimaries', false, 'USE_SIMX'),
         ANLSwigClass.new('AssignG4CopyNumber'),
         ANLSwigClass.new('InitialParticleTree', false, 'CS_BASIC2'),
         ANLSwigClass.new('DumpMass'),
        ]

clMod = [ANLSwigClass.new('VCSModule'),
         ANLSwigClass.new('SelectHit'),
         ANLSwigClass.new('MakePI', false, 'CS_BASIC2'),
         ANLSwigClass.new('VAnalyzeHitList', false, 'CS_BASIC2'),
         ANLSwigClass.new('AnalyzeHit', false, 'CS_BASIC2'),
         ANLSwigClass.new('ApplyNewPI', false, 'CS_BASIC2'),
         ANLSwigClass.new('AzimuthDistribution', false, 'CS_BASIC2'),
         ANLSwigClass.new('BackProjection', false, 'CS_BASIC2'),
         ANLSwigClass.new('BackProjectionSky', false, 'CS_BASIC2'),
         ANLSwigClass.new('CSHitCollection'),
         ANLSwigClass.new('CalcARM', false, 'CS_BASIC2'),
         ANLSwigClass.new('CalcARMEne', false, 'CS_BASIC2'),
         ANLSwigClass.new('CalcARMPos', false, 'CS_BASIC2'),
         ANLSwigClass.new('CalcARM_PS', false, 'CS_BASIC2'),
         ANLSwigClass.new('CalcPedestal', false, 'CS_BASIC2'),
         ANLSwigClass.new('CalcPedestal_SpW', false, 'CS_BASIC2'),
         ANLSwigClass.new('Clustering', false, 'CS_BASIC2'),
         ANLSwigClass.new('ComptonModeFilter', false, 'CS_BASIC2'),
         ANLSwigClass.new('ComptonModeSpectrum', false, 'CS_BASIC2'),
         ANLSwigClass.new('ComptonTree', false, 'CS_BASIC2'),
         ANLSwigClass.new('ConstructChannelTable'),
         ANLSwigClass.new('ConstructDetector'),
         ANLSwigClass.new('ConstructDetector_Sim'),
         ANLSwigClass.new('CorrectPHA', false, 'CS_BASIC2'),
         ANLSwigClass.new('DeadTimeTree_VME3', false, 'CS_BASIC2'),
         ANLSwigClass.new('DefineBadChannel'),
         ANLSwigClass.new('DefineBadChannel_Sim'),
         ANLSwigClass.new('Edep2DMap', false, 'CS_BASIC2'),
         ANLSwigClass.new('SelectHitPattern', false, 'CS_BASIC2'),
         ANLSwigClass.new('EventReconstruction'),
         ANLSwigClass.new('EventSelection'),
         ANLSwigClass.new('EventThreshold', false, 'CS_BASIC2'),
         ANLSwigClass.new('EventTree', false, 'CS_BASIC2'),
         ANLSwigClass.new('FilterFullDeposit', false, 'CS_BASIC2'),
         ANLSwigClass.new('HistPHA', false, 'CS_BASIC2'),
         ANLSwigClass.new('HistPI', false, 'CS_BASIC2'),
         ANLSwigClass.new('HitTree', false, 'CS_BASIC2'),
         ANLSwigClass.new('HitTree_Sim', false, 'CS_BASIC2'),
         ANLSwigClass.new('InitialConditionFilter', false, 'CS_BASIC2'),
         ANLSwigClass.new('MakeDetectorHit'),
         ANLSwigClass.new('MakeDetectorHitTR'),
         ANLSwigClass.new('MakeRawHit'),
         ANLSwigClass.new('PhAbsModeSpectrum', false, 'CS_BASIC2'),
         ANLSwigClass.new('RawTree', false, 'CS_BASIC2'),
         ANLSwigClass.new('RawTree_SpW', false, 'CS_BASIC2'),
         ANLSwigClass.new('ReadComptonTree', false, 'CS_BASIC2'),
         ANLSwigClass.new('ReadDataFile', false, 'CS_BASIC2'),
         ANLSwigClass.new('ReadDataFile_NT', false, 'CS_BASIC2'),
         ANLSwigClass.new('ReadDataFile_NB0', false, 'CS_BASIC2'),
         ANLSwigClass.new('ReadDataFile_SpW2', false, 'CS_BASIC2'),
         ANLSwigClass.new('ReadDataFile_VME3', false, 'CS_BASIC2'),
         ANLSwigClass.new('ReadHitTree', false, 'CS_BASIC2'),
         ANLSwigClass.new('ReadRawHitTree', false, 'CS_BASIC2'),
         ANLSwigClass.new('ReadDetectorHitTree', false, 'CS_BASIC2'),
         ANLSwigClass.new('ReprocessPI', false, 'CS_BASIC2'),
         ANLSwigClass.new('EnergySpectrum', false, 'CS_BASIC2'),
         ANLSwigClass.new('Resp2DSpec', false, 'CS_BASIC2'),
         ANLSwigClass.new('ResponseMatrix', false, 'CS_BASIC2'),
         ANLSwigClass.new('SaveData', false, 'CS_BASIC2'),
         ANLSwigClass.new('SetNoiseLevel'),
         ANLSwigClass.new('SetSimGainCorrection', false, 'CS_BASIC2'),
         ANLSwigClass.new('GainCorrection', false, 'CS_BASIC2'),
         ANLSwigClass.new('ComptonEventFilter', true, 'CS_BASIC2'),
         ANLSwigClass.new('SelectTime', false, 'CS_BASIC2'),
         ANLSwigClass.new('AnalyzeDSD', false, 'CS_BASIC2'),
         ANLSwigClass.new('EfficiencyMapSky', false, 'CS_BASIC2'),
         ANLSwigClass.new('HXIEventSelection'),
        ]

classList = clMod + clSim
classList.each{|s|
  s.include_path = ["../source/modules/include",
                    "../source/simulation/include"]
}

m = ANLSwigModule.new(name, classList, namespace)
m.includeFiles = ['class_list_anlGeant4.hh']
m.importModules = ['anl.i', 'anlGeant4.i']
m.includeModules = []

if ARGV.include? '-l'
  m.print_class_list
else
  m.print_interface
end
