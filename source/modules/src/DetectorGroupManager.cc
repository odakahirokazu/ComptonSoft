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

#include "DetectorGroupManager.hh"

#include <fstream>
#include <iterator>

using namespace anl;

namespace comptonsoft
{

DetectorGroupManager::DetectorGroupManager()
  : filename_("detector_group.txt")
{
}

ANLStatus DetectorGroupManager::mod_startup()
{
  register_parameter(&filename_, "filename",
                     "", "Detector group definition file (0 for no group)");
  return AS_OK;
}

ANLStatus DetectorGroupManager::mod_init()
{
  if (filename_[0] == '0') {
    std::cout << "DetectorGroupManager: There is no detector group." << std::endl;
    return AS_OK;
  }

  std::vector<std::string> defaultGroups = {"Anti", "LowZ", "HighZ"};
  for (auto& groupName: defaultGroups) {
    detectorGroupMap_[groupName]
      = std::unique_ptr<DetectorGroup>(new DetectorGroup(groupName));
  }
  
  ANLStatus rval = loadDetectorGroups();
  return rval;
}

ANLStatus DetectorGroupManager::loadDetectorGroups()
{
  std::ifstream fin;
  fin.open(filename_.c_str());
  if (!fin) {
    std::cout << "DetectorGroupManager: file cannot open" << std::endl;
    return AS_QUIT;
  }

  DetectorGroup* detectorGroup = nullptr;
  while (1) {
    char buf[256];
    fin.getline(buf, 256);
    if(fin.eof()) { break; }
    if (buf[0] == '#') { continue; }

    std::istringstream iss(buf);
    std::string str;
    iss >> str;
    const std::string label(str);
    if (label == "GROUP") {
      iss >> str;
      const std::string groupName(str);
      auto it = detectorGroupMap_.find(groupName);
      if (it == std::end(detectorGroupMap_)) {
        detectorGroupMap_[groupName] = std::unique_ptr<DetectorGroup>(new DetectorGroup(groupName));
      }
      detectorGroup = detectorGroupMap_[groupName].get();
    }
    else if (label == "ID") {
      int detectorID;
      while (iss >> detectorID) {
        if (detectorGroup) {
          detectorGroup->add(detectorID);
        }
      }
    }
    else if (label == "HITPATTERN") {
      HitPattern hitpat;
      iss >> str;
      hitpat.setName(str);
      iss >> str;
      hitpat.setShortName(str);
      while (iss >> str) {
        hitpat.add(getDetectorGroup(str));
      }
      hitPatterns_.push_back(std::move(hitpat));
    }
  }

  std::cout << std::endl;
  std::cout << "DetectorGroupManager:\n"
            << "List of Detector groups:" << std::endl;
  for (auto& pair: detectorGroupMap_) {
    DetectorGroup& dg = *(pair.second);
    dg.print();
  }
  std::cout << "\n"
            << "List of Hit patterns:" << std::endl;
  for (auto& hitpat: hitPatterns_) {
    std::cout << hitpat.Name() << std::endl;
  }

  return AS_OK;
}

} /* namespace comptonsoft */
