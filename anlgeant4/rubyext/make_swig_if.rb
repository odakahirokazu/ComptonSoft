#!/usr/bin/env ruby
require 'ANLLib'

name = 'anlGeant4'
namespace = 'anlgeant4'
classList = [
  ANL::SWIGClass.new('Geant4Body'),
  ANL::SWIGClass.new('Geant4Simple'),
  ANL::SWIGClass.new('VANLPhysicsList', true),
  ANL::SWIGClass.new('VANLGeometry', true),
  ANL::SWIGClass.new('ReadGDML', false, "USE_GDML"),
  ANL::SWIGClass.new('WriteGDML', false, "USE_GDML"),
  ANL::SWIGClass.new('VANLPrimaryGen', true),
  ANL::SWIGClass.new('BasicPrimaryGen', true),
  ANL::SWIGClass.new('PointSourcePrimaryGen'),
  ANL::SWIGClass.new('PlaneWavePrimaryGen'),
  ANL::SWIGClass.new('PlaneWaveRectanglePrimaryGen'),
  ANL::SWIGClass.new('GaussianBeamPrimaryGen'),
  ANL::SWIGClass.new('IsotropicPrimaryGen'),
  ANL::SWIGClass.new('PrimaryGenUniformSourceInVolume'),
  ANL::SWIGClass.new('NucleusPrimaryGen'),
  ANL::SWIGClass.new('NucleusPrimaryGenInVolume'),
  ANL::SWIGClass.new('VUserActionAssembly'),
  ANL::SWIGClass.new('VMasterUserActionAssembly'),
  ANL::SWIGClass.new('VAppendableUserActionAssembly'),
  ANL::SWIGClass.new('StandardUserActionAssembly'),
  ANL::SWIGClass.new('VisualizeG4Geom', false, 'USE_VIS'),
]

m = ANL::SWIGModule.new(name, classList, namespace)
m.includeFiles = []
m.importModules = ['anl/ruby/ANL.i']
m.includeModules = []

if ARGV.include? '-l'
  m.print_class_list
elsif ARGV.include? '-d'
  m.print_make_doc
else
  m.print_interface
end
