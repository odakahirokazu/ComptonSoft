#include "SetEnergy.hh"
#include "DetectorHit.hh"
using namespace anlnext;
namespace comptonsoft {
SetEnergy::SetEnergy(): sourceStr_("epi"), source_(SourceType::EPI) {}
SetEnergy::~SetEnergy() = default;

ANLStatus SetEnergy::mod_define() {
  define_parameter("source", &mod_class::sourceStr_);
  return AS_OK;
}
ANLStatus SetEnergy::mod_initialize() {
  get_module_NC("CSHitCollection", &m_HitCollection);
    if (sourceStr_ == "epi") {
      source_ = SourceType::EPI;
    }
    else if (sourceStr_ == "edep") {
      source_ = SourceType::EDEP;
    }
    else if (sourceStr_ == "echarge") {
      source_ = SourceType::ECHARGE;
    }
    else {
      // default: use EPI
      source_ = SourceType::EPI;
    }
  return AS_OK;
}
ANLStatus SetEnergy::mod_analyze() {
  const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
  for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
    std::vector<DetectorHit_sptr>& hits = m_HitCollection->getHits(timeGroup);
    for (DetectorHit_sptr& hit: hits) {
      if (source_ == SourceType::EPI) {
        hit->setEnergy(hit->EPI());
        hit->setEnergyError(hit->EPIError());
      }
      else if (source_ == SourceType::EDEP) {
        hit->setEnergy(hit->EnergyDeposit());
        hit->setEnergyError(0.0);
      }
      else if (source_ == SourceType::ECHARGE) {
        hit->setEnergy(hit->EnergyCharge());
        hit->setEnergyError(0.0);
      }
      else {
        // default: use EPI
        hit->setEnergy(hit->EPI());
        hit->setEnergyError(hit->EPIError());
      }
    }
  }
  
  return AS_OK;
}
} // namespace comptonsoft