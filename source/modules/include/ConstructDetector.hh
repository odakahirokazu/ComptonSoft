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

#ifndef COMPTONSOFT_ConstructDetector_H
#define COMPTONSOFT_ConstructDetector_H 1

#include "BasicModule.hh"

namespace comptonsoft {

class DetectorSystem;

/**
 * construct detector system
 * @author Hirokazu Odaka
 * @date 2008-08-27
 * @date 2015-10-11
 * @date 2016-08-31
 */
class ConstructDetector : public anl::BasicModule
{
  DEFINE_ANL_MODULE(ConstructDetector, 3.3);
public:
  ConstructDetector();
  ~ConstructDetector();

  anl::ANLStatus mod_define() override;
  anl::ANLStatus mod_initialize() override;
  anl::ANLStatus mod_analyze() override;

  DetectorSystem* getDetectorManager() { return detectorManager_.get(); }

protected:
  int VerboseLevel() const { return verboseLevel_; }

private:
  std::unique_ptr<DetectorSystem> detectorManager_;
  std::string configurationFile_;
  std::string parametersFile_;
  int verboseLevel_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ConstructDetector_H */
