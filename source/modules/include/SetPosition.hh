#ifndef COMPTONSOFT_SetPosition_hh
#define COMPTONSOFT_SetPosition_hh 1
#include "anlnext/BasicModule.hh"
#include "VCSModule.hh"
#include "CSHitCollection.hh"
namespace comptonsoft {
/**
 * @brief Set position of hits in CSHitCollection.
 * @author Shota Arai
 * @date 2026-04-27
 */
class SetPosition : public VCSModule {
  DEFINE_ANL_MODULE(SetPosition, 1.0);
  ENABLE_PARALLEL_RUN();
public:
  SetPosition();
  virtual ~SetPosition();

protected:
  SetPosition(const SetPosition &r);
public:
  enum class SourceType {
    UNKNOWN,
    REAL_POS,
    LOCAL_POS,
    NOTHING,
  };
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
private:
  SourceType source_;
  CSHitCollection* m_HitCollection;
  std::string sourceStr_;
};
} // namespace comptonsoft
#endif // COMPTONSOFT_SetPosition_hh