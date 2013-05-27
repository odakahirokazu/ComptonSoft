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

#ifndef COMPTONSOFT_DETECTOR_UNIT_DEF
#define COMPTONSOFT_DETECTOR_UNIT_DEF 1

#if USE_ROOT
#include "OneASICDataGainCal.hh"
#else
#include "OneASICData.hh"
#endif

#include "RealDetectorUnit2DPad.hh"
#include "RealDetectorUnit2DStrip.hh"
#include "RealDetectorUnitScintillator.hh"

#if USE_ROOT
#include "SimDetectorUnitLite.hh"
#include "SimDetectorUnit2DPad.hh"
#include "SimDetectorUnit2DStrip.hh"
#include "SimDetectorUnitScintillator.hh"
#else
#include "SimDetectorUnitLite.hh"
#endif

namespace comptonsoft {

#if USE_ROOT
typedef SimDetectorUnit2DPad SimDetectorUnit_Pad_t;
typedef SimDetectorUnit2DStrip SimDetectorUnit_Strip_t;
typedef SimDetectorUnitScintillator SimDetectorUnit_Scintillator_t;
typedef SimDetectorUnitLite SimDetectorUnit_Lite_t;
#else
typedef SimDetectorUnitLite SimDetectorUnit_Pad_t;
typedef SimDetectorUnitLite SimDetectorUnit_Strip_t;
typedef SimDetectorUnitLite SimDetectorUnit_Scintillator_t;
typedef SimDetectorUnitLite SimDetectorUnit_Lite_t;
#endif

}

#endif /* COMPTONSOFT_DETECTOR_UNIT_DEF */
