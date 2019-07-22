/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Tamotsu Sato, Hirokazu Odaka                       *
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

#ifndef COMPTONSOFT_AEObservationPrimaryGen_H
#define COMPTONSOFT_AEObservationPrimaryGen_H 1

#include <vector>
#include <boost/multi_array.hpp>
#include <memory>
#include "BasicPrimaryGen.hh"

namespace comptonsoft {

/**
 * ANLGeant4 PrimaryGen module.
 * Primary particles are generated according to a list of ray-tracing output.
 *
 * @author Tsubasa Tamba
 * @date 2019-07-04
 */

using image_t = boost::multi_array<double, 2>;

class AEObservationPrimaryGen : public anlgeant4::BasicPrimaryGen
{
  DEFINE_ANL_MODULE(AEObservationPrimaryGen, 1.0);
public:
  AEObservationPrimaryGen();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  void makePrimarySetting() override;

protected:
  G4ThreeVector samplePosition() override;
  void buildPositionIntegral();
  double overlap(double v1, double v2, double w1, double w2);
  
private:
  static const int numColumns_ = 3;
  G4ThreeVector offset_;

  std::string arfFileName_;
  double pixelSize_ = 0.0;
  int pixelX_ = 1;
  int pixelY_ = 1;

  double exposure_ = 0;
  double flux_ = 0.0;
  double fluxEnergyMin_ = 0.0;
  double fluxEnergyMax_ = 0.0;

  std::vector<double> effectiveArea_[3];
  int numEffectiveAreaPlot_ = 0;
  image_t PSF_;
  std::vector<double> PSFArray_;
  double sumFlux_ = 0.0;
  std::vector<double> positionIntegral_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_AHRayTracePrimaryGen_H */
