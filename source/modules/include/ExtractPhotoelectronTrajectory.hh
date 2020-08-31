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

#ifndef COMPTONSOFT_ExtractPhotoelectronTrajectory_H
#define COMPTONSOFT_ExtractPhotoelectronTrajectory_H 1

#include "VCSModule.hh"
#include <memory>
#include <boost/multi_array.hpp>

class TTree;
class TH2;

namespace anlgeant4 {
class InitialInformation;
}

namespace comptonsoft {

class HitTreeIOWithInitialInfo;
class CSHitCollection;

/**
 * 
 * @author Hirokazu Odaka
 * @date 2020-08-19
 */
class ExtractPhotoelectronTrajectory : public VCSModule
{
  DEFINE_ANL_MODULE(ExtractPhotoelectronTrajectory, 1.0);
public:
  ExtractPhotoelectronTrajectory();
  ~ExtractPhotoelectronTrajectory() = default;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  void resetImage();
  void copyImage(const TH2* image);
  int calculateImageWeight() const;

private:
  const CSHitCollection* hitCollection_ = nullptr;
  const anlgeant4::InitialInformation* initialInfo_ = nullptr;
  TTree* tree_ = nullptr;
  using Array2F = boost::multi_array<float, 2>;
  std::unique_ptr<Array2F> image_;
  float angle_ = 0.0;
  int weight_ = 0;

  int pixelNumber_ = 0;
  double pixelSize_ = 0.0;
  int weightMin_ = 0;
  double pixelThreshold_ = 0.0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ExtractPhotoelectronTrajectory_H */
