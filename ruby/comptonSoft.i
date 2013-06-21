%module comptonSoft
%{
#include "VCSModule.hh"
#include "SelectHit.hh"
#ifdef CS_BASIC2
#include "MakePI.hh"
#endif
#ifdef CS_BASIC2
#include "VAnalyzeHitList.hh"
#endif
#ifdef CS_BASIC2
#include "AnalyzeHit.hh"
#endif
#ifdef CS_BASIC2
#include "ApplyNewPI.hh"
#endif
#ifdef CS_BASIC2
#include "AzimuthDistribution.hh"
#endif
#ifdef CS_BASIC2
#include "BackProjection.hh"
#endif
#ifdef CS_BASIC2
#include "BackProjectionSky.hh"
#endif
#include "CSHitCollection.hh"
#ifdef CS_BASIC2
#include "CalcARM.hh"
#endif
#ifdef CS_BASIC2
#include "CalcARMEne.hh"
#endif
#ifdef CS_BASIC2
#include "CalcARMPos.hh"
#endif
#ifdef CS_BASIC2
#include "CalcARM_PS.hh"
#endif
#ifdef CS_BASIC2
#include "CalcPedestal.hh"
#endif
#ifdef CS_BASIC2
#include "CalcPedestal_SpW.hh"
#endif
#ifdef CS_BASIC2
#include "Clustering.hh"
#endif
#ifdef CS_BASIC2
#include "ComptonModeFilter.hh"
#endif
#ifdef CS_BASIC2
#include "ComptonModeSpectrum.hh"
#endif
#ifdef CS_BASIC2
#include "ComptonTree.hh"
#endif
#include "ConstructChannelTable.hh"
#include "ConstructDetector.hh"
#include "ConstructDetector_Sim.hh"
#ifdef CS_BASIC2
#include "CorrectPHA.hh"
#endif
#ifdef CS_BASIC2
#include "DeadTimeTree_VME3.hh"
#endif
#include "DefineBadChannel.hh"
#include "DefineBadChannel_Sim.hh"
#ifdef CS_BASIC2
#include "Edep2DMap.hh"
#endif
#ifdef CS_BASIC2
#include "SelectHitPattern.hh"
#endif
#include "EventReconstruction.hh"
#include "EventSelection.hh"
#ifdef CS_BASIC2
#include "EventThreshold.hh"
#endif
#ifdef CS_BASIC2
#include "EventTree.hh"
#endif
#ifdef CS_BASIC2
#include "FilterFullDeposit.hh"
#endif
#ifdef CS_BASIC2
#include "HistPHA.hh"
#endif
#ifdef CS_BASIC2
#include "HistPI.hh"
#endif
#ifdef CS_BASIC2
#include "HitTree.hh"
#endif
#ifdef CS_BASIC2
#include "HitTree_Sim.hh"
#endif
#ifdef CS_BASIC2
#include "InitialConditionFilter.hh"
#endif
#include "MakeDetectorHit.hh"
#include "MakeDetectorHitTR.hh"
#include "MakeRawHit.hh"
#ifdef CS_BASIC2
#include "PhAbsModeSpectrum.hh"
#endif
#ifdef CS_BASIC2
#include "RawTree.hh"
#endif
#ifdef CS_BASIC2
#include "RawTree_SpW.hh"
#endif
#ifdef CS_BASIC2
#include "ReadComptonTree.hh"
#endif
#ifdef CS_BASIC2
#include "ReadDataFile.hh"
#endif
#ifdef CS_BASIC2
#include "ReadDataFile_NT.hh"
#endif
#ifdef CS_BASIC2
#include "ReadDataFile_NB0.hh"
#endif
#ifdef CS_BASIC2
#include "ReadDataFile_SpW2.hh"
#endif
#ifdef CS_BASIC2
#include "ReadDataFile_VME3.hh"
#endif
#ifdef CS_BASIC2
#include "ReadHitTree.hh"
#endif
#ifdef CS_BASIC2
#include "ReadRawHitTree.hh"
#endif
#ifdef CS_BASIC2
#include "ReadDetectorHitTree.hh"
#endif
#ifdef CS_BASIC2
#include "ReprocessPI.hh"
#endif
#ifdef CS_BASIC2
#include "EnergySpectrum.hh"
#endif
#ifdef CS_BASIC2
#include "Resp2DSpec.hh"
#endif
#ifdef CS_BASIC2
#include "ResponseMatrix.hh"
#endif
#ifdef CS_BASIC2
#include "SaveData.hh"
#endif
#include "SetNoiseLevel.hh"
#ifdef CS_BASIC2
#include "SetSimGainCorrection.hh"
#endif
#ifdef CS_BASIC2
#include "GainCorrection.hh"
#endif
#ifdef CS_BASIC2
#include "ComptonEventFilter.hh"
#endif
#ifdef CS_BASIC2
#include "SelectTime.hh"
#endif
#ifdef CS_BASIC2
#include "AnalyzeDSD.hh"
#endif
#ifdef CS_BASIC2
#include "EfficiencyMapSky.hh"
#endif
#include "HXIEventSelection.hh"
#include "PlaneWavePrimaryGen.hh"
#ifdef CS_BASIC2
#include "PlaneWavePrimaryGenPol.hh"
#endif
#include "PlaneWaveRectanglePrimaryGen.hh"
#ifdef CS_BASIC2
#include "PlaneWaveRectanglePrimaryGenPol.hh"
#endif
#include "PointSourcePrimaryGen.hh"
#include "IsotropicPrimaryGen.hh"
#ifdef CS_BASIC2
#include "PrimaryGenInVolume.hh"
#endif
#include "NucleusPrimaryGen.hh"
#ifdef CS_BASIC2
#include "NucleusPrimaryGenInVolume.hh"
#endif
#ifdef USE_SIMX
#include "AHRayTracingPrimaryGen.hh"
#endif
#ifdef USE_SIMX
#include "SimXPrimaryGen.hh"
#endif
#include "SpenvisIsotropicPrimaryGen.hh"
#ifdef CS_BASIC2
#include "AHRadiationBackgroundPrimaryGen.hh"
#endif
#include "RDPickUpData.hh"
#include "ActivationPickUpData.hh"
#ifdef USE_SIMX
#include "AHStandardPickUpData.hh"
#endif
#ifdef CS_BASIC2
#include "KillStepPickUpData.hh"
#endif
#ifdef CS_BASIC2
#include "MaterialSamplePickUpData.hh"
#endif
#include "AHStandardANLPhysicsList.hh"
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
#ifdef CS_BASIC2
#include "InitialParticleTree.hh"
#endif
#include "DumpMass.hh"

#include "class_list_anlGeant4.hh"

%}


%import "anl.i"
%import "anlGeant4.i"

namespace comptonsoft {

class VCSModule : public anl::BasicModule
{
public:
  VCSModule();
  ~VCSModule();
};


class SelectHit : public VCSModule
{
public:
  SelectHit();
  ~SelectHit() {}
};


#ifdef CS_BASIC2
class MakePI : public VCSModule
{
public:
  MakePI();
  ~MakePI() {}
};

#endif

#ifdef CS_BASIC2
class VAnalyzeHitList : public VCSModule
{
public:
  VAnalyzeHitList();
  ~VAnalyzeHitList() {}
};

#endif

#ifdef CS_BASIC2
class AnalyzeHit : public SelectHit
{
public:
  AnalyzeHit() {}
  ~AnalyzeHit() {}
};

#endif

#ifdef CS_BASIC2
class ApplyNewPI : public MakePI
{
public:
  ApplyNewPI() {}
  ~ApplyNewPI() {}
};

#endif

#ifdef CS_BASIC2
class AzimuthDistribution : public VCSModule
{
public:
  AzimuthDistribution();
  ~AzimuthDistribution() {}
};

#endif

#ifdef CS_BASIC2
class BackProjection : public VCSModule
{
public:
  BackProjection();
  ~BackProjection() {}
};

#endif

#ifdef CS_BASIC2
class BackProjectionSky : public BackProjection
{
public:
  BackProjectionSky();
  ~BackProjectionSky() {}
};

#endif

class CSHitCollection : public anl::BasicModule
{
public:
  CSHitCollection() {}
  ~CSHitCollection() {}
};


#ifdef CS_BASIC2
class CalcARM : public VCSModule
{
public:
  CalcARM();
  ~CalcARM() {}
};

#endif

#ifdef CS_BASIC2
class CalcARMEne : public CalcARM
{
public:
  CalcARMEne() {}
  ~CalcARMEne() {}
};

#endif

#ifdef CS_BASIC2
class CalcARMPos : public CalcARM
{
public:
  CalcARMPos();
  ~CalcARMPos() {}
};

#endif

#ifdef CS_BASIC2
class CalcARM_PS : public CalcARM
{
public:
  CalcARM_PS();
  ~CalcARM_PS() {}
};

#endif

#ifdef CS_BASIC2
class CalcPedestal : public VCSModule
{
public:
  CalcPedestal();
  ~CalcPedestal() {}
};

#endif

#ifdef CS_BASIC2
class CalcPedestal_SpW : public VCSModule
{
public:
  CalcPedestal_SpW() {}
  ~CalcPedestal_SpW() {}
};

#endif

#ifdef CS_BASIC2
class Clustering : public anl::BasicModule
{
public:
  Clustering();
  ~Clustering() {}
};

#endif

#ifdef CS_BASIC2
class ComptonModeFilter : public VCSModule
{
public:
  ComptonModeFilter();
  ~ComptonModeFilter() {}
};

#endif

#ifdef CS_BASIC2
class ComptonModeSpectrum : public VCSModule
{
public:
  ComptonModeSpectrum();
  ~ComptonModeSpectrum() {}
};

#endif

#ifdef CS_BASIC2
class ComptonTree : public VCSModule
{
public:
  ComptonTree();
  ~ComptonTree() {}
};

#endif

class ConstructChannelTable : public VCSModule
{
public:
  ConstructChannelTable();
  ~ConstructChannelTable() {}
};


class ConstructDetector : public anl::BasicModule
{
public:
  ConstructDetector();
  ~ConstructDetector() {}
};


class ConstructDetector_Sim : public anl::BasicModule
{
public:
  ConstructDetector_Sim();
  ~ConstructDetector_Sim() {}
};


#ifdef CS_BASIC2
class CorrectPHA : public VCSModule
{
public:
  CorrectPHA();
  ~CorrectPHA() {}
};

#endif

#ifdef CS_BASIC2
class DeadTimeTree_VME3 : public VCSModule
{
public:
  DeadTimeTree_VME3();
  ~DeadTimeTree_VME3() {}
};

#endif

class DefineBadChannel : public VCSModule
{
public:
  DefineBadChannel();
  ~DefineBadChannel() {}
};


class DefineBadChannel_Sim : public VCSModule
{
public:
  DefineBadChannel_Sim() {}
  ~DefineBadChannel_Sim() {}
};


#ifdef CS_BASIC2
class Edep2DMap : public VCSModule
{
public:
  Edep2DMap();
  ~Edep2DMap() {}
};

#endif

#ifdef CS_BASIC2
class SelectHitPattern : public anl::BasicModule
{
public:
  SelectHitPattern();
  ~SelectHitPattern() {}
};

#endif

class EventReconstruction : public anl::BasicModule
{
public:
  EventReconstruction();
  ~EventReconstruction() {}
};


class EventSelection : public VCSModule
{
public:
  EventSelection();
  ~EventSelection() {}
};


#ifdef CS_BASIC2
class EventThreshold : public anl::BasicModule
{
public:
  EventThreshold();
  ~EventThreshold() {}
};

#endif

#ifdef CS_BASIC2
class EventTree : public VCSModule
{
public:
  EventTree();
  ~EventTree() {}
};

#endif

#ifdef CS_BASIC2
class FilterFullDeposit : public anl::BasicModule
{
public:
  FilterFullDeposit() {}
  ~FilterFullDeposit() {}
};

#endif

#ifdef CS_BASIC2
class HistPHA : public VCSModule
{
public:
  HistPHA();
  ~HistPHA() {}
};

#endif

#ifdef CS_BASIC2
class HistPI : public VCSModule
{
public:
  HistPI();
  ~HistPI() {}
};

#endif

#ifdef CS_BASIC2
class HitTree : public VCSModule
{
public:
  HitTree();
  ~HitTree() {}
};

#endif

#ifdef CS_BASIC2
class HitTree_Sim : public HitTree
{
public:
  HitTree_Sim();
  ~HitTree_Sim() {}
};

#endif

#ifdef CS_BASIC2
class InitialConditionFilter : public anl::BasicModule
{
public:
  InitialConditionFilter();
};

#endif

class MakeDetectorHit : public SelectHit
{
public:
  MakeDetectorHit() {}
  ~MakeDetectorHit() {}
};


class MakeDetectorHitTR : public MakeDetectorHit
{
public:
  MakeDetectorHitTR() {}
  ~MakeDetectorHitTR() {}
};


class MakeRawHit : public SelectHit
{
public:
  MakeRawHit() {}
  ~MakeRawHit() {}
};


#ifdef CS_BASIC2
class PhAbsModeSpectrum : public VCSModule
{
public:
  PhAbsModeSpectrum();
  ~PhAbsModeSpectrum() {}
};

#endif

#ifdef CS_BASIC2
class RawTree : public VCSModule
{
public:
  RawTree();
  ~RawTree() {}
};

#endif

#ifdef CS_BASIC2
class RawTree_SpW : public VCSModule
{
public:
  RawTree_SpW();
  ~RawTree_SpW() {}
};

#endif

#ifdef CS_BASIC2
class ReadComptonTree : public EventReconstruction
{
public:
  ReadComptonTree();
  ~ReadComptonTree() {}
};

#endif

#ifdef CS_BASIC2
class ReadDataFile : public VCSModule
{
public:
  ReadDataFile();
  ~ReadDataFile() {}
};

#endif

#ifdef CS_BASIC2
class ReadDataFile_NT : public ReadDataFile
{
public:
  ReadDataFile_NT() {}
  ~ReadDataFile_NT() {}
};

#endif

#ifdef CS_BASIC2
class ReadDataFile_NB0 : public ReadDataFile
{
public:
  ReadDataFile_NB0();
  ~ReadDataFile_NB0() {}
};

#endif

#ifdef CS_BASIC2
class ReadDataFile_SpW2 : public ReadDataFile
{
public:
  ReadDataFile_SpW2();
  ~ReadDataFile_SpW2() {}
};

#endif

#ifdef CS_BASIC2
class ReadDataFile_VME3 : public ReadDataFile
{
public:
  ReadDataFile_VME3();
  ~ReadDataFile_VME3() {}
};

#endif

#ifdef CS_BASIC2
class ReadHitTree : public anl::BasicModule
{
public:
  ReadHitTree();
  ~ReadHitTree() {}
};

#endif

#ifdef CS_BASIC2
class ReadRawHitTree : public VCSModule
{
public:
  ReadRawHitTree();
  ~ReadRawHitTree() {}
};

#endif

#ifdef CS_BASIC2
class ReadDetectorHitTree : public ReadRawHitTree
{
public:
  ReadDetectorHitTree() {}
  ~ReadDetectorHitTree() {}
};

#endif

#ifdef CS_BASIC2
class ReprocessPI : public anl::BasicModule
{
public:
  ReprocessPI();
  ~ReprocessPI() {}
};

#endif

#ifdef CS_BASIC2
class EnergySpectrum : public VCSModule
{
public:
  EnergySpectrum();  
  ~EnergySpectrum() {}
};

#endif

#ifdef CS_BASIC2
class Resp2DSpec : public VCSModule
{
public:
  Resp2DSpec();
  ~Resp2DSpec() {}
};

#endif

#ifdef CS_BASIC2
class ResponseMatrix : public VCSModule
{
public:
  ResponseMatrix();
  ~ResponseMatrix() {}
};

#endif

#ifdef CS_BASIC2
class SaveData : public anl::BasicModule
{
public:
  SaveData();
};

#endif

class SetNoiseLevel : public VCSModule
{
public:
  SetNoiseLevel();
  ~SetNoiseLevel() {}
};


#ifdef CS_BASIC2
class SetSimGainCorrection : public VCSModule
{
public:
  SetSimGainCorrection();
  ~SetSimGainCorrection() {}
};

#endif

#ifdef CS_BASIC2
class GainCorrection : public SetSimGainCorrection
{
public:
  GainCorrection();
  ~GainCorrection();
};

#endif

#ifdef CS_BASIC2
class ComptonEventFilter : public anl::BasicModule
{
public:
  ComptonEventFilter();
  ~ComptonEventFilter() {}

  void define_condition();
  void add_condition(const std::string& type,
                     double min_value, double max_value);
};

#endif

#ifdef CS_BASIC2
class SelectTime : public anl::BasicModule
{
public:
  SelectTime();
  ~SelectTime() {}
};

#endif

#ifdef CS_BASIC2
class AnalyzeDSD : public VCSModule
{
public:
  AnalyzeDSD();
  ~AnalyzeDSD() {}
};

#endif

#ifdef CS_BASIC2
class EfficiencyMapSky : public BackProjection
{
public:
  EfficiencyMapSky();
  ~EfficiencyMapSky() {}
};

#endif

class HXIEventSelection : public VCSModule
{
public:
  HXIEventSelection() {}
  ~HXIEventSelection() {}
};


class PlaneWavePrimaryGen : public anlgeant4::BasicPrimaryGen
{
public:
  PlaneWavePrimaryGen();
};


#ifdef CS_BASIC2
class PlaneWavePrimaryGenPol : public PlaneWavePrimaryGen
{
public:
  PlaneWavePrimaryGenPol() {}
};

#endif

class PlaneWaveRectanglePrimaryGen : public PlaneWavePrimaryGen
{
public:
  PlaneWaveRectanglePrimaryGen();
};


#ifdef CS_BASIC2
class PlaneWaveRectanglePrimaryGenPol : public PlaneWaveRectanglePrimaryGen
{
public:
  PlaneWaveRectanglePrimaryGenPol() {}
};

#endif

class PointSourcePrimaryGen : public anlgeant4::BasicPrimaryGen
{
public:
  PointSourcePrimaryGen();
};


class IsotropicPrimaryGen : public anlgeant4::BasicPrimaryGen
{
public:
  IsotropicPrimaryGen();
};


#ifdef CS_BASIC2
class PrimaryGenInVolume : public PointSourcePrimaryGen
{
public:
  PrimaryGenInVolume();
  virtual ~PrimaryGenInVolume(){}
};

#endif

class NucleusPrimaryGen : public anlgeant4::BasicPrimaryGen
{
public:
  NucleusPrimaryGen();
};


#ifdef CS_BASIC2
class NucleusPrimaryGenInVolume : public NucleusPrimaryGen
{
public:
  NucleusPrimaryGenInVolume();
  virtual ~NucleusPrimaryGenInVolume(){}
};

#endif

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

class SpenvisIsotropicPrimaryGen : public IsotropicPrimaryGen
{
public:
  SpenvisIsotropicPrimaryGen();
};


#ifdef CS_BASIC2
class AHRadiationBackgroundPrimaryGen : public IsotropicPrimaryGen
{
public:
  AHRadiationBackgroundPrimaryGen();
};

#endif

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

#ifdef CS_BASIC2
class KillStepPickUpData : public anlgeant4::StandardPickUpData
{
public:
  KillStepPickUpData();
};

#endif

#ifdef CS_BASIC2
class MaterialSamplePickUpData : public anlgeant4::StandardPickUpData
{
public:
  MaterialSamplePickUpData();
};

#endif

class AHStandardANLPhysicsList : public anlgeant4::VANLPhysicsList
{
public:
  AHStandardANLPhysicsList();
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


#ifdef CS_BASIC2
class InitialParticleTree : public VCSModule
{
public:
  InitialParticleTree();
  ~InitialParticleTree() {}
};

#endif

class DumpMass : public anl::BasicModule
{
public:
  DumpMass();
  ~DumpMass();
};


}
