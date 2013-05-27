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

#ifndef COMPTONSOFT_BasicDeviceSimulation_H
#define COMPTONSOFT_BasicDeviceSimulation_H 1

#include "VDeviceSimulation.hh"

namespace comptonsoft {


/**
 * A class for simulating a semiconductor detector device.
 * This class provides functions to convert a deposited energy of a hit into a detector output such as an electrical pulse height, taking into account of a detector response.
 * Since the class also provides interface to such functions, you can derive a new specific class from this class to realize special treatment of detector response. 
 * @author Hirokazu Odaka
 * @date 2012-06-29 | Hirokazu Odaka | basic functions are included in this class.
 */
class BasicDeviceSimulation : public VDeviceSimulation
{
public:
  BasicDeviceSimulation();
  virtual ~BasicDeviceSimulation();

public:
  virtual void initializeTable();

  virtual void setThresholdEnergyCathode(double /* val */) {}
  virtual void setThresholdEnergyAnode(double /* val */) {}
  virtual double ThresholdEnergyCathode() { return 0.0; }
  virtual double ThresholdEnergyAnode() { return 0.0; }

  virtual double calculatePHA(const StripPair& sp,
                              double edep, double x, double y, double z);
  virtual double calculatePI(double pha, const StripPair& sp);

  virtual void printSimParam();
};

}

#endif /* COMPTONSOFT_BasicDeviceSimulation_H */
