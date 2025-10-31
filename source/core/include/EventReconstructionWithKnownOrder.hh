#ifndef COMPTONSOFT_EventReconstructionWithKnownOrder_hh
#define COMPTONSOFT_EventReconstructionWithKnownOrder_hh 1
#include "AstroUnits.hh"
#include "DetectorHit.hh"
#include "DetectorHit_sptr.hh"
#include "VEventReconstructionAlgorithm.hh"
#include <boost/property_tree/json_parser.hpp>
#include <cstdint>
namespace comptonsoft {
namespace flags {
constexpr uint8_t HAS_PHOTOABSORPTION = 0x1U;
constexpr uint8_t IS_ESCAPED = 0x2U;
constexpr uint8_t TO_BE_EXCLUDED = 0x4U;
} // namespace flags

enum class EscapeDetectionMethod {
  TOTAL_ENERGY_DEPOSITION = 0,
  PHOTOABSORPTION_CHECK = 1,
  FLAG_CHECK = 2
};
class EventReconstructionWithKnownOrder: public VEventReconstructionAlgorithm {
public:
  EventReconstructionWithKnownOrder();
  virtual ~EventReconstructionWithKnownOrder();
  EventReconstructionWithKnownOrder(EventReconstructionWithKnownOrder &&) = default;
  bool loadParameters(boost::property_tree::ptree &pt) override;

  /**
   * perform the event reconstruction.
   * @param hits vector of hits to reconstruct.
   * @param baseEvent event based for new reconstructed events.
   * @param eventsReconstructed events reconstructed.
   * @return true if the reconstruction is successful.
   */
  bool reconstruct(const std::vector<DetectorHit_sptr> &hits,
                   const BasicComptonEvent &baseEvent,
                   std::vector<BasicComptonEvent_sptr> &eventsReconstructed) override;
  void initializeEvent() override {}

protected:
  bool sortByRealTime(std::vector<DetectorHit_sptr> &hits);
  bool reconstructEscapeEvent(const std::vector<DetectorHit_sptr> &ordered_hits, BasicComptonEvent_sptr &eventReconstructed);
  bool reconstructFullDepositEvent(const std::vector<DetectorHit_sptr> &ordered_hits, BasicComptonEvent_sptr &eventReconstructed);
  uint8_t checkEvent(const std::vector<DetectorHit_sptr> &ordered_hits);
  bool isEscapeEvent(uint8_t detectFlag) const;

private:
  static constexpr double fixedLikelihood_ = 1.;
  static constexpr double tolerance_ = 1e-3 * anlgeant4::unit::keV; // for photoabsorption check
  int processMode_ = 0; // 0: full + escape, 1: full only, 2: escape only
  double initialEnergy_ = 0.0;
  int verbose_ = 0;
  bool excludeEscapeBranchingGamma_ = false; // only for escape detection by flag check
  bool excludeBranchingGammaDeposit_ = false; // only for escape detection by flag check
  bool excludeRayleighScattering_ = false;
  int thresholdOfPixelz_ = -1;
  bool anodeUpside_ = true;
  size_t numLastHits_ = 0;
  EscapeDetectionMethod escapeDetectionMethod_ = EscapeDetectionMethod::TOTAL_ENERGY_DEPOSITION;
};
} /* namespace comptonsoft */
#endif //COMPTONSOFT_EventReconstructionWithKnownOrder_hh