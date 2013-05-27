/*************************************************************************
 *                                                                       *
 * Copyright (c) 2012 Tetsuya Yasuda, Tamotsu Sato                       *
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

#ifndef COMPTONSOFT_ReadSPENVIS_H
#define COMPTONSOFT_ReadSPENVIS_H 1

#include <string>
#include <vector>

namespace comptonsoft {


/**
 * Read a spectrum from a SPENVIS output.
 *
 * @author T. Yasuda
 * @date 2012-04-16 | T. Yasuda
 * @date 2012-04-23 | T. Yasuda 
 * @date 2012-07-02 | Tamotsu Sato | cleanup
 * @date 2012-07-06 | Hirokazu Odaka | rename class name, slight cleanup
 */
class ReadSPENVIS
{
public:
  explicit ReadSPENVIS(std::string filename);
  ~ReadSPENVIS();
  
  double GetParticleCountRate();
  double GetParticleEnergy();
  
private:
  int m_ParticleDataNum;
  double m_ParticleFluxIntegralSum;
  
  // Integral
  std::vector<double> m_ParticleFluxIntegral;
  std::vector<double> m_ParticleIndex;
  std::vector<double> m_ParticleNorm;

  // Semilog
  std::vector<double> m_ParticleSemiLogNorm;
  std::vector<double> m_ParticleSemiLogCont;
};

}

#endif /* COMPTONSOFT_ReadSPENVIS_H */
