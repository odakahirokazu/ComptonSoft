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

#ifndef COMPTONSOFT_AnalyzeDSD_H
#define COMPTONSOFT_AnalyzeDSD_H 1

#include "VCSModule.hh"

class TH1;
class TH2;

namespace comptonsoft
{

class RealDetectorUnit2DStrip;


/**
 * @author Hirokazu Odaka
 * @date 2011-11-21
 */

class AnalyzeDSD : public VCSModule
{
  DEFINE_ANL_MODULE(AnalyzeDSD, 1.0);
public:
  AnalyzeDSD();
  ~AnalyzeDSD() {}
  
  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();

private:
  comptonsoft::RealDetectorUnit2DStrip* m_DSD;
  int m_DetID;
  double m_Energy0;
  double m_Energy1;
  
  TH1* m_Spec1HitCathode;
  TH1* m_Spec1HitAnode;
  TH1* m_Spec2HitCathode;
  TH1* m_Spec2HitAnode;
  TH2* m_E2DCathode;
  TH2* m_E2DAnode;
  TH2* m_Image;
  TH1* m_Image1DX;
  TH1* m_Image1DY;
};

}

#endif /* COMPTONSOFT_AnalyzeDSD_H */
