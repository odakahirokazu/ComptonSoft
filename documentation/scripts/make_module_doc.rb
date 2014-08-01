#!/usr/bin/env ruby

require 'comptonSoft'
require 'anlGeant4'
require 'ANLLib'

a = ANL::AnalysisChain.new

a.push ComptonSoft::AnalyzeDSD.new
a.push ComptonSoft::AnalyzeHit.new
a.push ComptonSoft::ApplyNewPI.new
a.push ComptonSoft::AzimuthDistribution.new
a.push ComptonSoft::BackProjection.new
a.push ComptonSoft::CSHitCollection.new
a.text "A hit collection module."

a.push ComptonSoft::CalcARM.new
a.push ComptonSoft::CalcARMEne.new
a.push ComptonSoft::CalcARMPos.new
a.push ComptonSoft::CalcARM_PS.new
a.push ComptonSoft::CalcPedestal.new
a.text "This module calculates pedestal levels of each readout channels."

a.push ComptonSoft::CalcPedestal_SpW.new
a.text "This module calculates pedestal levels of each readout channels.  This is for 'SpW' file format."

a.push ComptonSoft::Clustering.new
a.push ComptonSoft::ComptonModeFilter.new
a.push ComptonSoft::ComptonModeSpectrum.new
a.push ComptonSoft::ComptonEventFilter.new
a.push ComptonSoft::ComptonTree.new
a.push ComptonSoft::ConstructChannelTable.new
a.push ComptonSoft::ConstructDetector.new
a.text "This loads a detector configuration file and constructs a detector system information."

a.push ComptonSoft::ConstructDetector_Sim.new
a.text "This loads a detector configuration file and constructs a detector system information. This module is for a simulation."

a.push ComptonSoft::CorrectPHA.new
a.text "Subtract a pedestal level and a common mode noise from a PHA value for every channel."

a.push ComptonSoft::DeadTimeTree_VME3.new
a.text "This module makes TTree of dead time information. This is for 'VME3' data format."

a.push ComptonSoft::DefineBadChannel.new
a.push ComptonSoft::DefineBadChannel_Sim.new
a.push ComptonSoft::Edep2DMap.new
a.push ComptonSoft::EventReconstruction.new
a.push ComptonSoft::EventSelection.new
a.push ComptonSoft::EventThreshold.new
a.push ComptonSoft::EventTree.new
a.push ComptonSoft::FilterFullDeposit.new
a.push ComptonSoft::HistPHA.new
a.push ComptonSoft::HistPI.new
a.push ComptonSoft::HitTree.new
a.push ComptonSoft::HitTree_Sim.new
a.push ComptonSoft::InitialConditionFilter.new
a.push ComptonSoft::MakeDetectorHit.new
a.push ComptonSoft::MakeDetectorHitTR.new
a.push ComptonSoft::MakePI.new
a.push ComptonSoft::MakeRawHit.new
a.push ComptonSoft::PhAbsModeSpectrum.new
a.push ComptonSoft::RawTree.new
a.push ComptonSoft::RawTree_SpW.new
a.push ComptonSoft::ReadComptonTree.new
a.push ComptonSoft::ReadDataFile.new
a.text "An abstract module for reading experimental data file."

# a.push ComptonSoft::ReadDataFile_NB0.new
a.push ComptonSoft::ReadDataFile_NT.new
a.push ComptonSoft::ReadDataFile_SpW2.new
a.push ComptonSoft::ReadDataFile_VME3.new
a.push ComptonSoft::ReadDetectorHitTree.new
a.push ComptonSoft::ReadHitTree.new
a.push ComptonSoft::ReadRawHitTree.new
a.push ComptonSoft::ReprocessPI.new
a.push ComptonSoft::Resp2DSpec.new
a.push ComptonSoft::SaveData.new
a.push ComptonSoft::SelectHit.new
a.push ComptonSoft::SelectTime.new
a.push ComptonSoft::SetNoiseLevel.new
a.push ComptonSoft::SetSimGainCorrection.new
# a.push ComptonSoft::VCSModule.new

# simulation

# a.push ComptonSoft::AHG4ANLVis.new

a.push ComptonSoft::ActivationPickUpData.new
a.push ComptonSoft::IsotropicPrimaryGen.new
a.text "Primary generator of MC simulations. Primaries are generated uniform isotropically in a sphere."

a.push ComptonSoft::RDPickUpData.new
a.push ComptonSoft::KillStepPickUpData.new
a.push ComptonSoft::MaterialSamplePickUpData.new
a.push ComptonSoft::NucleusPrimaryGen.new
a.text "Primary generator of MC simulations. This module puts a nucleus (radioactive isotope) as a point source."

a.push AnlGeant4::PlaneWavePrimaryGen.new
a.text "Primary generator of MC simulations. Primaries are generated as a plane wave inside a circle."

a.push ComptonSoft::PlaneWaveRectanglePrimaryGen.new
a.text "Primary generator of MC simulations. Primaries are generated as a plane wave inside a rectangle."

a.push AnlGeant4::PointSourcePrimaryGen.new
a.text "Primary generator of MC simulations. Primaries are generated as point source emissions."

a.push ComptonSoft::PrimaryGenInVolume.new
a.text "Primary generator of MC simulations. Primaries are generated uniformly inside an specified volume."

a.push ComptonSoft::NucleusPrimaryGenInVolume.new
a.text "Primary generator of MC simulations. This module puts a nucleus (radioactive isotope) uniformly inside a specified volume."


# ANLGeant4

a.push AnlGeant4::Geant4Body.new
a.text "Geant4 run manager module"
#a.push AnlGeant4::Geant4Simple.new
#a.text "Geant4 run manager module, simple version"

a.push AnlGeant4::ReadGDML.new
a.text "This module reads GDML mass model file and builds the geometry."

a.push AnlGeant4::WriteGDML.new
a.text "This module writes geometry built in Geant4 to GDML format."

a.push AnlGeant4::BasicPrimaryGen.new
a.text "Basic primary generator of MC simulations."

a.push AnlGeant4::StandardPickUpData.new

a.push AnlGeant4::VisualizeG4Geom.new
a.text "This module applies visualization for Geant4 simulation."


a.startup
a.make_doc "../html/cs_modules_detail.xml", "Compton Soft modules"
