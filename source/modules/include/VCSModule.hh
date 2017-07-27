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

#ifndef COMPTONSOFT_VCSModule_H
#define COMPTONSOFT_VCSModule_H 1

#include <anlnext/BasicModule.hh>
#include <memory>
#include "DetectorSystem.hh"
#include "VRealDetectorUnit.hh"

class TDirectory;

namespace comptonsoft {

/**
 * class VCSModule
 * @author Hirokazu Odaka
 * @date 2008-08-30
 * @date 2014-11-22
 * @date 2016-08-19 | Add isMCSimulation()
 * @date 2017-07-07 | merge mod_hit() to mod_initialize()
 */
class VCSModule : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(VCSModule, 1.3);
public:
  VCSModule();
  ~VCSModule();
  
  virtual anlnext::ANLStatus mod_initialize() override;

protected:
  void mkdir(const std::string& name="");
  DetectorSystem* getDetectorManager() { return detectorSystem_; }
  const DetectorSystem* getDetectorManager() const { return detectorSystem_; }
  bool isMCSimulation() const { return detectorSystem_->isMCSimulation(); }

private:
  DetectorSystem* detectorSystem_;
  TDirectory* saveDir_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_VCSModule_H */
