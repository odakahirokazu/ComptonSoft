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

#ifndef COMPTONSOFT_AEAttitudeCorrection_H
#define COMPTONSOFT_AEAttitudeCorrection_H 1

#include "VCSModule.hh"
#include <list>
#include "ReadEventTree.hh"
namespace cfitsio
{
extern "C" {
#include "fitsio.h"
}
}

namespace comptonsoft {

class CSHitCollection;


/**
 * @author Tsubasa Tamba
 * @date 2020-03-19
 */
class AEAttitudeCorrection : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(AEAttitudeCorrection, 1.0);
public:
  AEAttitudeCorrection();
  ~AEAttitudeCorrection();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

protected:

private:
  static const int numColumns_ = 3;
  std::string filename_;
  int pixelCenterX_ = 0;
  int pixelCenterY_ = 0;
  double timeBin_ = 1.0;

  std::vector<short> pixelArrayX_;
  std::vector<short> pixelArrayY_;
  std::vector<double> eventTime_;
  int numEvent_ = 0;
  double sumx_ = 0.0;
  double sumy_ = 0.0;
  cfitsio::fitsfile* fitsFile = nullptr;
  int startID_ = 0;
  double currentPixelCenterX_ = 0.0;
  double currentPixelCenterY_ = 0.0;
  double startTime_ = 0.0;
  int colid_[numColumns_] = {0};
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_AEAttitudeCorrection_H */
