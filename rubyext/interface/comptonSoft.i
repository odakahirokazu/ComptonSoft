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
#include "CreateRootFile.hh"
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
#include "WeightByInitialDirection.hh"
#include "RecalculateSimulationNoise.hh"
#include "ComptonModeFilter.hh"
#include "SelectFullDeposit.hh"
#include "ComptonEventFilter.hh"
#include "SelectEventsOnFocalPlane.hh"
#include "SelectTime.hh"
#include "FilterByGoodTimeIntervals.hh"
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
#include "HistogramEnergy1D.hh"
#include "HistogramEnergy2D.hh"
#include "HistogramARM.hh"
#include "HistogramARMByPositionMeasurement.hh"
#include "HistogramAzimuthAngle.hh"
#include "Histogram2DDeltaEnergyWithARM.hh"
#include "ResponseMatrix.hh"
#include "BackProjection.hh"
#include "BackProjectionSky.hh"
#include "EfficiencyMapSky.hh"
#include "QuickAnalysisForDSD.hh"
#include "AssignTime.hh"
#include "DefineFrame.hh"
#ifdef USE_FITSIO
#include "MakeFrameFITS.hh"
#endif
#include "ConstructFrame.hh"
#ifdef USE_FITSIO
#include "ConstructSXIFrame.hh"
#endif
#include "FillFrame.hh"
#include "LoadFrame.hh"
#include "XrayEventCollection.hh"
#include "XrayEventSelection.hh"
#include "AnalyzeFrame.hh"
#include "WriteXrayEventTree.hh"
#include "ReadXrayEventTree.hh"
#include "SortEventTreeWithTime.hh"
#ifdef USE_FITSIO
#include "SetPedestals.hh"
#endif
#ifdef USE_FITSIO
#include "WritePedestals.hh"
#endif
#include "WriteBadPixels.hh"
#include "AnalyzeDarkFrame.hh"
#include "LoadRootFrame.hh"
#include "DetectBadFrames.hh"
#include "SetBadFrames.hh"
#include "SetPedestalsByMedian.hh"
#include "HistogramFramePedestal.hh"
#include "HistogramFramePedestalMean.hh"
#include "HistogramFramePedestalSigma.hh"
#include "HistogramXrayEventSpectrum.hh"
#include "HistogramXrayEventAzimuthAngle.hh"
#include "HistogramXrayEventWeight.hh"
#include "ExtractXrayEventImage.hh"
#include "ProcessCodedAperture.hh"
#ifdef USE_HSQUICKLOOK
#include "PushToQuickLookDB.hh"
#endif
#include "GetInputFilesFromDirectory.hh"
#include "SelectEventsWithDetectorSpectrum.hh"
#include "AssignSXIGrade.hh"
#ifdef USE_FITSIO
#include "AEAttitudeCorrection.hh"
#endif
#ifdef USE_FITSIO
#include "SelectEventsWithCelestialSpectrum.hh"
#endif
#ifdef USE_FITSIO
#include "AEAssignWeightWithResponseMatrix.hh"
#endif
#include "ExtractPhotoelectronTrajectory.hh"
#ifdef USE_FITSIO
#include "AHRayTracingPrimaryGen.hh"
#endif
#ifdef USE_SIMX
#include "SimXPrimaryGen.hh"
#endif
#include "AHRadiationBackgroundPrimaryGen.hh"
#ifdef USE_FITSIO
#include "AEObservationPrimaryGen.hh"
#endif
#ifdef USE_FITSIO
#include "CelestialSourcePrimaryGen.hh"
#endif
#include "RadioactiveDecayUserActionAssembly.hh"
#include "ActivationUserActionAssembly.hh"
#ifdef USE_SIMX
#include "AHStandardUserActionAssembly.hh"
#endif
#include "SampleOpticalDepth.hh"
#include "ScatteringPickUpData.hh"
#include "ObservationPickUpData.hh"
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
#include "WriteObservationTree.hh"
#include "SimulateCXBShieldPlate.hh"
#include "RescaleSimulationNoiseOfSGDSiUntriggered.hh"
#include "UniformlyRandomizeEPI.hh"
#ifdef USE_FITSIO
#include "ReadSGDEventFITS.hh"
#endif
#ifdef USE_FITSIO
#include "WriteSGDEventFITS.hh"
#endif
#ifdef USE_FITSIO
#include "ReadHXIEventFITS.hh"
#endif
#ifdef USE_FITSIO
#include "WriteHXIEventFITS.hh"
#endif
#ifdef USE_FITSIO
#include "FilterByGoodTimeIntervalsForSGD.hh"
#endif
#ifdef USE_FITSIO
#include "FilterByGoodTimeIntervalsForHXI.hh"
#endif

#include "class_list_anlGeant4.hh"
#include "comptonsoft_basic_classes.hh"

%}


%import(module="anlnext/ANL") "anlnext/ruby/ANL.i"
%import(module="anlgeant4/anlGeant4") "anlGeant4.i"

namespace comptonsoft {

class ConstructDetector : public anlnext::BasicModule
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


class VCSModule : public anlnext::BasicModule
{
public:
  VCSModule();
  ~VCSModule();
};


class CSHitCollection : public anlnext::BasicModule
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


class CreateRootFile : public anlnext::BasicModule
{
public:
  CreateRootFile();
  ~CreateRootFile();
};


class SaveData : public anlnext::BasicModule
{
public:
  SaveData();
  ~SaveData();
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
  CorrectPHA();
  ~CorrectPHA();
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


class InitialConditionFilter : public anlnext::BasicModule
{
public:
  InitialConditionFilter();
  ~InitialConditionFilter() = default;
};


class WeightByInitialDirection : public anlnext::BasicModule
{
public:
  WeightByInitialDirection();
  ~WeightByInitialDirection() = default;
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


class SelectFullDeposit : public anlnext::BasicModule
{
public:
  SelectFullDeposit() = default;
  ~SelectFullDeposit() = default;
};


class ComptonEventFilter : public anlnext::BasicModule
{
public:
  ComptonEventFilter();
  ~ComptonEventFilter() {}

  void define_condition();
  void add_hit_pattern(const std::string& name);
  void add_hit_pattern_key(const std::string& key);
  void add_evs_key(const std::string& key);
  void add_condition(const std::string& type,
                     double min_value, double max_value);
};


class SelectEventsOnFocalPlane : public VCSModule
{
public:
  SelectEventsOnFocalPlane();
  ~SelectEventsOnFocalPlane() = default;
};


class SelectTime : public anlnext::BasicModule
{
public:
  SelectTime();
  ~SelectTime() = default;
};


class FilterByGoodTimeIntervals : public anlnext::BasicModule
{
public:
  FilterByGoodTimeIntervals();
  ~FilterByGoodTimeIntervals();
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


class HistogramEnergy1D : public VCSModule
{
public:
  HistogramEnergy1D();
  ~HistogramEnergy1D() = default;
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


class Histogram2DDeltaEnergyWithARM : public VCSModule
{
public:
  Histogram2DDeltaEnergyWithARM();
  ~Histogram2DDeltaEnergyWithARM() = default;
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


class AssignTime : public VCSModule
{
public:
  AssignTime();
  ~AssignTime();
};


class DefineFrame : public VCSModule
{
public:
  DefineFrame();
  ~DefineFrame();
};


#ifdef USE_FITSIO
class MakeFrameFITS : public VCSModule
{
public:
  MakeFrameFITS();
  ~MakeFrameFITS();
};

#endif

class ConstructFrame : public ConstructDetector
{
public:
  ConstructFrame();
};


#ifdef USE_FITSIO
class ConstructSXIFrame : public ConstructFrame
{
public:
  ConstructSXIFrame() = default;
};

#endif

class FillFrame : public VCSModule
{
public:
  FillFrame();
  ~FillFrame();
};


class LoadFrame : public anlnext::BasicModule
{
public:
  LoadFrame();
};


class XrayEventCollection : public anlnext::BasicModule
{
public:
  XrayEventCollection();
  ~XrayEventCollection();
};


class XrayEventSelection : public XrayEventCollection
{
public:
  XrayEventSelection();
  ~XrayEventSelection();
};


class AnalyzeFrame : public VCSModule
{
public:
  AnalyzeFrame();
};


class WriteXrayEventTree : public VCSModule
{
public:
  WriteXrayEventTree();
  ~WriteXrayEventTree() = default;
};


class ReadXrayEventTree : public anlnext::BasicModule
{
public:
  ReadXrayEventTree();
};


class SortEventTreeWithTime : public VCSModule
{
public:
  SortEventTreeWithTime();
  ~SortEventTreeWithTime();
};


#ifdef USE_FITSIO
class SetPedestals : public VCSModule
{
public:
  SetPedestals();
};

#endif

#ifdef USE_FITSIO
class WritePedestals : public VCSModule
{
public:
  WritePedestals();
};

#endif

class WriteBadPixels : public VCSModule
{
public:
  WriteBadPixels();
};


class AnalyzeDarkFrame : public VCSModule
{
public:
  AnalyzeDarkFrame();
};


class LoadRootFrame : public anlnext::BasicModule
{
public:
  LoadRootFrame();
};


class DetectBadFrames : public VCSModule
{
public:
  DetectBadFrames();
};


class SetBadFrames : public VCSModule
{
public:
  SetBadFrames();
};


class SetPedestalsByMedian : public VCSModule
{
public:
  SetPedestalsByMedian();
};


class HistogramFramePedestal : public VCSModule
{
public:
  HistogramFramePedestal();
};

class HistogramFramePedestalMean : public VCSModule
{
public:
  HistogramFramePedestalMean();
};

class HistogramFramePedestalSigma : public VCSModule
{
public:
  HistogramFramePedestalSigma();
};

class HistogramXrayEventSpectrum : public VCSModule
{
public:
  HistogramXrayEventSpectrum();
};


class HistogramXrayEventAzimuthAngle : public VCSModule
{
public:
  HistogramXrayEventAzimuthAngle();
};

class HistogramXrayEventWeight : public VCSModule
{
public:
  HistogramXrayEventWeight();
};

class ExtractXrayEventImage : public VCSModule
{
public:
  ExtractXrayEventImage();
};


class ProcessCodedAperture : public VCSModule
{
public:
  ProcessCodedAperture();
};


#ifdef USE_HSQUICKLOOK
class PushToQuickLookDB : public anlnext::BasicModule
{
public:
  PushToQuickLookDB();
};

#endif

class GetInputFilesFromDirectory : public anlnext::BasicModule
{
public:
  GetInputFilesFromDirectory();
};


class SelectEventsWithDetectorSpectrum : public anlnext::BasicModule
{
public:
  SelectEventsWithDetectorSpectrum();
  ~SelectEventsWithDetectorSpectrum();
};


class AssignSXIGrade : public VCSModule
{
public:
  AssignSXIGrade();
  ~AssignSXIGrade();
};


#ifdef USE_FITSIO
class AEAttitudeCorrection : public anlnext::BasicModule
{
public:
  AEAttitudeCorrection();
  ~AEAttitudeCorrection();
};

#endif

#ifdef USE_FITSIO
class SelectEventsWithCelestialSpectrum : public anlnext::BasicModule
{
public:
  SelectEventsWithCelestialSpectrum();
  ~SelectEventsWithCelestialSpectrum();
};

#endif

#ifdef USE_FITSIO
class AEAssignWeightWithResponseMatrix : public anlnext::BasicModule
{
public:
  AEAssignWeightWithResponseMatrix();
  ~AEAssignWeightWithResponseMatrix();
};

#endif

class ExtractPhotoelectronTrajectory : public VCSModule
{
public:
  ExtractPhotoelectronTrajectory();
  ~ExtractPhotoelectronTrajectory() = default;
};


#ifdef USE_FITSIO
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
  ~AHRadiationBackgroundPrimaryGen();
};


#ifdef USE_FITSIO
class AEObservationPrimaryGen : public anlgeant4::BasicPrimaryGen
{
public:
  AEObservationPrimaryGen();
};

#endif

#ifdef USE_FITSIO
class CelestialSourcePrimaryGen : public anlgeant4::IsotropicPrimaryGen
{
public:
  CelestialSourcePrimaryGen();
  ~CelestialSourcePrimaryGen();
};

#endif

class RadioactiveDecayUserActionAssembly : public anlgeant4::StandardUserActionAssembly
{
public:
  RadioactiveDecayUserActionAssembly();
};


class ActivationUserActionAssembly : public anlgeant4::StandardUserActionAssembly
{
public:
  ActivationUserActionAssembly();
  virtual ~ActivationUserActionAssembly();
};


#ifdef USE_SIMX
class AHStandardUserActionAssembly : public anlgeant4::StandardUserActionAssembly
{
public:
  AHStandardUserActionAssembly();
  virtual ~AHStandardUserActionAssembly();
};

#endif

class SampleOpticalDepth : public anlgeant4::VAppendableUserActionAssembly
{
public:
  SampleOpticalDepth();
};


class ScatteringPickUpData : public anlgeant4::VAppendableUserActionAssembly
{
public:
  ScatteringPickUpData();
};


class ObservationPickUpData : public anlgeant4::VAppendableUserActionAssembly
{
public:
  ObservationPickUpData();
  ~ObservationPickUpData() = default;
};


class PhysicsListManager : public anlgeant4::VANLPhysicsList
{
public:
  PhysicsListManager();
  ~PhysicsListManager() = default;
};


#ifdef USE_SIMX
class SimXIF : public anlnext::BasicModule
{
public:
  SimXIF();
  ~SimXIF();
};

#endif

#ifdef USE_SIMX
class GenerateSimXEvent : public anlnext::BasicModule
{
public:
  GenerateSimXEvent();
  ~GenerateSimXEvent() {}
};

#endif

#ifdef USE_SIMX
class OutputSimXPrimaries : public anlnext::BasicModule
{
public:
  OutputSimXPrimaries();
  ~OutputSimXPrimaries() {}
};

#endif

class AssignG4CopyNumber : public anlnext::BasicModule
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


class DumpMass : public anlnext::BasicModule
{
public:
  DumpMass();
  ~DumpMass();
};


class WriteObservationTree : public VCSModule
{
public:
  WriteObservationTree();
  ~WriteObservationTree() = default;
};


class SimulateCXBShieldPlate : public VCSModule
{
public:
  SimulateCXBShieldPlate();  
  ~SimulateCXBShieldPlate() = default;
};


class RescaleSimulationNoiseOfSGDSiUntriggered : public VCSModule
{
public:
  RescaleSimulationNoiseOfSGDSiUntriggered();
  ~RescaleSimulationNoiseOfSGDSiUntriggered() = default;
};


class UniformlyRandomizeEPI : public VCSModule
{
public:
  UniformlyRandomizeEPI();
  ~UniformlyRandomizeEPI();
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
class ReadHXIEventFITS : public VCSModule
{
public:
  ReadHXIEventFITS();
  ~ReadHXIEventFITS();
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

#ifdef USE_FITSIO
class FilterByGoodTimeIntervalsForSGD : public FilterByGoodTimeIntervals
{
public:
  FilterByGoodTimeIntervalsForSGD();
  ~FilterByGoodTimeIntervalsForSGD();
};

#endif

#ifdef USE_FITSIO
class FilterByGoodTimeIntervalsForHXI : public FilterByGoodTimeIntervals
{
public:
  FilterByGoodTimeIntervalsForHXI();
  ~FilterByGoodTimeIntervalsForHXI();
};

#endif

}
