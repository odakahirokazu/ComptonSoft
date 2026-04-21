#ifndef COMPTONSOFT_CorrectRecombination_hh
#define COMPTONSOFT_CorrectRecombination_hh 1
#include "VCSModule.hh"
#include "DetectorHit_sptr.hh"
#include "CSHitCollection.hh"
namespace comptonsoft {
class CorrectRecombination: public VCSModule {
  DEFINE_ANL_MODULE(CorrectRecombination, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  CorrectRecombination();
  virtual ~CorrectRecombination();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
private:
  CSHitCollection* m_HitCollection;
  std::vector<DetectorHitVector> m_detectorHits;
};
} // namespace comptonsoft

#endif //COMPTONSOFT_CorrectRecombination_hh