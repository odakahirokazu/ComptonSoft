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

#ifndef COMPTONSOFT_DetectorManager_H
#define COMPTONSOFT_DetectorManager_H 1

#include <vector>
#include <map>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

#if USE_ROOT
class TFile;
#endif

class VHXISGDSensitiveDetector;

namespace comptonsoft {

class OneASICData;
class RealDetectorUnit;
class DetectorReadModule;
class ChannelTable;
class VDeviceSimulation;

#if USE_ROOT
class DeviceSimulation;
#endif

/**
 * A manager class of a whole detector system.
 * @author Hirokazu Odaka
 * @date 2008-08-29
 * @date 2012-06-29
 */
class DetectorManager : private boost::noncopyable
{
public:
  DetectorManager();
  ~DetectorManager();

  void setMCSimulation(bool b) { m_MCSimulation = b; }
  bool MCSimulation() { return m_MCSimulation; }

  bool IsConstructed() { return m_DetectorConstructed; }

  bool loadDetectorConfiguration(const char* filename);

  int NumDetector() { return m_DetectorVector.size(); }
  RealDetectorUnit* getDetectorByID(int id);
  RealDetectorUnit* getDetectorByIndex(int index)
  { return m_DetectorVector[index]; }
  std::vector<RealDetectorUnit*>& getDetectorVector()
  { return m_DetectorVector; }
  std::vector<RealDetectorUnit*>::iterator DetectorBegin()
  { return m_DetectorVector.begin(); }
  std::vector<RealDetectorUnit*>::iterator DetectorEnd()
  { return m_DetectorVector.end(); }
  
  int NumReadModule() { return m_ReadModuleVector.size(); }
  DetectorReadModule* getReadModuleByID(int id);
  DetectorReadModule* getReadModuleByIndex(int index)
  { return m_ReadModuleVector[index]; }
  std::vector<DetectorReadModule*>& getReadModuleVector()
  { return m_ReadModuleVector; }

  std::vector<OneASICData*>& getASICDataVector()
  { return m_ASICDataVector; }

  // for MC simulation
  bool loadSimulationParameter(const char* filename);
  
  VDeviceSimulation* getDeviceSimulationByID(int id);
  VDeviceSimulation* getDeviceSimulationByIndex(int index)
  { return m_DeviceSimulationVector[index]; }
  std::vector<VDeviceSimulation*>& getDeviceSimulationVector()
  { return m_DeviceSimulationVector; }
  std::vector<VDeviceSimulation*>::iterator DeviceSimulationBegin()
  { return m_DeviceSimulationVector.begin(); }
  std::vector<VDeviceSimulation*>::iterator DeviceSimulationEnd()
  { return m_DeviceSimulationVector.end(); }

  int NumSensitiveDetector()
  { return m_SensitiveDetectorVector.size(); }
  VHXISGDSensitiveDetector* getSensitiveDetector(int i)
  { return m_SensitiveDetectorVector[i]; }
  
private:
  bool m_MCSimulation;

  bool m_DetectorConstructed;
  bool m_ReadoutModuleConstructed;

  std::vector<RealDetectorUnit*> m_DetectorVector;
  std::vector<DetectorReadModule*> m_ReadModuleVector;
  std::vector<OneASICData*> m_ASICDataVector;

  std::map<int, int> m_DetectorIDMap;
  std::map<int, int> m_ReadModuleIDMap;

  // for MC simulation
  std::vector<VDeviceSimulation*> m_DeviceSimulationVector;
  std::vector<VHXISGDSensitiveDetector*> m_SensitiveDetectorVector;

#if USE_ROOT
  TFile* m_CCEMapFile;
#endif

  bool m_AutoPosition;
  bool m_SDCheck;
                                        
private:
  bool constructDetectorUnits(const boost::property_tree::ptree& DetectorsNode);
  bool constructASICs(const boost::property_tree::ptree& ASICsNode,
                      RealDetectorUnit* detector,
                      bool priority_to_anode);
  bool constructReadoutModules(const boost::property_tree::ptree& ReadoutNode);

  void clearDetectorVector();
  void clearReadModuleVector();
  void clearASICDataVector();

  // for MC simulation
  bool setupSimulation(const boost::property_tree::ptree& DetectorsNode);
  bool setupSD(const boost::property_tree::ptree& SDNode);
#if USE_ROOT
  bool loadDeviceSimulationParam(std::string type,
                                 const boost::property_tree::ptree& ParamNode,
                                 DeviceSimulation& ds);
#endif

  void clearSensitiveDetectorVector();

private:
  DetectorManager(const DetectorManager&);
  DetectorManager& operator=(const DetectorManager&);
};


inline RealDetectorUnit* DetectorManager::getDetectorByID(int id)
{
  if (m_DetectorIDMap.count(id) != 0) {
    return m_DetectorVector[m_DetectorIDMap[id]];
  }
  else {
    return 0;
  }
}


inline DetectorReadModule* DetectorManager::getReadModuleByID(int id)
{
  if (m_ReadModuleIDMap.count(id) != 0) {
    return m_ReadModuleVector[m_ReadModuleIDMap[id]];
  }
  else {
    return 0;
  }
}


inline VDeviceSimulation* DetectorManager::getDeviceSimulationByID(int id)
{
  if (m_DetectorIDMap.count(id) != 0) {
    return m_DeviceSimulationVector[m_DetectorIDMap[id]];
  }
  else {
    return 0;
  }
}

}

#endif /* COMPTONSOFT_DetectorManager_H */
