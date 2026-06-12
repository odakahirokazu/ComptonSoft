#include "EvaluateEventRateForNanoGRAMS.hh"
#include "CSHitCollection.hh"
#include "DetectorHit.hh"
#include <random>
#include "CLHEP/Units/SystemOfUnits.h"

using namespace anlnext;

namespace comptonsoft {
EvaluateEventRateForNanoGRAMS::EvaluateEventRateForNanoGRAMS()
  :hitCollection_(nullptr), timeGroup_(0), intensity_(1.0), deadTimeNoDetection_(500e-6), deadTimeDetection_(2.0e-3), detectionEfficiency_(-1), noiseRate_(0.0), maxDriftTime_(100.0)
{
  numHits_.resize(MAX_HITS+2, 0);
}

EvaluateEventRateForNanoGRAMS::~EvaluateEventRateForNanoGRAMS() = default;


ANLStatus EvaluateEventRateForNanoGRAMS::mod_define()
{
  define_parameter("time_group", &mod_class::timeGroup_);
  define_parameter("intensity", &mod_class::intensity_, 1.0, "Bq");
  define_parameter("dead_time_no_detection", &mod_class::deadTimeNoDetection_, 1.0, "s");
  define_parameter("dead_time_detection", &mod_class::deadTimeDetection_, 1.0, "s");
  define_parameter("detection_efficiency", &mod_class::detectionEfficiency_);
  define_parameter("max_drift_time", &mod_class::maxDriftTime_, 1.0, "s");
  define_parameter("noise_rate", &mod_class::noiseRate_, 1.0, "Hz");
  return AS_OK;
}

ANLStatus EvaluateEventRateForNanoGRAMS::mod_initialize()
{
  if (noiseRate_ < 0.0 && detectionEfficiency_ < 0.0) {
    std::cerr << "Error: Either noise_rate or detection_efficiency must be set." << std::endl;
    return AS_QUIT_ERROR;
  }
  if (detectionEfficiency_ > 0 && noiseRate_ > 0) {
    std::cerr << "Error: Both noise_rate and detection_efficiency cannot be set at the same time." << std::endl;
    return AS_QUIT_ERROR;
  }
  intensity_ *= CLHEP::MBq;
  noiseRate_ *= CLHEP::hertz;
  maxDriftTime_ *= CLHEP::s;
  deadTimeDetection_ *= CLHEP::s;
  deadTimeNoDetection_ *= CLHEP::s;
  
  VCSModule::mod_initialize();
  if (!exist_module("CSHitCollection")) {
    return AS_ERROR;
  }
  get_module("CSHitCollection", &hitCollection_);
  numHits_.assign(MAX_HITS+2, 0);
  return AS_OK;  
}

ANLStatus EvaluateEventRateForNanoGRAMS::mod_analyze()
{
  std::vector<int> registeredHit;
  const auto &hits = hitCollection_->getHits(timeGroup_);
  for (const auto& hit: hits) {
    const auto id = hit->DetectorID();
    if (std::find(registeredHit.begin(), registeredHit.end(), id) == registeredHit.end()) {
      registeredHit.push_back(id);
    }
  }
  const int num_hits = registeredHit.size();
  if (num_hits <= MAX_HITS) {
    numHits_[num_hits]++;
  }
  else {
    numHits_[MAX_HITS]++;
  }
  return AS_OK;
}
ANLStatus EvaluateEventRateForNanoGRAMS::mod_end_run() {
  for (int i = 0; i <= MAX_HITS; i++) {
    std::cout << "Number of events with " << i << " hits: " << numHits_[i] << std::endl;
  }
  constexpr long num_samples = 1000000;
  const long totalHits = std::accumulate(numHits_.begin(), numHits_.end(), 0L);
  if (totalHits == 0) {
    return AS_OK;
  }
  for (int i = 1; i <= MAX_HITS; i++) {
    const double eventRate = calculateEventRate(totalHits, num_samples, i);
    std::cout << "Event rate at min hits " << i << ": " << eventRate << " events/s" << std::endl;
  }
  return AS_OK;
}

double EvaluateEventRateForNanoGRAMS::calculateEventRate(long totalEvents,  long num_samples, int min_hits) {
  double nondetection_probability = 0.0;
  for (int i = 0; i < min_hits && i <= MAX_HITS; i++) {
    nondetection_probability += static_cast<double>(numHits_[i]) / totalEvents;
  }
  const double detection_probability = 1.0 - nondetection_probability;
  std::mt19937 rng(std::random_device{}());
  std::exponential_distribution<double> exp_dist(intensity_);
  std::bernoulli_distribution bernoulli_dist(detection_probability);
  std::bernoulli_distribution efficiency_dist(detectionEfficiency_);
  double last_detection_time = 0.0;
  double current_time = 0.0;
  long actually_detected_events = 0;
  bool last_event_detected = false;
  for (long i = 0; i < num_samples; i++) {
    const double time = exp_dist(rng);
    current_time += time;
    const double dead_time = last_event_detected ? deadTimeDetection_ : deadTimeNoDetection_;
    if (bernoulli_dist(rng)) { //detection if no deadtime and max efficiency
      const bool deadtime_check = (current_time - last_detection_time >= dead_time);
      if (deadtime_check && efficiency_dist(rng)) {
        last_detection_time = current_time;
        actually_detected_events++;
        last_event_detected = true;
      }
      else if (deadtime_check) {
        last_detection_time = current_time + maxDriftTime_;
        last_event_detected = false;
      }
      else {
        last_event_detected = false;
      }
    }
    else {
      last_event_detected = false;
    }
  }
  return static_cast<double>(actually_detected_events) / num_samples;
}

} // namespace comptonsoft
