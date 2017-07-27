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

#ifndef ANLGEANT4_BasicPrimaryGen_H
#define ANLGEANT4_BasicPrimaryGen_H 1

#include "VANLPrimaryGen.hh"
#include "InitialInformation.hh"
#include "G4ThreeVector.hh"

class G4ParticleDefinition;

namespace anlgeant4 {

class BasicPrimaryGeneratorAction;
class VANLGeometry;


/**
 * ANLGeant4 PrimaryGen module
 *
 * @author Hirokazu Odaka
 * @date 2011-04-11
 * @date 2012-07-04 | Hirokazu Odaka | sampleEnergy(), printSpectralInfo() as virtual
 * @date 2012-07-10 | Hirokazu Odaka | virtual methods: sampleDirection(), samplePosition()
 * @date 2014-12-15 | Hirokazu Odaka | histogram spectral distribution
 * @date 2017-07-27 | Hirokazu Odaka | this can transfer its setting function to the generator action, introducing makePrimarySetting().
 * @date 2017-07-03 | 4.2 | Hirokazu Odaka | length unit is fixed to cm
 */
class BasicPrimaryGen : public VANLPrimaryGen, public InitialInformation
{
  DEFINE_ANL_MODULE(BasicPrimaryGen, 4.1);
public:
  enum class SpectralShape {
    Undefined, Mono, PowerLaw, Gaussian, BlackBody, Histogram, User,
  };

  BasicPrimaryGen();
  ~BasicPrimaryGen();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;

  G4VUserPrimaryGeneratorAction* create() override;
  
  G4int Number() const { return number_; }
  double TotalEnergy() const { return totalEnergy_; }

  void setPolarizationMode(int v) { polarizationMode_ = v; }
  int PolarizationMode() const { return polarizationMode_; }
  
  virtual void makePrimarySetting() = 0;
  virtual void confirmPrimarySetting();

protected:
  void setPrimary(double time0,
                  G4ThreeVector position,
                  double energy,
                  G4ThreeVector direction,
                  G4ThreeVector polarization)
  {
    time_ = time0;
    position_ = position;
    energy_ = energy;
    direction_ = direction;
    polarization_ = polarization;
  }

  void setPrimary(double time0,
                  G4ThreeVector position,
                  double energy,
                  G4ThreeVector direction)
  {
    time_ = time0;
    position_ = position;
    energy_ = energy;
    direction_ = direction;
  }

  void setPrimary(G4ThreeVector position,
                  double energy,
                  G4ThreeVector direction,
                  G4ThreeVector polarization)
  {
    position_ = position;
    energy_ = energy;
    direction_ = direction;
    polarization_ = polarization;
  }

  void setPrimary(G4ThreeVector position,
                  double energy,
                  G4ThreeVector direction)
  {
    position_ = position;
    energy_ = energy;
    direction_ = direction;
  }

  void setTime(double time0) { time_ = time0; }
  void setPosition(G4ThreeVector position) { position_ = position; }
  void setEnergy(double energy) { energy_ = energy; }
  void setDirection(G4ThreeVector direction) { direction_ = direction; }
  void setPolarization(G4ThreeVector polarization) { polarization_ = polarization; }

  void setDefinition(G4ParticleDefinition* def);
  void setParticleName(const std::string& name) { particleName_ = name; }

  void setEnergyDistribution(SpectralShape v, const std::string& name)
  {
    energyDistribution_ = v;
    energyDistributionName_ = name;
  }
  
  void enablePowerLawInput();
  void enableGaussianInput();
  void enableBlackBodyInput();
  void enableHistogramInput();
  void disableDefaultEnergyInput();

  void buildSpectrumPhotonIntegral();

  virtual void printSpectralInfo();

  /**
   * Sample a value of energy from energy distribution of the primaries.
   * Since this methods is virtual, you can override its implimentation.
   * By default, it samples an energy from a power-law distribution
   * specified by registered parameters.
   *
   * @return sampled energy
   */
  virtual double sampleEnergy();
  double sampleFromPowerLaw(double gamma, double e0, double e1);
  double sampleFromPowerLaw();
  double sampleFromGaussian(double mean, double sigma);
  double sampleFromGaussian();
  double sampleFromBlackBody(double kT, double upper_limit_factor);
  double sampleFromBlackBody();
  double sampleFromHistogram();
  
  virtual G4ThreeVector sampleDirection() { return direction_; }
  virtual G4ThreeVector samplePosition() { return position_; }

  void setUnpolarized();

  double LengthUnit() const;
  std::string LengthUnitName() const;
  
private:
  BasicPrimaryGeneratorAction* primaryGenerator_ = nullptr;
  const anlgeant4::VANLGeometry* geometry_ = nullptr;

  std::string particleName_;

  double time_;
  G4ThreeVector position_;
  double energy_;
  G4ThreeVector direction_;
  G4ThreeVector polarization_;

  G4int number_;
  double totalEnergy_;

  G4ParticleDefinition* definition_;

  G4int polarizationMode_;

  /* properties for the energy distribution */
  std::string energyDistributionName_;
  SpectralShape energyDistribution_;
  double energyMin_;
  double energyMax_;
  // power-law distribution
  double photonIndex_;
  // Gaussin distribution
  double energyMean_;
  double energySigma_;
  // black-body distribution
  double kT_;
  // histogram
  std::vector<double> spectrumEnergy_;
  std::vector<double> spectrumPhotons_;
  std::vector<double> spectrumPhotonIntegral_;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_BasicPrimaryGen_H */
