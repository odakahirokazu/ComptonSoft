#include "VLArRecombinationModel.hh"
namespace comptonsoft {
VLArRecombinationModel::VLArRecombinationModel() = default;
VLArRecombinationModel::VLArRecombinationModel(const std::string &name) : name_(name) {
  seed_ = std::random_device()();
  rng_ = std::mt19937(seed_);
}
VLArRecombinationModel::~VLArRecombinationModel() = default;

VLArRecombinationModel::VLArRecombinationModel(const std::string &name, const std::map<std::string, double> &params) : name_(name) {
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
  it = params.find("Seed");
  if (it != params.end()) {
    seed_ = static_cast<uint32_t>(it->second);
    rng_.seed(seed_);
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
     << "  Seed: " << seed_ << "\n";
}

int VLArRecombinationModel::Binomial(const int n, const double p) {
  auto gen = std::binomial_distribution<>(n, p);
  return gen(rng_);
}
} /* namespace comptonsoft */