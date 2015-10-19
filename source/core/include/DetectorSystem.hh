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
#include "ChannelID.hh"
#include "VRealDetectorUnit.hh"
#include "DetectorReadoutModule.hh"
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
 */
class DetectorSystem : private boost::noncopyable
{
public:
  DetectorSystem();
  ~DetectorSystem();

  void setMCSimulation(bool v=true) { MCSimulation_ = v; }
  bool isMCSimulation() { return MCSimulation_; }

  int NumberOfDetectors() const { return detectors_.size(); }
  VRealDetectorUnit* getDetectorByID(int id);
  VRealDetectorUnit* getDetectorByIndex(int index)
  { return detectors_[index].get(); }

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
  DetectorReadoutModule* getReadoutModuleByID(int id);
  DetectorReadoutModule* getReadoutModuleByIndex(int index)
  { return readoutModules_[index].get(); }

  std::vector<std::unique_ptr<DetectorReadoutModule>>& getReadoutModules()
  { return readoutModules_; }

  template <typename Func>
  void doForEachReadoutModule(const Func& func)
  {
    for (std::unique_ptr<DetectorReadoutModule>& readoutModule: readoutModules_) {
      func(*readoutModule);
    }
  }

  MultiChannelData* getMultiChannelData(const DetectorChannelID& channel);
  MultiChannelData* getMultiChannelData(const ReadoutChannelID& channel);

  template <typename Func>
  void doForEachMultiChannelDataInDetectorOrder(const Func& func)
  {
    for (std::unique_ptr<VRealDetectorUnit>& detector: detectors_) {
      const int NumMCD = detector->NumberOfMultiChannelData();
      for (int i=0; i<NumMCD; i++) {
        MultiChannelData* mcd = detector->getMultiChannelData(i);
        DetectorChannelID channel(detector->getID(), i);
        func(mcd, channel);
      }
    }
  }

  template <typename Func>
  void doForEachMultiChannelDataInReadoutOrder(const Func& func)
  {
    for (std::unique_ptr<DetectorReadoutModule>& readoutModule: readoutModules_) {
      const int NumMCD = readoutModule->NumberOfReadoutSections();
      for (int i=0; i<NumMCD; i++) {
        const int readoutModuleID = readoutModule->ID();
        const ReadoutChannelID readoutChannel(readoutModuleID, i);
        MultiChannelData* mcd = this->getMultiChannelData(readoutChannel);
        func(mcd, readoutChannel);
      }
    }
  }
  
  DeviceSimulation* getDeviceSimulationByID(int id);
  DeviceSimulation* getDeviceSimulationByIndex(int index)
  { return deviceSimulationVector_[index]; }
  std::vector<DeviceSimulation*>& getDeviceSimulationVector()
  { return deviceSimulationVector_; }

  bool isConstructed() { return detectorConstructed_; }
  bool loadDetectorConfiguration(const std::string& filename);
  bool loadSimulationParameters(const std::string& filename);
  void registerGeant4SensitiveDetectors();

  void initializeEvent();

  // Detector groups and hit patterns
  const DetectorGroup& getDetectorGroup(const std::string& name) const
  { return *(detectorGroupMap_.at(name)); }

  const std::vector<HitPattern>& getHitPatterns() const
  { return hitPatterns_; }

  void printDetectorGroups() const;

private:
  bool constructDetectors(const boost::property_tree::ptree& DetectorsNode, double LengthUnit);
  bool constructMultiChannelData(const boost::property_tree::ptree& readoutNode,
                                 VRealDetectorUnit* detector,
                                 ElectrodeSide priority_side);
  bool constructReadoutModules(const boost::property_tree::ptree& ReadoutNode);
  bool registerDetectorGroups(const boost::property_tree::ptree& GroupsNode);

  // for MC simulation
  bool setupSimulation(const boost::property_tree::ptree& DetectorsNode);
  bool setupSD(const boost::property_tree::ptree& SDNode);
  bool loadDeviceSimulationParam(const std::string& type,
                                 const boost::property_tree::ptree& ParamNode,
                                 DeviceSimulation& ds);
  
private:
  bool MCSimulation_;
  bool simAutoPosition_;
  bool simSDCheck_;

  bool detectorConstructed_;
  std::vector<std::unique_ptr<VRealDetectorUnit>> detectors_;
  std::vector<std::unique_ptr<DetectorReadoutModule>> readoutModules_;
  std::map<int, int> detectorIDMap_;
  std::map<int, int> readoutModuleIDMap_;

  std::vector<DeviceSimulation*> deviceSimulationVector_;
  std::vector<VCSSensitiveDetector*> sensitiveDetectorVector_;
  TFile* CCEMapFile_;

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

inline DetectorReadoutModule* DetectorSystem::getReadoutModuleByID(int id)
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

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_DetectorSystem_H */
