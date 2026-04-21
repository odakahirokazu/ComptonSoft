#include "NumHitsInDetectorFilter.hh"
#include "DetectorHit.hh"
using namespace anlnext;
namespace comptonsoft {

NumHitsInDetectorFilter::NumHitsInDetectorFilter() = default;
NumHitsInDetectorFilter::~NumHitsInDetectorFilter() = default;
ANLStatus NumHitsInDetectorFilter::mod_define() {
  define_parameter("num_hits_threshold", &mod_class::numHitsThreshold_);
  set_parameter_description("Threshold of the number of hits in a detector");
  return AS_OK;
}
ANLStatus NumHitsInDetectorFilter::mod_initialize() {
  get_module("CSHitCollection", &m_HitCollection);
  define_evs("NumHitsInDetectorFilter:OK");
  define_evs("NumHitsInDetectorFilter:NG");
  return AS_OK;
}
ANLStatus NumHitsInDetectorFilter::mod_analyze() {
  numHitsInDetector_.clear();
  const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
  for (int timeGroup = 0; timeGroup < NumTimeGroups; timeGroup++) {
    const std::vector<DetectorHit_sptr>& hits = m_HitCollection->getHits(timeGroup);
    for (const auto& hit: hits) {
      numHitsInDetector_[hit->DetectorID()]++;
    }
    for (const auto& pair: numHitsInDetector_) {
      if (pair.second > numHitsThreshold_) {
        set_evs("NumHitsInDetectorFilter:NG");
        return AS_SKIP;
      }
    }
  }
  set_evs("NumHitsInDetectorFilter:OK");
  return AS_OK;
}
} // namespace comptonsoft