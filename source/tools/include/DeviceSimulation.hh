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

#ifndef COMPTONSOFT_DeviceSimulation_H
#define COMPTONSOFT_DeviceSimulation_H 1

#include "BasicDeviceSimulation.hh"
#include "EFieldModel.hh"

class TSpline;

namespace comptonsoft {

class DetectorHit;


/**
 * A class for simulating a semiconductor detector device.
 * This class provides functions to convert a deposited energy of a hit into a detector output such as an electrical pulse height, taking into account of a detector response.
 * Since the class also provides interface to such functions, you can derive a new specific class from this class to realize special treatment of detector response. 
 * @author Hirokazu Odaka
 * @date 2008-07-xx
 * @date 2008-08-26
 * @date 2008-09-17
 * @date 2008-11-11
 * @date 2009-05-20 
 * @date 2009-06-01 
 * @date 2009-07-09 
 * @date 2009-10-26 
 * @date 2012-07-01
 */

class DeviceSimulation : public BasicDeviceSimulation
{
public:
  DeviceSimulation();
  virtual ~DeviceSimulation();

public:
  void setSimPHAMode(int val) { m_SimPHAMode = val; }
  int SimPHAMode() { return m_SimPHAMode; }

  void setCCEMapName(std::string val) { m_CCEMapName = val; }
  std::string CCEMapName() { return m_CCEMapName; }

  void setUpsideAnode(bool val=true) { m_EField->setUpsideAnode(val); }
  bool UpsideAnode() { return m_EField->UpsideAnode(); }
  void setUpsideCathode(bool val=true) { m_EField->setUpsideCathode(val); }
  bool UpsideCathode() { return m_EField->UpsideCathode(); }

  void setMuTau(double mte, double mth) { m_EField->setMuTau(mte, mth); }
  void setMuTauElectron(double mte) { m_EField->setMuTauElectron(mte); };
  void setMuTauHole(double mth) { m_EField->setMuTauHole(mth); };
  double MuTauElectron() { return m_EField->MuTauElectron(); }
  double MuTauHole() { return m_EField->MuTauHole(); }

  bool setEField(double bias, int mode,
                 double p0=0., double p1=0., double p2=0., double p3=0.)
  {
    m_EField->setThickness(DetectorThickness());
    return m_EField->setEField(bias, mode, p0, p1, p2, p3);
  }
  double BiasVoltage() const { return m_EField->BiasVoltage(); }
  int EFieldMode() const { return m_EField->EFieldMode(); }
  double EFieldParam(int i) const { return m_EField->EFieldParam(i); }

  void setNumPixelInWPCalculation(double val)
  { m_NumPixelInWPCalculation = val; }
  double NumPixelInWPCalculation()
  { return m_NumPixelInWPCalculation; }

  void setSimDiffusionMode(int val) { m_SimDiffusionMode = val; }
  int SimDiffusionMode() { return m_SimDiffusionMode; }

  void setDiffusionDivisionNumber(int val)
  { m_DiffusionDivisionNumber = val; }
  int DiffusionDivisionNumber()
  { return m_DiffusionDivisionNumber; }

  void setDiffusionSigmaConstant(double sigmaAnode, double sigmaCathode)
  {
    m_DiffusionSigmaConstantAnode = sigmaAnode;
    m_DiffusionSigmaConstantCathode = sigmaCathode;
  }
  void setDiffusionSigmaConstantAnode(double val)
  { m_DiffusionSigmaConstantAnode = val; }
  void setDiffusionSigmaConstantCathode(double val)
  { m_DiffusionSigmaConstantCathode = val; }
  double DiffusionSigmaConstantAnode()
  { return m_DiffusionSigmaConstantAnode; }
  double DiffusionSigmaConstantCathode()
  { return m_DiffusionSigmaConstantCathode; }

  void setDiffusionSpreadFactor(double spreadFactorAnode,
                                double spreadFactorCathode)
  {
    m_DiffusionSpreadFactorAnode = spreadFactorAnode;
    m_DiffusionSpreadFactorCathode = spreadFactorCathode;
  }
  void setDiffusionSpreadFactorAnode(double val)
  { m_DiffusionSpreadFactorAnode = val; }
  void setDiffusionSpreadFactorCathode(double val)
  { m_DiffusionSpreadFactorCathode = val; }
  double DiffusionSpreadFactorAnode()
  { return m_DiffusionSpreadFactorAnode; }
  double DiffusionSpreadFactorCathode()
  { return m_DiffusionSpreadFactorCathode; }

  void initializeTable();

  void setGainCorrectionFunction(const StripPair& sp, const TSpline* val);
  void setGainCorrectionFunction(int chip, int channel, const TSpline* val);
  const TSpline* GainCorrectionFunction(const StripPair& sp);
  const TSpline* GainCorrectionFunction(int chip, int channel);
  void setGainCorrectionFunctionAll(const TSpline* val);

  void setGainCorrectionFactor(const StripPair& sp, double val);
  void setGainCorrectionFactor(int chip, int channel, double val);
  double GainCorrectionFactor(const StripPair& sp);
  double GainCorrectionFactor(int chip, int channel);
  void setGainCorrectionFactorAll(double val);

  double correctGain(const StripPair& sp, double ePI);
  double correctGain(int chip, int channel, double ePI);

  virtual double ChargeCollectionEfficiency(const StripPair& /* sp */,
                                            double /* x */,
                                            double /* y */,
                                            double /* z */)
  {
    return 1.0;
  }

  double ChargeCollectionEfficiencyByHecht(double z)
  { return m_EField->CCEByHecht(z); }
  virtual double calculatePHA(const StripPair& sp,
                              double edep, double x, double y, double z);
  virtual double DiffusionSigmaAnode(double z);
  virtual double DiffusionSigmaCathode(double z);
  virtual double calculatePI(double pha, const StripPair& sp);

  virtual bool isCCEMapPrepared() { return false; }
  virtual void buildWPMap() {}
  virtual void buildWPMap(int /* nx */, int /* ny */, int /* nz */,
                          double /* num_px */) {}
  virtual void buildCCEMap() {}
  virtual void buildCCEMap(int /* nx */, int /* ny */, int /* nz */,
                           double /* num_px */) {}

  virtual void printSimParam();

protected:
  void setWPForCCECalculation(bool upside_electrode, boost::shared_array<double> wp, int num_point)
  {
    m_EField->setUpsideReadElectrode(upside_electrode);
    m_EField->setWP(wp, num_point);
  }

  double calculateCCE(double z)
  { return m_EField->CCE(z); }

  virtual void simulatePulseHeights() = 0;

private:
  void simulateDetectorHits();

private:
  int m_SimPHAMode;
  std::string m_CCEMapName;
  EFieldModel* m_EField;
  double m_NumPixelInWPCalculation;

  int m_SimDiffusionMode;
  int m_DiffusionDivisionNumber;
  double m_DiffusionSigmaConstantAnode;
  double m_DiffusionSigmaConstantCathode;
  double m_DiffusionSpreadFactorAnode;
  double m_DiffusionSpreadFactorCathode;

  std::vector<const TSpline*> m_GainCorrectionFunction;
  std::vector<double> m_GainCorrectionFactor;
};


inline void DeviceSimulation::setGainCorrectionFunction(const StripPair& sp, const TSpline* val)
{
  if (!isTableInitialized()) initializeTable();
  
  if (RangeCheck(sp)) {
    m_GainCorrectionFunction[IndexOfTable(sp)] = val;
  }
}


inline void DeviceSimulation::setGainCorrectionFunction(int chip, int channel, const TSpline* val)
{ 
  setGainCorrectionFunction(Channel2Strip(chip,channel), val);
}


inline const TSpline* DeviceSimulation::GainCorrectionFunction(const StripPair& sp)
{
  if (RangeCheck(sp)) {
    return m_GainCorrectionFunction[IndexOfTable(sp)];
  }
  else {
    return 0;
  }
}


inline const TSpline* DeviceSimulation::GainCorrectionFunction(int chip, int channel)
{
  return GainCorrectionFunction(Channel2Strip(chip,channel));
}


inline void DeviceSimulation::setGainCorrectionFactor(const StripPair& sp, double val)
{
  if (!isTableInitialized()) initializeTable();
  
  if (RangeCheck(sp)) {
    m_GainCorrectionFactor[IndexOfTable(sp)] = val;
  }
}


inline void DeviceSimulation::setGainCorrectionFactor(int chip, int channel, double val)
{ 
  setGainCorrectionFactor(Channel2Strip(chip,channel), val);
}


inline double DeviceSimulation::GainCorrectionFactor(const StripPair& sp)
{
  if (RangeCheck(sp)) {
    return m_GainCorrectionFactor[IndexOfTable(sp)];
  }
  else {
    return 1;
  }
}


inline double DeviceSimulation::GainCorrectionFactor(int chip, int channel)
{
  return GainCorrectionFactor(Channel2Strip(chip,channel));
}


inline double DeviceSimulation::correctGain(int chip, int channel, double ePI)
{
  return correctGain(Channel2Strip(chip,channel), ePI);
}

}

#endif /* COMPTONSOFT_DeviceSimulation_H */
