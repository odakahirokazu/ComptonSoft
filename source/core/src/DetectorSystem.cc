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

#include "DetectorSystem.hh"

#include <iostream>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>

#include "TFile.h"

#include "G4SDManager.hh"
#include "G4LogicalVolumeStore.hh"

#include "AstroUnits.hh"
#include "RealDetectorUnitFactory.hh"
#include "SimDetectorUnitFactory.hh"
#include "RealDetectorUnit2DPad.hh"
#include "RealDetectorUnit2DStrip.hh"
#include "SimDetectorUnit2DPad.hh"
#include "SimDetectorUnit2DStrip.hh"
#include "MultiChannelData.hh"
#include "DeviceSimulation.hh"
#include "CSSensitiveDetector.hh"

using boost::property_tree::ptree;

namespace comptonsoft {

DetectorSystem::DetectorSystem()
  : MCSimulation_(false),
    simAutoPosition_(false),
    simSDCheck_(false),
    detectorConstructed_(false),
    CCEMapFile_(nullptr)
{
}

DetectorSystem::~DetectorSystem()
{
  if (CCEMapFile_) {
    CCEMapFile_->Close();
    delete CCEMapFile_;
    CCEMapFile_ = nullptr;
  }
}

MultiChannelData* DetectorSystem::
getMultiChannelData(const DetectorChannelID& channel)
{
  const int detectorID = channel.Detector();
  const int section = channel.Section();
  return getDetectorByID(detectorID)->getMultiChannelData(section);
}

MultiChannelData* DetectorSystem::
getMultiChannelData(const ReadoutChannelID& channel)
{
  const int moduleID = channel.ReadoutModule();
  const int section = channel.Section();
  const DetectorChannelID detectorChannel
    = getReadoutModuleByID(moduleID)->ReadoutSection(section);
  return getMultiChannelData(detectorChannel);
}

void DetectorSystem::initializeEvent()
{
  for (auto& detector: detectors_) {
    detector->initializeEvent();
  }
}

bool DetectorSystem::loadDetectorConfiguration(const std::string& filename)
{
  std::cout << "Loading detector configuration: " << filename << std::endl;
  if (detectorConstructed_) {
    std::cout << "Error: the detector system is already constuceted." << std::endl;
    return false;
  }

  ptree pt;
  try {
    read_xml(filename, pt);
  }
  catch (boost::property_tree::xml_parser_error& ex) {
    std::cout << "Cannot parse: " << filename << std::endl;
    std::cout << "xml_parser_error ===> \n"
              << "message: " << ex.message() << "\n"
              << "filename: " << ex.filename() << "\n"
              << "line: " << ex.line() << std::endl;
    return false;
  }

  std::cout << "Detector Name: " << pt.get<std::string>("configuration.name") << std::endl;

  const ptree& rootNode = pt.find("configuration")->second;
  detectorConstructed_ = constructDetectors(rootNode.find("detectors")->second);
  if (!detectorConstructed_) {
    return false;
  }
  
  bool readoutModuleConstructed = constructReadoutModules(rootNode.find("readout")->second);
  if (!readoutModuleConstructed) {
    return false;
  }

  return true;
}

bool DetectorSystem::constructDetectors(const ptree& DetectorsNode)
{
  std::unique_ptr<VDetectorUnitFactory> factory;
  if (isMCSimulation()) {
    factory.reset(new SimDetectorUnitFactory);
  }
  else {
    factory.reset(new RealDetectorUnitFactory);
  }
  
  int detectorIndex = 0;
  for (const ptree::value_type& v: DetectorsNode.get_child("")) {
    if (v.first == "detector") {
      const ptree& detNode = v.second;

      const int detectorID = detNode.get<int>("<xmlattr>.id");
      const std::string type = detNode.get<std::string>("type");
      const std::string name = detNode.get<std::string>("name");
      const double widthx = detNode.get<double>("geometry.widthx") * mm;
      const double widthy = detNode.get<double>("geometry.widthy") * mm;
      const double thickness = detNode.get<double>("geometry.thickness") * mm;
      const double offsetx = detNode.get<double>("geometry.offsetx") * mm;
      const double offsety = detNode.get<double>("geometry.offsety") * mm;
      const int npx = detNode.get<int>("pixel.numx");
      const int npy = detNode.get<int>("pixel.numy");
      const double pitchx = detNode.get<double>("pixel.pitchx") * mm;
      const double pitchy = detNode.get<double>("pixel.pitchy") * mm;
      const double posx = detNode.get<double>("position.x") * mm;
      const double posy = detNode.get<double>("position.y") * mm;
      const double posz = detNode.get<double>("position.z") * mm;
      const double dirxx = detNode.get<double>("direction_xaxis.x");
      const double dirxy = detNode.get<double>("direction_xaxis.y");
      const double dirxz = detNode.get<double>("direction_xaxis.z");
      const double diryx = detNode.get<double>("direction_yaxis.x");
      const double diryy = detNode.get<double>("direction_yaxis.y");
      const double diryz = detNode.get<double>("direction_yaxis.z");

      ElectrodeSide prioritySide = ElectrodeSide::Undefined;
      boost::optional<std::string> sideOpt = detNode.get_optional<std::string>("energy_priority");
      if (sideOpt) {
        std::string side = *sideOpt;
        if (side=="anode") {
          prioritySide = ElectrodeSide::Anode;
        }
        else if (side=="cathode") {
          prioritySide = ElectrodeSide::Cathode;
        }
      }

      VRealDetectorUnit* detectorUnit = factory->createDetectorUnit(type);
      if (detectorUnit) {
        detectors_.push_back(std::unique_ptr<VRealDetectorUnit>(detectorUnit));
        detectorIDMap_.insert(std::make_pair(detectorID, detectorIndex));     
      }
      else {
        std::cout << "Invalid detector type : " << type << std::endl;
        return false;
      }

      detectorUnit->setID(detectorID);
      detectorUnit->setName(name);
      detectorUnit->setWidth(widthx, widthy);
      detectorUnit->setThickness(thickness);
      detectorUnit->setOffset(offsetx, offsety);
      detectorUnit->setPixelPitch(pitchx, pitchy);
      detectorUnit->setNumPixel(npx, npy);
      detectorUnit->setCenterPosition(posx, posy, posz);
      detectorUnit->setDirectionX(dirxx, dirxy, dirxz);
      detectorUnit->setDirectionY(diryx, diryy, diryz);

      if (type == "Detector2DStrip"){
        RealDetectorUnit2DStrip* detectorUnitStrip = 
          dynamic_cast<RealDetectorUnit2DStrip*>(detectorUnit);
        if (detectorUnitStrip==0) {
          std::cout << "dynamic cast error : RealDetectorUnit->RealDetector2DStrip" << std::endl;
        }
        detectorUnitStrip->setPrioritySide(prioritySide);
      }
      
      ptree::const_assoc_iterator readoutNode = detNode.find("readout");
      if ( readoutNode != detNode.not_found() ) {
        if ( !constructMultiChannelData(readoutNode->second, detectorUnit, prioritySide) ) {
          return false;
        }
      }

      if (isMCSimulation()) {
        DeviceSimulation* deviceSimulation = dynamic_cast<DeviceSimulation*>(detectorUnit);
        if (deviceSimulation==nullptr) {
          std::cout << "dynamic cast error : VRealDetectorUnit -> VDeviceSimulation" << std::endl;
        }
        deviceSimulation->initializeTables();
        deviceSimulationVector_.push_back(deviceSimulation);
      }
      
      detectorIndex++;
    }
  }
  
  return true;
}

bool DetectorSystem::constructMultiChannelData(const ptree& readoutNode, VRealDetectorUnit* detector, ElectrodeSide priority_side)
{
  for (const ptree::value_type& v: readoutNode.get_child("")) {
    if (v.first == "section") {
      const ptree& sectionNode = v.second;
      const int numChannels = sectionNode.get<int>("<xmlattr>.num_channels");
      const std::string electrodeStr = sectionNode.get<std::string>("<xmlattr>.electrode_side");
      ElectrodeSide electrode = ElectrodeSide::Undefined;
      if (electrodeStr=="anode") {
        electrode = ElectrodeSide::Anode;
      }
      else if (electrodeStr=="cathode") {
        electrode = ElectrodeSide::Cathode;
      }

      MultiChannelData* mcd = new MultiChannelData(numChannels, electrode);
      if (electrode == priority_side) {
        mcd->setPrioritySide(true);
      }
      else {
        mcd->setPrioritySide(false);
      }
      detector->registerMultiChannelData(mcd);
    }
  }
  
  return true;
}

bool DetectorSystem::constructReadoutModules(const ptree& ReadoutNode)
{
  int moduleIndex = 0;
  for (const ptree::value_type& v: ReadoutNode.get_child("")) {
    if (v.first == "module") {
      const ptree& moduleNode = v.second;
      int moduleID = moduleNode.get<int>("<xmlattr>.id");
      DetectorReadoutModule* module = new DetectorReadoutModule;
      readoutModules_.push_back(std::unique_ptr<DetectorReadoutModule>(module));
      readoutModuleIDMap_.insert(std::make_pair(moduleID, moduleIndex));
      module->setID(moduleID);

      int readoutSectionIndex = 0;
      for (const ptree::value_type& vv: moduleNode.get_child("")) {
        if (vv.first == "section") {
          const ptree& sectionNode = vv.second;
          int detectorID = sectionNode.get<int>("detector.<xmlattr>.id");
          int sectionIndex = sectionNode.get<int>("detector.<xmlattr>.section");
          module->push(detectorID, sectionIndex);

          MultiChannelData* mcd
            = getMultiChannelData(DetectorChannelID(detectorID, sectionIndex));
          mcd->setReadoutSection(moduleID, readoutSectionIndex);

          readoutSectionIndex++;
        }
      }

      moduleIndex++;
    }
  }

  return true;
}

bool DetectorSystem::loadSimulationParameters(const std::string& filename)
{
  std::cout << "Loading simulation parameters: " << filename << std::endl;
    
  if (!isMCSimulation()) {
    std::cout << "Error: MCSimulation is not set." << std::endl;
    return false;
  }

  if (!detectorConstructed_) {
    std::cout << "Error: Detector is not constucted. Construct detector first!" << std::endl;
    return false;
  }

  ptree pt;
  try {
    read_xml(filename, pt);
  }
  catch (boost::property_tree::xml_parser_error& ex) {
    std::cout << "Cannot parse: " << filename << std::endl;
    std::cout << "xml_parser_error ===> \n"
              << "message: " << ex.message() << "\n"
              << "filename: " << ex.filename() << "\n"
              << "line: " << ex.line() << std::endl;
    return false;
  }
  
  std::cout << "Detector Name: " << pt.get<std::string>("simulation.name") << std::endl;

  const ptree& rootNode = pt.find("simulation")->second;
  boost::optional<std::string> cceFile = rootNode.get_optional<std::string>("cce_file");

  if ( cceFile ) {
    boost::filesystem::path paramFilePath(filename);
    boost::filesystem::path dir = paramFilePath.branch_path();
    boost::filesystem::path relativePath(*cceFile);
    boost::filesystem::path fullPath = dir / relativePath;
    std::cout << "CCE map file: " << fullPath << std::endl;
    CCEMapFile_ = new TFile(fullPath.c_str());
  }

  if (rootNode.find("auto_position") != rootNode.not_found()) {
    std::cout << "Auto Position Mode On" << std::endl;
    simAutoPosition_ = true;
  }

  if (rootNode.find("sd_check") != rootNode.not_found()) {
    std::cout << "SD Check Mode On" << std::endl;
    simSDCheck_ = true;
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

bool DetectorSystem::setupSimulation(const ptree& DetectorsNode)
{
  for (const ptree::value_type& v: DetectorsNode.get_child("")) {
    if (v.first == "sensitive_detector") {
      const ptree& SDNode = v.second;
      if (!setupSD(SDNode)) {
        return false;
      }
    }
  }
  return true;
}

bool DetectorSystem::setupSD(const ptree& SDNode)
{
  std::string name = SDNode.get<std::string>("name");
  std::string type = SDNode.get<std::string>("type");

  SimDetectorUnitFactory factory;
  std::unique_ptr<VRealDetectorUnit> detector(factory.createDetectorUnit(type));
  DeviceSimulation* dscom = dynamic_cast<DeviceSimulation*>(detector.get());
  ptree::const_assoc_iterator paramCommon = SDNode.find("simulation_param_common");
  if ( paramCommon != SDNode.not_found() ) {
    if (!loadDeviceSimulationParam(type, paramCommon->second, *dscom)) {
      std::cout << "load error " << std::endl;
      return false;
    }
  }
  
  boost::optional<std::string> idMethod = SDNode.get_optional<std::string>("id_method");
  VCSSensitiveDetector* sd = nullptr;
  CSSensitiveDetector<int>* sdIDByCopyNo = nullptr;
  CSSensitiveDetector<G4String>* sdIDByKey = nullptr;
  
  if (idMethod && *idMethod=="copyno") {
    sdIDByCopyNo = new CSSensitiveDetector<int>(name);
    sd = sdIDByCopyNo;
    int layerOffset = SDNode.get<int>("layer_offset");
    sd->SetLayerOffset(layerOffset);
  }
  else if (idMethod && *idMethod=="key") {
    sdIDByKey = new CSSensitiveDetector<G4String>(name);
    sd = sdIDByKey;
  }
  else { // same as method is "key"
    sdIDByKey = new CSSensitiveDetector<G4String>(name);
    sd = sdIDByKey;
  }
  
  sensitiveDetectorVector_.push_back(sd);
  sd->SetDetectorSystem(this);
  if (simAutoPosition_) sd->SetPositionCalculation();
  if (simSDCheck_) sd->SetSDCheck();

  for (const ptree::value_type& v: SDNode.get_child("detector_list")) {
    if (v.first == "detector") {
      const ptree& aDetectorNode = v.second;
      int detid = aDetectorNode.get<int>("<xmlattr>.id");

      DeviceSimulation* simDevice = getDeviceSimulationByID(detid);
      if (simDevice) {
        if (type == "Detector2DPad") {
          SimDetectorUnit2DPad* simDevicePad = dynamic_cast<SimDetectorUnit2DPad*>(simDevice);
          SimDetectorUnit2DPad* dscomPad = dynamic_cast<SimDetectorUnit2DPad*>(dscom);
          simDevicePad->setBottomSideElectrode(dscom->BottomSideElectrode());
          simDevicePad->setReadoutElectrode(dscomPad->ReadoutElectrode());
          simDevicePad->setCCEMap(dscomPad->getCCEMap());
        }
        if (type == "Detector2DStrip") {
          SimDetectorUnit2DStrip* simDeviceStrip = dynamic_cast<SimDetectorUnit2DStrip*>(simDevice);
          SimDetectorUnit2DStrip* dscomStrip = dynamic_cast<SimDetectorUnit2DStrip*>(dscom);
          simDeviceStrip->setBottomSideElectrode(dscomStrip->BottomSideElectrode());
          simDeviceStrip->setXStripSideElectrode(dscomStrip->XStripSideElectrode());
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

bool DetectorSystem::loadDeviceSimulationParam(std::string type,
                                               const ptree& ParamNode,
                                               DeviceSimulation& ds)
{
  boost::optional<bool> upsideAnode = ParamNode.get_optional<bool>("upside_anode.<xmlattr>.set");
  if (upsideAnode) {
    if (*upsideAnode) {
      ds.setBottomSideElectrode(ElectrodeSide::Cathode);
    }
    else {
      ds.setBottomSideElectrode(ElectrodeSide::Anode);
    }
  }
  
  boost::optional<bool> upsidePad = ParamNode.get_optional<bool>("upside_pad.<xmlattr>.set");
  if (upsidePad) {
    if (type=="Detector2DPad") {
      SimDetectorUnit2DPad& dstmp
        = dynamic_cast<SimDetectorUnit2DPad&>(ds);
      if (*upsidePad) {
        dstmp.setReadoutElectrode(dstmp.UpSideElectrode());
      }
      else {
        dstmp.setReadoutElectrode(dstmp.BottomSideElectrode());
      }
    }
  }
  
  boost::optional<bool> upsideXStrip = ParamNode.get_optional<bool>("upside_xstrip.<xmlattr>.set");
  if (upsideXStrip) {
    if (type=="Detector2DStrip") {
      SimDetectorUnit2DStrip& dstmp
        = dynamic_cast<SimDetectorUnit2DStrip&>(ds);
      if (*upsideXStrip) {
        dstmp.setXStripSideElectrode(dstmp.UpSideElectrode());
      }
      else {
        dstmp.setXStripSideElectrode(dstmp.BottomSideElectrode());
      }
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
        TH3D* cce_map = (TH3D*)(CCEMapFile_->Get(mapName->c_str()));
        dstmp.setCCEMap(cce_map);
      }
      else if (type=="Detector2DStrip") {
        SimDetectorUnit2DStrip& dstmp
          = dynamic_cast<SimDetectorUnit2DStrip&>(ds);
        TH2D* cce_mapx = (TH2D*)(CCEMapFile_->Get((*mapName+"_x").c_str()));
        dstmp.setCCEMapXStrip(cce_mapx);
        TH2D* cce_mapy = (TH2D*)(CCEMapFile_->Get((*mapName+"_y").c_str()));
        dstmp.setCCEMapYStrip(cce_mapy);
      }
    }

    double bias = 0.0;
    double p0 = 0.0, p1 = 0.0, p2 = 0.0, p3 = 0.0;

    boost::optional<double> biasValue = ParamNode.get_optional<double>("sim_pha.bias");
    if ( biasValue ) {
      bias = (*biasValue) * volt;
    }

    boost::optional<int> emodeValue = ParamNode.get_optional<int>("sim_pha.efield.<xmlattr>.mode");
    EFieldModel::FieldShape emode;
    if ( emodeValue ) {
      emode = static_cast<EFieldModel::FieldShape>(*emodeValue);
      for (const ptree::value_type& v: ParamNode.get_child("sim_pha.efield")) {
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

void DetectorSystem::registerGeant4SensitiveDetectors()
{
  for (auto& sd: sensitiveDetectorVector_) {
    G4String name = sd->GetName();
    G4SDManager::GetSDMpointer()->AddNewDetector(sd);
    G4LogicalVolume* lvol =
      G4LogicalVolumeStore::GetInstance()->GetVolume(name);
    if (lvol) {
      lvol->SetSensitiveDetector(sd);
    }
    else {
      std::cout << "Error: not found: Logical volume " << name << std::endl;
    }
  }
}

} /* namespace comptonsoft */
