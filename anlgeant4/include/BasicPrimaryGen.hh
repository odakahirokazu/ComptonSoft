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

#include "globals.hh"
#include "G4ThreeVector.hh"

#include "StandardPickUpData.hh"

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
 */
class BasicPrimaryGen : public VANLPrimaryGen, public InitialInformation
{
  DEFINE_ANL_MODULE(BasicPrimaryGen, 1.3);
public:
  enum SpectralShape {
    User = 0,
    Mono = 1,
    PowerLaw = 2,
    Gaussian = 3,
    BlackBody = 4,
    Undefined = -1,
  };

  BasicPrimaryGen();
  
  virtual anl::ANLStatus mod_startup();
  virtual anl::ANLStatus mod_prepare();
  virtual anl::ANLStatus mod_init();
  virtual anl::ANLStatus mod_bgnrun();
  virtual anl::ANLStatus mod_ana();
  
  G4VUserPrimaryGeneratorAction* create();
  
  G4int Number() const { return m_Number; }
  G4double TotalEnergy() const { return m_TotalEnergy; }

  void setPolarizationMode(int v) { m_PolarizationMode = v; }
  int PolarizationMode() const { return m_PolarizationMode; }
  
protected:
  void setPrimary(G4double time0,
                  G4ThreeVector position,
                  G4double energy,
                  G4ThreeVector direction,
                  G4ThreeVector polarization)
  {
    m_Time = time0;
    m_Position = position;
    m_Energy = energy;
    m_Direction = direction;
    m_Polarization = polarization;
  }

  void setPrimary(G4double time0,
                  G4ThreeVector position,
                  G4double energy,
                  G4ThreeVector direction)
  {
    m_Time = time0;
    m_Position = position;
    m_Energy = energy;
    m_Direction = direction;
  }

  void setPrimary(G4ThreeVector position,
                  G4double energy,
                  G4ThreeVector direction,
                  G4ThreeVector polarization)
  {
    m_Position = position;
    m_Energy = energy;
    m_Direction = direction;
    m_Polarization = polarization;
  }

  void setPrimary(G4ThreeVector position,
                  G4double energy,
                  G4ThreeVector direction)
  {
    m_Position = position;
    m_Energy = energy;
    m_Direction = direction;
  }

  void setTime(G4double time0) { m_Time = time0; }
  void setPosition(G4ThreeVector position) { m_Position = position; }
  void setEnergy(G4double energy) { m_Energy = energy; }
  void setDirection(G4ThreeVector direction) { m_Direction = direction; }
  void setPolarization(G4ThreeVector polarization) { m_Polarization = polarization; }

  void setDefinition(G4ParticleDefinition* def);
  void setParticleName(const std::string& name) { m_ParticleName = name; }

  void setEnergyDistribution(SpectralShape v, const std::string& name)
  {
    m_EnergyDistribution = v;
    m_EnergyDistributionName = name;
  }
  
  void enablePowerLawInput();
  void enableGaussianInput();
  void enableBlackBodyInput();
  void disableDefaultEnergyInput();

  virtual void printSpectralInfo();

  /**
   * Sample a value of energy from energy distribution of the primaries.
   * Since this methods is virtual, you can override its implimentation.
   * By default, it samples an energy from a power-law distribution
   * specified by registered parameters.
   *
   * @return sampled energy
   */
  virtual G4double sampleEnergy();
  G4double sampleFromPowerLaw(double gamma, double e0, double e1);
  G4double sampleFromPowerLaw();
  G4double sampleFromGaussian(double mean, double sigma);
  G4double sampleFromGaussian();
  G4double sampleFromBlackBody(double kT, double upper_limit_factor);
  G4double sampleFromBlackBody();

  virtual G4ThreeVector sampleDirection() { return m_Direction; }
  virtual G4ThreeVector samplePosition() { return m_Position; }

  void setUnpolarized();

  double LengthUnit() const;
  std::string LengthUnitName() const;
  
private:
  BasicPrimaryGeneratorAction* m_PrimaryGen;
  const anlgeant4::StandardPickUpData* m_PickUpData;
  const anlgeant4::VANLGeometry* m_Geometry;

  std::string m_ParticleName;

  G4double m_Time;
  G4ThreeVector m_Position;
  G4double m_Energy;
  G4ThreeVector m_Direction;
  G4ThreeVector m_Polarization;  

  G4int m_Number;
  G4double m_TotalEnergy;

  G4ParticleDefinition* m_Definition;

  G4int m_PolarizationMode;

  /* properties for the energy distribution */
  std::string m_EnergyDistributionName;
  SpectralShape m_EnergyDistribution;
  G4double m_EnergyMin;
  G4double m_EnergyMax;
  // power-law distribution
  G4double m_PhotonIndex;
  // Gaussin distribution
  G4double m_EnergyMean;
  G4double m_EnergySigma;
  // black-body distribution
  G4double m_KT;
};

}

#endif /* ANLGEANT4_BasicPrimaryGen_H */
