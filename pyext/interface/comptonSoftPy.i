%module comptonSoftPy

%{
#include "NanoGRAMSReadTPCEvents.hh"
#include "NanoGRAMSQuickLookWriter.hh"
#include "NanoGRAMSWriteHitTree.hh"
#include "CSHitCollection.hh"
#include "ConstructDetector.hh"
#include "ReadHitTree.hh"
#include "EventReconstruction.hh"
#include "WriteComptonEventTree.hh"
#include "SaveData.hh"
%}

%import(module="anlnext.anlnextpy") "anlnext/python/anlnextpy.i"

namespace comptonsoft {

class NanoGRAMSReadTPCEvents : public anlnext::BasicModule
{
public:
  NanoGRAMSReadTPCEvents();
  ~NanoGRAMSReadTPCEvents();
};

class NanoGRAMSWriteHitTree : public anlnext::BasicModule
{
public:
  NanoGRAMSWriteHitTree();
  ~NanoGRAMSWriteHitTree();
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

}
