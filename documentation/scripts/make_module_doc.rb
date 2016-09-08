#!/usr/bin/env ruby

require 'comptonsoft/comptonSoft'
require 'anlGeant4'
require 'ANLLib'

a = ANL::AnalysisChain.new

### ANLGeant4
a.push AnlGeant4::Geant4Body.new
a.text "Geant4 run manager module"

# a.push AnlGeant4::Geant4Simple.new
# a.text "Geant4 run manager module, simple version"

# a.push AnlGeant4::VANLPhysicsList.new

# a.push AnlGeant4::VANLGeometry.new

a.push AnlGeant4::ReadGDML.new
a.text "This module reads GDML mass model file and builds the geometry."

a.push AnlGeant4::WriteGDML.new
a.text "This module writes geometry built in Geant4 to GDML format."

# a.push AnlGeant4::VANLPrimaryGen.new

a.push AnlGeant4::BasicPrimaryGen.new
a.text "Basic primary generator of MC simulations."

a.push AnlGeant4::PointSourcePrimaryGen.new
a.text "Primary generator of MC simulations. Primaries are generated as point source emissions."

a.push AnlGeant4::PlaneWavePrimaryGen.new
a.text "Primary generator of MC simulations. Primaries are generated as a plane wave inside a circle."

a.push AnlGeant4::PlaneWaveRectanglePrimaryGen.new
a.text "Primary generator of MC simulations. Primaries are generated as a plane wave inside a rectangle."

a.push AnlGeant4::IsotropicPrimaryGen.new
a.text "Primary generator of MC simulations. Primaries are generated uniform isotropically in a sphere."

a.push AnlGeant4::PrimaryGenUniformSourceInVolume.new
a.text "Primary generator of MC simulations. Primaries are generated uniformly inside an specified volume."

a.push AnlGeant4::NucleusPrimaryGen.new
a.text "Primary generator of MC simulations. This module puts a nucleus (radioactive isotope) as a point source."

a.push AnlGeant4::NucleusPrimaryGenInVolume.new
a.text "Primary generator of MC simulations. This module puts a nucleus (radioactive isotope) uniformly inside a specified volume."

a.push AnlGeant4::VPickUpData.new

a.push AnlGeant4::StandardPickUpData.new

a.push AnlGeant4::VisualizeG4Geom.new
a.text "This module applies visualization for Geant4 simulation."

### ComptonSoft/modules

a.push ComptonSoft::ConstructDetector.new
a.text "Construct a detector system, loading a detector configuration file."

a.push ComptonSoft::ConstructDetectorForSimulation.new

a.push ComptonSoft::VCSModule.new

a.push ComptonSoft::CSHitCollection.new
a.text "A hit collection module."

a.push ComptonSoft::ConstructChannelMap.new

a.push ComptonSoft::SetChannelProperties.new

a.push ComptonSoft::SelectHits.new

a.push ComptonSoft::AnalyzeHits.new

a.push ComptonSoft::MakeDetectorHits.new

a.push ComptonSoft::MakeDetectorHitsWithTimingProcess.new

a.push ComptonSoft::MakeRawHits.new

a.push ComptonSoft::ApplyEPICompensation.new

a.push ComptonSoft::EventSelection.new

a.push ComptonSoft::EventReconstruction.new

a.push ComptonSoft::HXIEventSelection.new

a.push ComptonSoft::SaveData.new

a.push ComptonSoft::ReadDataFile.new
a.text "An abstract module for reading experimental data file."

a.push ComptonSoft::ReadDataFile_VME3.new

a.push ComptonSoft::ReadDataFile_SpW2.new

a.push ComptonSoft::ReadDataFile_NT.new

a.push ComptonSoft::ReadDataFile_NB0.new

a.push ComptonSoft::CorrectPHA.new
a.text "Subtract a pedestal level and a common mode noise from a PHA value for every channel."

a.push ComptonSoft::RecalculateEPI.new

a.push ComptonSoft::CalculatePedestalLevels.new

a.push ComptonSoft::InitialConditionFilter.new

a.push ComptonSoft::RecalculateSimulationNoise.new

a.push ComptonSoft::ComptonModeFilter.new

a.push ComptonSoft::SelectFullDeposit.new

a.push ComptonSoft::ComptonEventFilter.new

a.push ComptonSoft::SelectTime.new

a.push ComptonSoft::WriteHitTree.new

a.push ComptonSoft::ReadHitTree.new

a.push ComptonSoft::ReadHitTreeAsRawHits.new

a.push ComptonSoft::ReadHitTreeAsDetectorHits.new

a.push ComptonSoft::WriteEventTree.new

a.push ComptonSoft::ReadEventTree.new

a.push ComptonSoft::ReadEventTreeAsRawHits.new

a.push ComptonSoft::ReadEventTreeAsDetectorHits.new

a.push ComptonSoft::WriteComptonEventTree.new

a.push ComptonSoft::ReadComptonEventTree.new

a.push ComptonSoft::HistogramPHA.new

a.push ComptonSoft::HistogramEnergySpectrum.new

a.push ComptonSoft::HistogramEnergy2D.new

a.push ComptonSoft::HistogramARM.new

a.push ComptonSoft::HistogramARMByPositionMeasurement.new

a.push ComptonSoft::HistogramAzimuthAngle.new

a.push ComptonSoft::ResponseMatrix.new

a.push ComptonSoft::BackProjection.new

a.push ComptonSoft::BackProjectionSky.new

a.push ComptonSoft::EfficiencyMapSky.new

a.push ComptonSoft::QuickAnalysisForDSD.new

### ComotonSoft/simulation

# a.push ComptonSoft::AHRayTracingPrimaryGen.new

# a.push ComptonSoft::SimXPrimaryGen.new

a.push ComptonSoft::AHRadiationBackgroundPrimaryGen.new

a.push ComptonSoft::RadioactiveDecayPickUpData.new

a.push ComptonSoft::ActivationPickUpData.new

# a.push ComptonSoft::AHStandardPickUpData.new

a.push ComptonSoft::KillStepPickUpData.new

a.push ComptonSoft::MaterialSamplePickUpData.new

a.push ComptonSoft::ScatteringPickUpData.new

a.push ComptonSoft::PhysicsListManager.new

# a.push ComptonSoft::SimXIF.new

# a.push ComptonSoft::GenerateSimXEvent.new

# a.push ComptonSoft::OutputSimXPrimaries.new

a.push ComptonSoft::AssignG4CopyNumber.new

a.push ComptonSoft::InitialParticleTree.new

a.push ComptonSoft::DumpMass.new

### make documentation
a.startup
a.make_doc(output: "../html/cs_modules_detail.xml", namespace: "ComptonSoft")
