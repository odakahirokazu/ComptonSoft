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

#include "DetectorManager.hh"

#include <iostream>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#if USE_ROOT
#include "TFile.h"
#endif

#include "globals.hh"

#include "DetectorUnitDef.hh"
#include "DetectorReadModule.hh"

#include "VHXISGDSensitiveDetector.hh"
#include "HXISGDSensitiveDetector.hh"
#include "AHSensitiveDetector.hh"


using boost::property_tree::ptree;

namespace comptonsoft {

DetectorManager::DetectorManager()
  : m_MCSimulation(false),
    m_DetectorConstructed(false),
    m_ReadoutModuleConstructed(false),
#if USE_ROOT
    m_CCEMapFile(0),
#endif
    m_AutoPosition(false),
    m_SDCheck(false)
{
  m_DetectorVector.clear();
  m_ReadModuleVector.clear();
  m_ASICDataVector.clear();
  m_DeviceSimulationVector.clear();
  m_SensitiveDetectorVector.clear();
}


DetectorManager::~DetectorManager()
{
  clearReadModuleVector();
  clearDetectorVector();
  clearASICDataVector();
  clearSensitiveDetectorVector();

#if USE_ROOT
  if (m_CCEMapFile) {
    m_CCEMapFile->Close();
    delete m_CCEMapFile;
    m_CCEMapFile = 0;
  }
#endif
}


bool DetectorManager::loadDetectorConfiguration(const char* filename)
{
  if (m_DetectorConstructed) {
    std::cout << "constuct error! : already constuceted" << std::endl;
    return false;
  }

  std::cout << "loading detector configuration: " << filename << std::endl;

  ptree pt;
  try {
    read_xml(filename, pt);
  }
  catch (boost::property_tree::xml_parser_error&) {
    std::cout << "cannot parse: " << filename << std::endl;
    return false;
  }

  std::cout << "Detector Name: " << pt.get<std::string>("configuration.name") << std::endl;

  const ptree& rootNode = pt.find("configuration")->second;
  m_DetectorConstructed = constructDetectorUnits(rootNode.find("detectors")->second);
  if (!m_DetectorConstructed) {
    return false;
  }
  
  if (!m_MCSimulation) {
    m_ReadoutModuleConstructed = constructReadoutModules(rootNode.find("readout")->second);
    if (!m_ReadoutModuleConstructed) {
      return false;
    }
  }

  return true;
}


bool DetectorManager::constructDetectorUnits(const ptree& DetectorsNode)
{
  int detIndex = 0;
  foreach (const ptree::value_type& v, DetectorsNode.get_child("")) {
    if (v.first == "detector") {
      ptree detNode = v.second;

      int detid = detNode.get<int>("<xmlattr>.id");
      std::string type = detNode.get<std::string>("type");
      std::string name = detNode.get<std::string>("name");
      double widthx = detNode.get<double>("geometry.widthx") * mm;
      double widthy = detNode.get<double>("geometry.widthy") * mm;
      double thickness = detNode.get<double>("geometry.thickness") * mm;
      double offsetx = detNode.get<double>("geometry.offsetx") * mm;
      double offsety = detNode.get<double>("geometry.offsety") * mm;
      int npx = detNode.get<int>("strip.numx");
      int npy = detNode.get<int>("strip.numy");
      double pitchx = detNode.get<double>("strip.pitchx") * mm;
      double pitchy = detNode.get<double>("strip.pitchy") * mm;
      double posx = detNode.get<double>("position.x") * mm;
      double posy = detNode.get<double>("position.y") * mm;
      double posz = detNode.get<double>("position.z") * mm;
      double dirxx = detNode.get<double>("direction_xaxis.x");
      double dirxy = detNode.get<double>("direction_xaxis.y");
      double dirxz = detNode.get<double>("direction_xaxis.z");
      double diryx = detNode.get<double>("direction_yaxis.x");
      double diryy = detNode.get<double>("direction_yaxis.y");
      double diryz = detNode.get<double>("direction_yaxis.z");

      bool priorityToAnode = false;
      boost::optional<std::string> sideOpt = detNode.get_optional<std::string>("energy_priority");
      if ( sideOpt ) {
        std::string side = *sideOpt;
	if (side=="N-side" || side=="anode" || side=="Al-side" || side=="In-side") {
	  priorityToAnode = true;
	}
	else if (side=="P-side" || side=="cathode" || side=="Pt-side") {
	  priorityToAnode = false;
	}
      }
    
      RealDetectorUnit* det;
      if (type == "Detector2DPad") {
        if (m_MCSimulation) {
          det = new SimDetectorUnit_Pad_t;
        }
        else {
          det = new RealDetectorUnit2DPad;
        }
      }
      else if (type == "Detector2DStrip"){
        if (m_MCSimulation) {
          det = new SimDetectorUnit_Strip_t;
        }
        else {
          det = new RealDetectorUnit2DStrip;
        }
      }
      else if (type == "DetectorScintillator"){
        if (m_MCSimulation) {
          det = new SimDetectorUnit_Scintillator_t;
        }
        else {
          det = new RealDetectorUnitScintillator;
        }
      }
      else if (type == "DetectorLite"){
        if (m_MCSimulation) {
          det = new SimDetectorUnitLite;
        }
        else {
          det = new RealDetectorUnitScintillator;
        }
      }
      else {
        std::cout << "Invalid detector type : " << type << std::endl;
        return false;
      }

      det->setID(detid);
      det->setName(name);
      det->setWidth(widthx, widthy);
      det->setThickness(thickness);
      det->setOffset(offsetx, offsety);
      det->setPixelPitch(pitchx, pitchy);
      det->setNumPixel(npx, npy);
      det->setCenterPosition(posx, posy, posz);
      det->setDirectionX(dirxx, dirxy, dirxz);
      det->setDirectionY(diryx, diryy, diryz);

      if (type == "Detector2DStrip"){
        RealDetectorUnit2DStrip* detStrip = 
          dynamic_cast<RealDetectorUnit2DStrip*>(det);
        if (detStrip==0) {
          std::cout << "dynamic cast error : RealDetectorUnit->RealDetector2DStrip" << std::endl;
        }
        detStrip->setPriorityToAnodeSide(priorityToAnode);
      }
      
      if (!m_MCSimulation) {
        ptree::const_assoc_iterator ASICsNode = detNode.find("asics");
        if ( ASICsNode != detNode.not_found() ) {
          if ( !constructASICs(ASICsNode->second, det, priorityToAnode) ) {
            delete det;
            return false;
          }
        }
      }
      
      m_DetectorVector.push_back(det);
      m_DetectorIDMap.insert(std::make_pair(detid, detIndex));
      
      if (m_MCSimulation) {
        VDeviceSimulation* detSim = dynamic_cast<VDeviceSimulation*>(det);
        if (detSim==0) {
          std::cout << "dynamic cast error : RealDetectorUnit->DeviceSimulation" << std::endl;
        }
        detSim->initializeTable();
        m_DeviceSimulationVector.push_back(detSim);
      }
      
      detIndex++;
    }
  }
  
  return true;
}


bool DetectorManager::constructASICs(const ptree& ASICsNode, RealDetectorUnit* detector, bool priority_to_anode)
{
  foreach (const ptree::value_type& v, ASICsNode.get_child("")) {
    if (v.first == "asic") {
      const ptree& asicNode = v.second;
      int numChannel = asicNode.get<int>("<xmlattr>.channel");
      bool nside = asicNode.get<bool>("<xmlattr>.nside");
      OneASICData* asicData = new OneASICData(numChannel, nside);
      if (nside == priority_to_anode) {
        asicData->setPrioritySide(true);
      }
      else {
        asicData->setPrioritySide(false);
      }
      
      m_ASICDataVector.push_back(asicData);
      detector->registerASICData(asicData);
    }
  }
  
  return true;
}


bool DetectorManager::constructReadoutModules(const ptree& ReadoutNode)
{
  int modIndex = 0;
  foreach (const ptree::value_type& v, ReadoutNode.get_child("")) {
    if (v.first == "module") {
      const ptree& modNode = v.second;
      DetectorReadModule* module = new DetectorReadModule;
      int modid = modNode.get<int>("<xmlattr>.id");
      module->setID(modid);
      
      foreach (const ptree::value_type& vv, modNode.get_child("")) {
        if (vv.first == "mod_asic") {
          const ptree& asicNode = vv.second;
          int mod_detid = asicNode.get<int>("detid");
          int mod_chipid = asicNode.get<int>("chipid");
          OneASICData* asicData = m_DetectorVector[ m_DetectorIDMap[mod_detid] ]->getASICData(mod_chipid);
          module->registerASICData(asicData);
        }
      }
      m_ReadModuleVector.push_back(module);
      m_ReadModuleIDMap.insert(std::make_pair(modid, modIndex));
      
      modIndex++;
    }
  }

  return true;
}


bool DetectorManager::loadSimulationParameter(const char* filename)
{
  if (!m_MCSimulation) {
    std::cout << "Error: MCSimulation is not set." << std::endl;
    return false;
  }

  if (!m_DetectorConstructed) {
    std::cout << "Error: Detector is not constucted. Construct detector first!" << std::endl;
    return false;
  }

  std::cout << "loading simulation parameters: " << filename << std::endl;

  ptree pt;
  try {
    read_xml(filename, pt);
  }
  catch (boost::property_tree::xml_parser_error&) {
    std::cout << "cannot parse: " << filename << std::endl;
    return false;
  }
  
  std::cout << "Detector Name: " << pt.get<std::string>("simulation.name") << std::endl;

  const ptree& rootNode = pt.find("simulation")->second;
  boost::optional<std::string> cceFile = rootNode.get_optional<std::string>("cce_file");
#if USE_ROOT
  if ( cceFile ) {
    boost::filesystem::path paramFilePath(filename);
    boost::filesystem::path dir = paramFilePath.branch_path();
    boost::filesystem::path relativePath(*cceFile);
    boost::filesystem::path fullPath = dir / relativePath;
    std::cout << "CCE map file: " << fullPath << std::endl;
    m_CCEMapFile = new TFile(fullPath.c_str());
  }
#endif

  if (rootNode.find("auto_position") != rootNode.not_found()) {
    std::cout << "Auto Position Mode On" << std::endl;
    m_AutoPosition = true;
  }

  if (rootNode.find("sd_check") != rootNode.not_found()) {
    std::cout << "SD Check Mode On" << std::endl;
    m_SDCheck = true;
  }

  ptree::const_assoc_iterator SDsNode = rootNode.find("sensitive_detectors");
  if (SDsNode  != rootNode.not_found()) {
    if (!setupSimulation(SDsNode->second)) {
      std::cout << "setSimulationParameters : failed" << std::endl;
      return false;
    }
  }
  else {
    std::cout << "\"sensitive_detectors\" element not found" << std::endl;
    return false;
  }

  return true;
}


bool DetectorManager::setupSimulation(const ptree& DetectorsNode)
{
  foreach (const ptree::value_type& v, DetectorsNode.get_child("")) {
    if (v.first == "sensitive_detector") {
      const ptree& SDNode = v.second;
      if (!setupSD(SDNode)) {
        return false;
      }
    }
  }
  return true;
}


bool DetectorManager::setupSD(const ptree& SDNode)
{
  std::string name = SDNode.get<std::string>("name");
  std::string type = SDNode.get<std::string>("type");

#if USE_ROOT
  boost::shared_ptr<DeviceSimulation> dscom;
  if (type == "Detector2DPad") {
    boost::shared_ptr<DeviceSimulation> tmp(new SimDetectorUnit_Pad_t);
    dscom = tmp;
  }
  else if (type == "Detector2DStrip") {
    boost::shared_ptr<DeviceSimulation> tmp(new SimDetectorUnit_Strip_t);
    dscom = tmp;
  }
  else if (type == "DetectorScintillator") {
    boost::shared_ptr<DeviceSimulation> tmp(new SimDetectorUnit_Scintillator_t);
    dscom = tmp;
  }
  else {
    std::cout << "Invalid detector type : " << type << std::endl;
    return false;
  }
  
  ptree::const_assoc_iterator paramCommon = SDNode.find("simulation_param_common");
  if ( paramCommon != SDNode.not_found() ) {
    if (!loadDeviceSimulationParam(type, paramCommon->second, *dscom)) {
      std::cout << "load error " << std::endl;
      return false;
    }
  }
#endif
  
  boost::optional<std::string> idMethod = SDNode.get_optional<std::string>("id_method");
  HXISGDSensitiveDetector* sdIDByKey = 0;
  AHSensitiveDetector* sdIDByCopyNo = 0;
  VHXISGDSensitiveDetector* sd = 0;
  
  if (idMethod && *idMethod=="copyno") {
    sdIDByCopyNo = new AHSensitiveDetector(name);
    int layerOffset = SDNode.get<int>("layer_offset");
    sdIDByCopyNo->SetLayerOffset(layerOffset);
    sd = sdIDByCopyNo;
  }
  else if (idMethod && *idMethod=="key") {
    sdIDByKey = new HXISGDSensitiveDetector(name);
    sd = sdIDByKey;
  }
  else { // same as method is "key"
    sdIDByKey = new HXISGDSensitiveDetector(name);
    sd = sdIDByKey;
  }
  
  m_SensitiveDetectorVector.push_back(sd);
  sd->SetDetectorManager(this);
  if (m_AutoPosition) sd->SetPositionCalculation();
  if (m_SDCheck) sd->SetSDCheck();

  foreach (const ptree::value_type& v, SDNode.get_child("detector_list")) {
    if (v.first == "detector") {
      const ptree& aDetectorNode = v.second;
      int detid = aDetectorNode.get<int>("<xmlattr>.id");

#if USE_ROOT
      DeviceSimulation* simDevice = dynamic_cast<DeviceSimulation*>(getDeviceSimulationByID(detid));
      if (simDevice) {
        simDevice->setUpsideAnode(dscom->UpsideAnode());
        if (type == "Detector2DPad") {
          SimDetectorUnit2DPad* simDevicePad = dynamic_cast<SimDetectorUnit2DPad*>(simDevice);
          boost::shared_ptr<SimDetectorUnit2DPad> dscomPad = boost::dynamic_pointer_cast<SimDetectorUnit2DPad>(dscom);
          simDevicePad->setUpsideReadElectrode(dscomPad->UpsideReadElectrode());
          simDevicePad->setCCEMap(dscomPad->getCCEMap());
        }
        if (type == "Detector2DStrip") {
          SimDetectorUnit2DStrip* simDeviceStrip = dynamic_cast<SimDetectorUnit2DStrip*>(simDevice);
          boost::shared_ptr<SimDetectorUnit2DStrip> dscomStrip = boost::dynamic_pointer_cast<SimDetectorUnit2DStrip>(dscom);
          simDeviceStrip->setUpsideXStrip(dscomStrip->UpsideXStrip());
          simDeviceStrip->setCCEMapXStrip(dscomStrip->getCCEMapXStrip());
          simDeviceStrip->setCCEMapYStrip(dscomStrip->getCCEMapYStrip());
        }
        simDevice->setSimPHAMode(dscom->SimPHAMode());
        simDevice->setMuTau(dscom->MuTauElectron(), dscom->MuTauHole());
        simDevice->setEField(dscom->BiasVoltage(),
                             dscom->EFieldMode(),
                             dscom->EFieldParam(0),
                             dscom->EFieldParam(1),
                             dscom->EFieldParam(2),
                             dscom->EFieldParam(3));
        simDevice->setSimDiffusionMode(dscom->SimDiffusionMode());
        simDevice->setDiffusionSigmaConstant(dscom->DiffusionSigmaConstantAnode(),
                                             dscom->DiffusionSigmaConstantCathode());
        simDevice->setDiffusionSpreadFactor(dscom->DiffusionSpreadFactorAnode(),
                                            dscom->DiffusionSpreadFactorCathode());
        
        ptree::const_assoc_iterator param = aDetectorNode.find("simulation_param");
        if ( param != aDetectorNode.not_found() ) {
          if (!loadDeviceSimulationParam(type, param->second, *simDevice)) {
            std::cout << "load error " << std::endl;
            return false;
          }
        }
        
        if (simDevice->SimPHAMode()>=2 && !simDevice->isCCEMapPrepared()) {
          simDevice->buildWPMap();
          simDevice->buildCCEMap();
        }
      }
#endif /* USE_ROOT */

      if (sdIDByKey) {
        std::string key = aDetectorNode.get<std::string>("<xmlattr>.key");
        sdIDByKey->RegisterDetectorID(detid, key);
      }

      if (sdIDByCopyNo) {
        int copyNo = aDetectorNode.get<int>("<xmlattr>.copyno");
        sdIDByCopyNo->RegisterDetectorID(detid, copyNo);
      }
    }
  }

  return true;
} 


#if USE_ROOT
bool DetectorManager::loadDeviceSimulationParam(std::string type,
                                                const ptree& ParamNode,
                                                DeviceSimulation& ds)
{
  boost::optional<bool> upsideAnode = ParamNode.get_optional<bool>("upside_anode.<xmlattr>.set");
  if (upsideAnode) ds.setUpsideAnode(*upsideAnode);
  
  boost::optional<bool> upsidePad = ParamNode.get_optional<bool>("upside_pad.<xmlattr>.set");
  if (upsidePad) {
    if (type=="Detector2DPad") {
      SimDetectorUnit2DPad& dstmp
        = dynamic_cast<SimDetectorUnit2DPad&>(ds);
      dstmp.setUpsideReadElectrode(*upsidePad);
    }
  }
  
  boost::optional<bool> upsideXStrip = ParamNode.get_optional<bool>("upside_xstrip.<xmlattr>.set");
  if (upsideXStrip) {
    if (type=="Detector2DStrip") {
      SimDetectorUnit2DStrip& dstmp
        = dynamic_cast<SimDetectorUnit2DStrip&>(ds);
      dstmp.setUpsideXStrip(*upsideXStrip);
    }
  }

  boost::optional<int> simPHA = ParamNode.get_optional<int>("sim_pha.<xmlattr>.mode");
  if (simPHA) {
    ds.setSimPHAMode(*simPHA);
    boost::optional<std::string> mapName = ParamNode.get_optional<std::string>("sim_pha.<xmlattr>.map");
    if ( mapName ) {
      ds.setCCEMapName(*mapName);
      if (type=="Detector2DPad") {
        SimDetectorUnit2DPad& dstmp = dynamic_cast<SimDetectorUnit2DPad&>(ds);
        TH3D* cce_map = (TH3D*)(m_CCEMapFile->Get(mapName->c_str()));
        dstmp.setCCEMap(cce_map);
      }
      else if (type=="Detector2DStrip") {
        SimDetectorUnit2DStrip& dstmp
          = dynamic_cast<SimDetectorUnit2DStrip&>(ds);
        TH2D* cce_mapx = (TH2D*)(m_CCEMapFile->Get((*mapName+"_x").c_str()));
        dstmp.setCCEMapXStrip(cce_mapx);
        TH2D* cce_mapy = (TH2D*)(m_CCEMapFile->Get((*mapName+"_y").c_str()));
        dstmp.setCCEMapYStrip(cce_mapy);
      }
    }

    double bias = 0.0;
    int emode = 1;
    double p0 = 0.0, p1 = 0.0, p2 = 0.0, p3 = 0.0;

    boost::optional<double> biasValue = ParamNode.get_optional<double>("sim_pha.bias");
    if ( biasValue ) {
      bias = (*biasValue) * volt;
    }

    boost::optional<int> emodeValue = ParamNode.get_optional<int>("sim_pha.efield.<xmlattr>.mode");
    if ( emodeValue ) {
      emode = *emodeValue;
      foreach (const ptree::value_type& v, ParamNode.get_child("sim_pha.efield")) {
        if ( v.first == "param0") p0 = boost::lexical_cast<double>(v.second.data());
        if ( v.first == "param1") p1 = boost::lexical_cast<double>(v.second.data());
        if ( v.first == "param2") p2 = boost::lexical_cast<double>(v.second.data());
        if ( v.first == "param3") p3 = boost::lexical_cast<double>(v.second.data());
      }
    }
    else {
      emode = ds.EFieldMode();
      p0 = ds.EFieldParam(0);
      p1 = ds.EFieldParam(1);
      p2 = ds.EFieldParam(2);
      p3 = ds.EFieldParam(3);
    }

    boost::optional<double> mutauElectronValue = ParamNode.get_optional<double>("sim_pha.mutau_electron");
    if ( mutauElectronValue ) {
      ds.setMuTauElectron((*mutauElectronValue) * (cm2/volt));
    }

    boost::optional<double> mutauHoleValue = ParamNode.get_optional<double>("sim_pha.mutau_hole");
    if ( mutauHoleValue ) {
      ds.setMuTauHole((*mutauHoleValue) * (cm2/volt));
    }

    ds.setEField(bias, emode, p0, p1, p2, p3);
  }

  boost::optional<int> simDiffusion = ParamNode.get_optional<int>("sim_diffusion.<xmlattr>.mode");
  if ( simDiffusion ) {
    ds.setSimDiffusionMode(*simDiffusion);

    boost::optional<double> spreadFactorAnode = ParamNode.get_optional<double>("sim_diffusion.spread_factor_anode");
    boost::optional<double> spreadFactorCathode = ParamNode.get_optional<double>("sim_diffusion.spread_factor_cathode");
    boost::optional<double> constantAnode = ParamNode.get_optional<double>("sim_diffusion.constant_anode");
    boost::optional<double> constantCathode = ParamNode.get_optional<double>("sim_diffusion.constant_cathode");
    
    if ( spreadFactorAnode) ds.setDiffusionSpreadFactorAnode(*spreadFactorAnode);
    if ( spreadFactorCathode) ds.setDiffusionSpreadFactorCathode(*spreadFactorCathode);
    if ( constantAnode ) ds.setDiffusionSigmaConstantAnode((*constantAnode)*um);
    if ( constantCathode ) ds.setDiffusionSigmaConstantCathode((*constantCathode)*um);
  }

  return true;
}
#endif /* USE_ROOT */


void DetectorManager::clearDetectorVector()
{
  std::vector<RealDetectorUnit*>::iterator it = m_DetectorVector.begin();
  while ( it != m_DetectorVector.end() ) {
    if (*it) delete *it;
    *it = 0;
    it++;
  }
  m_DetectorVector.clear();
}


void DetectorManager::clearReadModuleVector()
{
  std::vector<DetectorReadModule*>::iterator it = m_ReadModuleVector.begin();
  while ( it != m_ReadModuleVector.end() ) {
    if (*it) delete *it;
    *it = 0;
    it++;
  }
  m_ReadModuleVector.clear();
}


void DetectorManager::clearASICDataVector()
{
  std::vector<OneASICData*>::iterator it = m_ASICDataVector.begin();
  while ( it != m_ASICDataVector.end() ) {
    if (*it) delete *it;
    *it = 0;
    it++;
  }
  m_ASICDataVector.clear();
}


void DetectorManager::clearSensitiveDetectorVector()
{
  m_SensitiveDetectorVector.clear();
}

}
