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

#ifndef COMPTONSOFT_IsotopeInfo_H
#define COMPTONSOFT_IsotopeInfo_H 1

namespace comptonsoft {


/**
 * Isotope class
 *
 * @author Hirokazu Odaka
 * @date 2012-02-06
 */
class IsotopeInfo
{
public:
  IsotopeInfo()
    : m_Z(0), m_A(0), m_Energy(0.0), m_Count(0), m_Rate(0.0) 
  {
  }

  IsotopeInfo(int z, int a, double energy=0.0)
    : m_Z(z), m_A(a), m_Energy(energy),
      m_Count(0), m_Rate(0.0)
  {
  }
  
  int Z() const { return m_Z; }
  int A() const { return m_A; }
  double Energy() const { return m_Energy; }
  int Count() const { return m_Count; }
  double Rate() const { return m_Rate; }
  
  void setCount(int v) { m_Count = v; }
  void add1() { m_Count++; }

  void setRate(double v) { m_Rate = v; }

private:
  int m_Z;
  int m_A;
  double m_Energy;
  int m_Count;
  double m_Rate;
};

}

#endif /* COMPTONSOFT_IsotopeInfo_H */
