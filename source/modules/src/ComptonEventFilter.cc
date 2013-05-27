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

// Hiro Odaka

#include "ComptonEventFilter.hh"

#include <boost/bind.hpp>
#include "TMath.h"
#include "NextCLI.hh"

using namespace comptonsoft;
using namespace anl;

ComptonEventFilter::ComptonEventFilter()
  : EventReconstruction_ptr(0), hitpat_name(""), hitpat_selection(false)
{
}


ANLStatus ComptonEventFilter::mod_startup()
{
  register_parameter(&hitpat_name, "Hit pattern");
  
  return AS_OK;
}


ANLStatus ComptonEventFilter::mod_com()
{
  GetANLModuleNC("EventReconstruction", &EventReconstruction_ptr);
  std::vector<HitPattern>& hitpat_vec = EventReconstruction_ptr->GetHitPatternVector();

  std::cout << std::endl;
  std::cout << " ***** Hit Pattern List *****" << std::endl;
  for(unsigned int i=0; i<hitpat_vec.size(); i++) {
    std::cout << "  " << hitpat_vec[i].Name() << std::endl;
  }

  CLread("Select Hit Pattern : ", &hitpat_name);

  int num_conditions = 1;
  CLread("Number of conditions : ", &num_conditions);
  conditions_vector.resize(num_conditions);

  for (int i=0; i<num_conditions; i++) {
    while (1) {
      std::cout << "Selection mode list:" << '\n'
                << "  1: E1" << '\n'
                << "  2: E2" << '\n'
                << "  3: E1+E2" << '\n'
                << "  4: theta K" << '\n'
                << "  5: theta G" << '\n'
                << "  6: delta theta" << '\n'
                << "  7: time" << '\n'
                << "  8: hit 1 position x " << '\n'
                << "  9: hit 1 position y " << '\n'
                << " 10: hit 1 position z " << '\n'
                << " 11: hit 2 position x " << '\n'
                << " 12: hit 2 position y " << '\n'
                << " 13: hit 2 position z " << std::endl;

      int mode = 0;
      CLread("Selection mode (0 for quit)", &mode);
      
      boost::function<bool (const comptonsoft::TwoHitComptonEvent&)> condition;
      if (mode==0) {
        break;
      }
      else if (1<=mode && mode<=3) {
        double min = 0.0*keV;
        double max = 1000.0*keV;
        CLread("Minimum value", &min, keV, "keV");
        CLread("Maximum value", &max, keV, "keV");
	
        if (mode==1) {
          condition = boost::bind(comptonsoft::filter_compton<double>,
                                  &comptonsoft::TwoHitComptonEvent::getH1Energy, _1, min, max);
          conditions_vector[i].push_back(condition);
        }
        else if (mode==2) {
          condition = boost::bind(comptonsoft::filter_compton<double>,
                                  &comptonsoft::TwoHitComptonEvent::getH2Energy, _1, min, max);
          conditions_vector[i].push_back(condition);
        }
        else if (mode==3) {
          condition = boost::bind(comptonsoft::filter_compton<double>,
                                  &comptonsoft::TwoHitComptonEvent::TotalEnergy, _1, min, max);
          conditions_vector[i].push_back(condition);
        }
      }
      else if (4<=mode && mode<=6) {
        double min = 0.0*degree;
        double max = 180.0*degree;
        CLread("Minimum value", &min, degree, "degree");
        CLread("Maximum value", &max, degree, "degree");
	if (mode==4) {
          condition = boost::bind(comptonsoft::filter_compton<double>,
                                  &comptonsoft::TwoHitComptonEvent::ThetaE, _1, min, max);
          conditions_vector[i].push_back(condition);
        }
        else if (mode==5) {
          condition = boost::bind(comptonsoft::filter_compton<double>,
                                  &comptonsoft::TwoHitComptonEvent::ThetaG, _1, min, max);
          conditions_vector[i].push_back(condition);
        }
        else if (mode==6) {
          condition = boost::bind(comptonsoft::filter_compton<double>,
                                  &comptonsoft::TwoHitComptonEvent::DeltaTheta, _1, min, max);
          conditions_vector[i].push_back(condition);
        }
      }
      else if (mode==7) {
        double min = 0.0;
        double max = 0.0;
        CLread("Minimum value", &min, second, "second");
        CLread("Maximum value", &max, second, "second");
        condition = boost::bind(comptonsoft::filter_compton<double>,
                                &comptonsoft::TwoHitComptonEvent::getH1Time, _1, min, max);
        conditions_vector[i].push_back(condition);     
      }
      else if (8<=mode && mode<=13) {
        double min = -100.0*cm;
        double max = +100.0*cm;
        CLread("Minimum value", &min, cm, "cm");
        CLread("Maximum value", &max, cm, "cm");
        if (mode==8) {
          condition = boost::bind(comptonsoft::filter_compton<double>,
                                  &comptonsoft::TwoHitComptonEvent::getH1PosX, _1, min, max);
          conditions_vector[i].push_back(condition);
        }
        else if (mode==9) {
          condition = boost::bind(comptonsoft::filter_compton<double>,
                                  &comptonsoft::TwoHitComptonEvent::getH1PosY, _1, min, max);
          conditions_vector[i].push_back(condition);
        }
        else if (mode==10) {
          condition = boost::bind(comptonsoft::filter_compton<double>,
                                  &comptonsoft::TwoHitComptonEvent::getH1PosZ, _1, min, max);
          conditions_vector[i].push_back(condition);
        }
        else if (mode==11) {
          condition = boost::bind(comptonsoft::filter_compton<double>,
                                  &comptonsoft::TwoHitComptonEvent::getH2PosX, _1, min, max);
          conditions_vector[i].push_back(condition);
        }
        else if (mode==12) {
          condition = boost::bind(comptonsoft::filter_compton<double>,
                                  &comptonsoft::TwoHitComptonEvent::getH2PosY, _1, min, max);
          conditions_vector[i].push_back(condition);
        }
        else if (mode==13) {
          condition = boost::bind(comptonsoft::filter_compton<double>,
                                  &comptonsoft::TwoHitComptonEvent::getH2PosZ, _1, min, max);
          conditions_vector[i].push_back(condition);
        }
      }
      else {
        std::cout << "invalid number : " << mode << std::endl;
      }
    }
  }

  return AS_OK;
}


ANLStatus ComptonEventFilter::mod_init()
{
  GetANLModuleNC("EventReconstruction", &EventReconstruction_ptr);
  std::vector<HitPattern>& hitpat_vec = EventReconstruction_ptr->GetHitPatternVector();
  
  if (hitpat_name != "") {
    for(unsigned int i=0; i<hitpat_vec.size(); i++) {
      if (hitpat_vec[i].Name() == hitpat_name) {
        hitpat_selection = true;
        hitpat = hitpat_vec[i];
        break;
      }
    }

    if (hitpat_selection) {
      std::cout << "Hit pattern \"" << hitpat_name << "\" is selected." << std::endl;
    }
    else {
      std::cout << "There is not Hit Pattern named " << hitpat_name << std::endl;
      return AS_QUIT_ERR;
    }
  }
  
  return AS_OK;
}


ANLStatus ComptonEventFilter::mod_ana()
{
  const TwoHitComptonEvent& compton_event = EventReconstruction_ptr->GetTwoHitData();

  if (hitpat_selection) {
    std::vector<int> detids(2);
    detids[0] = compton_event.getH1DetId();
    detids[1] = compton_event.getH2DetId();
    
    if(!hitpat.match(detids)) {
      return AS_SKIP;
    }
  }

  if (conditions_vector.empty()) {
    return AS_OK;
  }

  bool selected = false;
  std::vector< std::vector< boost::function<bool (const comptonsoft::TwoHitComptonEvent&)> > >::iterator it;
  for (it=conditions_vector.begin(); it!=conditions_vector.end(); it++) {
    std::vector< boost::function<bool (const comptonsoft::TwoHitComptonEvent&)> >::iterator condition = it->begin();  
    bool good = true;
    while (condition!=it->end()) {
      if ( !(*condition)(compton_event) ) {
        good = false;
        break;
      }
      condition++;
    }
    if (good) {
      selected = true;
      break;
    }
  }

  if (!selected) {
    return AS_SKIP;
  }
  
  return AS_OK;
}


void ComptonEventFilter::define_condition()
{
  conditions_vector.resize(conditions_vector.size()+1);
}


void ComptonEventFilter::add_condition(const std::string& type,
                                       double min_value, double max_value)
{
  if (conditions_vector.size()==0) {
    define_condition();
  }

  boost::function<bool (const comptonsoft::TwoHitComptonEvent&)> condition;

  if (type == "E1") {
    condition = boost::bind(comptonsoft::filter_compton<double>,
                            &comptonsoft::TwoHitComptonEvent::getH1Energy,
                            _1, min_value*keV, max_value*keV);
  }
  else if (type == "E2") {
    condition = boost::bind(comptonsoft::filter_compton<double>,
                            &comptonsoft::TwoHitComptonEvent::getH2Energy,
                            _1, min_value*keV, max_value*keV);
  }
  else if (type == "E1+E2") {
    condition = boost::bind(comptonsoft::filter_compton<double>,
                            &comptonsoft::TwoHitComptonEvent::TotalEnergy,
                            _1, min_value*keV, max_value*keV);
  }
  else if (type == "theta K") {
    condition = boost::bind(comptonsoft::filter_compton<double>,
                            &comptonsoft::TwoHitComptonEvent::ThetaE,
                            _1, min_value, max_value);
  }
  else if (type == "theta G") {
    condition = boost::bind(comptonsoft::filter_compton<double>,
                            &comptonsoft::TwoHitComptonEvent::ThetaG,
                            _1, min_value, max_value);
  }
  else if (type == "delta theta") {
    condition = boost::bind(comptonsoft::filter_compton<double>,
                            &comptonsoft::TwoHitComptonEvent::DeltaTheta,
                            _1, min_value, max_value);
  }
  else if (type == "time") {
    condition = boost::bind(comptonsoft::filter_compton<double>,
                            &comptonsoft::TwoHitComptonEvent::getH1Time,
                            _1, min_value*s, max_value*s);
  }
  else if (type == "hit 1 position x") {
    condition = boost::bind(comptonsoft::filter_compton<double>,
                            &comptonsoft::TwoHitComptonEvent::getH1PosX,
                            _1, min_value*cm, max_value*cm);
  }
  else if (type == "hit 1 position y") {
    condition = boost::bind(comptonsoft::filter_compton<double>,
                            &comptonsoft::TwoHitComptonEvent::getH1PosY,
                            _1, min_value*cm, max_value*cm);
  }
  else if (type == "hit 1 position z") {
    condition = boost::bind(comptonsoft::filter_compton<double>,
                            &comptonsoft::TwoHitComptonEvent::getH1PosZ,
                            _1, min_value*cm, max_value*cm);
  }
  else if (type == "hit 2 position x") {
    condition = boost::bind(comptonsoft::filter_compton<double>,
                            &comptonsoft::TwoHitComptonEvent::getH2PosX,
                            _1, min_value*cm, max_value*cm);
  }
  else if (type == "hit 2 position y") {
    condition = boost::bind(comptonsoft::filter_compton<double>,
                            &comptonsoft::TwoHitComptonEvent::getH2PosY,
                            _1, min_value*cm, max_value*cm);
  }
  else if (type == "hit 2 position z") {
    condition = boost::bind(comptonsoft::filter_compton<double>,
                            &comptonsoft::TwoHitComptonEvent::getH2PosZ,
                            _1, min_value*cm, max_value*cm);
  }
  else {
    std::cout << "invalid condition type: " << type << std::endl;
    return;
  }
  
  conditions_vector.back().push_back(condition);
}
