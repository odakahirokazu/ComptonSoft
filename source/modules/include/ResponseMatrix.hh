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

#ifndef COMPTONSOFT_ResponseMatrix_H
#define COMPTONSOFT_ResponseMatrix_H 1

#include "VCSModule.hh"

class TH2;
namespace anlgeant4 { class InitialInformation; }

namespace comptonsoft {

class EventReconstruction;

/**
 * @author Hirokazu Odaka
 * @date 2015-10-15 | update
 */
class ResponseMatrix : public VCSModule
{
  DEFINE_ANL_MODULE(ResponseMatrix, 2.0);
public:
  ResponseMatrix();
  ~ResponseMatrix();

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();
  
private:
  const EventReconstruction* m_EventReconstruction;
  const anlgeant4::InitialInformation* m_InitialInfo;
  int m_NumBinEnergy;
  double m_RangeEnergy1;
  double m_RangeEnergy2;
  std::map<std::string, TH2*> m_Responses;
  std::vector<std::string> m_Selections;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ResponseMatrix_H */
