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

#include "CSHitCollection.hh"
#include "DetectorHit.hh"

using namespace anl;

namespace comptonsoft
{

CSHitCollection::CSHitCollection()
{
}

CSHitCollection::~CSHitCollection() = default;

ANLStatus CSHitCollection::mod_bgnrun()
{
  initializeEvent();
  return AS_OK;
}

ANLStatus CSHitCollection::mod_ana()
{
  initializeEvent();
  return AS_OK;
}

ANLStatus CSHitCollection::mod_endrun()
{
  initializeEvent();
  return AS_OK;
}

void CSHitCollection::initializeEvent()
{
  hitsVector_.clear();
}

void CSHitCollection::insertHit(const DetectorHit_sptr& hit)
{
  const int timeGroup = hit->TimeGroup();
  if (timeGroup >= NumberOfTimeGroups()) {
    hitsVector_.resize(timeGroup+1);
  }
  hitsVector_[timeGroup].push_back(hit);
}

} /* namespace comptonsoft */
