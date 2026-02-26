#include "VLArRecombinationModel.hh"
#include "TRandom3.h"
namespace comptonsoft {
VLArRecombinationModel::VLArRecombinationModel() = default;
VLArRecombinationModel::VLArRecombinationModel(const std::string &name) : name_(name){}
VLArRecombinationModel::~VLArRecombinationModel() = default;

VLArRecombinationModel::VLArRecombinationModel(const std::string &name, const std::map<std::string, double> &params) : name_(name) {
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
}

void VLArRecombinationModel::printInfo(std::ostream &os) const {
  os << "Recombination Model\n"
     << "  Name: " << name() << '\n'
     << "  Wion: " << Wion_ / (CLHEP::eV) << " eV\n"
     << "  Wexc: " << Wexc_ / (CLHEP::eV) << " eV\n"
     << "  Density: " << rho_ / (CLHEP::g / CLHEP::cm3) << " g/cm3\n"
     << "  Fano Factor: " << fanoFactor_ << "\n"
     << "  Randomize Mode: " << randomizeMode_ << "\n";
}

double VLArRecombinationModel::Binomial(const int n, double p) const {
  bool inverted = false;
  if (p > 0.5) {
    p = 1.0 - p;
    inverted = true;
  }
  const double lambda = n * p;
  const double nq = n * (1.0 - p);
  double T;
  if (n <= 200) {
    T = 20.0;
  }
  else if (n <= 2000) {
    T = 30.0;
  }
  else {
    T = 50.0;
  }

  if (lambda <= 5.0 && p <= 0.05) {
    const int nElectron = gRandom->Poisson(lambda);
    return inverted ? n - nElectron : nElectron;
  }
  else if (lambda >= T && nq >= T) {
    const double sigma = gRandom->Gaus(lambda, std::sqrt(lambda * (1.0 - p)));
    const int nElectron = static_cast<int>(std::lround(sigma));
    return inverted ? (n - std::clamp(nElectron, 0, n)) : std::clamp(nElectron, 0, n);
  }
  else {
    // Use binomial distribution directly
    const int nElectron = gRandom->Binomial(n, p);
    return inverted ? n - nElectron : nElectron;
  }
  return 0;
}
} /* namespace comptonsoft */