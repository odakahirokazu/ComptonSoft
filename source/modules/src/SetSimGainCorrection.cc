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

#include "SetSimGainCorrection.hh"

#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include "globals.hh"
#include "TMath.h"
#include "TFile.h"

#include "NextCLI.hh"
#include "SimDetectorUnit2DPad.hh"
#include "SimDetectorUnit2DStrip.hh"
#include "SimDetectorUnitScintillator.hh"

using namespace comptonsoft;
using namespace anl;


SetSimGainCorrection::SetSimGainCorrection()
  : m_ByFile(false), m_FileName("gain_cor.xml"), m_FunctionFile(0),
    m_Type(3), m_GainFunction0(""), m_GainFunction1(""), m_GainCoeff0(1.0), m_GainCoeff1(1.0)
{}


ANLStatus SetSimGainCorrection::mod_startup()
{
  register_parameter(&m_FileName, "File name");

  register_parameter_map(&m_GainMap, "Gain correction map",
                         "Detector name prefix", "Si");
  add_map_value(&m_Type, "Type of detector/function", "",
                "Type of detector/function (1: single/spline, 2: double/spline, 3: single/linear, 4:double/linear)");
  add_map_value(&m_GainFunction0, "Gain function 0");
  add_map_value(&m_GainFunction1, "Gain function 1");
  add_map_value(&m_GainCoeff0, "Gain coefficient 0");
  add_map_value(&m_GainCoeff1, "Gain coefficient 1");
  std::vector<int> enable1(1); enable1[0] = 1;
  std::vector<int> enable2(2); enable2[0] = 1; enable2[1]= 2;
  std::vector<int> enable3(1); enable3[0] = 3;
  std::vector<int> enable4(2); enable4[0] = 3; enable4[1]= 4;
  enable_map_value(1, enable1);
  enable_map_value(2, enable2);
  enable_map_value(3, enable3);
  enable_map_value(4, enable4);
  
  return AS_OK;
}


ANLStatus SetSimGainCorrection::mod_com()
{
  bool byFile = m_ByFile;
  CLread("Set by file? [1:yes 0:no]", &byFile);
  m_ByFile = byFile;

  if (m_ByFile) {
    unregister_parameter("Gain correction map");
  }
  else {
    unregister_parameter("File name");
  }
  ask_parameters();
  return AS_OK;
}


ANLStatus SetSimGainCorrection::mod_init()
{
  VCSModule::mod_init();

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


ANLStatus SetSimGainCorrection::mod_exit()
{
  if (m_FunctionFile) {
    m_FunctionFile->Close();
    delete m_FunctionFile;
    m_FunctionFile = 0;
  }
  
  return AS_OK;
}


bool SetSimGainCorrection::set_by_map()
{
  std::vector<RealDetectorUnit*>::iterator itDet;
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();

  for (itDet = GetDetectorVector().begin(); itDet != itDetEnd; itDet++) {
    std::string prefix = (*itDet)->getNamePrefix();
    if (m_GainMap.count(prefix) == 0) { continue; }
    
    std::string gain0_spline_name = m_GainMap[prefix].get<1>();
    std::string gain1_spline_name = m_GainMap[prefix].get<2>();

    double gain0_factor = m_GainMap[prefix].get<3>();
    double gain1_factor = m_GainMap[prefix].get<4>();
    
    int detid = (*itDet)->getID();
    if ((*itDet)->Type().find("SimDetector2DPad") == 0) {
      SimDetectorUnit2DPad* ds
        = dynamic_cast<SimDetectorUnit2DPad*>((*itDet));
      if (ds == 0) {
        std::cout << "Detector type is not 2D pad. (" << detid << std::endl;
        return false;
      }

      if (gain0_spline_name!="") {
        TSpline* gain0_spline = (TSpline*)m_FunctionFile->Get(gain0_spline_name.c_str());
        ds->setGainCorrectionFunctionAll(gain0_spline);
      }
      else {
        ds->setGainCorrectionFactorAll(gain0_factor);
      }
    }
    else if ((*itDet)->Type().find("SimDetector2DStrip") == 0) {
      SimDetectorUnit2DStrip* ds
        = dynamic_cast<SimDetectorUnit2DStrip*>((*itDet));
      if (ds == 0) {
        std::cout << "Detector type is not 2D strip. (" << detid << std::endl;
        return false;
      }
      
      if (gain0_spline_name!="" && gain1_spline_name!="") {
        TSpline* gain0_spline = (TSpline*)m_FunctionFile->Get(gain0_spline_name.c_str());
        TSpline* gain1_spline = (TSpline*)m_FunctionFile->Get(gain1_spline_name.c_str());
     
        if (ds->UpsideXStrip() == ds->UpsideAnode()) {
          ds->setGainCorrectionFunctionXStripSideAll(gain1_spline); // anode, N-side
          ds->setGainCorrectionFunctionYStripSideAll(gain0_spline); // cathode, P-side
        }
        else {
          ds->setGainCorrectionFunctionXStripSideAll(gain0_spline); // cathode, P-side
          ds->setGainCorrectionFunctionYStripSideAll(gain1_spline); // anode, N-side
        }
      }
      else {
        if (ds->UpsideXStrip() == ds->UpsideAnode()) {
          ds->setGainCorrectionFactorXStripSideAll(gain1_factor); // anode, N-side
          ds->setGainCorrectionFactorYStripSideAll(gain0_factor); // cathode, P-side
        }
        else {
          ds->setGainCorrectionFactorXStripSideAll(gain0_factor); // cathode, P-side
          ds->setGainCorrectionFactorYStripSideAll(gain1_factor); // anode, N-side
        }
      }
    }
    else if ((*itDet)->Type().find("SimDetectorScintillator") == 0) {
      SimDetectorUnitScintillator* ds
        = dynamic_cast<SimDetectorUnitScintillator*>((*itDet));
      if (ds == 0) {
        std::cout << "Detector type is not Scintillator. (" << detid << std::endl;
        return false;
      }

      if (gain0_spline_name!="") {
        TSpline* gain0_spline = (TSpline*)m_FunctionFile->Get(gain0_spline_name.c_str());
        ds->setGainCorrectionFunctionAll(gain0_spline);
      }
      else {
        ds->setGainCorrectionFactorAll(gain0_factor);
      }
    }
  }
  
  return true;
}


bool SetSimGainCorrection::set_by_file()
{
  using boost::property_tree::ptree;

  ptree pt;
  try {
    read_xml(m_FileName, pt);
  }
  catch (boost::property_tree::xml_parser_error&) {
    std::cout << "cannot parse: " << m_FileName << std::endl;
    return AS_QUIT;
  }

  boost::optional<std::string> file = pt.get_optional<std::string>("gain_correction.gain_cor_file");
  if (file) {
    m_FunctionFile = new TFile(file->c_str());
  }

  foreach (ptree::value_type& v, pt.get_child("gain_correction.detectors")) {
    if (v.first == "detector") {
      ptree detNode = v.second;
      int detid = detNode.get<int>("<xmlattr>.id");
      DeviceSimulation* ds = dynamic_cast<DeviceSimulation*>(GetDetectorManager()->getDeviceSimulationByID(detid));
      if (ds==0) { continue; }
      foreach (ptree::value_type& vv, detNode.get_child("")) {
        if (vv.first == "chip") {
          ptree chipNode = vv.second;
          int chipid = chipNode.get<int>("<xmlattr>.id");
          foreach (ptree::value_type& vvv, chipNode.get_child("")) {
            if (vvv.first == "channel") {
              ptree channelNode = vvv.second;
              int channel = channelNode.get<int>("<xmlattr>.id");
              
              boost::optional<double> factor =
                channelNode.get_optional<double>("factor");
              if (factor) {
                ds->setGainCorrectionFactor(chipid, channel, *factor);
              }
              
              boost::optional<std::string> functionName =
                channelNode.get_optional<std::string>("function");
              if (functionName) {
                TSpline* func = (TSpline*)m_FunctionFile->Get(functionName->c_str());
                ds->setGainCorrectionFunction(chipid, channel, func);
              }
            }
          }
        }
      }
    }
  }

  return true;
}
