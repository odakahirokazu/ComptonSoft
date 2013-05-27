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

#ifndef COMPTONSOFT_EFieldModel_H
#define COMPTONSOFT_EFieldModel_H 1

#include <boost/shared_array.hpp>

namespace comptonsoft {

/**
 * A class of an electric field inside the semiconductor detector.
 * @author Hirokazu Odaka
 * @date 2009-07-07
 * @date 2009-12-03 : fixed CCE calculation
 */

class EFieldModel
{
public:
  EFieldModel();

  void setUpsideAnode(bool val = true) { m_UpsideAnode = val; }
  void setUpsideCathode(bool val = true) { m_UpsideAnode = !val; }
  bool UpsideAnode() const { return m_UpsideAnode; }
  bool UpsideCathode() const { return !m_UpsideAnode; }

  void setUpsideReadElectrode(bool val = true) { m_UpsideReadElectrode = val; }
  bool UpsideReadElectrode() const { return m_UpsideReadElectrode; }

  void setThickness(double val) { m_Thickness = val; }
  double Thickness() const { return m_Thickness; }

  void setMuTau(double mutau_e, double mutau_h)
  {
    m_MuTauElectron = mutau_e;
    m_MuTauHole = mutau_h;
  }
  void setMuTauElectron(double val) { m_MuTauElectron = val; }
  void setMuTauHole(double val) { m_MuTauHole = val; }
  double MuTauElectron() const { return m_MuTauElectron; }
  double MuTauHole()const { return m_MuTauHole; }

  bool setEField(double bias, int mode);
  bool setEField(double bias, int mode, double p0);
  bool setEField(double bias, int mode, double p0, double p1);
  bool setEField(double bias, int mode, double p0, double p1, double p2);
  bool setEField(double bias, int mode, double p0, double p1, double p2, double p3);
  double BiasVoltage() const { return m_BiasVoltage; }
  int EFieldMode() const { return m_EFieldMode; }
  double EFieldParam(int i) const { return m_EFieldParam[i]; }

  void setWP(boost::shared_array<double> wp, int num_point)
  {
    m_NumDivision = num_point-1;
    m_WP = wp;
  }
  int NumPoint() const { return m_NumDivision+1; }
  int NumDivision() const { return m_NumDivision; }

  double WP(double z) const;
  double WPByIndex(int i) const;

  double PositionZ(double index) const;
  double PositionIndex(double z) const;

  double CCE(double z) const;
  double CCEOfElectron(double z) const { return CCE_impl(true, z); }
  double CCEOfHole(double z) const { return CCE_impl(false, z); }
  double DriftTimeMobility(double z, double zInitial) const;

  double CCEByHecht(double z) const;

private:
  double CCE_impl(bool electron, double z) const;

private:
  // geometry
  bool m_UpsideAnode;
  double m_Thickness;
  bool m_UpsideReadElectrode;

  // charge transport
  double m_MuTauElectron;
  double m_MuTauHole;

  // Actual electric field
  double m_BiasVoltage;
  int m_EFieldMode;
  double m_EFieldParam[4];
  double m_EFieldMean;
  double m_Alpha; // mode 2
  double m_Alpha1, m_Alpha2, m_BendingPoint, m_EFieldMax; // mode 3
  
  // Weighting potential
  int m_NumDivision;
  boost::shared_array<double> m_WP;
};


inline double EFieldModel::WPByIndex(int i) const
{
  if (i<0) { return m_WP[0]; }
  else if (m_NumDivision<i) { return m_WP[m_NumDivision]; }
  return m_WP[i];
}


inline double EFieldModel::PositionZ(double index) const
{
  return m_Thickness*(-0.5 + index/m_NumDivision);
}


inline double EFieldModel::PositionIndex(double z) const
{
  return (z/m_Thickness + 0.5)*m_NumDivision;
}


inline double EFieldModel::CCE(double z) const
{
  double cce = CCEOfElectron(z) + CCEOfHole(z);
  return cce;
}

}

#endif /* COMPTONSOFT_EFieldModel_H */
