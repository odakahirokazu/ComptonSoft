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
uint8_t HAS_PHOTOABSORPTION = 0x1U;
uint8_t IS_ESCAPED = 0x2U;
} // namespace flags
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
  uint8_t detectEscapeEvent(const std::vector<DetectorHit_sptr> &ordered_hits);

private:
  static constexpr double fixedLikelihood_ = 1.;
  static constexpr double tolerance_ = 1e-3 * anlgeant4::unit::keV;
  int processMode_ = 0; // 0: full + escape, 1: full only, 2: escape only
  double initialEnergy_ = 0.0;
  int verbose_ = 0;
  bool excludeGammaFromNonZeroTrackID_ = false;
};
} /* namespace comptonsoft */
#endif //COMPTONSOFT_EventReconstructionWithKnownOrder_hh