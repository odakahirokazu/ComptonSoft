#include "EventReconstructionWithKnownOrder.hh"
#include "AstroUnits.hh"
#include "BasicComptonEvent.hh"
#include "CLHEP/Random/RandGauss.h"
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
  processMode_ = pt.get<int>("known_order.process_mode", 0);
  initialEnergy_ = pt.get<double>("known_order.initial_energy") * unit::keV;
  excludeEscapeBranchingGamma_ = pt.get<bool>("known_order.exclude_escape_branching_gamma", false);
  excludeBranchingGammaDeposit_ = pt.get<bool>("known_order.exclude_branching_gamma_deposit", false);
  excludeRayleighScattering_ = pt.get<bool>("known_order.exclude_rayleigh_scattering", false);
  const int escape_method_mode = pt.get<int>("known_order.escape_detection_method", 2);
  if (escape_method_mode == 0) {
    escapeDetectionMethod_ = EscapeDetectionMethod::TOTAL_ENERGY_DEPOSITION;
  }
  else if (escape_method_mode == 1) {
    escapeDetectionMethod_ = EscapeDetectionMethod::PHOTOABSORPTION_CHECK;
  }
  else if (escape_method_mode == 2) {
    escapeDetectionMethod_ = EscapeDetectionMethod::FLAG_CHECK;
  }
  else {
    std::cerr << "Error: Unknown escape detection method mode: " << escape_method_mode << std::endl;
    return false;
  }
  numLastHits_ = pt.get<int>("known_order.num_last_hits", 0);
  thresholdOfPixelz_ = pt.get<int>("known_order.threshold_of_pixelz", -1);
  noiseLevelParam0_ = pt.get<double>("known_order.noise_level_param0", 0.0);
  noiseLevelParam1_ = pt.get<double>("known_order.noise_level_param1", 0.0);
  noiseLevelParam2_ = pt.get<double>("known_order.noise_level_param2", 0.0);
  if (noiseLevelParam0_ <= 0.0 && noiseLevelParam1_ <= 0.0 && noiseLevelParam2_ <= 0.0) {
    applyNoise_ = false;
  }
  else {
    applyNoise_ = true;
  }
  verbose_ = pt.get<int>("known_order.verbose", 0);
  std::cout << "--- EventReconstructionWithKnownOrder v2---" << std::endl;
  std::cout << "  process_mode: " << processMode_ << std::endl;
  std::cout << "  initial_energy: " << initialEnergy_ / unit::keV << " keV" << std::endl;
  std::cout << "  escape detection method: " << ((escapeDetectionMethod_ == EscapeDetectionMethod::TOTAL_ENERGY_DEPOSITION ? "Total Energy Deposition" : (escapeDetectionMethod_ == EscapeDetectionMethod::PHOTOABSORPTION_CHECK ? "Photoabsorption Check" : "Flag Check"))) << std::endl;
  if (escapeDetectionMethod_ == EscapeDetectionMethod::PHOTOABSORPTION_CHECK)
    std::cout << "    tolerance: " << tolerance_ / unit::keV << " keV" << std::endl;
  if (escapeDetectionMethod_ == EscapeDetectionMethod::FLAG_CHECK) {
    std::cout << "    exclude_escape_branching_gamma: " << excludeEscapeBranchingGamma_ << std::endl;
    std::cout << "    exclude_branching_gamma_deposit: " << excludeBranchingGammaDeposit_ << std::endl;
  }
  std::cout << "  exclude_rayleigh_scattering: " << excludeRayleighScattering_ << std::endl;
  std::cout << "  num_last_hits: " << numLastHits_ << std::endl;
  std::cout << "  threshold_of_pixelz: " << thresholdOfPixelz_ << std::endl;
  std::cout << "  noise_level_param0: " << noiseLevelParam0_ << std::endl;
  std::cout << "  noise_level_param1: " << noiseLevelParam1_ << std::endl;
  std::cout << "  noise_level_param2: " << noiseLevelParam2_ << std::endl;
  std::cout << "  verbose: " << verbose_ << std::endl;
  std::cout << "------------------------------------------" << std::endl;
  return true;
}
bool EventReconstructionWithKnownOrder::reconstruct(const std::vector<DetectorHit_sptr> &hits, const BasicComptonEvent &baseEvent, std::vector<BasicComptonEvent_sptr> &eventsReconstructed) {
  const auto num_hits = hits.size();
  if (num_hits < 2) {
    std::cerr << "Error: Not enough hits to reconstruct event. Number of hits: " << num_hits << ", Event ID: " << hits[0]->EventID() << std::endl;
    return false;
  }
  size_t hits_used = num_hits;
  if (numLastHits_ > 0 && numLastHits_ < num_hits) {
    hits_used = numLastHits_;
  }

  std::vector<DetectorHit_sptr> ordered_hits(hits.begin(), hits.begin() + hits_used);
  sortByRealTime(ordered_hits);
  std::transform(ordered_hits.begin(), ordered_hits.end(), ordered_hits.begin(),
                 [this](DetectorHit_sptr &hit) {
                   if (ApplyNoise()) {
                     // y^2 = param0^2 + (param1*x^(1/2))^2 + (param2*x)^2
                     // x: energy in keV
                     // y: noise value in keV

                     const double x = hit->Energy() / unit::keV; // in keV
                     const double param0 = getNoiseParam0();
                     const double param1 = getNoiseParam1();
                     const double param2 = getNoiseParam2();
                     const double y2 = param0 * param0 + param1 * param1 * x + param2 * param2 * x * x; // in keV2
                     const double sigma = std::sqrt(y2) * unit::keV;
                     if (verbose_ > 2) {
                       std::cout << "  Applying noise to hit (TrackID=" << hit->TrackID() << ", E=" << hit->Energy() / unit::keV << " keV): sigma = " << sigma / unit::keV << " keV" << std::endl;
                     }
                     const double ePI = CLHEP::RandGauss::shoot(hit->Energy(), sigma);
                     if (verbose_ > 2) {
                       std::cout << "    Energy before noise: " << hit->Energy() / unit::keV << " keV, after noise: " << ePI / unit::keV << " keV" << std::endl;
                     }
                     hit->setEnergy(ePI);
                   }
                   return hit;
                 });
  if (verbose_ > 0) {
    std::cout << "Reconstructing event ID: " << ordered_hits[0]->EventID() << " with " << num_hits << " (" << hits_used << ") hits." << std::endl;
    for (size_t i = 0; i < ordered_hits.size(); ++i) {
      std::cout << "  Hit " << i << ":"
                << ", Edep=" << ordered_hits[i]->EnergyDeposit() / unit::keV << " keV"
                << ", position=(" << ordered_hits[i]->PositionX() / unit::mm << ", "
                << ordered_hits[i]->PositionY() / unit::mm << ", "
                << ordered_hits[i]->PositionZ() / unit::mm << ") mm"
                << ", RealTime=" << ordered_hits[i]->RealTime() / unit::ns << " ns"
                << ", Process=" << ordered_hits[i]->Process()
                << ", TrackID=" << ordered_hits[i]->TrackID()
                << ", Particle=" << ordered_hits[i]->Particle()
                << std::endl;
    }
  }

  BasicComptonEvent_sptr eventReconstructed(new BasicComptonEvent(baseEvent));
  eventReconstructed->setHit1(0, ordered_hits[0]);
  eventReconstructed->setHit2(1, ordered_hits[1]);
  eventReconstructed->setNumberOfHits(ordered_hits.size());
  eventReconstructed->setEventID(ordered_hits[0]->EventID());
  const auto detect_flag = checkEvent(ordered_hits);
  if (verbose_ > 1) {
    std::cout << "  Detection flag: "
              << ((detect_flag & flags::HAS_PHOTOABSORPTION) ? "HAS_PHOTOABSORPTION " : "")
              << ((detect_flag & flags::IS_ESCAPED) ? "IS_ESCAPED " : "")
              << ((detect_flag & flags::TO_BE_EXCLUDED) ? "TO_BE_EXCLUDED " : "")
              << std::endl;
  }
  if (detect_flag & flags::TO_BE_EXCLUDED) {
    if (verbose_ > 0) {
      std::cout << "  Event marked to be excluded from escape event reconstruction. Skip..." << std::endl;
    }
    return false;
  }
  bool is_escaped = isEscapeEvent(detect_flag);
  if (num_hits != hits_used && numLastHits_ > 0) {
    is_escaped = true; // Escape event if not all hits are used
  }
  if ((processMode_ != 1) && is_escaped) {
    const auto result = reconstructEscapeEvent(ordered_hits, eventReconstructed);
    if (!result) {
      std::cerr << "Error: Failed to reconstruct escape event." << std::endl;
      return false;
    }
    if (eventReconstructed->CosThetaE() < -1.0 || eventReconstructed->CosThetaE() > 1.0) {
      std::cerr << "Error: Unphysical CosThetaE value after reconstruction: " << eventReconstructed->CosThetaE() << std::endl;
      return false;
    }
    eventsReconstructed.push_back(eventReconstructed);
  }
  else if ((processMode_ != 2) && !is_escaped) {
    const auto result = reconstructFullDepositEvent(ordered_hits, eventReconstructed);
    if (!result) {
      std::cerr << "Error: Failed to reconstruct full deposit event." << std::endl;
      return false;
    }
    if (eventReconstructed->CosThetaE() < -1.0 || eventReconstructed->CosThetaE() > 1.0) {
      std::cerr << "Error: Unphysical CosThetaE value after reconstruction: " << eventReconstructed->CosThetaE() << std::endl;
      return false;
    }
    eventsReconstructed.push_back(eventReconstructed);
  }
  else {
    std::cerr << "Event type does not match process mode. Skip..." << std::endl;
    return false;
  }
  if (verbose_ > 0) {
    std::cout << "  Total energy deposit: " << eventReconstructed->TotalEnergyDeposit() / unit::keV << " keV" << std::endl;
    std::cout << "  Delta energy: " << (initialEnergy_ - eventReconstructed->IncidentEnergy()) / unit::keV << " keV" << std::endl;
    std::cout << "  Dtheta (Compton angle difference): " << eventReconstructed->DeltaTheta() / unit::degree << " degree" << std::endl;
  }
  if (verbose_ > 1) {
    std::cout << "  Cone axis: " << eventReconstructed->ConeAxis() << std::endl;
    std::cout << "  CosThetaE: " << eventReconstructed->CosThetaE() << std::endl;
  }
  if (eventReconstructed->CosThetaE() < -1.0 || eventReconstructed->CosThetaE() > 1.0) {
    std::cerr << "Warning: Unphysical CosThetaE value: " << eventReconstructed->CosThetaE() << std::endl;
    std::cerr << "  Event ID: " << eventReconstructed->EventID() << std::endl;
    std::cerr << "  Initial energy: " << initialEnergy_ / unit::keV << " keV" << std::endl;
    std::cerr << "  Total energy deposit: " << eventReconstructed->TotalEnergyDeposit() / unit::keV << " keV" << std::endl;
  }
  return true;
}
uint8_t EventReconstructionWithKnownOrder::checkEvent(const std::vector<DetectorHit_sptr> &ordered_hits) {
  uint8_t flag = 0;
  double total_energy_deposition = 0.0;
  for (const auto &hit: ordered_hits) {
    if (escapeDetectionMethod_ == EscapeDetectionMethod::FLAG_CHECK) {
      if (hit->isFlags(flag::LastHitPhotoAbsorption)) {
        flag |= flags::HAS_PHOTOABSORPTION;
      }
      if (excludeEscapeBranchingGamma_ && hit->isFlags(flag::BranchingGammaIncluded)) {
        flag |= flags::TO_BE_EXCLUDED;
        if (verbose_ > 1) {
          std::cout << "  Hit marked as BranchingGammaIncluded. Mark event to be excluded." << std::endl;
        }
        break;
      }
      if (excludeBranchingGammaDeposit_ && hit->isFlags(flag::BranchingGammaDeposited)) {
        flag |= flags::TO_BE_EXCLUDED;
        if (verbose_ > 1) {
          std::cout << "  Hit marked as BranchingGammaDeposited. Mark event to be excluded." << std::endl;
        }
        break;
      }
    }
    else {
      if (hit->isProcess(process::PhotoelectricAbsorption) && hit->TrackID() == 1) {
        flag |= flags::HAS_PHOTOABSORPTION;
      }
      total_energy_deposition += hit->EnergyDeposit();
    }
    if (hit->isFlags(flag::HasRayleighScattering) && excludeRayleighScattering_) {
      flag |= flags::TO_BE_EXCLUDED;
      if (verbose_ > 1) {
        std::cout << "  Hit marked as RayleighScattering. Mark event to be excluded." << std::endl;
      }
      break;
    }
    if (thresholdOfPixelz_ >= 0 && ((anodeUpside_ && hit->VoxelZ() >= thresholdOfPixelz_) || (!anodeUpside_ && hit->VoxelZ() <= thresholdOfPixelz_))) {
      flag |= flags::TO_BE_EXCLUDED;
      if (verbose_ > 1) {
        std::cout << "  Hit pixel Z (" << hit->VoxelZ() << ") >= threshold_of_pixelz (" << thresholdOfPixelz_ << "). Mark event to be excluded." << std::endl;
      }
      break;
    }
  }

  if (escapeDetectionMethod_ == EscapeDetectionMethod::TOTAL_ENERGY_DEPOSITION || escapeDetectionMethod_ == EscapeDetectionMethod::PHOTOABSORPTION_CHECK) {
    if (initialEnergy_ - total_energy_deposition > tolerance_) {
      flag |= flags::IS_ESCAPED;
    }
  }
  else if (escapeDetectionMethod_ == EscapeDetectionMethod::FLAG_CHECK && ((flag & flags::HAS_PHOTOABSORPTION) == 0)) {
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
  if (verbose_ > 0) {
    std::cout << "  Reconstructing escape event." << std::endl;
  }
  double escaped_energy;
  const bool success = estimateEscapedEnergy(ordered_hits, escaped_energy);
  if (!success) {
    std::cerr << "Error: Failed to estimate escaped energy." << std::endl;
    return false;
  }
  if (verbose_ > 1) {
    std::cout << "  Estimated escaped energy: " << escaped_energy / unit::keV << " keV" << std::endl;
  }
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
  if (verbose_ > 0) {
    std::cout << "  Reconstructing full deposit event." << std::endl;
  }
  const double total_energy = total_energy_deposits(ordered_hits);
  eventReconstructed->setHit2Energy(total_energy - ordered_hits[0]->Energy());
  eventReconstructed->setTotalEnergyDeposit(total_energy);
  eventReconstructed->setEscapeFlag(false);
  eventReconstructed->setLikelihood(fixedLikelihood_);
  constexpr double reconstruction_fraction = 1.0;
  eventReconstructed->setReconstructionFraction(reconstruction_fraction);
  return true;
}
bool EventReconstructionWithKnownOrder::isEscapeEvent(uint8_t detectFlag) const {
  if (escapeDetectionMethod_ == EscapeDetectionMethod::TOTAL_ENERGY_DEPOSITION) {
    return (detectFlag & flags::IS_ESCAPED);
  }
  else if (escapeDetectionMethod_ == EscapeDetectionMethod::PHOTOABSORPTION_CHECK) {
    return !(detectFlag & flags::HAS_PHOTOABSORPTION);
  }
  else if (escapeDetectionMethod_ == EscapeDetectionMethod::FLAG_CHECK) {
    return (detectFlag & flags::IS_ESCAPED);
  }
  throw std::runtime_error("Error: Unknown escape detection method.");
}
} /* namespace comptonsoft */