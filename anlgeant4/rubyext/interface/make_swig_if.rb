#!/usr/bin/env ruby
require 'anlnext'

name = 'anlGeant4'
namespace = 'anlgeant4'
classList = [
  ANL::SWIGClass.new('Geant4Body'),
  ANL::SWIGClass.new('VANLPhysicsList', true),
  ANL::SWIGClass.new('VANLGeometry', true),
  ANL::SWIGClass.new('ReadGDML', false, "USE_GDML"),
  ANL::SWIGClass.new('WriteGDML', false, "USE_GDML"),
  ANL::SWIGClass.new('VANLPrimaryGenerator', true),
  ANL::SWIGClass.new('BasicPrimaryGenerator', true),
  ANL::SWIGClass.new('PointSourcePrimaryGenerator'),
  ANL::SWIGClass.new('SphericalSourcePrimaryGenerator'),
  ANL::SWIGClass.new('PlaneWavePrimaryGenerator'),
  ANL::SWIGClass.new('PlaneWaveRectanglePrimaryGenerator'),
  ANL::SWIGClass.new('GaussianBeamPrimaryGenerator'),
  ANL::SWIGClass.new('IsotropicPrimaryGenerator'),
  ANL::SWIGClass.new('UniformVolumePrimaryGenerator'),
  ANL::SWIGClass.new('VUserActionAssembly'),
  ANL::SWIGClass.new('StandardUserActionAssembly'),
  ANL::SWIGClass.new('VEventStore'),
  ANL::SWIGClass.new('VisualizeGeometry', false, 'USE_VIS'),
]

m = ANL::SWIGModule.new(name, classList, namespace)
m.includeFiles = []
m.importModules = [{name: 'anlnext/ANL', file: 'anlnext/ruby/ANL.i'}]
m.includeModules = []

if ARGV.include? '-l'
  m.print_class_list
elsif ARGV.include? '-d'
  m.print_make_doc
else
  m.print_interface
end
