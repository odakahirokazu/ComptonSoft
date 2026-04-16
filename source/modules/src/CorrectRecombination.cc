#include "CorrectRecombination.hh"
#include "DetectorHit.hh"
#include "DeviceSimulation.hh"
using namespace anlnext;
namespace comptonsoft {
CorrectRecombination::CorrectRecombination() = default;
CorrectRecombination::~CorrectRecombination() = default;
ANLStatus CorrectRecombination::mod_define() {
  return AS_OK;
}
ANLStatus CorrectRecombination::mod_initialize() {
  VCSModule::mod_initialize();
  get_module_NC("CSHitCollection", &m_HitCollection);
  return AS_OK;
}
ANLStatus CorrectRecombination::mod_analyze() {
  DetectorSystem* detectorManager = getDetectorManager();

  const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
  for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
    std::vector<DetectorHit_sptr>& hits = m_HitCollection->getHits(timeGroup);
    for (DetectorHit_sptr& hit: hits) {
      const int detectorID = hit->DetectorID();
      VRealDetectorUnit* d = detectorManager->getDetectorByID(detectorID);
      d->applyRecombinationCorrection(hit);
    }
  }
  
  return AS_OK;
}
} // namespace comptonsoft