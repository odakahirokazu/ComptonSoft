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

#include "VAnalyzeHitList.hh"

#include "DetectorHit.hh"
#include "DetectorGroup.hh"
#include "CSHitCollection.hh"

using namespace anl;
using namespace comptonsoft;


VAnalyzeHitList::VAnalyzeHitList()
  : hit_collection(0),
    directory_name("")
{
}


ANLStatus VAnalyzeHitList::mod_startup()
{
  register_parameter(&directory_name, "directory");
  return AS_OK;
}


ANLStatus VAnalyzeHitList::mod_init()
{
  GetANLModuleNC("CSHitCollection", &hit_collection);
  return AS_OK;
}


ANLStatus VAnalyzeHitList::mod_his()
{
  VCSModule::mod_his();
  mkdir_module(directory_name);
  define_histograms();
  return AS_OK;
}


ANLStatus VAnalyzeHitList::mod_ana()
{
  const int numTimeGroup = hit_collection->NumTimeGroup();
  for (int tgroup=comptonsoft::NOTIMEGROUP; tgroup<numTimeGroup; tgroup++) {
    std::vector<DetectorHit_sptr>& hitVec = hit_collection->GetHits(tgroup);
    fill_histograms(hitVec);
  }
  return AS_OK;
}


ANLStatus VAnalyzeHitList::mod_endrun()
{
  finish_histograms();
  return AS_OK;
}
