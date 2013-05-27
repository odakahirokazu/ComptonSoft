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

#include "TwoHitComptonEvent.hh"
#include <cmath>

namespace comptonsoft {

TwoHitComptonEvent::TwoHitComptonEvent()
{
  m_bCalc = false;
  m_H1Id = 0;
  m_H1DetId = 0;
  m_H1Process = 0;
  m_H1Time = 0.0;
  m_H1PosX = 0.0;
  m_H1PosY = 0.0;
  m_H1PosZ = 0.0;
  m_H1Energy = 0.0;
  m_H2Id = 0;
  m_H2DetId = 0;
  m_H2Process = 0;
  m_H2Time = 0.0;
  m_H2PosX = 0.0;
  m_H2PosY = 0.0;
  m_H2PosZ = 0.0;
  m_H2Energy = 0.0;
  m_Flag = 0;
  
  m_SourceDistant = true;
  m_SourceDirection.set(0.0, 0.0, 1.0);
  m_SourcePosition.set(0.0, 0.0, 0.0);
}


double TwoHitComptonEvent::CosThetaE() const 
{ 
  if (!m_bCalc) { calc(); }
  return m_CosThetaE;
}


double TwoHitComptonEvent::ThetaE() const
{ 
  if (!m_bCalc) { calc(); }
  return m_ThetaE;
}


double TwoHitComptonEvent::ThetaG() const
{ 
  if (!m_bCalc) { calc(); }
  return m_ThetaG;
}


double TwoHitComptonEvent::DeltaTheta() const
{ 
  if (!m_bCalc) { calc(); }
  return m_DeltaTheta;
}
  

vector3_t TwoHitComptonEvent::ConeAxis() const
{
  if (!m_bCalc) { calc(); }
  return m_ConeAxis;
}


vector3_t TwoHitComptonEvent::ConeVertex() const
{
  if (!m_bCalc) { calc(); }
  return m_ConeVertex;
}


void TwoHitComptonEvent::Swap()
{
  m_bCalc = false;

  int H1Id = m_H1Id;
  int H1DetId = m_H1DetId;
  int H1Process = m_H1Process;
  double H1Time = m_H1Time;
  double H1PosX = m_H1PosX;
  double H1PosY = m_H1PosY;
  double H1PosZ = m_H1PosZ;
  double H1Energy = m_H1Energy;
  m_H1Id = m_H2Id;
  m_H1DetId = m_H2DetId;
  m_H1Process = m_H2Process;
  m_H1Time = m_H2Time;
  m_H1PosX = m_H2PosX;
  m_H1PosY = m_H2PosY;
  m_H1PosZ = m_H2PosZ;
  m_H1Energy = m_H2Energy;
  m_H2Id = H1Id;
  m_H2DetId = H1DetId;
  m_H2Process = H1Process;
  m_H2Time = H1Time;
  m_H2PosX = H1PosX;
  m_H2PosY = H1PosY;
  m_H2PosZ = H1PosZ;
  m_H2Energy = H1Energy;
}


void TwoHitComptonEvent::calc() const
{
  using std::acos;
  using std::floor;

  const double ELECTRON_MASS = 0.5109989 * MeV;
  m_CosThetaE = 1. - (ELECTRON_MASS*m_H1Energy)/(m_H2Energy*(m_H1Energy+m_H2Energy));
  double k = floor(m_CosThetaE/2.0+0.5);
  m_ThetaE = acos(m_CosThetaE - 2.0*k) - pi*k;
  
  vector3_t v1(m_H1PosX, m_H1PosY, m_H1PosZ);
  vector3_t v2(m_H2PosX, m_H2PosY, m_H2PosZ);
  m_ConeVertex = v1;
  v1 -= v2;
  m_ConeAxis = v1.unit();

  m_ThetaG = getSourceDirection().angle(m_ConeAxis);
  m_DeltaTheta = m_ThetaE - m_ThetaG;
  
  m_bCalc = true;
}


double TwoHitComptonEvent::distanceTwoHits() const
{
  vector3_t v1(m_H1PosX, m_H1PosY, m_H1PosZ);
  vector3_t v2(m_H2PosX, m_H2PosY, m_H2PosZ);
  return (v1-v2).mag();
}

}
