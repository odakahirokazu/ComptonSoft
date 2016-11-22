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
#include <sstream>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>

#include "TFile.h"
#include "TSpline.h"

#include "G4SDManager.hh"
#include "G4LogicalVolumeStore.hh"

#include "AstroUnits.hh"
#include "RealDetectorUnitFactory.hh"
#include "SimDetectorUnitFactory.hh"
#include "RealDetectorUnit2DPixel.hh"
#include "RealDetectorUnit2DStrip.hh"
#include "SimDetectorUnit2DPixel.hh"
#include "SimDetectorUnit2DStrip.hh"
#include "MultiChannelData.hh"
#include "DeviceSimulation.hh"
#include "CSSensitiveDetector.hh"

namespace comptonsoft {

DetectorSystem::DetectorSystem()
  : MCSimulation_(false),
    simAutoPosition_(false),
    simSDCheck_(false),
    detectorConstructed_(false),
    ROOTFile_(nullptr)
{
  std::vector<std::string> defaultGroups = {"Anti", "Off", "LowZ", "HighZ"};
  for (auto& groupName: defaultGroups) {
    detectorGroupMap_[groupName]
      = std::unique_ptr<DetectorGroup>(new DetectorGroup(groupName));
  }
}

DetectorSystem::~DetectorSystem()
{
  if (ROOTFile_) {
    ROOTFile_->Close();
    delete ROOTFile_;
    ROOTFile_ = nullptr;
  }
}

MultiChannelData* DetectorSystem::
getMultiChannelData(const DetectorBasedChannelID& channelID)
{
  const int detectorID = channelID.Detector();
  const int section = channelID.Section();
  return getDetectorByID(detectorID)->getMultiChannelData(section);
}

MultiChannelData* DetectorSystem::
getMultiChannelData(const ReadoutBasedChannelID& channelID)
{
  const int moduleID = channelID.ReadoutModule();
  const int section = channelID.Section();
  const DetectorBasedChannelID detectorChannel
    = getReadoutModuleByID(moduleID)->getSection(section);
  return getMultiChannelData(detectorChannel);
}

DetectorBasedChannelID DetectorSystem::
convertToDetectorBasedChannelID(const ReadoutBasedChannelID& channelID)
{
  const int moduleID = channelID.ReadoutModule();
  const int section = channelID.Section();
  const int channel = channelID.Channel();
  const DetectorBasedChannelID detectorChannelID
    = getReadoutModuleByID(moduleID)->getSection(section);
  return DetectorBasedChannelID(detectorChannelID.Detector(),
                                detectorChannelID.Section(),
                                channel);
}

ReadoutBasedChannelID DetectorSystem::
convertToReadoutBasedChannelID(const DetectorBasedChannelID& channelID)
{
  const int channel = channelID.Channel();
  const ReadoutBasedChannelID readoutID = getMultiChannelData(channelID)->ReadoutID();
  const ReadoutBasedChannelID readoutChannelID(readoutID.ReadoutModule(),
                                               readoutID.Section(),
                                               channel);
  return readoutChannelID;
}

void DetectorSystem::printDetectorGroups() const
{
  std::cout << "List of Detector groups:\n";
  for (auto& pair: detectorGroupMap_) {
    DetectorGroup& dg = *(pair.second);
    dg.print();
  }
  std::cout << "\n"
            << "List of Hit patterns:\n";
  for (auto& hitpat: hitPatterns_) {
    std::cout << hitpat.Name() << '\n';
  }
  std::cout << std::endl;
}

void DetectorSystem::registerGeant4SensitiveDetectors()
{
  for (auto& sd: sensitiveDetectorVector_) {
    G4String name = sd->GetName();
    G4SDManager::GetSDMpointer()->AddNewDetector(sd);
    G4LogicalVolume* logicalVolume =
      G4LogicalVolumeStore::GetInstance()->GetVolume(name);
    if (logicalVolume) {
      logicalVolume->SetSensitiveDetector(sd);
    }
    else {
      std::ostringstream message;
      message << "Error: Logical volume = " << name << " is not found.\n";
      BOOST_THROW_EXCEPTION( CSException(message.str()) );
    }
  }
}

void DetectorSystem::initializeEvent()
{
  for (auto& detector: detectors_) {
    detector->initializeEvent();
  }
}

void DetectorSystem::readDetectorConfiguration(const std::string& filename)
{
  std::cout << "Loading detector configuration: " << filename << std::endl;
  if (detectorConstructed_) {
    std::string message = "Error: the detector system is already constuceted.";
    BOOST_THROW_EXCEPTION( CSException(message) );
  }

  boost::property_tree::ptree pt;
  try {
    read_xml(filename, pt);
  }
  catch (boost::property_tree::xml_parser_error& ex) {
    std::ostringstream message;
    message << "Cannot parse: " << filename << "\n"
            << "xml_parser_error ===> \n"
            << "message: " << ex.message() << "\n"
            << "filename: " << ex.filename() << "\n"
            << "line: " << ex.line() << "\n";
    BOOST_THROW_EXCEPTION( CSException(message.str()) );
  }

  try {
    loadDetectorConfigurationRootNode(pt);
  }
  catch (boost::property_tree::ptree_error& ex) {
    std::ostringstream message;
    message << "Loading " << filename << " failed\n";
    message << "ptree_error ===> \n";
    message << ex.what() << "\n";
    BOOST_THROW_EXCEPTION( CSException(message.str()) );
  }

  std::cout << std::endl;
}

void DetectorSystem::readDetectorParameters(const std::string& filename)
{
  std::cout << "Loading detector parameters: " << filename << std::endl;

  if (!detectorConstructed_) {
    std::string message = "Error: Detector is not constucted. Construct detector first!\n";
    BOOST_THROW_EXCEPTION( CSException(message) );
  }

  boost::property_tree::ptree pt;
  try {
    read_xml(filename, pt);
  }
  catch (boost::property_tree::xml_parser_error& ex) {
    std::ostringstream message;
    message << "Cannot parse: " << filename << "\n"
            << "xml_parser_error ===> \n"
            << "message: " << ex.message() << "\n"
            << "filename: " << ex.filename() << "\n"
            << "line: " << ex.line() << "\n";
    BOOST_THROW_EXCEPTION( CSException(message.str()) );
  }

  try {
    loadDetectorParametersRootNode(pt, filename);
  }
  catch (boost::property_tree::ptree_error& ex) {
    std::ostringstream message;
    message << "Loading " << filename << " failed\n";
    message << "ptree_error ===> \n";
    message << ex.what() << "\n";
    BOOST_THROW_EXCEPTION( CSException(message.str()) );
  }

  std::cout << std::endl;
}

void DetectorSystem::loadDetectorConfigurationRootNode(const boost::property_tree::ptree& RootNode)
{
  using boost::property_tree::ptree;
  using boost::optional;
  using boost::format;
  
  optional<std::string> name = RootNode.get_optional<std::string>("configuration.name");
  if (name) {
    std::cout << "Name: " << *name << std::endl;
  }
  else {
    std::string message("Error: detector name is not assigned.\n");
    BOOST_THROW_EXCEPTION( CSException(message) );
  }

  const ptree& configurationNode = RootNode.find("configuration")->second;
  const optional<std::string> lengthUnitName
    = configurationNode.get_optional<std::string>("length_unit");
  double lengthUnit = 1.0;
  if (lengthUnitName) {
    if (*lengthUnitName == "cm") {
      lengthUnit = cm;
    }
    else if (*lengthUnitName == "mm") {
      lengthUnit = mm;
    }
    else {
      std::ostringstream message;
      message << "Unknown length unit is given. Use cm or mm.";
      BOOST_THROW_EXCEPTION( CSException(message.str()) );
    }
  }
  else {
    std::ostringstream message;
    message << "length_unit is not given in the detector configuration.\n";
    message << "Set <length_unit>cm</length_unit> or <length_unit>mm</length_unit>.";
    BOOST_THROW_EXCEPTION( CSException(message.str()) );
  }

  loadDCDetectorsNode(configurationNode.find("detectors")->second, lengthUnit);
  detectorConstructed_ = true;

  loadDCReadoutNode(configurationNode.find("readout")->second);
  
  optional<const ptree&> groupsNode = configurationNode.get_child_optional("groups");
  if (groupsNode) {
    loadDCGroupsNode(*groupsNode);
  }
}

void DetectorSystem::loadDCDetectorsNode(const boost::property_tree::ptree& DetectorsNode,
                                         const double LengthUnit)
{
  using boost::property_tree::ptree;
  using boost::optional;
  using boost::format;

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
      const ptree& detectorNode = v.second;
      const boost::optional<int> detectorID = detectorNode.get_optional<int>("<xmlattr>.id");
      const boost::optional<std::string> type = detectorNode.get_optional<std::string>("<xmlattr>.type");
      const boost::optional<std::string> name = detectorNode.get_optional<std::string>("<xmlattr>.name");

      if (detectorID == boost::none) {
        std::ostringstream message;
        message << format("Error: Detector ID is not assigned at index %d\n") % detectorIndex;
        BOOST_THROW_EXCEPTION( CSException(message.str()) );
      }
      if (type == boost::none) {
        std::ostringstream message;
        message << format("Error: Detector type is not assigned at index %d\n") % detectorIndex;
        message << format("  Detector ID = %d\n") % *detectorID;
        BOOST_THROW_EXCEPTION( CSException(message.str()) );
      }
      if (name == boost::none) {
        std::ostringstream message;
        message << format("Error: Detector name is not assigned at index %d\n") % detectorIndex;
        message << format("  Detector ID = %d\n") % *detectorID;
        BOOST_THROW_EXCEPTION( CSException(message.str()) );
      }

      VRealDetectorUnit* detectorUnit = factory->createDetectorUnit(*type);
      if (detectorUnit) {
        detectors_.push_back(std::unique_ptr<VRealDetectorUnit>(detectorUnit));
        detectorIDMap_.insert(std::make_pair(*detectorID, detectorIndex));
      }
      else {
        std::ostringstream message;
        message << format("Error: Invalid detector type: %s\n") % *type;
        message << format("  Detector ID = %d\n") % *detectorID;
        BOOST_THROW_EXCEPTION( CSException(message.str()) );
      }

      detectorUnit->setID(*detectorID);
      detectorUnit->setName(*name);
      loadDCDetectorNode(detectorNode, LengthUnit, detectorUnit);
      detectorIndex++;
    }
  }
}

void DetectorSystem::
loadDCDetectorNode(const boost::property_tree::ptree& DetectorNode,
                   const double LengthUnit,
                   VRealDetectorUnit* detector)
{
  using boost::property_tree::ptree;
  using boost::optional;
  using boost::format;

  // geometry
  {
    const optional<double> geometry_x = DetectorNode.get_optional<double>("geometry.<xmlattr>.x");
    const optional<double> geometry_y = DetectorNode.get_optional<double>("geometry.<xmlattr>.y");
    const optional<double> geometry_z = DetectorNode.get_optional<double>("geometry.<xmlattr>.z");
    if (geometry_x && geometry_y && geometry_z) {
      detector->setWidth((*geometry_x)*LengthUnit, (*geometry_y)*LengthUnit);
      detector->setThickness((*geometry_z)*LengthUnit);
    }
    else if (geometry_x==boost::none && geometry_y==boost::none && geometry_z==boost::none) {
      std::cout << format("Warning: default geometry is used for Detector ID: %d") % detector->getID() << std::endl;
    }
    else {
      std::ostringstream message;
      message << format("Error: Detector geometry has an invalid setting for Detector ID: %d\n") % detector->getID();
      BOOST_THROW_EXCEPTION( CSException(message.str()) );
    }
  }
  
  // offset
  {
    const optional<double> offset_x = DetectorNode.get_optional<double>("offset.<xmlattr>.x");
    const optional<double> offset_y = DetectorNode.get_optional<double>("offset.<xmlattr>.y");
    if (offset_x && offset_y) {
      detector->setOffset((*offset_x)*LengthUnit, (*offset_y)*LengthUnit);
    }
    else if (offset_x==boost::none && offset_y==boost::none) {
      std::cout << format("Warning: default pixel offsets are used for Detector ID: %d") % detector->getID() << std::endl;
    }
    else {
      std::ostringstream message;
      message << format("Error: Detector pixel offset has an invalid setting for Detector ID: %d\n") % detector->getID();
      BOOST_THROW_EXCEPTION( CSException(message.str()) );
    }
  }
  
  // pixel
  {
    const optional<int> number_x = DetectorNode.get_optional<int>("pixel.<xmlattr>.number_x");
    const optional<int> number_y = DetectorNode.get_optional<int>("pixel.<xmlattr>.number_y");
    const optional<double> size_x = DetectorNode.get_optional<double>("pixel.<xmlattr>.size_x");
    const optional<double> size_y = DetectorNode.get_optional<double>("pixel.<xmlattr>.size_y");
    if (number_x && number_y && size_x && size_y) {
      detector->setNumPixel(*number_x, *number_y);
      detector->setPixelPitch((*size_x)*LengthUnit, (*size_y)*LengthUnit);
    }
    else if (number_x==boost::none && number_y==boost::none && size_x==boost::none && size_y==boost::none) {
      std::cout << format("Warning: default pixel settings are used for Detector ID: %d") % detector->getID() << std::endl;
    }
    else {
      std::ostringstream message;
      message << format("Error: Detector pixel settings are invalid for Detector ID: %d\n") % detector->getID();
      BOOST_THROW_EXCEPTION( CSException(message.str()) );
    }
  }
  
  // position
  {
    const optional<double> x = DetectorNode.get_optional<double>("position.<xmlattr>.x");
    const optional<double> y = DetectorNode.get_optional<double>("position.<xmlattr>.y");
    const optional<double> z = DetectorNode.get_optional<double>("position.<xmlattr>.z");
    if (x && y && z) {
      detector->setCenterPosition((*x)*LengthUnit, (*y)*LengthUnit, (*z)*LengthUnit);
    }
    else if (x==boost::none && y==boost::none && z==boost::none) {
      std::cout << format("Warning: default position is used for Detector ID: %d") % detector->getID() << std::endl;
    }
    else {
      std::ostringstream message;
      message << format("Error: Detector position has an invalid setting for Detector ID: %d\n") % detector->getID();
      BOOST_THROW_EXCEPTION( CSException(message.str()) );
    }
  }

  // direction
  {
    const optional<double> xdirx = DetectorNode.get_optional<double>("xaxis_direction.<xmlattr>.x");
    const optional<double> xdiry = DetectorNode.get_optional<double>("xaxis_direction.<xmlattr>.y");
    const optional<double> xdirz = DetectorNode.get_optional<double>("xaxis_direction.<xmlattr>.z");
    const optional<double> ydirx = DetectorNode.get_optional<double>("yaxis_direction.<xmlattr>.x");
    const optional<double> ydiry = DetectorNode.get_optional<double>("yaxis_direction.<xmlattr>.y");
    const optional<double> ydirz = DetectorNode.get_optional<double>("yaxis_direction.<xmlattr>.z");
    if (xdirx && xdiry && xdirz && ydirx && ydiry && ydirz) {
      detector->setXAxisDirection(*xdirx, *xdiry, *xdirz);
      detector->setYAxisDirection(*ydirx, *ydiry, *ydirz);
    }
    else if (xdirx==boost::none && xdiry==boost::none && xdirz==boost::none &&
             ydirx==boost::none && ydiry==boost::none && ydirz==boost::none) {
      std::cout << format("Warning: default axes directions are used for Detector ID: %d") % detector->getID() << std::endl;
    }
    else {
      std::ostringstream message;
      message << format("Error: Detector direction has an invalid setting for Detector ID: %d\n") % detector->getID();
      BOOST_THROW_EXCEPTION( CSException(message.str()) );
    }
  }

  // energy priority side
  ElectrodeSide prioritySide = ElectrodeSide::Undefined;
  {
    const optional<std::string> electrode_side
      = DetectorNode.get_optional<std::string>("energy_priority.<xmlattr>.electrode_side");
    if (electrode_side) {
      if(*electrode_side=="anode") {
        prioritySide = ElectrodeSide::Anode;
      }
      else if (*electrode_side=="cathode") {
        prioritySide = ElectrodeSide::Cathode;
      }
      else if (*electrode_side=="undefined") {
        prioritySide = ElectrodeSide::Undefined;
      }
      else {
        std::ostringstream message;
        message << format("Error: Unknown electrode side keyword is given for Detector ID: %d\n") % detector->getID();
        BOOST_THROW_EXCEPTION( CSException(message.str()) );
      }

      if (detector->checkType(DetectorType::DoubleSidedStripDetector)) {
        RealDetectorUnit2DStrip* detectorUnitStrip = 
          dynamic_cast<RealDetectorUnit2DStrip*>(detector);
        if (detectorUnitStrip == nullptr) {
          std::ostringstream message;
          message << format("Error: dynamic cast error: VRealDetectorUnit->RealDetector2DStrip for Detector ID: %d\n") % detector->getID();
          BOOST_THROW_EXCEPTION( CSException(message.str()) );
        }
        detectorUnitStrip->setPrioritySide(prioritySide);
      }
    }
  }

  ptree::const_assoc_iterator sectionsNode = DetectorNode.find("sections");
  if ( sectionsNode != DetectorNode.not_found() ) {
    loadDCDetectorSectionsNode(sectionsNode->second, prioritySide, detector);
  }
  
  if (isMCSimulation()) {
    DeviceSimulation* deviceSimulation = dynamic_cast<DeviceSimulation*>(detector);
    if (deviceSimulation == nullptr) {
      std::ostringstream message;
      message << format("Error: dynamic cast error: VRealDetectorUnit -> DeviceSimulation for Detector ID: %d\n") % detector->getID();
      BOOST_THROW_EXCEPTION( CSException(message.str()) );
    }
    deviceSimulation->initializeTables();
    deviceSimulationVector_.push_back(deviceSimulation);
  }
}

void DetectorSystem::
loadDCDetectorSectionsNode(const boost::property_tree::ptree& SectionsNode,
                           ElectrodeSide priority_side,
                           VRealDetectorUnit* detector)
{
  using boost::property_tree::ptree;
  using boost::optional;
  using boost::format;
  
  for (const ptree::value_type& v: SectionsNode.get_child("")) {
    if (v.first == "section") {
      const ptree& sectionNode = v.second;
      const int numChannels = sectionNode.get<int>("<xmlattr>.num_channels");
      const optional<std::string> electrode_o = sectionNode.get_optional<std::string>("<xmlattr>.electrode_side");
      ElectrodeSide electrode = ElectrodeSide::Undefined;
      if (electrode_o) {
        if (*electrode_o=="anode") {
          electrode = ElectrodeSide::Anode;
        }
        else if (*electrode_o=="cathode") {
          electrode = ElectrodeSide::Cathode;
        }
        else if (*electrode_o=="undefined") {
          electrode = ElectrodeSide::Undefined;
        }
        else {
          std::ostringstream message;
          message << format("Error: Unknown electrode side keyword is given for Detector ID: %d\n") % detector->getID();
          BOOST_THROW_EXCEPTION( CSException(message.str()) );
        }
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
}

void DetectorSystem::loadDCReadoutNode(const boost::property_tree::ptree& ReadoutNode)
{
  using boost::property_tree::ptree;

  int moduleIndex = 0;
  for (const ptree::value_type& v: ReadoutNode.get_child("")) {
    if (v.first == "module") {
      const ptree& moduleNode = v.second;
      const int moduleID = moduleNode.get<int>("<xmlattr>.id");
      readoutModules_.push_back(std::unique_ptr<ReadoutModule>(new ReadoutModule));
      ReadoutModule* module = readoutModules_.back().get();
      readoutModuleIDMap_.insert(std::make_pair(moduleID, moduleIndex));
      module->setID(moduleID);

      int readoutSectionIndex = 0;
      for (const ptree::value_type& vv: moduleNode.get_child("")) {
        if (vv.first == "section") {
          const ptree& sectionNode = vv.second;
          const int detectorID = sectionNode.get<int>("<xmlattr>.detector_id");
          const int section = sectionNode.get<int>("<xmlattr>.section");
          module->push(detectorID, section);

          MultiChannelData* mcd
            = getMultiChannelData(DetectorBasedChannelID(detectorID, section));
          mcd->setReadoutID(moduleID, readoutSectionIndex);

          readoutSectionIndex++;
        }
      }

      moduleIndex++;
    }
  }
}

void DetectorSystem::loadDCGroupsNode(const boost::property_tree::ptree& GroupsNode)
{
  using boost::property_tree::ptree;
  
  for (const ptree::value_type& v: GroupsNode.get_child("")) {
    if (v.first == "group") {
      const ptree& groupNode = v.second;
      const std::string name = groupNode.get<std::string>("<xmlattr>.name");
      DetectorGroup* group = new DetectorGroup(name);
      detectorGroupMap_[name] = std::unique_ptr<DetectorGroup>(group);
      for (const ptree::value_type& vv: groupNode.get_child("")) {
        if (vv.first == "detector") {
          const int detectorID = vv.second.get<int>("<xmlattr>.id");
          group->add(detectorID);
        }
      }
    }
    else if (v.first=="hit_pattern") {
      HitPattern hitpat;
      const ptree& hitpatNode = v.second;
      hitpat.setName(hitpatNode.get<std::string>("<xmlattr>.name"));
      hitpat.setShortName(hitpatNode.get<std::string>("<xmlattr>.short_name"));
      hitpat.setBit(hitpatNode.get<unsigned int>("<xmlattr>.bit"));
      for (const ptree::value_type& vv: hitpatNode.get_child("")) {
        if (vv.first == "group") {
          const std::string group = vv.second.get<std::string>("<xmlattr>.name");
          hitpat.add(getDetectorGroup(group));
        }
      }
      hitPatterns_.push_back(std::move(hitpat));
    }
  }
}

void DetectorSystem::
loadDetectorParametersRootNode(const boost::property_tree::ptree& RootNode,
                               const std::string& filename)
{
  using boost::property_tree::ptree;
  using boost::optional;
  using boost::format;
  
  std::cout << "Name: " << RootNode.get<std::string>("detector_parameters.name") << std::endl;

  const ptree& mainNode = RootNode.find("detector_parameters")->second;

  if (optional<std::string> rootFile = mainNode.get_optional<std::string>("root_file")) {
    boost::filesystem::path paramFilePath(filename);
    boost::filesystem::path dir = paramFilePath.branch_path();
    boost::filesystem::path relativePath(*rootFile);
    boost::filesystem::path fullPath = dir / relativePath;
    std::cout << "ROOT file: " << fullPath << std::endl;
    ROOTFile_ = new TFile(fullPath.c_str());
  }

  if (optional<int> autoPositionFlag = mainNode.get_optional<int>("auto_position.<xmlattr>.flag")) {
    if (autoPositionFlag && *autoPositionFlag==1) {
      std::cout << "Auto position mode: On" << std::endl;
      simAutoPosition_ = true;
    }
  }

  if (optional<int> SDCheckFlag = mainNode.get_optional<int>("sensitive_detector_not_found_warning.<xmlattr>.flag")) {
    if (SDCheckFlag && *SDCheckFlag==1) {
      std::cout << "Sensitive detector not found warning (SD Check Mode): On" << std::endl;
      simSDCheck_ = true;
    }
  }

  ptree::const_assoc_iterator dataNode = mainNode.find("data");
  if (dataNode != RootNode.not_found()) {
    loadDetectorParametersDataNode(dataNode->second);
  }
  else {
    std::ostringstream message;
    message << "\"data\" element not found\n";
    BOOST_THROW_EXCEPTION( CSException(message.str()) );
  }
}

void DetectorSystem::
loadDetectorParametersDataNode(const boost::property_tree::ptree& DataNode)
{
  using boost::property_tree::ptree;
  
  for (const ptree::value_type& v: DataNode.get_child("")) {
    if (v.first == "sensitive_detector") {
      const ptree& node = v.second;
      loadDPDetectorSetNode(node, true);
    }
    else if (v.first == "detector_set") {
      const ptree& node = v.second;
      loadDPDetectorSetNode(node, false);
    }
  }
}

void DetectorSystem::loadDPDetectorSetNode(const boost::property_tree::ptree& SetNode,
                                           bool isSensitiveDetector)
{
  using boost::property_tree::ptree;
  using boost::optional;
  
  const optional<std::string> name = SetNode.get_optional<std::string>("<xmlattr>.name");
  const optional<std::string> prefix = SetNode.get_optional<std::string>("<xmlattr>.prefix");
  const optional<std::string> type = SetNode.get_optional<std::string>("<xmlattr>.type");

  if (isSensitiveDetector && name == boost::none) {
    std::ostringstream message;
    message << "Sensitive detector name is not assigned.\n";
    BOOST_THROW_EXCEPTION( CSException(message.str()) );
  }
  if (!isSensitiveDetector && prefix == boost::none) {
    std::ostringstream message;
    message << "Detector prefix is not assigned.\n";
    BOOST_THROW_EXCEPTION( CSException(message.str()) );
  }
  if (type == boost::none) {
    std::ostringstream message;
    message << "Detector type is not assigned.\n";
    if (name) { message << "Detector name: " << name << "\n"; }
    if (prefix) { message << "Detector prefix: " << prefix << "\n"; }
    BOOST_THROW_EXCEPTION( CSException(message.str()) );
  }

  ParametersNodeContents commonParameters;
  if (optional<const ptree&> parametersNode = SetNode.get_child_optional("common.parameters")) {
    commonParameters.load(*parametersNode);
  }

  if (!isSensitiveDetector && prefix) {
    for (auto& detector: getDetectors()) {
      if (detector->getNamePrefix() == *prefix) {
        if (isMCSimulation()) {
          DeviceSimulation* device = getDeviceSimulationByID(detector->getID());
          setupDetectorParameters(commonParameters, device);
        }
        else {
          setupDetectorParameters(commonParameters, detector.get());
        }
      }
    }
  }

  CSSensitiveDetector<int>* sensitiveDetector_copyno = nullptr;
  CSSensitiveDetector<G4String>* sensitiveDetector_path = nullptr;
  if (isMCSimulation() && isSensitiveDetector) {
    const optional<std::string> idMethod = SetNode.get_optional<std::string>("detector_list.<xmlattr>.id_method");
    VCSSensitiveDetector* sensitiveDetector = nullptr;
    if (idMethod && *idMethod=="copyno") {
      sensitiveDetector_copyno = new CSSensitiveDetector<int>(*name);
      sensitiveDetector = sensitiveDetector_copyno;
    }
    else if (idMethod && *idMethod=="path") {
      sensitiveDetector_path = new CSSensitiveDetector<G4String>(*name);
      sensitiveDetector = sensitiveDetector_path;
    }
    else {
      std::ostringstream message;
      message << "Invalid volume identification method: " << *idMethod << "\n"
              << "Use a keyword \"path\" or \"copyno\".\n"
              << "Sensitive detector name: " << *name << "\n";
      BOOST_THROW_EXCEPTION( CSException(message.str()) );
    }

    const optional<int> layerOffset = SetNode.get_optional<int>("detector_list.<xmlattr>.layer_offset");
    if (layerOffset) {
      sensitiveDetector->SetLayerOffset(*layerOffset);
    }
  
    sensitiveDetectorVector_.push_back(sensitiveDetector);
    sensitiveDetector->SetDetectorSystem(this);
    if (simAutoPosition_) sensitiveDetector->SetPositionCalculation();
    if (simSDCheck_) sensitiveDetector->SetSDCheck();
  }

  if (optional<const ptree&> detectorListNode = SetNode.get_child_optional("detector_list")) {
    for (const ptree::value_type& v: *detectorListNode) {
      if (v.first == "detector") {
        const ptree& DetectorNode = v.second;
        const int DetectorID = DetectorNode.get<int>("<xmlattr>.id");
        ParametersNodeContents parameters(commonParameters);
        if (optional<const ptree&> parametersNode = DetectorNode.get_child_optional("parameters")) {
          parameters.load(*parametersNode);
        }

        if (isMCSimulation()) {
          DeviceSimulation* device = getDeviceSimulationByID(DetectorID);
          if (device == nullptr) {
            std::ostringstream message;
            message << "Error: Detector ID = " << DetectorID << " is not registered.\n";
            BOOST_THROW_EXCEPTION( CSException(message.str()) );
          }

          setupDetectorParameters(parameters, device);

          if (isSensitiveDetector) {
            if (sensitiveDetector_path) {
              const std::string path = DetectorNode.get<std::string>("<xmlattr>.path");
              sensitiveDetector_path->RegisterDetectorID(DetectorID, path);
            }
            if (sensitiveDetector_copyno) {
              const int copyNo = DetectorNode.get<int>("<xmlattr>.copyno");
              sensitiveDetector_copyno->RegisterDetectorID(DetectorID, copyNo);
            }
          }
        }
        else {
          VRealDetectorUnit* detector = getDetectorByID(DetectorID);
          if (detector == nullptr) {
            std::ostringstream message;
            message << "Error: Detector ID = " << DetectorID << " is not registered.\n";
            BOOST_THROW_EXCEPTION( CSException(message.str()) );
          }
          setupDetectorParameters(parameters, detector);
        }
      }
    }
  }
}

void DetectorSystem::setupDetectorParameters(const DetectorSystem::ParametersNodeContents parameters,
                                             VRealDetectorUnit* detector)
{
  using boost::optional;
  
  const int NumSections = detector->NumberOfMultiChannelData();
  for (int section=0; section<NumSections; section++) {
    MultiChannelData* mcd = detector->getMultiChannelData(section);
    if (detector->checkType(DetectorType::DoubleSidedStripDetector)) {
      if (mcd->isAnodeSide()) {
        const ChannelNodeContents& channel_properties = parameters.channel_properties_anode;
        if (optional<int> o = channel_properties.disable_status) {
          mcd->resetChannelDisabledVector(*o);
        }
        if (optional<double> o = channel_properties.threshold_value) {
          const double value = (*o)*keV;
          mcd->resetThresholdEnergyVector(value);
        }
      }
      else if (mcd->isCathodeSide()) {
        const ChannelNodeContents& channel_properties = parameters.channel_properties_cathode;
        if (optional<int> o = channel_properties.disable_status) {
          mcd->resetChannelDisabledVector(*o);
        }
        if (optional<double> o = channel_properties.threshold_value) {
          const double value = (*o)*keV;
          mcd->resetThresholdEnergyVector(value);
        }
      }
    }
    else {
      const ChannelNodeContents& channel_properties = parameters.channel_properties;
      if (optional<int> o = channel_properties.disable_status) {
        mcd->resetChannelDisabledVector(*o);
      }
      if (optional<double> o = channel_properties.threshold_value) {
        const double value = (*o)*keV;
        mcd->resetThresholdEnergyVector(value);
      }
    }
  }

  setupReconstructionParameters(parameters, detector);
}

void DetectorSystem::setupDetectorParameters(const DetectorSystem::ParametersNodeContents parameters,
                                             DeviceSimulation* ds)
{
  if (auto upside_anode = parameters.upside_anode) {
    if (*upside_anode == 1) {
      ds->setBottomSideElectrode(ElectrodeSide::Cathode);
    }
    else if (*upside_anode == 0) {
      ds->setBottomSideElectrode(ElectrodeSide::Anode);
    }
  }

  if (ds->checkType(DetectorType::PixelDetector)) {
    if (auto upside_pixel = parameters.upside_pixel) {
      SimDetectorUnit2DPixel* ds1
        = dynamic_cast<SimDetectorUnit2DPixel*>(ds);
      if (ds1) {
        if (*upside_pixel == 1) {
          ds1->setReadoutElectrode(ds1->UpSideElectrode());
        }
        else if (*upside_pixel == 0) {
          ds1->setReadoutElectrode(ds1->BottomSideElectrode());
        }
      }
    }
  }

  if (ds->checkType(DetectorType::DoubleSidedStripDetector)) {
    if (auto upside_xstrip = parameters.upside_xstrip) {
      SimDetectorUnit2DStrip* ds1
        = dynamic_cast<SimDetectorUnit2DStrip*>(ds);
      if (ds1) {
        if (*upside_xstrip == 1) {
          ds1->setXStripSideElectrode(ds1->UpSideElectrode());
        }
        else if (*upside_xstrip == 0) {
          ds1->setXStripSideElectrode(ds1->BottomSideElectrode());
        }
      }
    }
  }

  if (auto o = parameters.quenching_factor) {
    ds->setQuenchingFactor(*o);
  }

  if (auto o = parameters.temperature_value) {
    const double value = (*o)*kelvin;
    ds->setTemperature(value);
  }

  if (auto o = parameters.efield_bias) {
    const double bias = (*o)*volt;
    const int mode = (parameters.efield_mode) ? *(parameters.efield_mode) : 1;
    const double p0 = (parameters.efield_param0) ? *(parameters.efield_param0) : 0.0;
    const double p1 = (parameters.efield_param1) ? *(parameters.efield_param1) : 0.0;
    const double p2 = (parameters.efield_param2) ? *(parameters.efield_param2) : 0.0;
    const double p3 = (parameters.efield_param3) ? *(parameters.efield_param3) : 0.0;
    EFieldModel::FieldShape emode = static_cast<EFieldModel::FieldShape>(mode);
    ds->setEField(bias, emode, p0, p1, p2, p3);
  }

  if (auto o = parameters.charge_collection_mode) {
    const int mode = *o;
    ds->setChargeCollectionMode(mode);

    if (auto mapName = parameters.charge_collection_cce_map) {
      ds->setCCEMapName(*mapName);
      if (ds->checkType(DetectorType::PixelDetector)) {
        SimDetectorUnit2DPixel* ds1
          = dynamic_cast<SimDetectorUnit2DPixel*>(ds);
        if (ds1) {
          TH3D* cce_map = (TH3D*)(ROOTFile_->Get((*mapName).c_str()));
          ds1->setCCEMap(cce_map);
        }
      }
      else if (ds->checkType(DetectorType::DoubleSidedStripDetector)) {
        SimDetectorUnit2DStrip* ds1
          = dynamic_cast<SimDetectorUnit2DStrip*>(ds);
        if (ds1) {
          TH2D* cce_mapx = (TH2D*)(ROOTFile_->Get((*mapName+"_x").c_str()));
          ds1->setCCEMapXStrip(cce_mapx);
          TH2D* cce_mapy = (TH2D*)(ROOTFile_->Get((*mapName+"_y").c_str()));
          ds1->setCCEMapYStrip(cce_mapy);
        }
      }
    }

    if (auto o = parameters.charge_collection_mutau_electron) {
      const double value = (*o)*(cm2/volt);
      ds->setMuTauElectron(value);
    }
    if (auto o = parameters.charge_collection_mutau_hole) {
      const double value = (*o)*(cm2/volt);
      ds->setMuTauHole(value);
    }

    if (ds->ChargeCollectionMode()>=2 && !ds->isCCEMapPrepared()) {
      ds->buildWPMap();
      ds->buildCCEMap();
    }
  }
  
  if (auto o = parameters.diffusion_mode) {
    const int mode = *o;
    ds->setDiffusionMode(mode);
    if (auto o = parameters.diffusion_spread_factor_cathode) {
      ds->setDiffusionSpreadFactorCathode(*o);
    }
    if (auto o = parameters.diffusion_spread_factor_anode) {
      ds->setDiffusionSpreadFactorAnode(*o);
    }
    if (auto o = parameters.diffusion_constant_cathode) {
      const double value = (*o)*cm;
      ds->setDiffusionSigmaConstantCathode(value);
    }
    if (auto o = parameters.diffusion_constant_anode) {
      const double value = (*o)*cm;
      ds->setDiffusionSigmaConstantAnode(value);
    }
  }
  
  if (auto o = parameters.timing_resolution_trigger) {
    const double value = (*o)*second;
    ds->setTimingResolutionForTrigger(value);
  }
  if (auto o = parameters.timing_resolution_energy_measurement) {
    const double value = (*o)*second;
    ds->setTimingResolutionForEnergyMeasurement(value);
  }
  if (auto o = parameters.pedestal_generation_flag) {
    if (*o == 1) {
      ds->enablePedestal(true);
    }
    else {
      ds->enablePedestal(false);
    }
  }

  if (ds->checkType(DetectorType::DoubleSidedStripDetector)) {
    SimDetectorUnit2DStrip* ds1
      = dynamic_cast<SimDetectorUnit2DStrip*>(ds);
    if (ds1) {
      const ChannelNodeContents& xstrip_info =
        (ds1->isXStripSideCathode()) ?
        parameters.channel_properties_cathode :
        parameters.channel_properties_anode;
      const ChannelNodeContents& ystrip_info =
        (ds1->isXStripSideCathode()) ?
        parameters.channel_properties_anode :
        parameters.channel_properties_cathode;
    
      constexpr int XSIDE = 1;
      constexpr int YSIDE = 2;
      for (int side: {XSIDE, YSIDE}) {
        auto selector = (side==XSIDE) ?
          std::mem_fn(&PixelID::isXStrip) :
          std::mem_fn(&PixelID::isYStrip);
        const ChannelNodeContents& strip_info = (side==XSIDE) ?
          xstrip_info :
          ystrip_info;
        
        if (auto o = strip_info.disable_status) {
          ds->setChannelDisabledToSelected(*o, selector);
        }
        if (auto o = strip_info.trigger_discrimination_center) {
          const double value = (*o)*keV;
          ds->setTriggerDiscriminationCenterToSelected(value, selector);
        }
        if (auto o = strip_info.trigger_discrimination_sigma) {
          const double value = (*o)*keV;
          ds->setTriggerDiscriminationSigmaToSelected(value, selector);
        }
        if (auto o = strip_info.noise_level_param0) {
          ds->setNoiseParam0ToSelected(*o, selector);
        }
        if (auto o = strip_info.noise_level_param1) {
          ds->setNoiseParam1ToSelected(*o, selector);
        }
        if (auto o = strip_info.noise_level_param2) {
          ds->setNoiseParam2ToSelected(*o, selector);
        }
        if (auto o = strip_info.compensation_factor) {
          ds->setEPICompensationFactorToSelected(*o, selector);
        }
        if (auto o = strip_info.compensation_function) {
          const TSpline* func = nullptr;
          ROOTFile_->GetObject(o->c_str(), func);
          ds->setEPICompensationFunctionToSelected(func, selector);
        }
        if (auto o = strip_info.threshold_value) {
          const double value = (*o)*keV;
          ds->setThresholdToSelected(value, selector);
        }
      }
    }
  }
  else {
    const ChannelNodeContents& channel_properties = parameters.channel_properties;
    if (auto o = channel_properties.disable_status) {
      ds->resetChannelDisabledVector(*o);
    }
    if (auto o = channel_properties.trigger_discrimination_center) {
      const double value = (*o)*keV;
      ds->resetTriggerDiscriminationCenterVector(value);
    }
    if (auto o = channel_properties.trigger_discrimination_sigma) {
      const double value = (*o)*keV;
      ds->resetTriggerDiscriminationSigmaVector(value);
    }
    if (auto o = channel_properties.noise_level_param0) {
      ds->resetNoiseParam0Vector(*o);
    }
    if (auto o = channel_properties.noise_level_param1) {
      ds->resetNoiseParam1Vector(*o);
    }
    if (auto o = channel_properties.noise_level_param2) {
      ds->resetNoiseParam2Vector(*o);
    }
    if (auto o = channel_properties.compensation_factor) {
      ds->resetEPICompensationFactorVector(*o);
    }
    if (auto o = channel_properties.compensation_function) {
      const TSpline* func = nullptr;
      ROOTFile_->GetObject(o->c_str(), func);
      ds->resetEPICompensationFunctionVector(func);
    }
    if (auto o = channel_properties.threshold_value) {
      const double value = (*o)*keV;
      ds->resetThresholdVector(value);
    }
  }

  VRealDetectorUnit* detector = dynamic_cast<VRealDetectorUnit*>(ds);
  if (detector) {
    setupReconstructionParameters(parameters, detector);
  }
}

void DetectorSystem::setupReconstructionParameters(const DetectorSystem::ParametersNodeContents parameters,
                                                   VRealDetectorUnit* detector)
{
  if (auto o = parameters.reconstruction_mode) {
    detector->setReconstructionMode(*o);

    double lc0(0.0), lc1(1.0), uc0(0.0), uc1(1.0);
    RealDetectorUnit2DStrip* dsd = dynamic_cast<RealDetectorUnit2DStrip*>(detector);
    if (dsd) {
      if (auto o1 = parameters.reconstruction_energy_consistency_check_lower_function_c0) {
        if (auto o2 = parameters.reconstruction_energy_consistency_check_lower_function_c1) {
          if (auto o3 = parameters.reconstruction_energy_consistency_check_upper_function_c0) {
            if (auto o4 = parameters.reconstruction_energy_consistency_check_upper_function_c1) {
              lc0 = (*o1);
              lc1 = (*o2) * keV;
              uc0 = (*o3);
              uc1 = (*o4) * keV;
              dsd->setEnergyConsistencyCheckFunctions(lc0, lc1, uc0, uc1);
            }
          }
        }
      }
    }
  }
}

void DetectorSystem::ChannelNodeContents::
load(const boost::property_tree::ptree& node)
{
  if (auto o = node.get_optional<int>("<xmlattr>.id")) {
    id = o;
  }
  if (auto o = node.get_optional<int>("disable.<xmlattr>.status")) {
    disable_status = o;
  }
  if (auto o = node.get_optional<double>("noise_level.<xmlattr>.param0")) {
    noise_level_param0 = o;
  }
  if (auto o = node.get_optional<double>("noise_level.<xmlattr>.param1")) {
    noise_level_param1 = o;
  }
  if (auto o = node.get_optional<double>("noise_level.<xmlattr>.param2")) {
    noise_level_param2 = o;
  }
  if (auto o = node.get_optional<double>("trigger_discrimination.<xmlattr>.center")) {
    trigger_discrimination_center = o;
  }
  if (auto o = node.get_optional<double>("trigger_discrimination.<xmlattr>.sigma")) {
    trigger_discrimination_sigma = o;
  }
  if (auto o = node.get_optional<double>("compensation.<xmlattr>.factor")) {
    compensation_factor = o;
  }
  if (auto o = node.get_optional<std::string>("compensation.<xmlattr>.function")) {
    compensation_function = o;
  }
  if (auto o = node.get_optional<double>("threshold.<xmlattr>.value")) {
    threshold_value = o;
  }
}

void DetectorSystem::ParametersNodeContents::
load(const boost::property_tree::ptree& node)
{
  using boost::optional;
  using boost::property_tree::ptree;
  
  if (auto o=node.get_optional<int>("upside.<xmlattr>.anode")) {
    upside_anode = o;
  }
  if (auto o=node.get_optional<int>("upside.<xmlattr>.pixel")) {
    upside_pixel = o;
  }
  if (auto o=node.get_optional<int>("upside.<xmlattr>.xstrip")) {
    upside_xstrip = o;
  }
  if (auto o=node.get_optional<double>("quenching.<xmlattr>.factor")) {
    quenching_factor = o;
  }
  if (auto o=node.get_optional<double>("temperature.<xmlattr>.value")) {
    temperature_value = o;
  }
  if (auto o=node.get_optional<double>("efield.<xmlattr>.bias")) {
    efield_bias = o;
  }
  if (auto o=node.get_optional<int>("efield.<xmlattr>.mode")) {
    efield_mode = o;
  }
  if (auto o=node.get_optional<double>("efield.<xmlattr>.param0")) {
    efield_param0 = o;
  }
  if (auto o=node.get_optional<double>("efield.<xmlattr>.param1")) {
    efield_param1 = o;
  }
  if (auto o=node.get_optional<double>("efield.<xmlattr>.param2")) {
    efield_param2 = o;
  }
  if (auto o=node.get_optional<double>("efield.<xmlattr>.param3")) {
    efield_param3 = o;
  }
  if (auto o=node.get_optional<int>("charge_collection.<xmlattr>.mode")) {
    charge_collection_mode = o;
  }
  if (auto o=node.get_optional<std::string>("charge_collection.<xmlattr>.cce_map")) {
    charge_collection_cce_map = o;
  }
  if (auto o=node.get_optional<double>("charge_collection.mutau.<xmlattr>.electron")) {
    charge_collection_mutau_electron = o;
  }
  if (auto o=node.get_optional<double>("charge_collection.mutau.<xmlattr>.hole")) {
    charge_collection_mutau_hole = o;
  }
  if (auto o=node.get_optional<int>("diffusion.<xmlattr>.mode")) {
    diffusion_mode = o;
  }
  if (auto o=node.get_optional<double>("diffusion.spread_factor.<xmlattr>.cathode")) {
    diffusion_spread_factor_cathode = o;
  }
  if (auto o=node.get_optional<double>("diffusion.spread_factor.<xmlattr>.anode")) {
    diffusion_spread_factor_anode = o;
  }
  if (auto o=node.get_optional<double>("diffusion.constant.<xmlattr>.cathode")) {
    diffusion_constant_cathode = o;
  }
  if (auto o=node.get_optional<double>("diffusion.constant.<xmlattr>.anode")) {
    diffusion_constant_anode = o;
  }
  if (auto o=node.get_optional<double>("timing_resolution.<xmlattr>.trigger")) {
    timing_resolution_trigger = o;
  }
  if (auto o=node.get_optional<double>("timing_resolution.<xmlattr>.energy_measurement")) {
    timing_resolution_energy_measurement = o;
  }
  if (auto o=node.get_optional<int>("pedestal_generation.<xmlattr>.flag")) {
    pedestal_generation_flag = o;
  }
  if (auto o=node.get_optional<int>("reconstruction.<xmlattr>.mode")) {
    reconstruction_mode = o;
  }
  if (auto o=node.get_optional<int>("reconstruction.energy_consistency_check.<xmlattr>.lower_function_c0")) {
    reconstruction_energy_consistency_check_lower_function_c0 = o;
  }
  if (auto o=node.get_optional<int>("reconstruction.energy_consistency_check.<xmlattr>.lower_function_c1")) {
    reconstruction_energy_consistency_check_lower_function_c1 = o;
  }
  if (auto o=node.get_optional<int>("reconstruction.energy_consistency_check.<xmlattr>.upper_function_c0")) {
    reconstruction_energy_consistency_check_upper_function_c0 = o;
  }
  if (auto o=node.get_optional<int>("reconstruction.energy_consistency_check.<xmlattr>.upper_function_c1")) {
    reconstruction_energy_consistency_check_upper_function_c1 = o;
  }

  for (const ptree::value_type& v: node.get_child("")) {
    if (v.first == "channel_properties") {
      const ptree& channelsNode = v.second;
      optional<std::string> side = channelsNode.get_optional<std::string>("<xmlattr>.side");
      if (side && *side == "cathode") {
        channel_properties_cathode.load(channelsNode);
      }
      else if (side && *side == "anode") {
        channel_properties_anode.load(channelsNode);
      }
      else {
        channel_properties.load(channelsNode);
      }
    }
  }
}

} /* namespace comptonsoft */
