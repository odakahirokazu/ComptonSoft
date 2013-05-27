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

// 2008-08-28 Hirokazu Odaka

#ifndef COMPTONSOFT_EventSelection_H
#define COMPTONSOFT_EventSelection_H 1

#include "VCSModule.hh"

#include "DetectorGroup.hh"
#include "CSHitCollection.hh"

namespace comptonsoft {

class EventSelection : public VCSModule
{
  DEFINE_ANL_MODULE(EventSelection, 1.0)
public:
  EventSelection();
  ~EventSelection() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();

private:
  CSHitCollection* hit_collection;

  std::string filename;
  comptonsoft::DetectorGroup si_detector_group;
  comptonsoft::DetectorGroup cdte_detector_group;
  comptonsoft::DetectorGroup anti_detector_group;

  bool remove_veto_event;

  static const int NUM_OF_FLUOR_CDTE = 10; 
  double fluor_cdte[NUM_OF_FLUOR_CDTE];
  double check_fluor_cdte(double ene, int& index, double& fluorene);
  double fluor_range;
};

}

#endif /* COMPTONSOFT_EventSelection_H */
