#ifndef COMPTONSOFT_EliminateLowEnergyHits_hh
#define COMPTONSOFT_EliminateLowEnergyHits_hh 1
#include "anlnext/BasicModule.hh"
#include "CSHitCollection.hh"
#include "CLHEP/Units/SystemOfUnits.h"

namespace comptonsoft {
class EliminateLowEnergyHits : public anlnext::BasicModule {
  DEFINE_ANL_MODULE(EliminateLowEnergyHits, 1.0);
  ENABLE_PARALLEL_RUN();
public:
  EliminateLowEnergyHits();
  virtual ~EliminateLowEnergyHits();

protected:
  EliminateLowEnergyHits(const EliminateLowEnergyHits &r);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
private:
  CSHitCollection* m_HitCollection = nullptr;
  double m_EnergyThreshold = 0.0;
  double m_EnergyThreshold_keV = 0.0;
  
};
} // namespace comptonsoft
#endif // COMPTONSOFT_EliminateLowEnergyHits_hh