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

#ifndef COMPTONSOFT_CelestialSourcePrimaryGenerator_H
#define COMPTONSOFT_CelestialSourcePrimaryGenerator_H 1

#include <boost/multi_array.hpp>
#include "BasicPrimaryGenerator.hh"
#include "IsotropicPrimaryGenerator.hh"
#include "G4ThreeVector.hh"

namespace comptonsoft {


/**
 * ComptonSoft PrimaryGenerator module.
 * Imaging polarimetry realized.
 *
 * @author Tsubasa Tamba
 * @date 2020-04-01
 *
 */

using image_t = boost::multi_array<double, 2>;

class CelestialSourcePrimaryGenerator : public anlgeant4::IsotropicPrimaryGenerator
{
  DEFINE_ANL_MODULE(CelestialSourcePrimaryGenerator, 1.0);
public:
  CelestialSourcePrimaryGenerator();
  ~CelestialSourcePrimaryGenerator();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_end_run() override;

  anlgeant4::PrimarySetting make_primary_setting() const override;

protected:
  void inputImage(std::string filename, image_t& image, anlnext::ANLStatus* status);
  void makePolarizationMap(anlnext::ANLStatus* status);
  void setCoordinate(anlnext::ANLStatus* status);
  void buildPixelIntegral();
  std::pair<int, int> samplePixel() const;

private:
  std::string fitsFilenameI_;
  std::string fitsFilenameQ_;
  std::string fitsFilenameU_;
  image_t imageI_;
  image_t imageQ_;
  image_t imageU_;
  int pixelX_ = 1;
  int pixelY_ = 1;
  image_t polarizationDegree_;
  image_t polarizationAngle_;
  image_t imageRA_;
  image_t imageDec_;
  std::vector<double> pixelIntegral_;
  double inputImageRotationAngle_ = 0.0;
  double detectorRollAngle_ = 0.0;
  bool setPolarization_ = false;
  double degPixelX_ = 0.0;
  double degPixelY_ = 0.0;
  double sourceFlux_ = 0.0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_CelestialSourcePrimaryGenerator_H */
