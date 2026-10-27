#include "VLArRecombinationModel.hh"
#include "TMath.h"
#include "TRandom3.h"
#include <algorithm>

namespace comptonsoft {
VLArRecombinationModel::VLArRecombinationModel() = default;
VLArRecombinationModel::VLArRecombinationModel(const std::string& name)
  : name_(name)
{
  seed_ = std::random_device()();
  rng_ = std::mt19937(seed_);
}
VLArRecombinationModel::~VLArRecombinationModel() = default;

VLArRecombinationModel::VLArRecombinationModel(const std::string& name, const std::map<std::string, double>& params)
  : name_(name),
    parameters_(params)
{
  seed_ = std::random_device()();
  rng_ = std::mt19937(seed_);
  auto it = params.find("Wion");
  if (it != params.end()) {
    Wion_ = it->second * CLHEP::eV;
  }
  it = params.find("Wexc");
  if (it != params.end()) {
    Wexc_ = it->second * CLHEP::eV;
  }
  it = params.find("rho");
  if (it != params.end()) {
    rho_ = it->second * (CLHEP::g / CLHEP::cm3);
  }
  it = params.find("FanoFactor");
  if (it != params.end()) {
    fanoFactor_ = it->second;
  }
  it = params.find("RandomizeMode");
  if (it != params.end()) {
    randomizeMode_ = static_cast<int>(it->second);
  }
  it = params.find("SigmaP");
  if (it != params.end()) {
    sigmaP_ = it->second;
  }
  it = params.find("Seed");
  if (it != params.end()) {
    seed_ = static_cast<uint32_t>(it->second);
    rng_.seed(seed_);
  }
}

void VLArRecombinationModel::printInfo(std::ostream& os) const
{
  os << "Recombination Model\n"
     << "  Name: " << name() << '\n'
     << "  Wion: " << Wion_ / (CLHEP::eV) << " eV\n"
     << "  Wexc: " << Wexc_ / (CLHEP::eV) << " eV\n"
     << "  Density: " << rho_ / (CLHEP::g / CLHEP::cm3) << " g/cm3\n"
     << "  Randomize Mode: " << randomizeMode_ << "\n";
  if (randomizeMode_ > 0) {
    os << "    Seed: " << seed_ << "\n";
  }
  if (randomizeMode_ == 1) {
    os << "    Fano Factor: " << fanoFactor_ << "\n";
  }
  if (randomizeMode_ == 2) {
    os << "    Sigma_p: " << sigmaP_ << "\n";
  }
}

RecombinationResult VLArRecombinationModel::calcResultWithoutRandomization(double edep, double p) const
{
  const double nQuanta = edep / Wexc(); // number of quanta
  RecombinationResult result;
  result.recombinationSurvivingRate = p;
  result.totalQuanta = nQuanta;
  result.electronYield = edep * p / Wion();
  result.electronEnergy = edep * p;
  result.photonYield = std::max(result.totalQuanta - result.electronYield, 0);
  return result;
}

RecombinationResult VLArRecombinationModel::calculateRecombination(double energy, double dx, double electricField,
                                                                   double dedx)
{
  if (dedx < 0) {
    dedx = energy / dx;
  }
  double p = recombinationRate(dedx, electricField);
  if (p < 0.0) {
    p = 0;
  }
  else if (p > 1.0) {
    p = 1.0;
  }

#ifdef VERBOSE
  std::cout << name() << "::calculateRecombination: dedx=" << dedx / (CLHEP::MeV / CLHEP::cm)
            << "MeV/cm, electricField=" << electricField / (CLHEP::kilovolt / CLHEP::cm) << " kV/cm, rate = " << p
            << std::endl;
#endif
  RecombinationResult result;
  if (RandomizeMode() == 0) {
    result = calcResultWithoutRandomization(energy, p);
    // std::cout << "recombination rate: " << p << ", totalQuanta: " << nQuanta << ", electronYield: " <<
    // result.electronYield << ", electron energy: " << result.electronEnergy / CLHEP::keV  << ", photon yield: " <<
    // result.photonYield << ", edep: " << energy << std::endl;
    return result;
  }
  else if (RandomizeMode() == 1) {
    // Binomial randomization
    const double nQuanta = energy / Wexc(); // number of quanta
#ifdef VERBOSE
    std::cout << "energy: " << energy / CLHEP::keV << "keV nQuanta: " << nQuanta << std::endl;
#endif
    int nQuantaWithFluctuations =
        TMath::Nint(gRandom->Gaus(nQuanta, TMath::Sqrt(TMath::Max(0.0, nQuanta * FanoFactor()))));
    if (nQuantaWithFluctuations < 0) {
      nQuantaWithFluctuations = 0;
    }
    result.totalQuanta = nQuantaWithFluctuations;
    const double distribution_ratio = 1. / (1. + IonExitonRatio());
    int nElectronInt = Binomial(nQuantaWithFluctuations, distribution_ratio * p);
#ifdef VERBOSE
    std::cout << "Nq: " << nQuantaWithFluctuations << "\nNelectron: " << nElectronInt << std::endl;
#endif
    if (nElectronInt < 0.0) {
      nElectronInt = 0;
    }
    else if (nElectronInt > nQuantaWithFluctuations) {
      nElectronInt = nQuantaWithFluctuations;
    }
    result.electronYield = nElectronInt;
    result.electronEnergy = nElectronInt * Wion();
    result.photonYield = nQuantaWithFluctuations - nElectronInt;
    return result;
  }
  else if (RandomizeMode() == 2) {
    const double sigma_p = SigmaP();
    const int nIon = TMath::Nint(energy / Wion());
    const int nQuanta = TMath::Nint(energy / Wexc());
    const double p_fluctuated = std::clamp(gRandom->Gaus(p, sigma_p), 0.0, 1.0);
    const int nElectronInt = std::clamp(Binomial(nIon, p_fluctuated), 0, nIon);
    result.electronYield = nElectronInt;
    result.electronEnergy = nElectronInt * Wion();
    result.photonYield = std::clamp(nQuanta - nElectronInt, 0, nQuanta);
    result.recombinationSurvivingRate = p_fluctuated;
    result.totalQuanta = nQuanta;
    return result;
  }
  else {
    // No randomization
    throw std::runtime_error("Invalid Randomization mode");
  }
}

int VLArRecombinationModel::Binomial(const int n, const double p)
{
  auto gen = std::binomial_distribution<>(n, p);
  return gen(rng_);
}
} /* namespace comptonsoft */