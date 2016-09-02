%module comptonSoft
%{
#include "ConstructDetector.hh"
#include "ConstructDetectorForSimulation.hh"
#include "VCSModule.hh"
#include "CSHitCollection.hh"
#include "ConstructChannelMap.hh"
#include "SetNoiseLevels.hh"
#include "SetBadChannels.hh"
#include "SetChannelProperties.hh"
#include "SelectHits.hh"
#include "AnalyzeHits.hh"
#include "MakeDetectorHits.hh"
#include "MakeRawHits.hh"
#include "MakeDetectorHitsWithTimingProcess.hh"
#include "ApplyEPICompensation.hh"
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
#include "ReadHitTreeAsRawHits.hh"
#include "ReadHitTreeAsDetectorHits.hh"
#include "WriteEventTree.hh"
#include "ReadEventTree.hh"
#include "ReadEventTreeAsRawHits.hh"
#include "ReadEventTreeAsDetectorHits.hh"
#include "WriteComptonEventTree.hh"
#include "ReadComptonEventTree.hh"
#include "HistogramPHA.hh"
#include "HistogramEnergySpectrum.hh"
#include "HistogramEnergy2D.hh"
#include "HistogramARM.hh"
#include "HistogramARMByPositionMeasurement.hh"
#include "HistogramAzimuthAngle.hh"
#include "ResponseMatrix.hh"
#include "BackProjection.hh"
#include "BackProjectionSky.hh"
#include "EfficiencyMapSky.hh"
#include "QuickAnalysisForDSD.hh"
#ifdef USE_SIMX
#include "AHRayTracingPrimaryGen.hh"
#endif
#ifdef USE_SIMX
#include "SimXPrimaryGen.hh"
#endif
#include "AHRadiationBackgroundPrimaryGen.hh"
#include "RadioactiveDecayPickUpData.hh"
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
#ifdef USE_FITSIO
#include "ReadSGDEventFITS.hh"
#endif
#ifdef USE_FITSIO
#include "WriteSGDEventFITS.hh"
#endif
#ifdef USE_FITSIO
#include "WriteHXIEventFITS.hh"
#endif

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


class SetNoiseLevels : public VCSModule
{
public:
  SetNoiseLevels();
  ~SetNoiseLevels();
};


class SetBadChannels : public VCSModule
{
public:
  SetBadChannels();
  ~SetBadChannels();
};


class SetChannelProperties : public VCSModule
{
public:
  SetChannelProperties();
  ~SetChannelProperties();
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


class MakeDetectorHitsWithTimingProcess : public MakeDetectorHits
{
public:
  MakeDetectorHitsWithTimingProcess() = default;
  ~MakeDetectorHitsWithTimingProcess() = default;
};


class ApplyEPICompensation : public VCSModule
{
public:
  ApplyEPICompensation();
  ~ApplyEPICompensation();
};


class EventSelection : public VCSModule
{
public:
  EventSelection();
  ~EventSelection() = default;
};


class EventReconstruction : public VCSModule
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


class ReadHitTreeAsRawHits : public ReadHitTree
{
public:
  ReadHitTreeAsRawHits();
  ~ReadHitTreeAsRawHits() = default;
};


class ReadHitTreeAsDetectorHits : public ReadHitTreeAsRawHits
{
public:
  ReadHitTreeAsDetectorHits() = default;
  ~ReadHitTreeAsDetectorHits() = default;
};


class WriteEventTree : public VCSModule
{
public:
  WriteEventTree();
  ~WriteEventTree() = default;
};


class ReadEventTree : public VCSModule
{
public:
  ReadEventTree();
  ~ReadEventTree();
};


class ReadEventTreeAsRawHits : public ReadEventTree
{
public:
  ReadEventTreeAsRawHits();
  ~ReadEventTreeAsRawHits() = default;
};


class ReadEventTreeAsDetectorHits : public ReadEventTreeAsRawHits
{
public:
  ReadEventTreeAsDetectorHits() = default;
  ~ReadEventTreeAsDetectorHits() = default;
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


class HistogramARMByPositionMeasurement : public HistogramARM
{
public:
  HistogramARMByPositionMeasurement();
  ~HistogramARMByPositionMeasurement();
};


class HistogramAzimuthAngle : public VCSModule
{
public:
  HistogramAzimuthAngle();
  ~HistogramAzimuthAngle() = default;
};


class ResponseMatrix : public VCSModule
{
public:
  ResponseMatrix();
  ~ResponseMatrix();
};


class BackProjection : public VCSModule
{
public:
  BackProjection();
  ~BackProjection();
};


class BackProjectionSky : public BackProjection
{
public:
  BackProjectionSky();
  ~BackProjectionSky();
};


class EfficiencyMapSky : public BackProjection
{
public:
  EfficiencyMapSky();
  ~EfficiencyMapSky();
};


class QuickAnalysisForDSD : public VCSModule
{
public:
  QuickAnalysisForDSD();
  ~QuickAnalysisForDSD();
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

class AHRadiationBackgroundPrimaryGen : public anlgeant4::IsotropicPrimaryGen
{
public:
  AHRadiationBackgroundPrimaryGen();
};


class RadioactiveDecayPickUpData : public anlgeant4::StandardPickUpData
{
public:
  RadioactiveDecayPickUpData();
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


#ifdef USE_FITSIO
class ReadSGDEventFITS : public VCSModule
{
public:
  ReadSGDEventFITS();
  ~ReadSGDEventFITS();
};

#endif

#ifdef USE_FITSIO
class WriteSGDEventFITS : public VCSModule
{
public:
  WriteSGDEventFITS();
  ~WriteSGDEventFITS();
};

#endif

#ifdef USE_FITSIO
class WriteHXIEventFITS : public VCSModule
{
public:
  WriteHXIEventFITS();
  ~WriteHXIEventFITS();
};

#endif

}
