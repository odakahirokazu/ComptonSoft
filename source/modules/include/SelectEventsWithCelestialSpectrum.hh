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

#ifndef COMPTONSOFT_SelectEventsWithCelestialSpectrum_H
#define COMPTONSOFT_SelectEventsWithCelestialSpectrum_H 1

#include "VCSModule.hh"
#include <list>
#include "ReadEventTree.hh"
#include "XrayEvent.hh"

namespace comptonsoft {

class CSHitCollection;


/**
 * @author Tsubasa Tamba
 * @date 2020-06-24
 */
class SelectEventsWithCelestialSpectrum : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(SelectEventsWithCelestialSpectrum, 1.0);

public:
  SelectEventsWithCelestialSpectrum();
  ~SelectEventsWithCelestialSpectrum();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

  struct Effarea {
    double emin, emax, area;
    Effarea(double emin=0.0, double emax=0.0, double area=0.0): emin(emin), emax(emax), area(area) {}
  };

  void inputImage(std::string filename, image_t& image, anlnext::ANLStatus* status);
  void inputEffectiveArea(std::string filename, std::vector<Effarea>& effarea, anlnext::ANLStatus* status);
  void buildPositionIntegral();
  void setPosition();
  std::pair<int, int> samplePixel();

private:
  double exposure_ = 0.0;
  std::vector<double> energyArray_;
  std::vector<double> photonsArray_;
  std::string arfFilename_;
  bool assignPosition_ = true;

  image_t image_;
  std::vector<Effarea> effectiveArea_;
  std::vector<double> positionIntegral_;
  std::vector<int> countArray_;
  int numRemainedBin_;
  std::vector<int> photonStack_;

  CSHitCollection* hitCollection_ = nullptr;
  ReadEventTree* readEventTree_ = nullptr;

};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SelectEventsWithCelestialSpectrum_H */
