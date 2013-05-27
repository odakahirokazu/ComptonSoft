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

#ifndef COMPTONSOFT_VDeviceSimulation_H
#define COMPTONSOFT_VDeviceSimulation_H 1

#include <vector>
#include <list>
#include <boost/utility.hpp>
#include "DetectorHit_sptr.hh"
#include "RealDetectorUnit.hh"

namespace comptonsoft {

class DetectorHit;


/**
 * A virtual class for simulating a semiconductor detector device.
 * This class will provide methods to convert a deposited energy of a hit into a detector output such as an electrical pulse height, taking into account of a detector response.
 * Since the class also provides interface to such functions, you can derive a new specific class from this class to realize special treatment of detector response. 
 *
 * @author Hirokazu Odaka
 * @date 2007-xx-xx | DeviceSimulation
 * @date 2008-07-29
 * @date 2009-10-26 
 * @date 2012-07-01 | VDeviceSimulation 
 */
class VDeviceSimulation : private boost::noncopyable
{
public:
  VDeviceSimulation();
  virtual ~VDeviceSimulation();

public:
  void setQuenchingFactor(double val) { m_QuenchingFactor = val; } 
  double QuenchingFactor() { return m_QuenchingFactor; }

  virtual void initializeTable() = 0;

  void setBadChannel(const StripPair& sp, int val);
  void setBadChannel(int chip, int channel, int val);
  int BadChannel(const StripPair& sp);
  int BadChannel(int chip, int channel);
  void setBadChannelAll(int val);

  void setNoiseFWHM(const StripPair& sp,
                    double param0, double param1, double param2);
  void setNoiseFWHM(int chip, int channel,
                    double param0, double param1, double param2);
  double NoiseFWHMParam0(const StripPair& sp);
  double NoiseFWHMParam0(int chip, int channel);
  double NoiseFWHMParam1(const StripPair& sp);
  double NoiseFWHMParam1(int chip, int channel);
  double NoiseFWHMParam2(const StripPair& sp);
  double NoiseFWHMParam2(int chip, int channel);
  void setNoiseFWHMAll(double v0, double v1, double v2);

  void setTimeResolutionFast(double val) { m_TimeResolutionFast = val;}
  double TimeResolutionFast() { return m_TimeResolutionFast;}
  void setTimeResolutionSlow(double val) { m_TimeResolutionSlow = val;}
  double TimeResolutionSlow() { return m_TimeResolutionSlow;}

  void setRemoveBadChannel(bool val) { m_RemoveBadChannel = val; }
  bool getRemoveBadChannel() { return m_RemoveBadChannel; }

  virtual void setThresholdEnergyCathode(double /* val */) = 0;
  virtual void setThresholdEnergyAnode(double /* val */) = 0;
  virtual double ThresholdEnergyCathode() = 0;
  virtual double ThresholdEnergyAnode() = 0;

  void setThresholdEnergyX(double thr) { m_ThresholdEnergyX = thr; }
  void setThresholdEnergyY(double thr) { m_ThresholdEnergyY = thr; }
  void setThresholdEnergy(double thr) {
    m_ThresholdEnergyX = thr;
    m_ThresholdEnergyY = thr;
  }
  double ThresholdEnergyX() { return m_ThresholdEnergyX; }
  double ThresholdEnergyY() { return m_ThresholdEnergyY; }
  double ThresholdEnergy(const StripPair& sp);

  void fillStrip(DetectorHit_sptr hit);

  virtual double calculatePHA(const StripPair& sp,
                              double edep, double x, double y, double z) = 0;
  virtual double calculatePI(double pha, const StripPair& sp) = 0;

  virtual double correctGain(const StripPair& , double ePI) { return ePI; }
  virtual double correctGain(int , int , double ePI) { return ePI; }

  void insertRawHit(DetectorHit_sptr hit) { m_RawHits.push_back(hit); }
  int NumRawHits() { return m_RawHits.size(); }
  const_DetectorHit_sptr getRawHit(int i) { return m_RawHits[i]; }
  void clearRawHits() { return m_RawHits.clear(); }

  int makeDetectorHits();
  int makeRawDetectorHits();

  void initializeTimingProcess();
  bool EmptySimulatedHits() { return m_SimulatedHits.empty(); }
  double FirstEventTime();
  int makeDetectorHitsAtTime(double time_start, int time_group);

  int DetectorID() { return m_DetectorUnit->getID(); }
  std::string DetectorName() { return m_DetectorUnit->getName(); }

  virtual void printSimParam() = 0;

protected:
  void registerDetectorUnit(RealDetectorUnit* det) { m_DetectorUnit = det; }
 
  double DetectorWidthX() { return m_DetectorUnit->getWidthX(); }
  double DetectorWidthY() { return m_DetectorUnit->getWidthY(); }
  double DetectorThickness() { return m_DetectorUnit->getThickness(); }

  virtual bool RangeCheck(const StripPair& sp) = 0;
  virtual int IndexOfTable(const StripPair& sp) = 0;
  virtual int SizeOfTable() = 0;

  virtual void simulatePulseHeights() = 0;
  void removeHitsOutOfStripRange();
  void sumSimulatedHits();
  void insertSimulatedHit(DetectorHit_sptr hit)
  { m_SimulatedHits.push_back(hit); }

  void sumSimulatedHitsIfCoincident(double time_width);
  void sortSimulatedHitsInTimeOrder();
  
  bool isTableInitialized() { return m_TableInitialized; }
  StripPair Channel2Strip(int chip, int channel)
  { return m_DetectorUnit->convertChannel2Strip(chip, channel); }


protected:
  std::vector<DetectorHit_sptr> m_RawHits;
  std::list<DetectorHit_sptr> m_SimulatedHits;

private:
  void simulateDetectorHits();

private:
  double m_QuenchingFactor;
  
  bool m_TableInitialized;
  std::vector<int> m_BadChannelTable;
  std::vector<double> m_NoiseFWHMParam0; // [keV]
  std::vector<double> m_NoiseFWHMParam1; // [keV^(1/2)]
  std::vector<double> m_NoiseFWHMParam2; // [1]

  double m_TimeResolutionFast;
  double m_TimeResolutionSlow;

  bool m_RemoveBadChannel;
  double m_ThresholdEnergyX;
  double m_ThresholdEnergyY;
  
  RealDetectorUnit* m_DetectorUnit;
};


inline void VDeviceSimulation::setBadChannel(const StripPair& sp, int val)
{
  if (!m_TableInitialized) initializeTable();
  
  if (RangeCheck(sp)) {
    m_BadChannelTable[IndexOfTable(sp)] = val;
  }
}


inline void VDeviceSimulation::setBadChannel(int chip, int channel, int val)
{ 
  setBadChannel(m_DetectorUnit->convertChannel2Strip(chip,channel), val);
}


inline int VDeviceSimulation::BadChannel(const StripPair& sp)
{
  if (RangeCheck(sp)) {
    return m_BadChannelTable[IndexOfTable(sp)];
  }
  else {
    return 1;
  }
}


inline int VDeviceSimulation::BadChannel(int chip, int channel)
{
  return BadChannel(m_DetectorUnit->convertChannel2Strip(chip,channel));
}


inline
void VDeviceSimulation::setNoiseFWHM(const StripPair& sp,
                                    double param0, double param1, double param2)
{
  if (!m_TableInitialized) initializeTable();
  
  if (RangeCheck(sp)) {
    m_NoiseFWHMParam0[IndexOfTable(sp)] = param0;
    m_NoiseFWHMParam1[IndexOfTable(sp)] = param1;
    m_NoiseFWHMParam2[IndexOfTable(sp)] = param2;
  }
}


inline
void VDeviceSimulation::setNoiseFWHM(int chip, int channel,
                                    double param0, double param1, double param2)
{
  setNoiseFWHM(m_DetectorUnit->convertChannel2Strip(chip,channel),
               param0, param1, param2);
}


inline double VDeviceSimulation::NoiseFWHMParam0(const StripPair& sp)
{
  if (RangeCheck(sp)) {
    return m_NoiseFWHMParam0[IndexOfTable(sp)];
  }
  else {
    return 0.0;
  }
}


inline double VDeviceSimulation::NoiseFWHMParam0(int chip, int channel)
{
  return NoiseFWHMParam0(m_DetectorUnit->convertChannel2Strip(chip,channel));
}


inline double VDeviceSimulation::NoiseFWHMParam1(const StripPair& sp)
{
  if (RangeCheck(sp)) {
    return m_NoiseFWHMParam1[IndexOfTable(sp)];
  }
  else {
    return 0.0;
  }
}


inline double VDeviceSimulation::NoiseFWHMParam1(int chip, int channel)
{
  return NoiseFWHMParam1(m_DetectorUnit->convertChannel2Strip(chip,channel));
}


inline double VDeviceSimulation::NoiseFWHMParam2(const StripPair& sp)
{
  if (RangeCheck(sp)) {
    return m_NoiseFWHMParam2[IndexOfTable(sp)];
  }
  else {
    return 0.0;
  }
}


inline double VDeviceSimulation::NoiseFWHMParam2(int chip, int channel)
{
  return NoiseFWHMParam2(m_DetectorUnit->convertChannel2Strip(chip,channel));
}


inline double VDeviceSimulation::ThresholdEnergy(const StripPair& sp)
{
  if (sp.X()==-1) {
    return m_ThresholdEnergyY;
  }
  else {
    return m_ThresholdEnergyX;
  }
}

typedef std::vector<VDeviceSimulation*> VDeviceSimulationVector;
typedef std::vector<VDeviceSimulation*>::iterator VDeviceSimulationIter;

}

#endif /* COMPTONSOFT_VDeviceSimulation_H */
