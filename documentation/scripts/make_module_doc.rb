#!/usr/bin/env ruby

require 'comptonsoft'

a = ANL::AnalysisChain.new

### ANLGeant4

a.push AnlGeant4::Geant4Body.new
a.text "Geant4 Run Manager module"

# a.push AnlGeant4::Geant4Simple.new
# a.text "Simple version of Geant4 Run Manager module. This module uses G4RunManager::BeamOn()."

# a.push AnlGeant4::VANLPhysicsList.new
# a.text ""

# a.push AnlGeant4::VANLGeometry.new
# a.text ""

a.push AnlGeant4::ReadGDML.new
a.text "This module reads a GDML mass model file and builds a geometry."

a.push AnlGeant4::WriteGDML.new
a.text "This module writes geometry built in Geant4 to a GDML format."

# a.push AnlGeant4::VANLPrimaryGen.new
# a.text ""

# a.push AnlGeant4::BasicPrimaryGen.new
# a.text "Basic primary generator of MC simulations."

a.push AnlGeant4::PointSourcePrimaryGen.new
a.text "Primary generator of MC simulations. Primaries are generated as point source emissions."

a.push AnlGeant4::PlaneWavePrimaryGen.new
a.text "Primary generator of MC simulations. Primaries are generated as a plane wave inside a circle."

a.push AnlGeant4::PlaneWaveRectanglePrimaryGen.new
a.text "Primary generator of MC simulations. Primaries are generated as a plane wave inside a rectangle."

a.push AnlGeant4::GaussianBeamPrimaryGen.new
a.text "Primary generator of MC simulations. Primaries are generated similarly to a plane wave, but has a Gaussian profile instead of a uniform circle."

a.push AnlGeant4::IsotropicPrimaryGen.new
a.text "Primary generator of MC simulations. Primaries are generated to be homogeneous and isotropic in a sphere."

a.push AnlGeant4::PrimaryGenUniformSourceInVolume.new
a.text "Primary generator of MC simulations. Primaries are generated to be homogeneous inside a specified volume."

a.push AnlGeant4::NucleusPrimaryGen.new
a.text "Primary generator of MC simulations. This module puts a nucleus (radioactive isotope) as a point source."

a.push AnlGeant4::NucleusPrimaryGenInVolume.new
a.text "Primary generator of MC simulations. This module puts a nucleus (radioactive isotope) whose position is sampled uniformly inside a specified volume."

a.push AnlGeant4::VUserActionAssembly.new
# a.text ""

a.push AnlGeant4::VMasterUserActionAssembly.new
# a.text ""

a.push AnlGeant4::VAppendableUserActionAssembly.new
# a.text ""

a.push AnlGeant4::StandardUserActionAssembly.new
a.text "A standard user actions to be set."

a.push AnlGeant4::VisualizeG4Geom.new
a.text "This module applies visualization for Geant4 simulation."

### ComptonSoft/modules

a.push ComptonSoft::ConstructDetector.new
a.text "Construct a detector system, loading a detector configuration file."

a.push ComptonSoft::ConstructDetectorForSimulation.new
# a.text ""

a.push ComptonSoft::VCSModule.new
# a.text ""

a.push ComptonSoft::CSHitCollection.new
a.text "A hit collection module."

a.push ComptonSoft::ConstructChannelMap.new
# a.text ""

a.push ComptonSoft::SetNoiseLevels.new
# a.text ""

a.push ComptonSoft::SetBadChannels.new
# a.text ""

a.push ComptonSoft::SetChannelProperties.new
# a.text ""

a.push ComptonSoft::SelectHits.new
# a.text ""

a.push ComptonSoft::AnalyzeHits.new
# a.text ""

a.push ComptonSoft::MakeDetectorHits.new
# a.text ""

a.push ComptonSoft::MakeRawHits.new
# a.text ""

a.push ComptonSoft::MakeDetectorHitsWithTimingProcess.new
# a.text ""

a.push ComptonSoft::ApplyEPICompensation.new
# a.text ""

a.push ComptonSoft::EventSelection.new
# a.text ""

a.push ComptonSoft::EventReconstruction.new
# a.text ""

a.push ComptonSoft::HXIEventSelection.new
# a.text ""

a.push ComptonSoft::CreateRootFile.new
# a.text ""

a.push ComptonSoft::SaveData.new
# a.text ""

a.push ComptonSoft::ReadDataFile.new
a.text "An abstract module for reading experimental data file."

a.push ComptonSoft::ReadDataFile_VME3.new
# a.text ""

a.push ComptonSoft::ReadDataFile_SpW2.new
# a.text ""

a.push ComptonSoft::ReadDataFile_NT.new
# a.text ""

a.push ComptonSoft::ReadDataFile_NB0.new
# a.text ""

a.push ComptonSoft::CorrectPHA.new
a.text "Subtract a pedestal level and a common mode noise from a PHA value for every channel."

a.push ComptonSoft::RecalculateEPI.new
# a.text ""

a.push ComptonSoft::CalculatePedestalLevels.new
# a.text ""

a.push ComptonSoft::InitialConditionFilter.new
# a.text ""

a.push ComptonSoft::WeightByInitialDirection.new
# a.text ""

a.push ComptonSoft::RecalculateSimulationNoise.new
# a.text ""

a.push ComptonSoft::ComptonModeFilter.new
# a.text ""

a.push ComptonSoft::SelectFullDeposit.new
# a.text ""

a.push ComptonSoft::ComptonEventFilter.new
# a.text ""

a.push ComptonSoft::SelectEventsOnFocalPlane.new
# a.text ""

a.push ComptonSoft::SelectTime.new
# a.text ""

a.push ComptonSoft::FilterByGoodTimeIntervals.new
# a.text ""

a.push ComptonSoft::WriteHitTree.new
# a.text ""

a.push ComptonSoft::ReadHitTree.new
# a.text ""

a.push ComptonSoft::ReadHitTreeAsRawHits.new
# a.text ""

a.push ComptonSoft::ReadHitTreeAsDetectorHits.new
# a.text ""

a.push ComptonSoft::WriteEventTree.new
# a.text ""

a.push ComptonSoft::ReadEventTree.new
# a.text ""

a.push ComptonSoft::ReadEventTreeAsRawHits.new
# a.text ""

a.push ComptonSoft::ReadEventTreeAsDetectorHits.new
# a.text ""

a.push ComptonSoft::WriteComptonEventTree.new
# a.text ""

a.push ComptonSoft::ReadComptonEventTree.new
# a.text ""

a.push ComptonSoft::HistogramPHA.new
# a.text ""

a.push ComptonSoft::HistogramEnergySpectrum.new
# a.text ""

a.push ComptonSoft::HistogramEnergy1D.new
# a.text ""

a.push ComptonSoft::HistogramEnergy2D.new
# a.text ""

a.push ComptonSoft::HistogramARM.new
# a.text ""

a.push ComptonSoft::HistogramARMByPositionMeasurement.new
# a.text ""

a.push ComptonSoft::HistogramAzimuthAngle.new
# a.text ""

a.push ComptonSoft::ResponseMatrix.new
# a.text ""

a.push ComptonSoft::BackProjection.new
# a.text ""

a.push ComptonSoft::BackProjectionSky.new
# a.text ""

a.push ComptonSoft::EfficiencyMapSky.new
# a.text ""

a.push ComptonSoft::QuickAnalysisForDSD.new
# a.text ""

a.push ComptonSoft::AHRayTracingPrimaryGen.new
a.text ""

# a.push ComptonSoft::SimXPrimaryGen.new
# a.text ""

a.push ComptonSoft::AHRadiationBackgroundPrimaryGen.new
# a.text ""

a.push ComptonSoft::RadioactiveDecayUserActionAssembly.new
# a.text ""

a.push ComptonSoft::ActivationUserActionAssembly.new
# a.text ""

# a.push ComptonSoft::AHStandardUserActionAssembly.new
# a.text ""

a.push ComptonSoft::SampleOpticalDepth.new
# a.text ""

a.push ComptonSoft::ScatteringPickUpData.new
# a.text ""

a.push ComptonSoft::ObservationPickUpData.new
# a.text ""

a.push ComptonSoft::PhysicsListManager.new
# a.text ""

# a.push ComptonSoft::SimXIF.new
# a.text ""

# a.push ComptonSoft::GenerateSimXEvent.new
# a.text ""

# a.push ComptonSoft::OutputSimXPrimaries.new
# a.text ""

a.push ComptonSoft::AssignG4CopyNumber.new
# a.text ""

a.push ComptonSoft::InitialParticleTree.new
# a.text ""

a.push ComptonSoft::DumpMass.new
# a.text ""

a.push ComptonSoft::WriteObservationTree.new
# a.text ""

a.push ComptonSoft::SimulateCXBShieldPlate.new
# a.text ""

a.push ComptonSoft::RescaleSimulationNoiseOfSGDSiUntriggered.new
# a.text ""

a.push ComptonSoft::UniformlyRandomizeEPI.new
# a.text ""

a.push ComptonSoft::ReadSGDEventFITS.new
# a.text ""

a.push ComptonSoft::WriteSGDEventFITS.new
# a.text ""

a.push ComptonSoft::ReadHXIEventFITS.new
# a.text ""

a.push ComptonSoft::WriteHXIEventFITS.new
# a.text ""

a.push ComptonSoft::FilterByGoodTimeIntervalsForSGD.new
# a.text ""

### make documentation
begin
  a.define
rescue RuntimeError => e
  puts e
  nil
ensure
  a.make_doc(output: "../html/cs_modules_detail.xml", namespace: "ComptonSoft")
end
