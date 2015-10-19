%module anlGeant4
%{
#include "Geant4Body.hh"
#include "Geant4Simple.hh"
#include "VANLPhysicsList.hh"
#include "VANLGeometry.hh"
#ifdef USE_GDML
#include "ReadGDML.hh"
#endif
#ifdef USE_GDML
#include "WriteGDML.hh"
#endif
#include "VANLPrimaryGen.hh"
#include "BasicPrimaryGen.hh"
#include "PointSourcePrimaryGen.hh"
#include "PlaneWavePrimaryGen.hh"
#include "PlaneWaveRectanglePrimaryGen.hh"
#include "IsotropicPrimaryGen.hh"
#include "PrimaryGenUniformSourceInVolume.hh"
#include "NucleusPrimaryGen.hh"
#include "NucleusPrimaryGenInVolume.hh"
#include "VPickUpData.hh"
#include "StandardPickUpData.hh"
#ifdef USE_VIS
#include "VisualizeG4Geom.hh"
#endif


%}


%import "ANL.i"

namespace anlgeant4 {

class Geant4Body : public anl::BasicModule
{
public:
  Geant4Body();
  ~Geant4Body();
};


class Geant4Simple : public anl::BasicModule
{
public:
  Geant4Simple();
  ~Geant4Simple();
};


%nodefault;
class VANLPhysicsList : public anl::BasicModule
{
};
%makedefault;


%nodefault;
class VANLGeometry : public anl::BasicModule
{
public:
  void SetLengthUnit(const std::string& name);
  double GetLengthUnit() const;
  std::string GetLengthUnitName() const;
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
class WriteGDML : public anl::BasicModule
{
public:
  WriteGDML();
};

#endif

%nodefault;
class VANLPrimaryGen : public anl::BasicModule
{
};
%makedefault;


class BasicPrimaryGen : public VANLPrimaryGen
{
public:
  };


class PointSourcePrimaryGen : public BasicPrimaryGen
{
public:
  PointSourcePrimaryGen();
};


class PlaneWavePrimaryGen : public anlgeant4::BasicPrimaryGen
{
public:
  PlaneWavePrimaryGen();
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


class PrimaryGenUniformSourceInVolume : public PointSourcePrimaryGen
{
public:
  PrimaryGenUniformSourceInVolume();
  ~PrimaryGenUniformSourceInVolume() = default;
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


class VPickUpData : public anl::BasicModule
{
public:
  VPickUpData();
};


class StandardPickUpData : public VPickUpData
{
public:
  StandardPickUpData();
};


#ifdef USE_VIS
class VisualizeG4Geom  : public anl::BasicModule
{
public:
  VisualizeG4Geom();
  ~VisualizeG4Geom();
};

#endif

}
