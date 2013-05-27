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

#include "OneASICDataGainCal.hh"

#include "globals.hh"
#include "TSpline.h"

namespace comptonsoft {

OneASICDataGainCal::OneASICDataGainCal(int n, bool anode)
  : OneASICData(n, anode)
{
  const int nch = NumChannel();
  m_CalSpline = new const TSpline*[nch];
  resetCalSpline();
}


OneASICDataGainCal::OneASICDataGainCal(const OneASICDataGainCal& r)
  : OneASICData(r)
{
  const int nch = NumChannel();
  m_CalSpline = new const TSpline*[nch];
  registerCalSpline(r.m_CalSpline, nch);
}


OneASICDataGainCal::~OneASICDataGainCal()
{
  delete[] m_CalSpline;
}


double OneASICDataGainCal::PHA2PI(int ch, double pha) const
{
  double vPI = 0.0;
  const double vPHA = pha;
  const TSpline* calFunc = m_CalSpline[ch];
  
  if (calFunc == 0) {
    double gain = getGainFactor(ch);
    vPI = vPHA * gain * keV;
    return vPI;
  }

  double calmin = calFunc->GetXmin();
  double calmax = calFunc->GetXmax();

  if ( vPHA < calmin) {
    // extrapolation
    double derivative = 0.0;
    double slope = (calFunc->Eval(calmax) - calFunc->Eval(calmin))/(calmax-calmin);
    const TSpline3* spline3 = dynamic_cast<const TSpline3*>(calFunc);
    if (spline3) {
      derivative = spline3->Derivative(calmin);
    }
    else {
      const TSpline5* spline5 = dynamic_cast<const TSpline5*>(calFunc);
      if (spline5) {
        derivative = spline5->Derivative(calmin);
      }
      else {
        derivative = slope;
      }
    }
    if (derivative*slope < 0.0) {
      std::cout << "Calibration function has bad derivative: " << calFunc->GetName() << std::endl;
      return 0.0;
    }
    vPI = ( calFunc->Eval(calmin) + derivative * (vPHA-calmin) ) * keV;
  }
  else if (calmax < vPHA) {
    // extrapolation
    double derivative = 0.0;
    double slope = (calFunc->Eval(calmax) - calFunc->Eval(calmin))/(calmax-calmin);
    const TSpline3* spline3 = dynamic_cast<const TSpline3*>(calFunc);
    if (spline3) { 
      derivative = spline3->Derivative(calmax);
    }
    else {
      const TSpline5* spline5 = dynamic_cast<const TSpline5*>(calFunc);
      if (spline5) {
        derivative = spline5->Derivative(calmax);
      }
      else {
        derivative = slope;
      }
    }
    if (derivative*slope < 0.0) {
      std::cout << "Calibration function has bad derivative: " << calFunc->GetName() << std::endl;
      return 0.0;
    }
    vPI = ( calFunc->Eval(calmax) + derivative * (vPHA-calmax) ) * keV;
  }
  else {
    vPI = calFunc->Eval(vPHA) * keV;
  }

  return vPI;
}


bool OneASICDataGainCal::convertPHA2PI()
{
  const int nCH = NumChannel();
  for (int i=0; i<nCH; i++) {
    if (getBadChannel(i) == 0) {
      if (m_CalSpline[i] == 0) {
        setPI(i, getPHA(i) * getGainFactor(i) * keV);
        continue;
      }
      
      double calmin = m_CalSpline[i]->GetXmin();
      double calmax = m_CalSpline[i]->GetXmax();
      
      if ( getPHA(i) < calmin ) {
        // extrapolation
        double derivative = 0.0;
        double slope = (m_CalSpline[i]->Eval(calmax) - m_CalSpline[i]->Eval(calmin))/(calmax-calmin);
        const TSpline3* spline3 = dynamic_cast<const TSpline3*>(m_CalSpline[i]);
        if (spline3) { 
          derivative = spline3->Derivative(calmin);
        }
        else {
          const TSpline5* spline5 = dynamic_cast<const TSpline5*>(m_CalSpline[i]);
          if (spline5) {
            derivative = spline5->Derivative(calmin);
          }
          else {
            derivative = slope;
          }
        }
        if (derivative*slope < 0.0) {
          std::cout << "Calibration function has bad derivative: " << m_CalSpline[i]->GetName() << std::endl;
          return false;
        }
        setPI(i, ( m_CalSpline[i]->Eval(calmin) + derivative * (getPHA(i)-calmin) ) * keV);
      }
      else if ( getPHA(i) > calmax ) {
        // extrapolation
        double derivative = 0.0;
        double slope = (m_CalSpline[i]->Eval(calmax) - m_CalSpline[i]->Eval(calmin))/(calmax-calmin);
        const TSpline3* spline3 = dynamic_cast<const TSpline3*>(m_CalSpline[i]);
        if (spline3) { 
          derivative = spline3->Derivative(calmax);
        }
        else {
          const TSpline5* spline5 = dynamic_cast<const TSpline5*>(m_CalSpline[i]);
          if (spline5) {
            derivative = spline5->Derivative(calmax);
          }
          else {
            derivative = slope;
          }
        }
        if (derivative*slope < 0.0) {
          std::cout << "Calibration function has bad derivative: " << m_CalSpline[i]->GetName() << std::endl;
          return false;
        }
        setPI(i, ( m_CalSpline[i]->Eval(calmax) + derivative * (getPHA(i)-calmax) ) * keV);
      }
      else {
        setPI(i, m_CalSpline[i]->Eval(getPHA(i) * keV));
      }
    }
    else {
      setPI(i, 0.);
    }
  }
  
  return true;
}

}
