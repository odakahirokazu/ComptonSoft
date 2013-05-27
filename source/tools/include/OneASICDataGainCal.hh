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

#ifndef COMPTONSOFT_OneASICDataGainCal_H
#define COMPTONSOFT_OneASICDataGainCal_H 1

#include <vector>
#include <algorithm>
#include "OneASICData.hh"

class TSpline;

namespace comptonsoft {

/**
 * A class of data output from one ASIC.
 * @author Hirokazu Odaka
 * @date 2007-12-xx
 * @date 2008-09-03
 * @date 2009-05-20
 * @date 2009-06-02
 * @date 2011-06-06
 * @date 2011-09-10 | separation of OneASICData
 */
class OneASICDataGainCal : public OneASICData
{
public:
  OneASICDataGainCal(int n, bool anode);
  OneASICDataGainCal(const OneASICDataGainCal& r);
  virtual ~OneASICDataGainCal();

  void resetCalSpline()
  { std::fill(m_CalSpline, m_CalSpline+NumChannel(), (const TSpline*)0); }
  void registerCalSpline(int ch, const TSpline* ptr)
  { m_CalSpline[ch] = ptr; }
  void registerCalSpline(const TSpline** a, int n)
  { std::copy(a, a+n, m_CalSpline); }
  void registerCalSpline(const std::vector<const TSpline*>& v)
  { std::copy(v.begin(), v.end(), m_CalSpline); }
  
  /**
   * convert PHA values to PI values according to a calibaration curve for all channels.
   * @return true if successful
   */
  bool convertPHA2PI();

  /**
   * convert a pha value to a pi value according to a calibaration curve.
   * @param ch channel ID
   * @param pha PHA value
   * @return PI value
   */
  double PHA2PI(int ch, double pha) const;

private:
  const TSpline **m_CalSpline;

private:
  OneASICDataGainCal& operator=(const OneASICDataGainCal& r);
};

}

#endif /* COMPTONSOFT_OneASICDataGainCal_H */
