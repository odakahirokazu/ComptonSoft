%module comptonSoftPy

%{
#include "NanoGRAMSReadTPCEvents.hh"
#include "NanoGRAMSReadDppEvents.hh"
#include "NanoGRAMSCorrectLightWaveform.hh"
#include "NanoGRAMSAnalyzeLight.hh"
#include "NanoGRAMSSelectEvents.hh"
#include "NanoGRAMSQuickLookWriter.hh"
#include "CSHitCollection.hh"
#include "ConstructDetector.hh"
#include "ReadHitTree.hh"
#include "EventReconstruction.hh"
#include "WriteComptonEventTree.hh"
#include "SaveData.hh"
#include "ConstructDetectorForSimulation.hh"
#include "SetChannelProperties.hh"
#include "NanoGRAMSLoadConfig.hh"
#include "ReadEventTreeAsDetectorHits.hh"
#include "RecalculateSimulationNoise.hh"
#include "AnalyzeHits.hh"
#include "WriteHitTree.hh"
#include "HitTreeIOWithInitialInfo.hh"
%}

%import(module="anlnext.anlnextpy") "anlnext/python/anlnextpy.i"

namespace comptonsoft {

class NanoGRAMSReadTPCEvents : public anlnext::BasicModule
{
public:
  NanoGRAMSReadTPCEvents();
  ~NanoGRAMSReadTPCEvents();
};

class NanoGRAMSReadDppEvents : public anlnext::BasicModule
{
public:
  NanoGRAMSReadDppEvents();
  ~NanoGRAMSReadDppEvents();
};

class NanoGRAMSCorrectLightWaveform : public anlnext::BasicModule
{
public:
  NanoGRAMSCorrectLightWaveform();
  ~NanoGRAMSCorrectLightWaveform();
};

class NanoGRAMSAnalyzeLight : public anlnext::BasicModule
{
public:
  NanoGRAMSAnalyzeLight();
  ~NanoGRAMSAnalyzeLight();
};

class NanoGRAMSSelectEvents : public anlnext::BasicModule
{
public:
  NanoGRAMSSelectEvents();
  ~NanoGRAMSSelectEvents();
};

class NanoGRAMSQuickLookWriter : public anlnext::BasicModule
{
public:
  NanoGRAMSQuickLookWriter();
  ~NanoGRAMSQuickLookWriter();
};

class CSHitCollection : public anlnext::BasicModule
{
public:
  CSHitCollection();
  ~CSHitCollection();
};

class ConstructDetector : public anlnext::BasicModule
{
public:
  ConstructDetector();
  ~ConstructDetector();
};

class ReadHitTree : public anlnext::BasicModule
{
public:
  ReadHitTree();
  ~ReadHitTree();
};

class EventReconstruction : public anlnext::BasicModule
{
public:
  EventReconstruction();
  ~EventReconstruction();
};

class WriteComptonEventTree : public anlnext::BasicModule
{
public:
  WriteComptonEventTree();
  ~WriteComptonEventTree();
};

class SaveData : public anlnext::BasicModule
{
public:
  SaveData();
  ~SaveData();
};

class ConstructDetectorForSimulation : public ConstructDetector
{
public:
  ConstructDetectorForSimulation();
  ~ConstructDetectorForSimulation();
};

class SetChannelProperties : public anlnext::BasicModule
{
public:
  SetChannelProperties();
  ~SetChannelProperties();
};

class NanoGRAMSLoadConfig : public anlnext::BasicModule
{
public:
  NanoGRAMSLoadConfig();
  ~NanoGRAMSLoadConfig();
};

class ReadEventTreeAsDetectorHits : public anlnext::BasicModule
{
public:
  ReadEventTreeAsDetectorHits();
  ~ReadEventTreeAsDetectorHits();
};

class RecalculateSimulationNoise : public anlnext::BasicModule
{
public:
  RecalculateSimulationNoise();
  ~RecalculateSimulationNoise();
};

class AnalyzeHits : public anlnext::BasicModule
{
public:
  AnalyzeHits();
  ~AnalyzeHits();
};

class WriteHitTree : public anlnext::BasicModule
{
public:
  WriteHitTree();
  ~WriteHitTree();
};

}
