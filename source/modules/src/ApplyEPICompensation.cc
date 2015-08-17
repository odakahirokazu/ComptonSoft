/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
 *************************************************************************/

#include "ApplyEPICompensation.hh"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "TFile.h"
#include "TSpline.h"

#include "NextCLI.hh"
#include "SimDetectorUnit2DPad.hh"
#include "SimDetectorUnit2DStrip.hh"
#include "SimDetectorUnitScintillator.hh"

using namespace anl;

namespace comptonsoft
{

ApplyEPICompensation::ApplyEPICompensation()
  : m_ByFile(true), m_FileName(""), m_FunctionFile(nullptr),
    m_Type(1), m_Factor0(1.0), m_Factor1(1.0)
{
}

ApplyEPICompensation::~ApplyEPICompensation() = default;

ANLStatus ApplyEPICompensation::mod_startup()
{
  register_parameter(&m_FileName, "filename");
  register_parameter_map(&m_Map, "epi_compensation_map",
                         "detector_name_prefix", "Si");
  add_map_value(&m_Type, "detector_type", "",
                "Type of detector (1: single, 2:double)");
  add_map_value(&m_Factor0, "factor0");
  add_map_value(&m_Factor1, "factor1");
  std::vector<int> enable1(1); enable1[0] = 1;
  std::vector<int> enable2(2); enable2[0] = 1; enable2[1]= 2;
  enable_map_value(1, enable1);
  enable_map_value(2, enable2);
  
  return AS_OK;
}

ANLStatus ApplyEPICompensation::mod_com()
{
  bool byFile = m_ByFile;
  CLread("Set by file? [1:yes 0:no]", &byFile);
  m_ByFile = byFile;

  if (m_ByFile) {
    m_FileName = ".xml";
    unregister_parameter("epi_compensation_map");
  }
  else {
    unregister_parameter("filename");
  }
  ask_parameters();
  return AS_OK;
}

ANLStatus ApplyEPICompensation::mod_init()
{
  VCSModule::mod_init();

  if (m_FileName == "") { m_ByFile = false; }

  bool rval;
  if (m_ByFile) {
    rval = set_by_file();
  }
  else {
    rval = set_by_map();
  }

  if (!rval) {
    return AS_QUIT;
  }

  return AS_OK;
}

ANLStatus ApplyEPICompensation::mod_exit()
{
  if (m_FunctionFile) {
    m_FunctionFile->Close();
    delete m_FunctionFile;
    m_FunctionFile = 0;
  }
  
  return AS_OK;
}

bool ApplyEPICompensation::set_by_map()
{
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& detector: detectorManager->getDetectors()) {
    const std::string prefix = detector->getNamePrefix();
    if (m_Map.count(prefix) == 0) { continue; }
    
    const double factor0 = std::get<1>(m_Map[prefix]);
    const double factor1 = std::get<2>(m_Map[prefix]);
    
    const int detid = detector->getID();
    if (detector->Type().find("SimDetector2DStrip") == 0) {
      SimDetectorUnit2DStrip* ds
        = dynamic_cast<SimDetectorUnit2DStrip*>(detector.get());
      if (ds == 0) {
        std::cout << "Detector type is not 2D strip. Detector ID: " << detid << std::endl;
        return false;
      }
      auto xStripSelector = std::mem_fn(&PixelID::isXStrip);
      auto yStripSelector = std::mem_fn(&PixelID::isYStrip);
      if (ds->isXStripSideCathode()) {
        // cathode
        ds->setEPICompensationFactorToSelected(factor0, xStripSelector);
        // anode
        ds->setEPICompensationFactorToSelected(factor1, yStripSelector);
      }
      else if (ds->isXStripSideAnode()){
        // anode
        ds->setEPICompensationFactorToSelected(factor1, xStripSelector);
        // cathode
        ds->setEPICompensationFactorToSelected(factor0, yStripSelector);
      }
    }
    else {
      DeviceSimulation* ds
        = dynamic_cast<DeviceSimulation*>(detector.get());
      if (ds == 0) {
        std::cout << "Detector type is not DeviceSimulation. Detector ID: " << detid << std::endl;
        return false;
      }
      ds->resetEPICompensationFactorVector(factor0);
    }

  }  
  return true;
}

bool ApplyEPICompensation::set_by_file()
{
  using boost::property_tree::ptree;
  using boost::optional;

  ptree pt;
  try {
    read_xml(m_FileName, pt);
  }
  catch (boost::property_tree::xml_parser_error&) {
    std::cout << "cannot parse: " << m_FileName << std::endl;
    return false;
  }

  optional<std::string> file = pt.get_optional<std::string>("epi_compensation.function_file");
  if (file) {
    m_FunctionFile = new TFile(file->c_str());
  }

  for (ptree::value_type& v: pt.get_child("epi_compensation.data")) {
    if (v.first == "detector") {
      const ptree detectorNode = v.second;
      const int detectorID = detectorNode.get<int>("<xmlattr>.id");
      DeviceSimulation* ds = dynamic_cast<DeviceSimulation*>(getDetectorManager()->getDeviceSimulationByID(detectorID));
      if (ds==0) { continue; }
      for (const ptree::value_type& vv: detectorNode.get_child("")) {
        if (vv.first == "section") {
          const ptree sectionNode = vv.second;
          const int section = sectionNode.get<int>("<xmlattr>.id");
          for (const ptree::value_type& vvv: sectionNode.get_child("")) {
            if (vvv.first == "channel") {
              const ptree channelNode = vvv.second;
              const int channel = channelNode.get<int>("<xmlattr>.id");
              optional<double> factor =
                channelNode.get_optional<double>("factor");
              if (factor) {
                ds->setEPICompensationFactor(ChannelID(section, channel), *factor);
              }
              optional<std::string> functionName =
                channelNode.get_optional<std::string>("function");
              if (functionName) {
                TSpline* func = nullptr;
                m_FunctionFile->GetObject(functionName->c_str(), func);
                ds->setEPICompensationFunction(ChannelID(section, channel), func);
              }
            }
          }
        }
      }
    }
    else if (v.first == "readout_module") {
      const ptree moduleNode = v.second;
      const int moduleID = moduleNode.get<int>("<xmlattr>.id");
      const DetectorReadoutModule* readoutModule
        = getDetectorManager()->getReadoutModuleByID(moduleID);

      for (const ptree::value_type& vv: moduleNode.get_child("")) {
        if (vv.first == "section") {
          const ptree sectionNode = vv.second;
          const int mod_section = sectionNode.get<int>("<xmlattr>.id");
          DetectorChannelID channelID
            = readoutModule->ReadoutSection(mod_section);
          const int detectorID = channelID.Detector();
          const int det_section = channelID.Section();
          
          DeviceSimulation* ds = dynamic_cast<DeviceSimulation*>(getDetectorManager()->getDeviceSimulationByID(detectorID));
          if (ds==0) { continue; }
          
          for (const ptree::value_type& vvv: sectionNode.get_child("")) {
            if (vvv.first == "channel") {
              const ptree channelNode = vvv.second;
              const int channel = channelNode.get<int>("<xmlattr>.id");
              optional<double> factor =
                channelNode.get_optional<double>("factor");
              if (factor) {
                ds->setEPICompensationFactor(ChannelID(det_section, channel), *factor);
              }
              optional<std::string> functionName =
                channelNode.get_optional<std::string>("function");
              if (functionName) {
                TSpline* func = nullptr;
                m_FunctionFile->GetObject(functionName->c_str(), func);
                ds->setEPICompensationFunction(ChannelID(det_section, channel), func);
              }
            }
          }
        }
      }
    }
  }

  return true;
}

} /* namespace comptonsoft */
