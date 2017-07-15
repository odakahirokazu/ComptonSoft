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
#include "GaussianBeamPrimaryGen.hh"
#include "IsotropicPrimaryGen.hh"
#include "PrimaryGenUniformSourceInVolume.hh"
#include "NucleusPrimaryGen.hh"
#include "NucleusPrimaryGenInVolume.hh"
#include "VUserActionAssembly.hh"
#include "VMasterUserActionAssembly.hh"
#include "VAppendableUserActionAssembly.hh"
#include "StandardUserActionAssembly.hh"
#ifdef USE_VIS
#include "VisualizeG4Geom.hh"
#endif


%}


%import "anl/ruby/ANL.i"

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


%nodefault;
class BasicPrimaryGen : public VANLPrimaryGen
{
};
%makedefault;


class PointSourcePrimaryGen : public BasicPrimaryGen
{
public:
  PointSourcePrimaryGen();
  ~PointSourcePrimaryGen();
};


class PlaneWavePrimaryGen : public anlgeant4::BasicPrimaryGen
{
public:
  PlaneWavePrimaryGen();
  ~PlaneWavePrimaryGen();
};


class PlaneWaveRectanglePrimaryGen : public anlgeant4::PlaneWavePrimaryGen
{
public:
  PlaneWaveRectanglePrimaryGen();
  ~PlaneWaveRectanglePrimaryGen();
};


class GaussianBeamPrimaryGen : public anlgeant4::PlaneWavePrimaryGen
{
public:
  GaussianBeamPrimaryGen();
  ~GaussianBeamPrimaryGen();
};


class IsotropicPrimaryGen : public anlgeant4::BasicPrimaryGen
{
public:
  IsotropicPrimaryGen();
  ~IsotropicPrimaryGen();
};


class PrimaryGenUniformSourceInVolume : public PointSourcePrimaryGen
{
public:
  PrimaryGenUniformSourceInVolume();
  ~PrimaryGenUniformSourceInVolume() = default;
};


class NucleusPrimaryGen : public BasicPrimaryGen
{
public:
  NucleusPrimaryGen();
  ~NucleusPrimaryGen();
};


class NucleusPrimaryGenInVolume : public NucleusPrimaryGen
{
public:
  NucleusPrimaryGenInVolume();
  ~NucleusPrimaryGenInVolume();
};


class VUserActionAssembly : public anl::BasicModule
{
public:
  VUserActionAssembly();
  virtual ~VUserActionAssembly();
};


class VMasterUserActionAssembly : public VUserActionAssembly
{
public:
  VMasterUserActionAssembly();
  virtual ~VMasterUserActionAssembly();
};


class VAppendableUserActionAssembly : public VUserActionAssembly
{
public:
  VAppendableUserActionAssembly();
  virtual ~VAppendableUserActionAssembly();
};


class StandardUserActionAssembly : public VMasterUserActionAssembly
{
public:
  StandardUserActionAssembly();
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
