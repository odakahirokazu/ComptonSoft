#include "VLArRecombinationModel.hh"
namespace comptonsoft {
VLArRecombinationModel::VLArRecombinationModel() = default;
VLArRecombinationModel::VLArRecombinationModel(const std::string &name, unsigned int seed) : name_(name), rng_(std::mt19937(seed)), seed_(seed) {}
VLArRecombinationModel::~VLArRecombinationModel() = default;

VLArRecombinationModel::VLArRecombinationModel(const std::string &name, const std::map<std::string, double> &params, unsigned int seed) : name_(name), rng_(std::mt19937(seed)), seed_(seed) {
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
     << "  Randomize Mode: " << randomizeMode_ << "\n"
     << "  Random Number Generator Seed: " << seed_ << "\n";
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
    std::poisson_distribution<int> pois(lambda);
    const int nElectron = pois(rng_);
    return inverted ? n - nElectron : nElectron;
  }
  else if (lambda >= T && nq >= T) {
    std::normal_distribution<double> norm(lambda, std::sqrt(lambda * (1.0 - p)));
    const int nElectron = static_cast<int>(std::lround(norm(rng_)));
    return inverted ? (n - std::clamp(nElectron, 0, n)) : std::clamp(nElectron, 0, n);
  }
  else {
    // Use binomial distribution directly
    std::binomial_distribution<int> binom(n, p);
    const int nElectron = binom(rng_);
    return inverted ? n - nElectron : nElectron;
  }
  return 0;
}
} /* namespace comptonsoft */