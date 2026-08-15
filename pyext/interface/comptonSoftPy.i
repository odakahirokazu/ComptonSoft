%module comptonSoftPy

%{
#include "NanoGRAMSHitExtraction.hh"
#include "NanoGRAMSCalibration.hh"
%}

%import(module="anlnext.anlnextpy") "anlnext/python/anlnextpy.i"

namespace comptonsoft {

class NanoGRAMSHitExtraction : public anlnext::BasicModule
{
public:
  NanoGRAMSHitExtraction();
  ~NanoGRAMSHitExtraction();
};

class NanoGRAMSCalibration : public anlnext::BasicModule
{
public:
  NanoGRAMSCalibration();
  ~NanoGRAMSCalibration();
};

}
