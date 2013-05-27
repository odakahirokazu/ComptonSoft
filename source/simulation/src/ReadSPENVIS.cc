/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Tetsuya Yasuda, Tamotsu Sato                       *
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

#include "ReadSPENVIS.hh"

#include <iostream>
#include <fstream>
#include <cmath>

#include "Randomize.hh"

using namespace comptonsoft;


ReadSPENVIS::ReadSPENVIS(std::string filename)
 : m_ParticleDataNum(0),
   m_ParticleFluxIntegralSum(0.0)
{
  using std::pow;
  // Initialize /////////////////////////////////////////
  // Raw data
  std::vector<double> ParticleEnergy, ParticleFlux;
  
  // open output files of SPENVIS ////////////////////////
  //
  std::ifstream InputParticle00(filename.c_str());

  if( !InputParticle00 ){
    std::cout << "could not open file: " << filename << " !!" << std::endl;
    exit(1);
  }

  // read data ///////////////////////////////////////
  // ---------------------------------------
  while( !InputParticle00.eof() ){ 
    if( (int)ParticleEnergy.size() <= m_ParticleDataNum ){
      ParticleEnergy.resize( m_ParticleDataNum + 1);
      ParticleFlux.resize( m_ParticleDataNum + 1);
    }
    InputParticle00 >> ParticleEnergy[m_ParticleDataNum] >> ParticleFlux[m_ParticleDataNum];
    if(ParticleFlux[m_ParticleDataNum] < 10e-306){
      std::cout << "!! break !!!" << std::endl; 
      std::cout << m_ParticleDataNum << std::endl;
      break;
    }
    ++m_ParticleDataNum;
  }
  
  // Integraration
  // ---------------------------------------
  m_ParticleIndex.resize(m_ParticleDataNum-1);
  m_ParticleNorm.resize(m_ParticleDataNum-1);
  m_ParticleFluxIntegral.resize(m_ParticleDataNum);
  
  // -------------------------------------------
  for(int i=0; i<m_ParticleDataNum-1; ++i){
    // between i and i+1
    m_ParticleIndex[i] = log10(ParticleFlux[i]/ParticleFlux[i+1])/log10(ParticleEnergy[i]/ParticleEnergy[i+1]);
    m_ParticleNorm[i] = ParticleFlux[i]/pow(ParticleEnergy[i],m_ParticleIndex[i]);
    m_ParticleFluxIntegral[i+1] = m_ParticleNorm[i]/(m_ParticleIndex[i]+1)*(pow(ParticleEnergy[i+1],m_ParticleIndex[i]+1)-pow(ParticleEnergy[i],m_ParticleIndex[i]+1));
    m_ParticleFluxIntegralSum = m_ParticleFluxIntegralSum +  m_ParticleFluxIntegral[i+1];
  }

  // -------------------------------------------
  m_ParticleFluxIntegral[0] = 0.0;
  for(int i=1; i<m_ParticleDataNum; ++i){
    m_ParticleFluxIntegral[i] = (m_ParticleFluxIntegral[i]/m_ParticleFluxIntegralSum) + m_ParticleFluxIntegral[i-1];
  }
  m_ParticleSemiLogNorm.resize(m_ParticleDataNum-1);
  m_ParticleSemiLogCont.resize(m_ParticleDataNum-1);
  for(int i=0; i<m_ParticleDataNum-1; ++i){
    m_ParticleSemiLogNorm[i] = (m_ParticleFluxIntegral[i+1]-m_ParticleFluxIntegral[i])/(pow(ParticleEnergy[i+1],m_ParticleIndex[i]+1)-pow(ParticleEnergy[i],m_ParticleIndex[i]+1));
    m_ParticleSemiLogCont[i] = m_ParticleFluxIntegral[i+1] - (m_ParticleSemiLogNorm[i]*pow(ParticleEnergy[i+1],m_ParticleIndex[i]+1));
  }
  
}


ReadSPENVIS::~ReadSPENVIS()
{
}


double ReadSPENVIS::GetParticleCountRate()
{
  return m_ParticleFluxIntegralSum;
}


double ReadSPENVIS::GetParticleEnergy()
{
  using std::pow;
  
  double Energy(0.0);
  double randomnum = G4UniformRand();
  
  for(int i=0; i<m_ParticleDataNum-1; ++i){
    if( m_ParticleFluxIntegral[i] <= randomnum && randomnum <= m_ParticleFluxIntegral[i+1]){
      Energy = pow((randomnum-m_ParticleSemiLogCont[i])/m_ParticleSemiLogNorm[i],
                   1.0/(m_ParticleIndex[i]+1));
      break;
    }
  }
  return Energy;
}
