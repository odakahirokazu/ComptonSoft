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

#include "BackProjectionSky.hh"

#include "AstroUnits.hh"
#include "TDirectory.h"
#include "TRandom3.h"
#include "TMath.h"
#include "BasicComptonEvent.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

BackProjectionSky::BackProjectionSky()
  : m_Rotation(0.0), m_Image_Center_Theta(0.0*unit::degree), m_Image_Center_Phi(0.0*unit::degree), 
                     m_Image_YAxis_Theta(90.0*unit::degree), m_Image_YAxis_Phi(90.0*unit::degree)
{
  setUnit(unit::degree, "degree");
}

BackProjectionSky::~BackProjectionSky() = default;

ANLStatus BackProjectionSky::mod_define()
{
  BackProjection::mod_define();

  unregister_parameter("plane_normal");
  unregister_parameter("plane_point");
  register_parameter(&m_Rotation, "rotation_angle", unit::degree, "degree");

  define_parameter("image_center_theta", &mod_class::m_Image_Center_Theta, unit::degree, "degree");
  define_parameter("image_center_phi",   &mod_class::m_Image_Center_Phi,   unit::degree, "degree");
  define_parameter("image_yaxis_theta",  &mod_class::m_Image_YAxis_Theta,  unit::degree, "degree");
  define_parameter("image_yaxis_phi",    &mod_class::m_Image_YAxis_Phi,    unit::degree, "degree");

  return AS_OK;
}

ANLStatus BackProjectionSky::mod_initialize()
{
  BackProjection::mod_initialize();

  vector3_t zDirection_unit(std::sin(m_Image_Center_Theta)*std::cos(m_Image_Center_Phi), 
                            std::sin(m_Image_Center_Theta)*std::sin(m_Image_Center_Phi), 
                            std::cos(m_Image_Center_Theta));

  vector3_t yDirectionRaw_unit(std::sin(m_Image_YAxis_Theta)*std::cos(m_Image_YAxis_Phi), 
                               std::sin(m_Image_YAxis_Theta)*std::sin(m_Image_YAxis_Phi), 
                               std::cos(m_Image_YAxis_Theta));

  //obtain component vertical to image center direction
  vector3_t yDirection      = yDirectionRaw_unit - (zDirection_unit * yDirectionRaw_unit) * zDirection_unit;
  vector3_t yDirection_unit = yDirection.unit();
  vector3_t xDirection      = yDirection_unit.cross(zDirection_unit);
  vector3_t xDirection_unit = xDirection.unit();

  //rotation
  xDirection_unit = (xDirection_unit.rotate(m_Rotation, zDirection_unit)).unit();
  yDirection_unit = (yDirection_unit.rotate(m_Rotation, zDirection_unit)).unit();

  m_XAxis.set(xDirection_unit.x(), xDirection_unit.y(), xDirection_unit.z());
  m_YAxis.set(yDirection_unit.x(), yDirection_unit.y(), yDirection_unit.z());
  m_ZAxis.set(zDirection_unit.x(), zDirection_unit.y(), zDirection_unit.z());

  std::cout << "X Axis: (" << m_XAxis.x() << ", " << m_XAxis.y() << ", " << m_XAxis.z() << ")" << std::endl;
  std::cout << "Y Axis: (" << m_YAxis.x() << ", " << m_YAxis.y() << ", " << m_YAxis.z() << ")" << std::endl;
  std::cout << "Z Axis: (" << m_ZAxis.x() << ", " << m_ZAxis.y() << ", " << m_ZAxis.z() << ")" << std::endl;

  return AS_OK;
}

ANLStatus BackProjectionSky::mod_analyze()
{
  const std::vector<BasicComptonEvent_sptr> events = getEventReconstructionModule()->getReconstructedEvents();
  for (const auto& event: events) {
    const double fraction = event->ReconstructionFraction();

    static TRandom3 randgen;
    const int Times = 1000;
    const double FillWeight = fraction/Times;
  
    const vector3_t coneVertex = event->ConeVertex();
    const vector3_t coneAxis = event->ConeAxis();
    const vector3_t coneAxisOrtho = coneAxis.orthogonal();
    const double cosThetaE = event->CosThetaE();

    vector3_t cone1(coneAxis);
    double thetaE = TMath::ACos(cosThetaE);
    cone1.rotate(thetaE, coneAxisOrtho);

    vector3_t coneSample;
    double phi;
  
    for (int t=0; t<Times; t++) {
      coneSample = cone1;
      phi = randgen.Uniform(0.0, TMath::TwoPi());
      coneSample.rotate(phi, coneAxis);
      double ux = coneSample.dot(m_XAxis);
      double uy = coneSample.dot(m_YAxis);
      double uz = coneSample.dot(m_ZAxis);
      double x = std::atan2(ux, uz);
      double y = 0.5*CLHEP::pi - std::acos(uy);

      fillImage(x/PixelUnit(), y/PixelUnit(), FillWeight);
    }
  }
  
  return AS_OK;
}

} /* namespace comptonsoft */
