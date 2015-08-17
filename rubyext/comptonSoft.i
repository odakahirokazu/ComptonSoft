%module comptonSoft
%{
#include "ConstructDetector.hh"
#include "ConstructDetectorForSimulation.hh"
#include "VCSModule.hh"
#include "CSHitCollection.hh"
#include "ConstructChannelMap.hh"
#include "DefineBadChannels.hh"
#include "SetNoiseLevels.hh"
#include "SetChannelsInfo.hh"
#include "SelectHits.hh"
#include "AnalyzeHits.hh"
#include "MakeDetectorHits.hh"
#include "MakeRawHits.hh"
#include "MakeDetectorHitsTR.hh"
#include "ApplyEPICompensation.hh"
#include "DetectorGroupManager.hh"
#include "EventSelection.hh"
#include "EventReconstruction.hh"
#include "HXIEventSelection.hh"
#include "SaveData.hh"
#include "ReadDataFile.hh"
#include "ReadDataFile_VME3.hh"
#include "ReadDataFile_SpW2.hh"
#include "ReadDataFile_NT.hh"
#include "ReadDataFile_NB0.hh"
#include "CorrectPHA.hh"
#include "RecalculateEPI.hh"
#include "CalculatePedestalLevels.hh"
#include "InitialConditionFilter.hh"
#include "RecalculateSimulationNoise.hh"
#include "ComptonModeFilter.hh"
#include "SelectFullDeposit.hh"
#include "ComptonEventFilter.hh"
#include "SelectTime.hh"
#include "WriteHitTree.hh"
#include "ReadHitTree.hh"
#include "ReadRawHitTree.hh"
#include "ReadDetectorHitTree.hh"
#include "WriteComptonEventTree.hh"
#include "ReadComptonEventTree.hh"
#include "HistogramPHA.hh"
#include "HistogramEnergySpectrum.hh"
#include "HistogramEnergy2D.hh"
#include "HistogramARM.hh"
#include "HistogramAzimuthAngle.hh"
#include "PlaneWaveRectanglePrimaryGen.hh"
#include "IsotropicPrimaryGen.hh"
#include "NucleusPrimaryGen.hh"
#include "NucleusPrimaryGenInVolume.hh"
#ifdef USE_SIMX
#include "AHRayTracingPrimaryGen.hh"
#endif
#ifdef USE_SIMX
#include "SimXPrimaryGen.hh"
#endif
#include "AHRadiationBackgroundPrimaryGen.hh"
#include "RDPickUpData.hh"
#include "ActivationPickUpData.hh"
#ifdef USE_SIMX
#include "AHStandardPickUpData.hh"
#endif
#include "KillStepPickUpData.hh"
#include "MaterialSamplePickUpData.hh"
#include "ScatteringPickUpData.hh"
#include "PhysicsListManager.hh"
#ifdef USE_SIMX
#include "SimXIF.hh"
#endif
#ifdef USE_SIMX
#include "GenerateSimXEvent.hh"
#endif
#ifdef USE_SIMX
#include "OutputSimXPrimaries.hh"
#endif
#include "AssignG4CopyNumber.hh"
#include "InitialParticleTree.hh"
#include "DumpMass.hh"

#include "class_list_anlGeant4.hh"
#include "comptonsoft_basic_classes.hh"

%}


%import "ANL.i"
%import "anlGeant4.i"

namespace comptonsoft {

class ConstructDetector : public anl::BasicModule
{
public:
  ConstructDetector();
  ~ConstructDetector();
};


class ConstructDetectorForSimulation : public ConstructDetector
{
public:
  ConstructDetectorForSimulation();
  ~ConstructDetectorForSimulation();
};


class VCSModule : public anl::BasicModule
{
public:
  VCSModule();
  ~VCSModule();
};


class CSHitCollection : public anl::BasicModule
{
public:
  CSHitCollection();
  ~CSHitCollection();
};


class ConstructChannelMap : public VCSModule
{
public:
  ConstructChannelMap();
  ~ConstructChannelMap();
};


class DefineBadChannels : public VCSModule
{
public:
  DefineBadChannels();
  ~DefineBadChannels();
};


class SetNoiseLevels : public VCSModule
{
public:
  SetNoiseLevels();
  ~SetNoiseLevels();
};


class SetChannelsInfo : public VCSModule
{
public:
  SetChannelsInfo();
  ~SetChannelsInfo();
};


class SelectHits : public VCSModule
{
public:
  SelectHits();
  ~SelectHits();
};


class AnalyzeHits : public SelectHits
{
public:
  AnalyzeHits() = default;
  ~AnalyzeHits() = default;
};


class MakeDetectorHits : public SelectHits
{
public:
  MakeDetectorHits() = default;
  ~MakeDetectorHits() = default;
};


class MakeRawHits : public SelectHits
{
public:
  MakeRawHits() = default;
  ~MakeRawHits() = default;
};


class MakeDetectorHitsTR : public MakeDetectorHits
{
public:
  MakeDetectorHitsTR() = default;
  ~MakeDetectorHitsTR() = default;
};


class ApplyEPICompensation : public VCSModule
{
public:
  ApplyEPICompensation();
  ~ApplyEPICompensation();
};


class DetectorGroupManager : public VCSModule
{
public:
  DetectorGroupManager();
  ~DetectorGroupManager() = default;
};


class EventSelection : public VCSModule
{
public:
  EventSelection();
  ~EventSelection() = default;
};


class EventReconstruction : public anl::BasicModule
{
public:
  EventReconstruction();
  ~EventReconstruction() = default;
};


class HXIEventSelection : public VCSModule
{
public:
  HXIEventSelection() = default;
  ~HXIEventSelection() = default;
};


class SaveData : public anl::BasicModule
{
public:
  SaveData();
  ~SaveData() = default;
};


class ReadDataFile : public VCSModule
{
public:
  ReadDataFile();
  ~ReadDataFile() = default;
};


class ReadDataFile_VME3 : public ReadDataFile
{
public:
  ReadDataFile_VME3();
  ~ReadDataFile_VME3() = default;
};


class ReadDataFile_SpW2 : public ReadDataFile
{
public:
  ReadDataFile_SpW2();
  ~ReadDataFile_SpW2() = default;
};


class ReadDataFile_NT : public ReadDataFile
{
public:
  ReadDataFile_NT() = default;
  ~ReadDataFile_NT() = default;
};


class ReadDataFile_NB0 : public ReadDataFile
{
public:
  ReadDataFile_NB0();
  ~ReadDataFile_NB0() = default;
};


class CorrectPHA : public VCSModule
{
public:
  };


class RecalculateEPI : public CorrectPHA
{
public:
  RecalculateEPI() = default;
  ~RecalculateEPI() = default;
};


class CalculatePedestalLevels : public VCSModule
{
public:
  CalculatePedestalLevels();
  ~CalculatePedestalLevels() = default;
};


class InitialConditionFilter : public anl::BasicModule
{
public:
  InitialConditionFilter();
  ~InitialConditionFilter() = default;
};


class RecalculateSimulationNoise : public VCSModule
{
public:
  RecalculateSimulationNoise();
  ~RecalculateSimulationNoise() = default;
};


class ComptonModeFilter : public VCSModule
{
public:
  ComptonModeFilter();
  ~ComptonModeFilter() = default;
};


class SelectFullDeposit : public anl::BasicModule
{
public:
  SelectFullDeposit() = default;
  ~SelectFullDeposit() = default;
};


class ComptonEventFilter : public anl::BasicModule
{
public:
  ComptonEventFilter();
  ~ComptonEventFilter() {}

  void define_condition();
  void add_condition(const std::string& type,
                     double min_value, double max_value);
};


class SelectTime : public anl::BasicModule
{
public:
  SelectTime();
  ~SelectTime() = default;
};


class WriteHitTree : public VCSModule
{
public:
  WriteHitTree();
  ~WriteHitTree() = default;
};


class ReadHitTree : public VCSModule
{
public:
  ReadHitTree();
  ~ReadHitTree();
};


class ReadRawHitTree : public ReadHitTree
{
public:
  ReadRawHitTree();
  ~ReadRawHitTree() = default;
};


class ReadDetectorHitTree : public ReadRawHitTree
{
public:
  ReadDetectorHitTree() = default;
  ~ReadDetectorHitTree() = default;
};


class WriteComptonEventTree : public VCSModule
{
public:
  WriteComptonEventTree();
  ~WriteComptonEventTree();
};


class ReadComptonEventTree : public EventReconstruction
{
public:
  ReadComptonEventTree();
  ~ReadComptonEventTree() = default;
};


class HistogramPHA : public VCSModule
{
public:
  HistogramPHA();
  ~HistogramPHA() = default;
};


class HistogramEnergySpectrum : public VCSModule
{
public:
  HistogramEnergySpectrum();  
  ~HistogramEnergySpectrum() = default;
};


class HistogramEnergy2D : public VCSModule
{
public:
  HistogramEnergy2D();
  ~HistogramEnergy2D() = default;
};


class HistogramARM : public VCSModule
{
public:
  HistogramARM();
  ~HistogramARM() = default;
};


class HistogramAzimuthAngle : public VCSModule
{
public:
  HistogramAzimuthAngle();
  ~HistogramAzimuthAngle() = default;
};


class PlaneWaveRectanglePrimaryGen : public anlgeant4::PlaneWavePrimaryGen
{
public:
  PlaneWaveRectanglePrimaryGen();
};


class IsotropicPrimaryGen : public anlgeant4::BasicPrimaryGen
{
public:
  IsotropicPrimaryGen();
};


class NucleusPrimaryGen : public anlgeant4::BasicPrimaryGen
{
public:
  NucleusPrimaryGen();
};


class NucleusPrimaryGenInVolume : public NucleusPrimaryGen
{
public:
  NucleusPrimaryGenInVolume();
  virtual ~NucleusPrimaryGenInVolume() = default;
};


#ifdef USE_SIMX
class AHRayTracingPrimaryGen : public anlgeant4::BasicPrimaryGen
{
public:
  AHRayTracingPrimaryGen();
};

#endif

#ifdef USE_SIMX
class SimXPrimaryGen : public anlgeant4::BasicPrimaryGen
{
public:
  SimXPrimaryGen();
  ~SimXPrimaryGen();
};

#endif

class AHRadiationBackgroundPrimaryGen : public IsotropicPrimaryGen
{
public:
  AHRadiationBackgroundPrimaryGen();
};


class RDPickUpData : public anlgeant4::StandardPickUpData
{
public:
  RDPickUpData();
};


class ActivationPickUpData : public anlgeant4::StandardPickUpData
{
public:
  ActivationPickUpData();
  virtual ~ActivationPickUpData();
};


#ifdef USE_SIMX
class AHStandardPickUpData : public anlgeant4::StandardPickUpData
{
public:
  AHStandardPickUpData();
  virtual ~AHStandardPickUpData();
};

#endif

class KillStepPickUpData : public anlgeant4::StandardPickUpData
{
public:
  KillStepPickUpData();
};


class MaterialSamplePickUpData : public anlgeant4::StandardPickUpData
{
public:
  MaterialSamplePickUpData();
};


class ScatteringPickUpData : public anlgeant4::StandardPickUpData
{
public:
  ScatteringPickUpData();
};


class PhysicsListManager : public anlgeant4::VANLPhysicsList
{
public:
  PhysicsListManager();
  ~PhysicsListManager() = default;
};


#ifdef USE_SIMX
class SimXIF : public anl::BasicModule
{
public:
  SimXIF();
  ~SimXIF();
};

#endif

#ifdef USE_SIMX
class GenerateSimXEvent : public anl::BasicModule
{
public:
  GenerateSimXEvent();
  ~GenerateSimXEvent() {}
};

#endif

#ifdef USE_SIMX
class OutputSimXPrimaries : public anl::BasicModule
{
public:
  OutputSimXPrimaries();
  ~OutputSimXPrimaries() {}
};

#endif

class AssignG4CopyNumber : public anl::BasicModule
{
public:
  AssignG4CopyNumber();
  ~AssignG4CopyNumber();
};


class InitialParticleTree : public VCSModule
{
public:
  InitialParticleTree();
  ~InitialParticleTree() {}
};


class DumpMass : public anl::BasicModule
{
public:
  DumpMass();
  ~DumpMass();
};


}
