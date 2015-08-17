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

#ifndef COMPTONSOFT_DetectorGroupManager_H
#define COMPTONSOFT_DetectorGroupManager_H 1

#include "VCSModule.hh"

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "DetectorGroup.hh"
#include "HitPattern.hh"

namespace comptonsoft {

class CSHitCollection;

/**
 * event selection
 * @author Hirokazu Odaka
 * @date 2014-11-27
 */
class DetectorGroupManager : public VCSModule
{
  DEFINE_ANL_MODULE(DetectorGroupManager, 1.0)
public:
  DetectorGroupManager();
  ~DetectorGroupManager() = default;

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  
public:
  const DetectorGroup& getDetectorGroup(const std::string& name) const
  { return *(detectorGroupMap_.at(name)); }

  const std::vector<HitPattern>& getHitPatterns() const
  { return hitPatterns_; }

private:
  anl::ANLStatus loadDetectorGroups();

private:
  std::string filename_;
  std::map<std::string, std::unique_ptr<DetectorGroup>> detectorGroupMap_;
  std::vector<HitPattern> hitPatterns_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_DetectorGroupManager_H */
