#include "EventReconstructionWithKnownOrder.hh"
#include "AstroUnits.hh"
#include "BasicComptonEvent.hh"
#include "ComptonConstraints.hh"
#include "FlagDefinition.hh"
#include <algorithm>
#include <iostream>
#include <string>
namespace unit = anlgeant4::unit;
namespace comptonsoft {
using namespace compton_constraints;
EventReconstructionWithKnownOrder::EventReconstructionWithKnownOrder() = default;
EventReconstructionWithKnownOrder::~EventReconstructionWithKnownOrder() = default;
bool EventReconstructionWithKnownOrder::loadParameters(boost::property_tree::ptree &pt) {
  processMode_ = pt.get<int>("known_order.process_mode");
  initialEnergy_ = pt.get<double>("known_order.initial_energy") * unit::keV;
  verbose_ = pt.get<int>("known_order.verbose", 0);
  std::cout << "--- EventReconstructionWithKnownOrder ---" << std::endl;
  std::cout << "  process_mode: " << processMode_ << std::endl;
  std::cout << "  initial_energy: " << initialEnergy_ / unit::keV << " keV" << std::endl;
  std::cout << "------------------------------------------" << std::endl;
  return true;
}
bool EventReconstructionWithKnownOrder::reconstruct(const std::vector<DetectorHit_sptr> &hits, const BasicComptonEvent &baseEvent, std::vector<BasicComptonEvent_sptr> &eventsReconstructed) {
  const auto num_hits = hits.size();
  if (num_hits < 2) {
    return false;
  }
  std::vector<DetectorHit_sptr> ordered_hits = hits;
  sortByRealTime(ordered_hits);
  if (verbose_ > 0) {
    std::cout << "Reconstructing event ID: " << ordered_hits[0]->EventID() << " with " << num_hits << " hits." << std::endl;
    for (size_t i = 0; i < ordered_hits.size(); ++i) {
      std::cout << "  Hit " << i << ":"
                << ", Edep=" << ordered_hits[i]->EnergyDeposit() / unit::keV << " keV"
                << ", position=(" << ordered_hits[i]->PositionX() / unit::mm << ", "
                << ordered_hits[i]->PositionY() / unit::mm << ", "
                << ordered_hits[i]->PositionZ() / unit::mm << ") mm"
                << ", RealTime=" << ordered_hits[i]->RealTime() / unit::ns << " ns"
                << ", Process=" << ordered_hits[i]->Process() << std::endl;
    }
  }

  BasicComptonEvent_sptr eventReconstructed(new BasicComptonEvent(baseEvent));
  eventReconstructed->setHit1(0, ordered_hits[0]);
  eventReconstructed->setHit2(1, ordered_hits[1]);
  eventReconstructed->setNumberOfHits(num_hits);
  const auto detect_flag = detectEscapeEvent(ordered_hits);
  if (verbose_ > 1) {
    std::cout << "  Detection flag: "
              << ((detect_flag & flags::HAS_PHOTOABSORPTION) ? "HAS_PHOTOABSORPTION " : "")
              << ((detect_flag & flags::IS_ESCAPED) ? "IS_ESCAPED " : "")
              << std::endl;
  }
  if ((processMode_ != 1) && (detect_flag & flags::IS_ESCAPED)) {
    const auto result = reconstructEscapeEvent(ordered_hits, eventReconstructed);
    if (!result) {
      std::cerr << "Error: Failed to reconstruct escape event." << std::endl;
      return false;
    }
    if (verbose_ > 0) {
      std::cout << "  Total energy deposit + escaped energy: " << eventReconstructed->TotalEnergyDeposit() / unit::keV << " keV" << std::endl;
      std::cout << "  Escaped energy: " << (eventReconstructed->TotalEnergyDeposit() - total_energy_deposits(ordered_hits)) / unit::keV << " keV" << std::endl;
      std::cout << "  Dtheta (Compton angle difference): " << eventReconstructed->DeltaTheta() / unit::degree << " degree" << std::endl;
    }
    eventsReconstructed.push_back(eventReconstructed);
  }
  else if ((processMode_ == 1) && !(flags::IS_ESCAPED & detect_flag)) {
    const auto result = reconstructFullDepositEvent(ordered_hits, eventReconstructed);
    if (!result) {
      std::cerr << "Error: Failed to reconstruct full deposit event." << std::endl;
      return false;
    }
    if (verbose_ > 0) {
      std::cout << "  Total energy deposit: " << eventReconstructed->TotalEnergyDeposit() / unit::keV << " keV" << std::endl;
      std::cout << "  Escaped energy: " << (eventReconstructed->TotalEnergyDeposit() - total_energy_deposits(ordered_hits)) / unit::keV << " keV" << std::endl;
      std::cout << "  Dtheta (Compton angle difference): " << eventReconstructed->DeltaTheta() / unit::degree << " degree" << std::endl;
    }
    eventsReconstructed.push_back(eventReconstructed);
  }
  return true;
}
uint8_t EventReconstructionWithKnownOrder::detectEscapeEvent(const std::vector<DetectorHit_sptr> &ordered_hits) {
  uint8_t flag = 0;
  double total_energy_deposition = 0.0;
  for (const auto &hit: ordered_hits) {
    if (hit->isProcess(process::PhotoelectricAbsorption)) {
      flag |= flags::HAS_PHOTOABSORPTION;
    }
    total_energy_deposition += hit->EnergyDeposit();
  }
  if (std::abs(total_energy_deposition - initialEnergy_) > tolerance_) {
    flag |= flags::IS_ESCAPED;
  }
  return flag;
}
bool EventReconstructionWithKnownOrder::sortByRealTime(std::vector<DetectorHit_sptr> &hits) {
  std::sort(hits.begin(), hits.end(), [](const DetectorHit_sptr &a, const DetectorHit_sptr &b) {
    return a->RealTime() < b->RealTime();
  });
  return true;
}

bool EventReconstructionWithKnownOrder::reconstructEscapeEvent(const std::vector<DetectorHit_sptr> &ordered_hits, BasicComptonEvent_sptr &eventReconstructed) {
  eventReconstructed->setEventID(ordered_hits[0]->EventID());
  eventReconstructed->setNumberOfHits(ordered_hits.size());
  double escaped_energy;
  const bool success = estimateEscapedEnergy(ordered_hits, escaped_energy);
  if (!success) {
    return false;
  };
  const double total_edep = total_energy_deposits(ordered_hits);
  const double total_energy = total_edep + escaped_energy;
  if (total_energy > initialEnergy_ + 100 * unit::keV) {
    std::cout << "Warning: total energy deposit + escaped energy > initial energy: " << total_energy / unit::keV << " keV > " << initialEnergy_ / unit::keV << " keV" << std::endl;
    std::cout << "eventID: " << eventReconstructed->EventID() << std::endl;
    std::cout << "  total energy deposit: " << total_edep / unit::keV << " keV" << std::endl;
    std::cout << "  escaped energy: " << escaped_energy / unit::keV << " keV" << std::endl;
  }
  eventReconstructed->setHit2Energy(total_energy - ordered_hits[0]->Energy());
  eventReconstructed->setTotalEnergyDeposit(total_energy);
  eventReconstructed->setEscapeFlag(true);
  eventReconstructed->setLikelihood(fixedLikelihood_);
  constexpr double reconstruction_fraction = 1.0;
  eventReconstructed->setReconstructionFraction(reconstruction_fraction);
  return true;
}

bool EventReconstructionWithKnownOrder::reconstructFullDepositEvent(const std::vector<DetectorHit_sptr> &ordered_hits, BasicComptonEvent_sptr &eventReconstructed) {
  const double total_energy = total_energy_deposits(ordered_hits);
  eventReconstructed->setTotalEnergyDeposit(total_energy);
  eventReconstructed->setEscapeFlag(false);
  eventReconstructed->setLikelihood(fixedLikelihood_);
  constexpr double reconstruction_fraction = 1.0;
  eventReconstructed->setReconstructionFraction(reconstruction_fraction);
  return true;
}

} /* namespace comptonsoft */