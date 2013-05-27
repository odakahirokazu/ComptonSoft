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

#include "DeadTimeTree_VME3.hh"
#include "ReadDataFile_VME3.hh"

using namespace comptonsoft;
using namespace anl;


DeadTimeTree_VME3::DeadTimeTree_VME3()
  : m_ReadDataModule(0),
    m_DeadTimeTree(0), m_time(0), m_deadtime(0)
{
}


ANLStatus DeadTimeTree_VME3::mod_init()
{
  GetANLModule("ReadDataFile_VME3", &m_ReadDataModule);
  return AS_OK;
}


ANLStatus DeadTimeTree_VME3::mod_his()
{
  VCSModule::mod_his();
  
  m_DeadTimeTree = new TTree("deadtimetree","deadtimetree");
  m_DeadTimeTree->Branch("time", &m_time, "time/I");
  m_DeadTimeTree->Branch("deadtime", &m_deadtime, "deadtime/D");

  return AS_OK;
}


ANLStatus DeadTimeTree_VME3::mod_ana()
{
  static int i = 0;  
  static int s_time;
  m_time = m_ReadDataModule->Time();
  if(i == 0) { s_time = m_time; }
  
  unsigned short int tmp_deadtime = m_ReadDataModule->DeadTime();
  m_deadtime = 100.0 * ((double)tmp_deadtime / 65535.0);
  
  if (i == 0) {
    m_DeadTimeTree -> Fill();
  }

  if(s_time != m_time) {
    m_DeadTimeTree -> Fill();
    s_time=m_time;
  }
  
  i++;
  
  return AS_OK;
}
