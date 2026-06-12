#include "SetPosition.hh"
#include "CSHitCollection.hh"
#include "FlagDefinition.hh"
#include "DetectorHit.hh"
using namespace anlnext;
namespace comptonsoft{
  SetPosition::SetPosition() :VCSModule(), source_(SourceType::LOCAL_POS), m_HitCollection(nullptr) {}
  SetPosition::~SetPosition() = default; 
  SetPosition::SetPosition(const SetPosition &r) = default;
  
  ANLStatus SetPosition::mod_define() {
    define_parameter("source", &mod_class::sourceStr_);
    return AS_OK;
  }
  ANLStatus SetPosition::mod_initialize() {
    VCSModule::mod_initialize();
    get_module_NC("CSHitCollection", &m_HitCollection);
    if (sourceStr_ == "real_pos") {
      source_ = SourceType::REAL_POS;
    }
    else if (sourceStr_ == "local_pos") {
      source_ = SourceType::LOCAL_POS;
    }
    else if (sourceStr_ == "nothing") {
      source_ = SourceType::NOTHING;// do nothing
    }
    else {
      source_ = SourceType::UNKNOWN;
      std::cerr << "Invalid source type for SetPosition: " << sourceStr_ << std::endl;
      return AS_QUIT_ERROR;
    }
    return AS_OK;
  }
  ANLStatus SetPosition::mod_analyze() {
    const auto detectorManager = getDetectorManager();
    const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
    for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
      std::vector<DetectorHit_sptr>& hits = m_HitCollection->getHits(timeGroup);
      for (DetectorHit_sptr& hit: hits) {
        if (source_ == SourceType::REAL_POS) {
          hit->setPosition(hit->RealPosition());
          hit->setPositionError({0.0, 0.0, 0.0});
        }
        else if (source_ == SourceType::LOCAL_POS) {
          const auto d = detectorManager->getDetectorByID(hit->DetectorID());
          hit->setPosition(d->PositionWithDepth(hit->Pixel(), hit->LocalPositionZ())); // for now we assume the depth sensing is on
          hit->setPositionError(d->PositionError(hit->LocalPositionError()));
        }
        else if (source_ == SourceType::NOTHING) {
          ;// do nothing
        }
        else {
          throw std::runtime_error("Invalid source type for SetPosition: " + sourceStr_);
        }
        }
      }
    return AS_OK;
  }
} // namespace comptonsoft
