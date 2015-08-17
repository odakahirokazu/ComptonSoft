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

#include "VDeviceSimulation.hh"
#include <memory>
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
 * @date 2012-07-01
 * @date 2014-11-07
 * @date 2015-05-14 | EPI compensation.
 */
class DeviceSimulation : public VDeviceSimulation
{
public:
  DeviceSimulation();
  virtual ~DeviceSimulation();

public:
  void setSimPHAMode(int val) { simPHAMode_ = val; }
  int SimPHAMode() const { return simPHAMode_; }

  void setCCEMapName(std::string val) { CCEMapName_ = val; }
  std::string CCEMapName() const { return CCEMapName_; }

  void setMuTau(double mte, double mth) { EField_->setMuTau(mte, mth); }
  void setMuTauElectron(double mte) { EField_->setMuTauElectron(mte); };
  void setMuTauHole(double mth) { EField_->setMuTauHole(mth); };
  double MuTauElectron() const { return EField_->MuTauElectron(); }
  double MuTauHole() const { return EField_->MuTauHole(); }

  bool setEField(double bias, EFieldModel::FieldShape mode,
                 double p0=0., double p1=0., double p2=0., double p3=0.)
  {
    EField_->setThickness(getThickness());
    EField_->setUpSideAnode(isUpSideAnode());
    return EField_->setEField(bias, mode, p0, p1, p2, p3);
  }
  double BiasVoltage() const { return EField_->BiasVoltage(); }
  EFieldModel::FieldShape EFieldMode() const { return EField_->EFieldMode(); }
  double EFieldParam(int i) const { return EField_->EFieldParam(i); }

  void setNumPixelsInWPCalculation(double val)
  { numPixelsInWPCalculation_ = val; }
  double NumPixelsInWPCalculation() const
  { return numPixelsInWPCalculation_; }

  void setSimDiffusionMode(int val) { simDiffusionMode_ = val; }
  int SimDiffusionMode() const { return simDiffusionMode_; }

  void setDiffusionDivisionNumber(int val)
  { diffusionDivisionNumber_ = val; }
  int DiffusionDivisionNumber() const
  { return diffusionDivisionNumber_; }

  void setDiffusionSigmaConstant(double sigmaAnode, double sigmaCathode)
  {
    diffusionSigmaConstantAnode_ = sigmaAnode;
    diffusionSigmaConstantCathode_ = sigmaCathode;
  }
  void setDiffusionSigmaConstantAnode(double val)
  { diffusionSigmaConstantAnode_ = val; }
  void setDiffusionSigmaConstantCathode(double val)
  { diffusionSigmaConstantCathode_ = val; }
  double DiffusionSigmaConstantAnode() const
  { return diffusionSigmaConstantAnode_; }
  double DiffusionSigmaConstantCathode() const
  { return diffusionSigmaConstantCathode_; }

  void setDiffusionSpreadFactor(double spreadFactorAnode,
                                double spreadFactorCathode)
  {
    diffusionSpreadFactorAnode_ = spreadFactorAnode;
    diffusionSpreadFactorCathode_ = spreadFactorCathode;
  }
  void setDiffusionSpreadFactorAnode(double val)
  { diffusionSpreadFactorAnode_ = val; }
  void setDiffusionSpreadFactorCathode(double val)
  { diffusionSpreadFactorCathode_ = val; }
  double DiffusionSpreadFactorAnode() const
  { return diffusionSpreadFactorAnode_; }
  double DiffusionSpreadFactorCathode() const
  { return diffusionSpreadFactorCathode_; }

  void setTemperature(double v) { temperature_ = v; }
  double Temperature() const { return temperature_; }

  void initializeTables();

  template <typename IndexType>
  void setEPICompensationFunction(IndexType i, const TSpline* v)
  { setTableValue(&DeviceSimulation::EPICompensationFunctionVector_, i, v); }

  template <typename SelectorType>
  void setEPICompensationFunctionToSelected(const TSpline* v, SelectorType selector)
  { setTableValueToSelected(&DeviceSimulation::EPICompensationFunctionVector_, v, selector); }

  template <typename IndexType>
  const TSpline* getEPICompensationFunction(IndexType i) const
  { return getTableValue(&DeviceSimulation::EPICompensationFunctionVector_, i); }

  void resetEPICompensationFunctionVector(const TSpline* v)
  { resetTable(&DeviceSimulation::EPICompensationFunctionVector_, v); }

  template <typename IndexType>
  void setEPICompensationFactor(IndexType i, double v)
  { setTableValue(&DeviceSimulation::EPICompensationFactorVector_, i, v); }

  template <typename SelectorType>
  void setEPICompensationFactorToSelected(double v, SelectorType selector)
  { setTableValueToSelected(&DeviceSimulation::EPICompensationFactorVector_, v, selector); }

  template <typename IndexType>
  double getEPICompensationFactor(IndexType i) const
  { return getTableValue(&DeviceSimulation::EPICompensationFactorVector_, i); }

  void resetEPICompensationFactorVector(double v)
  { resetTable(&DeviceSimulation::EPICompensationFactorVector_, v); }

  double compensateEPI(const PixelID& sp, double ePI) const;
  double compensateEPI(int chip, int channel, double ePI) const;

  virtual double ChargeCollectionEfficiency(const PixelID& /* sp */,
                                            double /* x */,
                                            double /* y */,
                                            double /* z */) const
  {
    return 1.0;
  }

  double ChargeCollectionEfficiencyByHecht(double z) const
  { return EField_->CCEByHecht(z); }
  
  virtual double calculateEnergyCharge(const PixelID& pixel,
                                       double energyDeposit,
                                       double x, double y, double z) const;
  virtual double calculateEPI(double energyCharge, const PixelID& pixel) const;

  virtual double DiffusionSigmaAnode(double z);
  virtual double DiffusionSigmaCathode(double z);

  virtual bool isCCEMapPrepared() { return false; }
  virtual void buildWPMap() {}
  virtual void buildWPMap(int /* nx */, int /* ny */, int /* nz */,
                          double /* num_px */) {}
  virtual void buildCCEMap() {}
  virtual void buildCCEMap(int /* nx */, int /* ny */, int /* nz */,
                           double /* num_px */) {}

  virtual void printSimulationParameters();

protected:
  void setWeightingPotential(bool upside_electrode, boost::shared_array<double> wp, int num_point)
  {
    EField_->setUpSideReadElectrode(upside_electrode);
    EField_->setWeightingPotential(wp, num_point);
  }

  double calculateCCE(double z)
  { return EField_->CCE(z); }

private:
  int simPHAMode_;
  std::string CCEMapName_;
  std::unique_ptr<EFieldModel> EField_;
  double numPixelsInWPCalculation_;

  int simDiffusionMode_;
  int diffusionDivisionNumber_;
  double diffusionSigmaConstantAnode_;
  double diffusionSigmaConstantCathode_;
  double diffusionSpreadFactorAnode_;
  double diffusionSpreadFactorCathode_;
  double temperature_;

  std::vector<const TSpline*> EPICompensationFunctionVector_;
  std::vector<double> EPICompensationFactorVector_;
};

inline
double DeviceSimulation::compensateEPI(int group, int channel, double ePI) const
{
  return compensateEPI(ChannelToPixel(group,channel), ePI);
}

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_DeviceSimulation_H */
