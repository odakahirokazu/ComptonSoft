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

#ifndef COMPTONSOFT_TwoHitComptonEvent_H
#define COMPTONSOFT_TwoHitComptonEvent_H 1

#include <iostream>
#include <vector>
#include "cs_globals.hh"

namespace comptonsoft {

/**
 * A class of a reconstructed Compton event which contains information on one Compton scattering and one hit associated with the recoil electron.
 * @author Hirokazu Odaka
 */

class TwoHitComptonEvent
{
public:
  TwoHitComptonEvent();
  virtual ~TwoHitComptonEvent(){}

  int getH1Id() const { return m_H1Id; }
  int getH1DetId() const { return m_H1DetId; }
  int getH1StripX() const { return m_H1StripX; }
  int getH1StripY() const { return m_H1StripY; }
  int getH1Chip() const { return m_H1Chip; }
  int getH1Channel() const { return m_H1Channel; }
  int getH1Process() const { return m_H1Process; }
  double getH1Time() const { return m_H1Time; }
  double getH1PosX() const { return m_H1PosX; }
  double getH1PosY() const { return m_H1PosY; }
  double getH1PosZ() const { return m_H1PosZ; }
  double getH1Energy() const { return m_H1Energy; }

  int getH2Id() const { return m_H2Id; }
  int getH2DetId() const { return m_H2DetId; }
  int getH2StripX() const { return m_H2StripX; }
  int getH2StripY() const { return m_H2StripY; }
  int getH2Chip() const { return m_H2Chip; }
  int getH2Channel() const { return m_H2Channel; }
  int getH2Process() const { return m_H2Process; }
  double getH2Time() const { return m_H2Time; }
  double getH2PosX() const { return m_H2PosX; }
  double getH2PosY() const { return m_H2PosY; }
  double getH2PosZ() const { return m_H2PosZ; }
  double getH2Energy() const { return m_H2Energy; }

  unsigned int getFlag() const { return m_Flag; }

  void setH1Id(int r){ m_H1Id = r; m_bCalc = false; }
  void setH1DetId(int r){ m_H1DetId = r; m_bCalc = false; }
  void setH1StripX(int r){ m_H1StripX = r; m_bCalc = false; }
  void setH1StripY(int r){ m_H1StripY = r; m_bCalc = false; }
  void setH1Chip(int r){ m_H1Chip = r; m_bCalc = false; }
  void setH1Channel(int r){ m_H1Channel = r; m_bCalc = false; }

  void setH1Process(int r){ m_H1Process = r; m_bCalc = false; }
  void setH1Time(double r) { m_H1Time = r; m_bCalc = false; }

  void setH1Pos(double x, double y, double z)
  { 
    m_H1PosX = x;  m_H1PosY = y;  m_H1PosZ = z;
    m_bCalc = false; 
  }

  void setH1Energy(double r) { m_H1Energy = r; m_bCalc = false; }

  void setH2Id(int r){ m_H2Id = r; m_bCalc = false; }
  void setH2DetId(int r){ m_H2DetId = r; m_bCalc = false; }
  void setH2StripX(int r){ m_H2StripX = r; m_bCalc = false; }
  void setH2StripY(int r){ m_H2StripY = r; m_bCalc = false; }
  void setH2Chip(int r){ m_H2Chip = r; m_bCalc = false; }
  void setH2Channel(int r){ m_H2Channel = r; m_bCalc = false; }

  void setH2Process(int r){ m_H2Process = r; m_bCalc = false; }
  void setH2Time(double r) { m_H2Time = r; m_bCalc = false; }

  void setH2Pos(double x, double y, double z)
  { 
    m_H2PosX = x;  m_H2PosY = y;  m_H2PosZ = z;
    m_bCalc = false; 
  }

  void setH2Energy(double r) { m_H2Energy = r; m_bCalc = false; }

  void setFlag(unsigned int v) { m_Flag = v; }

  double CosThetaE() const;
  double ThetaE() const;
  vector3_t ConeAxis() const;
  vector3_t ConeVertex() const;
  double ThetaG() const;
  double DeltaTheta() const;
  double TotalEnergy()  const { return m_H1Energy + m_H2Energy; }
  bool TimeOrder() const { return (m_H1Time <= m_H2Time) ? true : false; }
  bool EnergyOrder() const { return (m_H1Energy <= m_H2Energy) ? true : false; }
  void Swap();

  void setSourceDirection(double x, double y, double z)
  {
    m_SourceDirection.set(x, y, z);
    m_SourcePosition.set(0., 0., 0.);
    m_SourceDistant = true;
    m_bCalc = false;
  }

  void setSourcePosition(double x, double y, double z)
  {
    m_SourcePosition.set(x, y, z);
    m_SourceDirection.set(0., 0., 0.);
    m_SourceDistant = false;
    m_bCalc = false;
  }

  bool SourceDistant() const { return m_SourceDistant; }
  vector3_t getSourceDirection() const {
    vector3_t sourceDirection(m_SourceDirection);
    if (!m_SourceDistant) {
      sourceDirection = m_SourcePosition - m_ConeVertex;
    }
    return sourceDirection;
  }
  vector3_t getSourcePosition() const { return m_SourcePosition; }

  double distanceTwoHits() const;

private:
  mutable bool m_bCalc;

  int m_H1Id;
  int m_H1Process;
  int m_H1DetId;
  int m_H1StripX;
  int m_H1StripY;
  int m_H1Chip;
  int m_H1Channel;
  double m_H1Time;
  double m_H1PosX;
  double m_H1PosY;
  double m_H1PosZ;
  double m_H1Energy;
  int m_H2Id;
  int m_H2DetId;
  int m_H2StripX;
  int m_H2StripY;
  int m_H2Chip;
  int m_H2Channel;
  int m_H2Process;
  double m_H2Time;
  double m_H2PosX;
  double m_H2PosY;
  double m_H2PosZ;
  double m_H2Energy;
  unsigned int m_Flag;

  mutable double m_CosThetaE;
  mutable double m_ThetaE;
  mutable double m_ThetaG;
  mutable double m_DeltaTheta;
  mutable vector3_t m_ConeVertex;
  mutable vector3_t m_ConeAxis;

  bool m_SourceDistant;
  vector3_t m_SourceDirection;
  vector3_t m_SourcePosition;

private:
  void calc() const;
};


template <typename T>
bool filter_compton(T (TwoHitComptonEvent::*getter)() const,
                    const TwoHitComptonEvent& event,
                    T min, T max)
{
  //  std::cout << min << " " << (event.*getter)() << " " << max << std::endl;
  return min <= (event.*getter)() && (event.*getter)() <= max;
}

}

#endif /* COMPTONSOFT_TwoHitComptonEvent_H */
