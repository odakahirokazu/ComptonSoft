/*************************************************************************
 *                                                                       *
 * Copyright (c) 2019 Hirokazu Odaka                                     *
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

#ifndef COMPTONSOFT_PolarizedXrayEvent_H
#define COMPTONSOFT_PolarizedXrayEvent_H 1

#include "XrayEvent.hh"

namespace comptonsoft
{

/**
 * A class of an X-ray event measured with a pixel detector.
 *
 * @author Hirokazu Odaka
 * @date 2026-02-25 | polarization
 */
class PolarizedXrayEvent : public XrayEvent
{
public:
  explicit PolarizedXrayEvent(int size);
  virtual ~PolarizedXrayEvent();

  PolarizedXrayEvent(const PolarizedXrayEvent& r) = default;
  PolarizedXrayEvent(PolarizedXrayEvent&& r) = default;

protected:
  double calculateEventAngle() const override;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_XrayEvent_H */
