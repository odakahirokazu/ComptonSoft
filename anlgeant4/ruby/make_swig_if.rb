#!/usr/bin/env ruby
require 'ANLSwigClass'

name = 'anlGeant4'
namespace = 'anlgeant4'
classList = [ANLSwigClass.new('Geant4Body'),
             ANLSwigClass.new('Geant4Simple'),
             ANLSwigClass.new('VANLPhysicsList', true),
             ANLSwigClass.new('VANLGeometry', true),
             ANLSwigClass.new('ReadGDML', false, "USE_GDML"),
             ANLSwigClass.new('WriteGDML', false, "USE_GDML"),
             ANLSwigClass.new('VANLPrimaryGen', true),
             ANLSwigClass.new('BasicPrimaryGen'),
             ANLSwigClass.new('PlaneWavePrimaryGen'),
             ANLSwigClass.new('PointSourcePrimaryGen'),
             ANLSwigClass.new('VPickUpData'),
             ANLSwigClass.new('StandardPickUpData'),
             ANLSwigClass.new('VisualizeG4Geom', false, 'USE_VIS'),
            ]

m = ANLSwigModule.new(name, classList, namespace)
m.includeFiles = []
m.importModules = ['anl.i']
m.includeModules = []

if ARGV.include? '-l'
  m.print_class_list
else
  m.print_interface
end
