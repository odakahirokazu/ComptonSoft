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

#include <anlnext/BasicModule.hh>

namespace comptonsoft {

class DetectorSystem;

/**
 * construct detector system
 * @author Hirokazu Odaka
 * @date 2008-08-27
 * @date 2015-10-11
 * @date 2016-08-31
 * @date 2020-04-01
 */
class ConstructDetector : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(ConstructDetector, 3.4);
public:
  ConstructDetector();
  ~ConstructDetector();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  const DetectorSystem* getDetectorManager() const { return detector_manager_.get(); }
  DetectorSystem* getDetectorManager() { return detector_manager_.get(); }

protected:
  int VerboseLevel() const { return verbose_level_; }

private:
  std::unique_ptr<DetectorSystem> detector_manager_;
  std::string configuration_file_;
  std::string parameters_file_;
  int verbose_level_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ConstructDetector_H */
