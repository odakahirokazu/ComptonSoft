#ifndef COMPTONSOFT_NumHitsInDetectorFilter_hh
#define COMPTONSOFT_NumHitsInDetectorFilter_hh 1
#include "anlnext/BasicModule.hh"
#include "CSHitCollection.hh"
#include <map>
namespace comptonsoft {
class NumHitsInDetectorFilter : public anlnext::BasicModule {
  DEFINE_ANL_MODULE(NumHitsInDetectorFilter, 1.0);
  ENABLE_PARALLEL_RUN();
public:
  NumHitsInDetectorFilter();
  virtual ~NumHitsInDetectorFilter();

protected:
  NumHitsInDetectorFilter(const NumHitsInDetectorFilter &r) = default;
public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
private:
  const CSHitCollection* m_HitCollection = nullptr;
  int numHitsThreshold_ = 1;
  std::map<int, int> numHitsInDetector_;
  
};
} // namespace comptonsoft
#endif // COMPTONSOFT_NumHitsInDetectorFilter_hh