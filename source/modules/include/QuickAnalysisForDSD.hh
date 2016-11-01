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

#ifndef COMPTONSOFT_QuickAnalysisForDSD_H
#define COMPTONSOFT_QuickAnalysisForDSD_H 1

#include "VCSModule.hh"

class TH1;
class TH2;

namespace comptonsoft
{

class RealDetectorUnit2DStrip;


/**
 * @author Hirokazu Odaka
 * @date 2011-11-21
 * @date 2016-10-26
 */
class QuickAnalysisForDSD : public VCSModule
{
  DEFINE_ANL_MODULE(QuickAnalysisForDSD, 1.2);
public:
  QuickAnalysisForDSD();
  ~QuickAnalysisForDSD();
  
  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();

private:
  comptonsoft::RealDetectorUnit2DStrip* m_DSD;
  int m_DetectorID;
  int m_NBins;
  double m_Energy0;
  double m_Energy1;
  double m_CutEnergy0;
  double m_CutEnergy1;
  
  TH1* m_Spectrum1HitCathode;
  TH1* m_Spectrum1HitAnode;
  TH1* m_Spectrum2HitCathode;
  TH1* m_Spectrum2HitAnode;
  TH2* m_E2DDoubleSide;
  TH2* m_E2D2HitCathode;
  TH2* m_E2D2HitAnode;
  TH2* m_Image;
  TH1* m_Image1DX;
  TH1* m_Image1DY;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_QuickAnalysisForDSD_H */
