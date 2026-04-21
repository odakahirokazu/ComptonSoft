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
ANLStatus CorrectRecombination::mod_begin_run(){
  DetectorSystem* detectorManager = getDetectorManager();
  const int numDetectors = detectorManager->NumberOfDetectors();
  m_detectorHits.resize(numDetectors);
  for (int i = 0; i < numDetectors; ++i) {
    m_detectorHits[i].clear();
  }
  return AS_OK;
}
ANLStatus CorrectRecombination::mod_analyze() {
  DetectorSystem* detectorManager = getDetectorManager();

  const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
  int sz = m_detectorHits.size();
  for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
    std::vector<DetectorHit_sptr>& hits = m_HitCollection->getHits(timeGroup);
    for (DetectorHit_sptr &hit: hits) {
      const int detectorID = hit->DetectorID();
      if (detectorID >= sz) {
        m_detectorHits.resize(detectorID + 1);
        sz = m_detectorHits.size();
      }
      m_detectorHits[detectorID].push_back(hit);
    }
  }
  for (int detectorID = 0; detectorID < sz; ++detectorID) {
    VRealDetectorUnit *detector = detectorManager->getDetectorByID(detectorID);
    if (!detector) {
      continue;
    }
    if (m_detectorHits[detectorID].empty()) {
      continue;
    }
    detector->applyRecombinationCorrection(m_detectorHits[detectorID]);
    m_detectorHits[detectorID].clear();
  }
  return AS_OK;
}
} // namespace comptonsoft