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

#ifndef COMPTONSOFT_DetectorSystem_H
#define COMPTONSOFT_DetectorSystem_H 1

#include <vector>
#include <map>
#include <memory>
#include <boost/utility.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/optional.hpp>
#include "CSException.hh"
#include "ChannelID.hh"
#include "VRealDetectorUnit.hh"
#include "ReadoutModule.hh"
#include "DetectorGroup.hh"
#include "HitPattern.hh"

class TFile;

namespace comptonsoft {

enum class ElectrodeSide;
class MultiChannelData;
class ChannelMap;
class DeviceSimulation;
class VCSSensitiveDetector;

/**
 * A manager class of a whole detector system.
 * @author Hirokazu Odaka
 * @date 2008-08-29
 * @date 2012-06-29
 * @date 2016-08-22 | new XML scheme (version 4)
 * @date 2018-03-09 | new XML schema (detector config v5, detector parameters v2)
 * @date 2020-03-30 | new XML schema (channel properties v2, detector parameters v3)
 */
class DetectorSystem : private boost::noncopyable
{
public:

  struct ChannelNodeContents
  {
    boost::optional<int> id = boost::none;
    boost::optional<int> x = boost::none;
    boost::optional<int> y = boost::none;
    boost::optional<int> disable_status = boost::none;
    boost::optional<double> trigger_discrimination_center = boost::none;
    boost::optional<double> trigger_discrimination_sigma = boost::none;
    boost::optional<double> noise_level_param0 = boost::none;
    boost::optional<double> noise_level_param1 = boost::none;
    boost::optional<double> noise_level_param2 = boost::none;
    boost::optional<double> compensation_factor = boost::none;
    boost::optional<std::string> compensation_function = boost::none;
    boost::optional<double> pedestal_value = boost::none;
    boost::optional<double> gain_correction_c0 = boost::none;
    boost::optional<double> gain_correction_c1 = boost::none;
    boost::optional<double> gain_correction_c2 = boost::none;
    boost::optional<double> gain_correction_c3 = boost::none;
    boost::optional<double> threshold_value = boost::none;

    void load(const boost::property_tree::ptree& node);
  };

  struct ParametersNodeContents
  {
    boost::optional<int> upside_anode = boost::none;
    boost::optional<int> upside_pixel = boost::none;
    boost::optional<int> upside_xstrip = boost::none;
    boost::optional<int> depth_sensing_mode = boost::none;
    boost::optional<double> depth_sensing_resolution = boost::none;
    boost::optional<double> quenching_factor = boost::none;
    boost::optional<double> temperature_value = boost::none;
    boost::optional<double> efield_bias = boost::none;
    boost::optional<int> efield_mode = boost::none;
    boost::optional<double> efield_param0 = boost::none;
    boost::optional<double> efield_param1 = boost::none;
    boost::optional<double> efield_param2 = boost::none;
    boost::optional<double> efield_param3 = boost::none;
    boost::optional<int> charge_collection_mode = boost::none;
    boost::optional<std::string> charge_collection_cce_map = boost::none;
    boost::optional<double> charge_collection_mutau_electron = boost::none;
    boost::optional<double> charge_collection_mutau_hole = boost::none;
    boost::optional<int> diffusion_mode = boost::none;
    boost::optional<double> diffusion_spread_factor_cathode = boost::none;
    boost::optional<double> diffusion_spread_factor_anode = boost::none;
    boost::optional<double> diffusion_constant_cathode = boost::none;
    boost::optional<double> diffusion_constant_anode = boost::none;
    boost::optional<double> timing_resolution_trigger = boost::none;
    boost::optional<double> timing_resolution_energy_measurement = boost::none;
    boost::optional<int> pedestal_generation_flag = boost::none;
    ChannelNodeContents channel_properties;
    ChannelNodeContents channel_properties_cathode;
    ChannelNodeContents channel_properties_anode;
    boost::optional<int> reconstruction_mode = boost::none;
    boost::optional<double> reconstruction_energy_consistency_check_lower_function_c0 = boost::none;
    boost::optional<double> reconstruction_energy_consistency_check_lower_function_c1 = boost::none;
    boost::optional<double> reconstruction_energy_consistency_check_upper_function_c0 = boost::none;
    boost::optional<double> reconstruction_energy_consistency_check_upper_function_c1 = boost::none;

    void load(const boost::property_tree::ptree& node);
  };
  
public:
  DetectorSystem();
  virtual ~DetectorSystem();

  void setMCSimulation(bool v=true) { MCSimulation_ = v; }
  bool isMCSimulation() { return MCSimulation_; }

  int NumberOfDetectors() const { return detectors_.size(); }
  VRealDetectorUnit* getDetectorByID(int id);
  const VRealDetectorUnit* getDetectorByID(int id) const;
  VRealDetectorUnit* getDetectorByIndex(int index)
  { return detectors_[index].get(); }
  const VRealDetectorUnit* getDetectorByIndex(int index) const
  { return detectors_[index].get(); }
  bool existDetector(int id) const
  { return detectorIDMap_.count(id); }

  std::vector<std::unique_ptr<VRealDetectorUnit>>& getDetectors()
  { return detectors_; }

  template <typename Func>
  void doForEachDetector(const Func& func)
  {
    for (std::unique_ptr<VRealDetectorUnit>& detector: detectors_) {
      func(*detector);
    }
  }

  int NumberOfReadoutModules() const { return readoutModules_.size(); }
  ReadoutModule* getReadoutModuleByID(int id);
  const ReadoutModule* getReadoutModuleByID(int id) const;
  ReadoutModule* getReadoutModuleByIndex(int index)
  { return readoutModules_[index].get(); }
  const ReadoutModule* getReadoutModuleByIndex(int index) const
  { return readoutModules_[index].get(); }

  std::vector<std::unique_ptr<ReadoutModule>>& getReadoutModules()
  { return readoutModules_; }

  template <typename Func>
  void doForEachReadoutModule(const Func& func)
  {
    for (std::unique_ptr<ReadoutModule>& readoutModule: readoutModules_) {
      func(*readoutModule);
    }
  }

  MultiChannelData* getMultiChannelData(const DetectorBasedChannelID& channelID);
  MultiChannelData* getMultiChannelData(const ReadoutBasedChannelID& channelID);

  template <typename Func>
  void doForEachMultiChannelDataInDetectorOrder(const Func& func)
  {
    for (std::unique_ptr<VRealDetectorUnit>& detector: detectors_) {
      const int NumMCD = detector->NumberOfMultiChannelData();
      for (int i=0; i<NumMCD; i++) {
        MultiChannelData* mcd = detector->getMultiChannelData(i);
        DetectorBasedChannelID channel(detector->getID(), i);
        func(mcd, channel);
      }
    }
  }

  template <typename Func>
  void doForEachMultiChannelDataInReadoutOrder(const Func& func)
  {
    for (std::unique_ptr<ReadoutModule>& readoutModule: readoutModules_) {
      const int NumMCD = readoutModule->NumberOfSections();
      for (int i=0; i<NumMCD; i++) {
        const int readoutModuleID = readoutModule->ID();
        const ReadoutBasedChannelID readoutChannel(readoutModuleID, i);
        MultiChannelData* mcd = this->getMultiChannelData(readoutChannel);
        func(mcd, readoutChannel);
      }
    }
  }

  DetectorBasedChannelID convertToDetectorBasedChannelID(const ReadoutBasedChannelID& channelID);
  ReadoutBasedChannelID convertToReadoutBasedChannelID(const DetectorBasedChannelID& channelID);

  // Detector groups and hit patterns
  const DetectorGroup& getDetectorGroup(const std::string& name) const
  { return *(detectorGroupMap_.at(name)); }

  const std::vector<HitPattern>& getHitPatterns() const
  { return hitPatterns_; }

  void printDetectorGroups() const;

  // Device simulations
  DeviceSimulation* getDeviceSimulationByID(int id);
  const DeviceSimulation* getDeviceSimulationByID(int id) const;
  DeviceSimulation* getDeviceSimulationByIndex(int index)
  { return deviceSimulationVector_[index]; }
  const DeviceSimulation* getDeviceSimulationByIndex(int index) const
  { return deviceSimulationVector_[index]; }
  std::vector<DeviceSimulation*>& getDeviceSimulationVector()
  { return deviceSimulationVector_; }

  // Setup detector system manually
  void addDetector(std::unique_ptr<VRealDetectorUnit>&& detector);
  void addReadoutModule(std::unique_ptr<ReadoutModule>&& detector);
  void addDetectorGroup(std::unique_ptr<DetectorGroup>&& group);
  void addHitPattern(const HitPattern& hitpat);
  void setConstructed();
  void addSenstiveDetector(VCSSensitiveDetector* sd);

  // Setup detector system by database files
  void readDetectorConfiguration(const std::string& filename);
  bool isConstructed() const { return detectorConstructed_; }
  void readDetectorParameters(const std::string& filename);
  void registerGeant4SensitiveDetectors();

  // for an event loop
  void initializeEvent();

private:
  // detector configuration
  void loadDetectorConfigurationRootNode(const boost::property_tree::ptree& RootNode);
  void loadDCDetectorsNode(const boost::property_tree::ptree& DetectorsNode,
                           double LengthUnit);
  void loadDCDetectorNode(const boost::property_tree::ptree& DetectorsNode,
                          double LengthUnit,
                          VRealDetectorUnit* detector);
  void loadDCDetectorSectionsNode(const boost::property_tree::ptree& SectionsNode,
                                  ElectrodeSide priority_side,
                                  VRealDetectorUnit* detector);
  void loadDCReadoutNode(const boost::property_tree::ptree& ReadoutNode);
  void loadDCGroupsNode(const boost::property_tree::ptree& GroupsNode);

  // detector parameters
  void loadDetectorParametersRootNode(const boost::property_tree::ptree& RootNode,
                                      const std::string& filename);
  void loadDetectorParametersDataNode(const boost::property_tree::ptree& DataNode);
  void loadDPDetectorSetNode(const boost::property_tree::ptree& SetNode,
                             bool isSensitiveDetector);
  void setupDetectorParameters(const DetectorSystem::ParametersNodeContents parameters,
                               VRealDetectorUnit* detector);
  void setupDetectorParameters(const DetectorSystem::ParametersNodeContents parameters,
                               DeviceSimulation* ds);
  void setupReconstructionParameters(const DetectorSystem::ParametersNodeContents parameters,
                                     VRealDetectorUnit* detector);
  
private:
  bool MCSimulation_;
  bool simAutoPosition_;
  bool simSDCheck_;

  bool detectorConstructed_;
  std::vector<std::unique_ptr<VRealDetectorUnit>> detectors_;
  std::vector<std::unique_ptr<ReadoutModule>> readoutModules_;
  std::map<int, int> detectorIDMap_;
  std::map<int, int> readoutModuleIDMap_;

  std::vector<DeviceSimulation*> deviceSimulationVector_;
  std::vector<VCSSensitiveDetector*> sensitiveDetectorVector_;
  std::unique_ptr<TFile> ROOTFile_;

  std::map<std::string, std::unique_ptr<DetectorGroup>> detectorGroupMap_;
  std::vector<HitPattern> hitPatterns_;
  
private:
  DetectorSystem(const DetectorSystem&) = delete;
  DetectorSystem(DetectorSystem&&) = delete;
  DetectorSystem& operator=(const DetectorSystem&) = delete;
  DetectorSystem& operator=(DetectorSystem&&) = delete;
};

inline VRealDetectorUnit* DetectorSystem::getDetectorByID(int id)
{
  auto it = detectorIDMap_.find(id);
  if (it==detectorIDMap_.end()) { return nullptr; }
  return getDetectorByIndex((*it).second);
}

inline const VRealDetectorUnit* DetectorSystem::getDetectorByID(int id) const
{
  auto it = detectorIDMap_.find(id);
  if (it==detectorIDMap_.end()) { return nullptr; }
  return getDetectorByIndex((*it).second);
}

inline ReadoutModule* DetectorSystem::getReadoutModuleByID(int id)
{
  auto it = readoutModuleIDMap_.find(id);
  if (it==readoutModuleIDMap_.end()) { return nullptr; }
  return getReadoutModuleByIndex((*it).second);
}

inline const ReadoutModule* DetectorSystem::getReadoutModuleByID(int id) const
{
  auto it = readoutModuleIDMap_.find(id);
  if (it==readoutModuleIDMap_.end()) { return nullptr; }
  return getReadoutModuleByIndex((*it).second);
}

inline DeviceSimulation* DetectorSystem::getDeviceSimulationByID(int id)
{
  auto it = detectorIDMap_.find(id);
  if (it==detectorIDMap_.end()) { return nullptr; }
  return getDeviceSimulationByIndex((*it).second);
}

inline const DeviceSimulation* DetectorSystem::getDeviceSimulationByID(int id) const
{
  auto it = detectorIDMap_.find(id);
  if (it==detectorIDMap_.end()) { return nullptr; }
  return getDeviceSimulationByIndex((*it).second);
}

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_DetectorSystem_H */
