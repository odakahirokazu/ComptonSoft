%module MyGeometry
%{
#include "MyMainGeometry.hh"
#include "class_list_anlGeant4.hh"

using namespace mygeometry;

%}

%import(module="anlgeant4/anlGeant4") "anlGeant4.i"

namespace mygeometry
{

class MyMainGeometry : public anlgeant4::VANLGeometry {
public:
  MyMainGeometry();
  virtual ~MyMainGeometry();
};

}
