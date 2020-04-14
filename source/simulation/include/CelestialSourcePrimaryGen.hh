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

#ifndef COMPTONSOFT_CelestialSourcePrimaryGen_H
#define COMPTONSOFT_CelestialSourcePrimaryGen_H 1

#include <boost/multi_array.hpp>
#include "BasicPrimaryGen.hh"
#include "IsotropicPrimaryGen.hh"
#include "G4ThreeVector.hh"

namespace comptonsoft {


/**
 * ComptonSoft PrimaryGen module.
 * Imaging polarimetry realized.
 *
 * @author Tsubasa Tamba
 * @date 2020-04-01
 *
 */

using image_t = boost::multi_array<double, 2>;

class CelestialSourcePrimaryGen : public anlgeant4::IsotropicPrimaryGen
{
  DEFINE_ANL_MODULE(CelestialSourcePrimaryGen, 1.0);
public:
  CelestialSourcePrimaryGen();
  ~CelestialSourcePrimaryGen();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_end_run() override;

  void makePrimarySetting() override;

protected:
  void inputImage(std::string filename, image_t& image, anlnext::ANLStatus* status);
  void makePolarizationMap(anlnext::ANLStatus* status);
  void setCoordinate(anlnext::ANLStatus* status);
  void buildPixelIntegral();
  std::pair<int, int> samplePixel();
  
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

#endif /* COMPTONSOFT_CelestialSourcePrimaryGen_H */
