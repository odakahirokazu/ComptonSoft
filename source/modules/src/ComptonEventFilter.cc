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

#include "ComptonEventFilter.hh"
#include <algorithm>
#include "NextCLI.hh"
#include "AstroUnits.hh"
#include "BasicComptonEvent.hh"
#include "EventReconstruction.hh"

using namespace anl;

namespace comptonsoft
{

ComptonEventFilter::ComptonEventFilter()
  : m_EventReconstruction(nullptr)
{
}

ANLStatus ComptonEventFilter::mod_startup()
{
  register_parameter(&m_HitPatternNames, "hit_patterns");
  return AS_OK;
}

ANLStatus ComptonEventFilter::mod_init()
{
  VCSModule::mod_init();
  
  EvsDef("ComptonEventFilter:Selected");
  EvsDef("ComptonEventFilter:RejectedByHitPattern");
  EvsDef("ComptonEventFilter:RejectedByConditions");
  
  GetANLModuleNC("EventReconstruction", &m_EventReconstruction);
  
  auto& hitPatterns = getDetectorManager()->getHitPatterns();
  for (const std::string& hitpatName: m_HitPatternNames) {
    auto it = std::find_if(std::begin(hitPatterns), std::end(hitPatterns),
                           [&](const HitPattern hitpat) {
                             return (hitpat.Name()==hitpatName);
                           });
    if (it!=std::end(hitPatterns)) {
      m_HitPatterns.push_back(*it);
      std::cout << "Hit pattern \"" << hitpatName << "\" is selected." << std::endl;

      std::string evsKey = "HitPattern:";
      evsKey += it->ShortName();
      m_HitPatternEvsKeys.push_back(evsKey);
    }
    else {
      std::cout << "Hit pattern \"" << hitpatName << "\" is not found." << std::endl;
    }
  }
  
  return AS_OK;
}

ANLStatus ComptonEventFilter::mod_ana()
{
  const BasicComptonEvent& comptonEvent = m_EventReconstruction->getComptonEvent();

  if (m_HitPatterns.size() != 0) {
    bool matched = false;
    for (const std::string& hitpatKey: m_HitPatternEvsKeys) {
      if (Evs(hitpatKey)) {
        matched = true;
        break;
      }
    }
    
    if (!matched) {
      EvsSet("ComptonEventFilter:RejectedByHitPattern");
      return AS_SKIP;
    }
  }

  if (m_ConditionsVector.empty()) {
    EvsSet("ComptonEventFilter:Selected");
    return AS_OK;
  }
  
  bool selected = false;
  for (auto it=m_ConditionsVector.begin(); it!=m_ConditionsVector.end(); ++it) {
    std::vector<std::function<bool (const BasicComptonEvent&)>>::iterator condition = it->begin();  
    bool good = true;
    while (condition!=it->end()) {
      if ( !(*condition)(comptonEvent) ) {
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
    EvsSet("ComptonEventFilter:RejectedByConditions");
    m_EventReconstruction->clearAllHitPatternEVS();
    return AS_SKIP;
  }

  EvsSet("ComptonEventFilter:Selected");
  return AS_OK;
}

void ComptonEventFilter::define_condition()
{
  m_ConditionsVector.resize(m_ConditionsVector.size()+1);
}

void ComptonEventFilter::add_condition(const std::string& type,
                                       double min_value, double max_value)
{
  if (m_ConditionsVector.size()==0) {
    define_condition();
  }

  std::function<bool (const BasicComptonEvent&)> condition;

  namespace arg = std::placeholders;
  if (type == "E1") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit1Energy,
                          arg::_1, min_value*keV, max_value*keV);
  }
  else if (type == "E2") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit2Energy,
                          arg::_1, min_value*keV, max_value*keV);
  }
  else if (type == "E1+E2") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::TotalEnergy,
                          arg::_1, min_value*keV, max_value*keV);
  }
  else if (type == "theta K") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::ThetaE,
                          arg::_1, min_value, max_value);
  }
  else if (type == "theta G") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::ThetaG,
                          arg::_1, min_value, max_value);
  }
  else if (type == "delta theta") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::DeltaTheta,
                          arg::_1, min_value, max_value);
  }
  else if (type == "time") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit1Time,
                          arg::_1, min_value*s, max_value*s);
  }
  else if (type == "hit 1 position x") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit1PositionX,
                          arg::_1, min_value*cm, max_value*cm);
  }
  else if (type == "hit 1 position y") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit1PositionY,
                          arg::_1, min_value*cm, max_value*cm);
  }
  else if (type == "hit 1 position z") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit1PositionZ,
                          arg::_1, min_value*cm, max_value*cm);
  }
  else if (type == "hit 2 position x") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit2PositionX,
                          arg::_1, min_value*cm, max_value*cm);
  }
  else if (type == "hit 2 position y") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit2PositionY,
                          arg::_1, min_value*cm, max_value*cm);
  }
  else if (type == "hit 2 position z") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit2PositionZ,
                          arg::_1, min_value*cm, max_value*cm);
  }
  else {
    std::cout << "invalid condition type: " << type << std::endl;
    return;
  }
  
  m_ConditionsVector.back().push_back(condition);
}

ANLStatus ComptonEventFilter::mod_com()
{
  std::cout << std::endl;
  std::cout << " ***** Hit Pattern List *****" << std::endl;
  for (auto& hitpatName: m_HitPatternNames) {
    std::cout << "  " << hitpatName << std::endl;
  }

  std::string hitpatName;
  CLread("Select Hit Pattern : ", &hitpatName);
  m_HitPatternNames.push_back(hitpatName);

  int numConditions = 1;
  CLread("Number of conditions : ", &numConditions);
  m_ConditionsVector.resize(numConditions);
  
  for (int i=0; i<numConditions; i++) {
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
      
      std::function<bool (const BasicComptonEvent&)> condition;
      namespace arg = std::placeholders;
      if (mode==0) {
        break;
      }
      else if (1<=mode && mode<=3) {
        double min = 0.0*keV;
        double max = 1000.0*keV;
        CLread("Minimum value", &min, keV, "keV");
        CLread("Maximum value", &max, keV, "keV");
	
        if (mode==1) {
          condition = std::bind(filter_compton<double>,
                                &BasicComptonEvent::Hit1Energy, arg::_1, min, max);
          m_ConditionsVector[i].push_back(condition);
        }
        else if (mode==2) {
          condition = std::bind(filter_compton<double>,
                                &BasicComptonEvent::Hit2Energy, arg::_1, min, max);
          m_ConditionsVector[i].push_back(condition);
        }
        else if (mode==3) {
          condition = std::bind(filter_compton<double>,
                                &BasicComptonEvent::TotalEnergy, arg::_1, min, max);
          m_ConditionsVector[i].push_back(condition);
        }
      }
      else if (4<=mode && mode<=6) {
        double min = 0.0*degree;
        double max = 180.0*degree;
        CLread("Minimum value", &min, degree, "degree");
        CLread("Maximum value", &max, degree, "degree");
        if (mode==4) {
          condition = std::bind(filter_compton<double>,
                                &BasicComptonEvent::ThetaE, arg::_1, min, max);
          m_ConditionsVector[i].push_back(condition);
        }
        else if (mode==5) {
          condition = std::bind(filter_compton<double>,
                                &BasicComptonEvent::ThetaG, arg::_1, min, max);
          m_ConditionsVector[i].push_back(condition);
        }
        else if (mode==6) {
          condition = std::bind(filter_compton<double>,
                                &BasicComptonEvent::DeltaTheta, arg::_1, min, max);
          m_ConditionsVector[i].push_back(condition);
        }
      }
      else if (mode==7) {
        double min = 0.0;
        double max = 0.0;
        CLread("Minimum value", &min, second, "second");
        CLread("Maximum value", &max, second, "second");
        condition = std::bind(filter_compton<double>,
                              &BasicComptonEvent::Hit1Time, arg::_1, min, max);
        m_ConditionsVector[i].push_back(condition);     
      }
      else if (8<=mode && mode<=13) {
        double min = -100.0*cm;
        double max = +100.0*cm;
        CLread("Minimum value", &min, cm, "cm");
        CLread("Maximum value", &max, cm, "cm");
        if (mode==8) {
          condition = std::bind(filter_compton<double>,
                                &BasicComptonEvent::Hit1PositionX, arg::_1, min, max);
          m_ConditionsVector[i].push_back(condition);
        }
        else if (mode==9) {
          condition = std::bind(filter_compton<double>,
                                &BasicComptonEvent::Hit1PositionY, arg::_1, min, max);
          m_ConditionsVector[i].push_back(condition);
        }
        else if (mode==10) {
          condition = std::bind(filter_compton<double>,
                                &BasicComptonEvent::Hit1PositionZ, arg::_1, min, max);
          m_ConditionsVector[i].push_back(condition);
        }
        else if (mode==11) {
          condition = std::bind(filter_compton<double>,
                                &BasicComptonEvent::Hit2PositionX, arg::_1, min, max);
          m_ConditionsVector[i].push_back(condition);
        }
        else if (mode==12) {
          condition = std::bind(filter_compton<double>,
                                &BasicComptonEvent::Hit2PositionY, arg::_1, min, max);
          m_ConditionsVector[i].push_back(condition);
        }
        else if (mode==13) {
          condition = std::bind(filter_compton<double>,
                                &BasicComptonEvent::Hit2PositionZ, arg::_1, min, max);
          m_ConditionsVector[i].push_back(condition);
        }
      }
      else {
        std::cout << "invalid number : " << mode << std::endl;
      }
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
