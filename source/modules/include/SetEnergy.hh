#ifndef COMPTONSOFT_SetEnergy_hh
#define COMPTONSOFT_SetEnergy_hh 1
#include "anlnext/BasicModule.hh"
#include "CSHitCollection.hh"
namespace comptonsoft {
class SetEnergy : public anlnext::BasicModule {
  DEFINE_ANL_MODULE(SetEnergy, 1.0);
  ENABLE_PARALLEL_RUN();
  
enum class SourceType {
  UNKNOWN,
  EPI,
  EDEP,
  ECHARGE,
};
public:
  SetEnergy();
  virtual ~SetEnergy();

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  CSHitCollection* m_HitCollection;
  std::string sourceStr_;
  SourceType source_;
};
} // namespace comptonsoft
#endif // COMPTONSOFT_SetEnergy_hh