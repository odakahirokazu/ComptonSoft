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

#include "GainCorrection.hh"

#include "CSHitCollection.hh"
#include "DetectorHit.hh"
#include "VDeviceSimulation.hh"

using namespace comptonsoft;
using namespace anl;


GainCorrection::GainCorrection()
{
}


GainCorrection::~GainCorrection()
{
}


ANLStatus GainCorrection::mod_init()
{
  GetANLModuleNC("CSHitCollection", &hit_collection);
  return SetSimGainCorrection::mod_init();
}


ANLStatus GainCorrection::mod_ana()
{
  int numTimeGroup = hit_collection->NumTimeGroup();
  for (int tgroup=comptonsoft::NOTIMEGROUP; tgroup<numTimeGroup; tgroup++) {
    std::vector<DetectorHit_sptr>& hitVec
      = hit_collection->GetHits(tgroup);

    std::vector<DetectorHit_sptr>::iterator it;
    for (it=hitVec.begin(); it!=hitVec.end(); it++) {
      const int detid = (*it)->getDetectorID();
      VDeviceSimulation* ds = GetDetectorManager()->getDeviceSimulationByID(detid);
      const StripPair strip((*it)->getStripX(), (*it)->getStripY());
      const double ePI = (*it)->getPI();
      const double newPI = ds->correctGain(strip, ePI);
      (*it)->setPI(newPI);
    }
  }
  
  return AS_OK;
}
