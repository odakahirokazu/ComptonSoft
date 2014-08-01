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

#include "BackProjection.hh"

#include "TDirectory.h"
#include "TRandom3.h"
#include "TMath.h"

using namespace anl;
using namespace comptonsoft;


BackProjection::BackProjection()
  : m_EventReconstruction(0),
    m_PlaneNormal(0.0, 0.0, 1.0),
    m_PlanePoint(0.0*cm, 0.0*cm, 10.0*cm),
    m_NumPixelX(256), m_NumPixelY(256),
    m_RangeX1(-4.0*cm), m_RangeX2(+4.0*cm),
    m_RangeY1(-4.0*cm), m_RangeY2(+4.0*cm),
    m_PixelUnit(cm), m_PixelUnitName("cm")
{
}


ANLStatus BackProjection::mod_startup()
{
  register_parameter(&m_PlaneNormal, "plane_normal");
  register_parameter(&m_PlanePoint, "plane_point", 1, m_PixelUnitName);
  register_parameter(&m_NumPixelX, "num_pixel_x");
  register_parameter(&m_NumPixelY, "num_pixel_y");
  register_parameter(&m_RangeX1, "x_min", 1, m_PixelUnitName);
  register_parameter(&m_RangeX2, "x_max", 1, m_PixelUnitName);
  register_parameter(&m_RangeY1, "y_min", 1, m_PixelUnitName);
  register_parameter(&m_RangeY2, "y_max", 1, m_PixelUnitName);

  return AS_OK;
}


ANLStatus BackProjection::mod_his()
{
  VCSModule::mod_his();
  mkdir_module();
  
  m_hist_bp_All  = new TH2D("h_bp_All","Back Projection (All)", m_NumPixelX, m_RangeX1, m_RangeX2, m_NumPixelY, m_RangeY1, m_RangeY2);
  m_hist_bp_All_nf  = new TH2D("h_bp_All_nf","Back Projection (All, Fl Cut)", m_NumPixelX, m_RangeX1, m_RangeX2, m_NumPixelY, m_RangeY1, m_RangeY2);

  GetANLModuleNC("EventReconstruction", &m_EventReconstruction);
  std::vector<HitPattern>& hitpat_vec = m_EventReconstruction->GetHitPatternVector();
  m_hist_vec.resize(hitpat_vec.size());
  m_hist_nf_vec.resize(hitpat_vec.size());
  
  for (unsigned int i=0; i<hitpat_vec.size(); i++) {
    std::string hist_name = "h_bp_";
    std::string hist_title = "BackProjection (";
    hist_name += hitpat_vec[i].ShortName();
    hist_title += hitpat_vec[i].Name();
    hist_title += ")";
    m_hist_vec[i] = new TH2D(hist_name.c_str(), hist_title.c_str(), m_NumPixelX, m_RangeX1, m_RangeX2, m_NumPixelY, m_RangeY1, m_RangeY2);

    std::string hist_nf_name = "h_bp_";
    std::string hist_nf_title = "BackProjection (";
    hist_nf_name += hitpat_vec[i].ShortName();
    hist_nf_name += "_nf";
    hist_nf_title += hitpat_vec[i].Name();
    hist_nf_title += ", Fl Cut)";
    m_hist_nf_vec[i] = new TH2D(hist_nf_name.c_str(), hist_nf_title.c_str(), m_NumPixelX, m_RangeX1, m_RangeX2, m_NumPixelY, m_RangeY1, m_RangeY2);
  }

  return AS_OK;
}


ANLStatus BackProjection::mod_ana()
{
  const TwoHitComptonEvent& compton_event = GetComptonEvent();

  static TRandom3 randgen;
  const int Times = 1000;
  const double FillWeight = 1.0/double(Times);
  
  const vector3_t coneVertex = compton_event.ConeVertex();
  const vector3_t coneAxis = compton_event.ConeAxis();
  const vector3_t coneAxisOrtho = coneAxis.orthogonal();
  const double cosThetaE = compton_event.CosThetaE();

  vector3_t cone1(coneAxis);
  double thetaE = TMath::ACos(cosThetaE);
  cone1.rotate(thetaE, coneAxisOrtho);

  vector3_t coneSample;
  double phi;
  vector3_t coneProjected;

  for (int t=0; t<Times; t++) {
    coneSample = cone1;
    phi = randgen.Uniform(0.0, TMath::TwoPi());
    coneSample.rotate(phi, coneAxis);
    bool bPositive = SectionConeAndPlane(coneVertex, coneSample, coneProjected);
  
    if (bPositive == false) {
      continue;
    }

    FillImage(coneProjected.x()/m_PixelUnit, coneProjected.y()/m_PixelUnit, FillWeight);
  }
  
  return AS_OK;
}


const TwoHitComptonEvent& BackProjection::GetComptonEvent()
{
  return m_EventReconstruction->GetTwoHitData();
}


void BackProjection::FillImage(double x, double y, double weight)
{
  // Filling histograms 
  // All
  m_hist_bp_All->Fill(x, y, weight);
  if (!Evs("EventReconst:CdTeFluor")) { 
    m_hist_bp_All_nf->Fill(x, y, weight);
  }
  
  for(unsigned int i=0; i<m_hist_vec.size(); i++) {
    if(m_EventReconstruction->HitPatternFlag(i)) {
      m_hist_vec[i]->Fill(x, y, weight);
      if(!Evs("EventReconst:CdTeFluor")) {
        m_hist_nf_vec[i]->Fill(x, y, weight);
      }
    }
  }
}


bool BackProjection::SectionConeAndPlane(const vector3_t& vertex, const vector3_t& cone, vector3_t& coneProjected)
{
  double t;
  t = (m_PlaneNormal*(m_PlanePoint-vertex)) / (m_PlaneNormal*cone);
  coneProjected = vertex + t*cone;
  if (t < 0.) { return false; }
  return true;
}
