%module anlGeant4Py
%{
#include "Geant4Body.hh"
#include "VANLPhysicsList.hh"
#include "VANLGeometry.hh"
#ifdef USE_GDML
#include "ReadGDML.hh"
#endif
#ifdef USE_GDML
#include "WriteGDML.hh"
#endif
#include "VANLPrimaryGenerator.hh"
#include "BasicPrimaryGenerator.hh"
#include "PointSourcePrimaryGenerator.hh"
#include "SphericalSourcePrimaryGenerator.hh"
#include "PlaneWavePrimaryGenerator.hh"
#include "PlaneWaveRectanglePrimaryGenerator.hh"
#include "GaussianBeamPrimaryGenerator.hh"
#include "IsotropicPrimaryGenerator.hh"
#include "UniformVolumePrimaryGenerator.hh"
#include "VUserActionAssembly.hh"
#include "StandardUserActionAssembly.hh"
#include "VEventStore.hh"
#ifdef USE_VIS
#include "VisualizeGeometry.hh"
#endif


%}

%import(module="anlnext.anlnextpy") "anlnext/python/anlnextpy.i"

namespace anlgeant4 {

class Geant4Body : public anlnext::BasicModule
{
public:
  Geant4Body();
  ~Geant4Body();
};


%nodefault;
class VANLPhysicsList : public anlnext::BasicModule
{
};
%makedefault;


%nodefault;
class VANLGeometry : public anlnext::BasicModule
{
};
%makedefault;


#ifdef USE_GDML
class ReadGDML : public VANLGeometry
{
public:
  ReadGDML();
};

#endif

#ifdef USE_GDML
class WriteGDML : public anlnext::BasicModule
{
public:
  WriteGDML();
};

#endif

%nodefault;
class VANLPrimaryGenerator : public anlnext::BasicModule
{
};
%makedefault;


%nodefault;
class BasicPrimaryGenerator : public VANLPrimaryGenerator
{
};
%makedefault;


class PointSourcePrimaryGenerator : public BasicPrimaryGenerator
{
public:
  PointSourcePrimaryGenerator();
  ~PointSourcePrimaryGenerator();
};


class SphericalSourcePrimaryGenerator : public PointSourcePrimaryGenerator
{
public:
  SphericalSourcePrimaryGenerator();
  ~SphericalSourcePrimaryGenerator();
};


class PlaneWavePrimaryGenerator : public anlgeant4::BasicPrimaryGenerator
{
public:
  PlaneWavePrimaryGenerator();
  ~PlaneWavePrimaryGenerator();
};


class PlaneWaveRectanglePrimaryGenerator : public anlgeant4::PlaneWavePrimaryGenerator
{
public:
  PlaneWaveRectanglePrimaryGenerator();
  ~PlaneWaveRectanglePrimaryGenerator();
};


class GaussianBeamPrimaryGenerator : public anlgeant4::PlaneWavePrimaryGenerator
{
public:
  GaussianBeamPrimaryGenerator();
  ~GaussianBeamPrimaryGenerator();
};


class IsotropicPrimaryGenerator : public anlgeant4::BasicPrimaryGenerator
{
public:
  IsotropicPrimaryGenerator();
  ~IsotropicPrimaryGenerator();
};


class UniformVolumePrimaryGenerator : public PointSourcePrimaryGenerator
{
public:
  UniformVolumePrimaryGenerator();
  ~UniformVolumePrimaryGenerator() = default;
};


class VUserActionAssembly : public anlnext::BasicModule
{
public:
  VUserActionAssembly();
  virtual ~VUserActionAssembly();
};


class StandardUserActionAssembly : public VUserActionAssembly
{
public:
  StandardUserActionAssembly();
};


class VEventStore : public anlnext::BasicModule
{
public:
  VEventStore();
  virtual ~VEventStore();
};


#ifdef USE_VIS
class VisualizeGeometry  : public anlnext::BasicModule
{
public:
  VisualizeGeometry();
  ~VisualizeGeometry();
};

#endif

}
