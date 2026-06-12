#include "EliminateLowEnergyHits.hh"
#include "DetectorHit.hh"
using namespace anlnext;

namespace comptonsoft {
  EliminateLowEnergyHits::EliminateLowEnergyHits() : BasicModule(), m_HitCollection(nullptr) {}
  EliminateLowEnergyHits::~EliminateLowEnergyHits() = default; 
  EliminateLowEnergyHits::EliminateLowEnergyHits(const EliminateLowEnergyHits &r) = default;
  
  ANLStatus EliminateLowEnergyHits::mod_define() {
    define_parameter("energy_threshold", &mod_class::m_EnergyThreshold_keV, 1.0, "keV");
    return AS_OK;
  }
  ANLStatus EliminateLowEnergyHits::mod_initialize() {
    get_module_NC("CSHitCollection", &m_HitCollection);
    m_EnergyThreshold = m_EnergyThreshold_keV * CLHEP::keV;
    return AS_OK;
  }
  ANLStatus EliminateLowEnergyHits::mod_analyze() {
    const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
    for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
      std::vector<DetectorHit_sptr>& hits = m_HitCollection->getHits(timeGroup);
      hits.erase(std::remove_if(hits.begin(), hits.end(),
                                [this](const DetectorHit_sptr& hit) { return hit->EPI() < m_EnergyThreshold; }),
                 hits.end());
    }
    return AS_OK;
  }
} // namespace comptonsoft