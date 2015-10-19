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
 */
class ConstructDetector : public anl::BasicModule
{
  DEFINE_ANL_MODULE(ConstructDetector, 3.1);
public:
  ConstructDetector();
  ~ConstructDetector();

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();

  DetectorSystem* getDetectorManager() { return detectorManager_.get(); }

protected:
  int VerboseLevel() const { return verboseLevel_; }

private:
  std::unique_ptr<DetectorSystem> detectorManager_;
  std::string configurationFile_;
  int verboseLevel_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ConstructDetector_H */
