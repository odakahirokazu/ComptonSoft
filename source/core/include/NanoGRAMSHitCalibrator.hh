/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                    *
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

#ifndef COMPTONSOFT_NanoGRAMSHitCalibrator_H
#define COMPTONSOFT_NanoGRAMSHitCalibrator_H 1

#include <vector>

#include "DetectorHit_sptr.hh"
#include "NanoGRAMSCalibrationData.hh"

namespace comptonsoft
{

class TPCProperty;

namespace grams
{
struct RawFECHit;
}

std::vector<DetectorHit_sptr> buildCalibratedHits(
    const CalibrationConfig& config,
    const TPCProperty& tpc_property,
    const std::vector<grams::RawFECHit>& raw_hits);

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSHitCalibrator_H */
