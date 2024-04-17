/**
 * @file RIDecayChains.cc
 * @brief source file of class RIDecayChains
 * @author Hirokazu Odaka
 * @date 2016-05-04
 * @date 2024-04-17 |geant4 v11.2 changed interface
 */

#include "RIDecayChains.hh"

#include <memory>
#include <iterator>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <boost/format.hpp>

#include "G4ParticleDefinition.hh"
#include "G4Ions.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4DecayTable.hh"
#include "G4RadioactiveDecay.hh"
#include "G4NuclearDecay.hh"
#include "G4NuclearLevelData.hh"
#include "G4LevelManager.hh"
#include "G4NucLevel.hh"

#include "AstroUnits.hh"

namespace unit = anlgeant4::unit;

namespace {

bool areSameLevels(const comptonsoft::RIDecayProperties& a, const comptonsoft::RIDecayProperties& b)
{
  if (a.Isotope().Z()==b.Isotope().Z() &&
      a.Isotope().A()==b.Isotope().A() &&
      a.Isotope().FloatingLevel()==b.Isotope().FloatingLevel()) {
    const double e1 = a.Isotope().Energy();
    const double e2 = b.Isotope().Energy();
    if (e1==0.0 && e2==0.0) {
      return true;
    }
    else {
      const double r = 1.0e-6;
      if (e1!=0.0 && std::abs(e2/e1-1.0) < r) {
        return true;
      }
    }
  }
  return false;
}

} /* anonymous namespace */

namespace comptonsoft
{

double invert_lifetime(double lifetime)
{
  const double decay_constant =
    (lifetime<0.0) ? 0.0 :
    (lifetime==0.0) ? (1.0/(1.0e-15*unit::s)) :
    (1.0/lifetime);
  return decay_constant;
}

RIDecayChains::RIDecayChains(const IsotopeInfo& isotope)
  : verbose_level_(0),
    use_photon_evaporation_data_(true),
    decay_constant_threshold_(0.0),
    lifetime_threshold_(1.0e-3*unit::second),
    branching_ratio_threshold_(1.0e-6),
    low_energy_limit_(0.5*unit::keV),
    isotope_(isotope)
{
}

RIDecayChains::~RIDecayChains() = default;

void RIDecayChains::build()
{
  const int Z = isotope_.Z();
  const int A = isotope_.A();
  const double ExcitationEnergy = isotope_.Energy();

  if (verbose_level_ >= 2) {
    std::cout << '\n'
              << "Decay chains for isotope "
              << (boost::format("(%d,%d,%.1f)") % Z % A % (ExcitationEnergy/unit::keV))
              << std::endl;
  }

  {
    RIDecayChain newChain;
    RIDecayProperties startingIsotope(0.0, 1.0, isotope_);
    newChain.push_back(startingIsotope);
    chains_.push_back(newChain);
  }
  
  buildChain(isotope_, 0);

  if (verbose_level_ >= 3) {
    printChains();
  }
}

void RIDecayChains::buildChain(const IsotopeInfo& parentIsotope, int depth)
{
  const int ParentZ = parentIsotope.Z();
  const int ParentA = parentIsotope.A();
  const double ParentExcitationEnergy = parentIsotope.Energy();
  const int ParentFloatingLevel = parentIsotope.FloatingLevel();

#define DEPTH_LIMIT_FOR_DEBUG 0
#if DEPTH_LIMIT_FOR_DEBUG
  if (depth > 3) {
    std::cout << "Exiting buildChain() as reached the depth limit for debug.\n"
              << "  depth = " << depth << std::endl;
    return;
  }
#endif // DEPTH_LIMIT_FOR_DEBUG

  if (verbose_level_ >= 6) {
    std::cout << boost::format("buildChain(): depth = %d, ") % depth
              << boost::format("isotope = (%d,%d,%.1f)") % ParentZ % ParentA % (ParentExcitationEnergy/unit::keV)
              << std::endl;
  }

  if (ParentA <= 4) {
    std::cout << "Skips this isotope as A <= 4. "
              << boost::format("isotope = (%d,%d,%.1f)") % ParentZ % ParentA % (ParentExcitationEnergy/unit::keV)
              << std::endl;
    return;
  }

#if STOP_DECAY_FOR_FLOATING_LEVEL
  if (parentIsotope.FloatingLevel() != 0) {
    std::cout << "This decay stops here for floating level. "
              << boost::format("isotope = (%d,%d,%.1f)") % ParentZ % ParentA % (ParentExcitationEnergy/unit::keV)
              << '\n'
              << boost::format("floating level = %d") % parentIsotope.FloatingLevel()
              << std::endl;
    return;
  }
#endif // STOP_DECAY_FOR_FLOATING_LEVEL

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  const G4ParticleDefinition* nucleus_base
    = particleTable->GetIonTable()->GetIon(ParentZ,
                                           ParentA,
                                           ParentExcitationEnergy,
                                           G4Ions::FloatLevelBase(ParentFloatingLevel));
  const G4Ions* nucleus = dynamic_cast<const G4Ions*>(nucleus_base);
  if (nucleus==nullptr) {
    return;
  }

  std::list<RIDecayProperties> decayProducts = collectDecayProducts(nucleus);
  compressDecayProducts(decayProducts);
  forceInstantaneousDecay(decayProducts);
  compressDecayProducts(decayProducts);

  bool firstChain = true;
  const RIDecayChain previousChain = chains_.back();
  for (const RIDecayProperties& decayProperties: decayProducts) {
    if (verbose_level_ >= 7) {
      std::cout << boost::format("Product: depth = %d, ") % depth
                << boost::format("isotope = (%d,%d,%.1f)") % (decayProperties.Isotope().Z()) % (decayProperties.Isotope().A()) % (decayProperties.Isotope().Energy()/unit::keV)
                << std::endl;
    }

    if (ParentExcitationEnergy < low_energy_limit_) {
      if (decayProperties.Isotope().Z() == ParentZ
          && decayProperties.Isotope().A() == ParentA) {
        std::cout << "Warning: a ground state decays to itself. "
                  << boost::format("isotope = (%d,%d,%.1f)") % ParentZ % ParentA % 0.0
                  << "\n"
                  << "This decay channel stops here."
                  << std::endl;
        continue;
      }
    }

    if (firstChain) {
      chains_.back().push_back(decayProperties);
      firstChain = false;
    }
    else {
      RIDecayChain newChain = previousChain;
      newChain.push_back(decayProperties);
      chains_.push_back(newChain);
    }
    
    buildChain(decayProperties.Isotope(), depth+1);
  }
}

std::list<RIDecayProperties>
RIDecayChains::collectDecayProducts(const G4Ions* parent)
{
  const double lifetime = parent->GetPDGLifeTime();
  const double decayConstant = invert_lifetime(lifetime);

  if (decayConstant>=0.0 && decayConstant<DecayConstantThreshold()) {
    return std::list<RIDecayProperties>();
  }
  
  // decay_process_->SetVerboseLevel(2);
  std::unique_ptr<const G4DecayTable> decayTable(decay_process_->LoadDecayTable(parent));

  if (decayTable.get()==nullptr) {
    return std::list<RIDecayProperties>();
  }

  std::list<RIDecayProperties> decayProducts;
  const int numChannels = decayTable->entries();
  for (int i=0; i<numChannels; i++) {
    G4VDecayChannel* channel_base = decayTable->GetDecayChannel(i);
    G4NuclearDecay* channel = dynamic_cast<G4NuclearDecay*>(channel_base);
    if (channel==nullptr) {
      continue;
    }

    const double branchingRatio = channel->GetBR();
    if (branchingRatio < BranchingRatioThreshold()) {
      continue;
    }

    std::list<RIDecayProperties> decayProductsViaIT;
    if (use_photon_evaporation_data_ && channel->GetDecayMode()==IT) {
      decayProductsViaIT = collectInternalTransitionProducts(parent);
      for (RIDecayProperties& decayProperties: decayProductsViaIT) {
        decayProperties.setBranchingRatio(branchingRatio*decayProperties.BranchingRatio());
      }
    }

    if (decayProductsViaIT.size() > 0) {
      decayProducts.splice(decayProducts.end(), decayProductsViaIT);
    }
    else {
      const G4ParticleDefinition* daughter_base = channel->GetDaughterNucleus();
      const G4Ions* daughter = dynamic_cast<const G4Ions*>(daughter_base);
      if (daughter) {
        const int Z = daughter->GetAtomicNumber();
        const int A = daughter->GetAtomicMass();
        const double Energy = daughter->GetExcitationEnergy();
        const int floatingLevel = daughter->GetFloatLevelBaseIndex();
        IsotopeInfo daughterIsotope(Z, A, Energy);
        daughterIsotope.setFloatingLevel(floatingLevel);
        RIDecayProperties decayProperties(decayConstant, branchingRatio, daughterIsotope);
        decayProducts.push_back(decayProperties);
      }
    }
  }

  return decayProducts;
}

std::list<RIDecayProperties>
RIDecayChains::collectInternalTransitionProducts(const G4Ions* parent)
{
  const int parentZ = parent->GetAtomicNumber();
  const int parentA = parent->GetAtomicMass();
  const double parentE = parent->GetExcitationEnergy();

  G4NuclearLevelData* nuclearLevelData = G4NuclearLevelData::GetInstance();
  const G4LevelManager* levelManager
    = nuclearLevelData->GetLevelManager(parentZ, parentA);

  if (levelManager==nullptr) {
    return std::list<RIDecayProperties>();
  }

  const int upperLevelIndex = levelManager->NearestLevelIndex(parentE);
  const G4NucLevel* upperLevel = levelManager->GetLevel(upperLevelIndex);
  if (upperLevel==nullptr) {
    return std::list<RIDecayProperties>();
  }

  const double lifetime = upperLevel->GetTimeGamma();
  const double decayConstant = invert_lifetime(lifetime);

  using TransitionList = std::list<std::pair<int, double>>;
  auto getLowerLevels = [this](const G4NucLevel* level, double weight) -> TransitionList {
    TransitionList transitions;
    const std::size_t n = level->NumberOfTransitions();
    double previousCumulativeProbability = 0.0;
    for (std::size_t i=0; i<n; i++) {
      const int lowerIndex = level->FinalExcitationIndex(i);
      const double cumulativeProbability = level->GammaCumProbability(i);
      const double probability = cumulativeProbability-previousCumulativeProbability;
      transitions.emplace_back(lowerIndex, probability*weight);
      previousCumulativeProbability = cumulativeProbability;
    }

    transitions.remove_if([this](std::pair<int, double>& trans) -> bool {
        const double probability = trans.second;
        return (probability < BranchingRatioThreshold());
      });
    return transitions;
  };

  TransitionList transitions = getLowerLevels(upperLevel, 1.0);
  for (TransitionList::iterator it=transitions.begin(); it!=transitions.end(); ) {
    const int lowerLevelIndex = it->first;
    const double probability = it->second;

    if (verbose_level_>=8) {
      std::cout << boost::format("Transition in %3d %3d %12.6f : %3d [%12.6f] -> %3d [%12.6f] | floating = %d")
        % parentZ % parentA % (parentE/unit::keV)
        % upperLevelIndex % (levelManager->LevelEnergy(upperLevelIndex)/unit::keV)
        % lowerLevelIndex % (levelManager->LevelEnergy(lowerLevelIndex)/unit::keV)
        % levelManager->FloatingLevel(lowerLevelIndex) << std::endl;
    }

    if (lowerLevelIndex==0 || levelManager->LevelEnergy(lowerLevelIndex)==0.0) {
      // ground state
      ++it;
      continue;
    }

    const double lowerLevelLifetime = levelManager->LifeTime(lowerLevelIndex);
    if (false && lowerLevelLifetime < LifetimeThreshold()) {
      const G4NucLevel* lowerLevel = levelManager->GetLevel(lowerLevelIndex);
      if (lowerLevel==nullptr) {
        ++it;
        continue;
      }

      TransitionList transitionsLower = getLowerLevels(lowerLevel, probability);
      if (transitionsLower.size() > 0) {
        transitions.splice(transitions.end(), transitionsLower);
        it = transitions.erase(it);
      }
    }
    else {
      ++it;
    }
  }

  for (TransitionList::iterator it=transitions.begin(); it!=transitions.end(); ++it) {
    TransitionList::iterator it1 = it;
    ++it1;
    while (it1 != transitions.end()) {
      if (it1->first == it->first) {
        it->second += it1->second;
        it1 = transitions.erase(it1);
      }
      else {
        ++it1;
      }
    }
  }

  std::list<RIDecayProperties> decayProducts;
  for (auto& trans: transitions) {
    const int lowerIndex = trans.first;
    const double probability = trans.second;
    const double lowerLevelEnergy = levelManager->LevelEnergy(lowerIndex);
    const int floatingLevel = levelManager->FloatingLevel(lowerIndex);
    IsotopeInfo daughterIsotope(parentZ, parentA, lowerLevelEnergy);
    daughterIsotope.setFloatingLevel(floatingLevel);
    RIDecayProperties decayProperties(decayConstant, probability, daughterIsotope);
    decayProducts.push_back(decayProperties);
  }
  return decayProducts;
}

void RIDecayChains::
forceInstantaneousDecay(std::list<RIDecayProperties>& decayProducts)
{
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  using Iter = std::list<RIDecayProperties>::iterator;
  for (Iter it=decayProducts.begin(); it!=decayProducts.end(); ) {
    const double branchingRatio = it->BranchingRatio();
    const double decayConstant = it->DecayConstant();
    const int Z = it->Isotope().Z();
    const int A = it->Isotope().A();
    const double Energy = it->Isotope().Energy();
    const int FloatingLevel = it->Isotope().FloatingLevel();

    if (Z == 0) {
      // to avoid an error by G4IonTable::GetIon() with Z=0, A=1
      ++it;
      continue;
    }
    
    const G4ParticleDefinition* nucleus_base
      = particleTable->GetIonTable()->GetIon(Z, A, Energy, G4Ions::FloatLevelBase(FloatingLevel));
    const G4Ions* nucleus = dynamic_cast<const G4Ions*>(nucleus_base);
    if (nucleus) {
      const double lifetime = nucleus->GetPDGLifeTime();
      if (Energy>0.0 && lifetime==0.0) {
        if (use_photon_evaporation_data_) {
          std::list<RIDecayProperties> decayProductsViaIT = collectInternalTransitionProducts(nucleus);
          if (decayProductsViaIT.size() > 0) {
            for (RIDecayProperties& decayProperties: decayProductsViaIT) {
              decayProperties.setBranchingRatio(branchingRatio*decayProperties.BranchingRatio());
              decayProperties.setDecayConstant(decayConstant);
            }
            decayProducts.splice(decayProducts.end(), decayProductsViaIT);
            it = decayProducts.erase(it);
            continue;
          }
        }
        else {
          // transition to the ground state
          const double finalEnergy = 0.0;
          it->setIsotope(IsotopeInfo(Z, A, finalEnergy));
        }
      }
    }
    ++it;
  }
}

void RIDecayChains::
compressDecayProducts(std::list<RIDecayProperties>& decayProducts)
{
  using Iter = std::list<RIDecayProperties>::iterator;

  Iter it = std::begin(decayProducts);
  while (it != std::end(decayProducts)) {
    Iter it1 = it;
    ++it1;
    while (it1 != std::end(decayProducts)) {
      if (areSameLevels(*it, *it1)) {
        const int Z = it->Isotope().Z();
        const int A = it->Isotope().A();
        const double Energy = it->Isotope().Energy();
        const int floatingLevel = it->Isotope().FloatingLevel();
        const double br = it->BranchingRatio();
        const double br1 = it1->BranchingRatio();
        it->setIsotope(IsotopeInfo(Z, A, Energy, floatingLevel));
        it->setBranchingRatio(br+br1);
        it->setMerged(true);
        it1 = decayProducts.erase(it1);
      }
      else {
        ++it1;
      }
    }
    ++it;
  }
}

void RIDecayChains::prepareSolutions()
{
  solutions_.clear();
  for (RIDecayChain& chain: chains_) {
    BatemanSolution solution;
    solution.setVerboseLevel(verbose_level_);
    solution.solve(chain);
    solutions_.push_back(std::move(solution));
  }
}

void RIDecayChains::solve(double t)
{
  const std::size_t NumChains = chains_.size();
  for (std::size_t iChain=0; iChain<NumChains; iChain++) {
    solveChain(solutions_[iChain], t, chains_[iChain]);
  }
  makeTotal();
}

void RIDecayChains::solve(const TimeProfile& timeProfile, double t)
{
  const std::size_t NumChains = chains_.size();
  for (std::size_t iChain=0; iChain<NumChains; iChain++) {
    solveChain(solutions_[iChain], timeProfile, t, chains_[iChain]);
  }
  makeTotal();
}

void RIDecayChains::solve(const TimeProfile& timeProfile,
                          double t1, double t2)
{
  const std::size_t NumChains = chains_.size();
  for (std::size_t iChain=0; iChain<NumChains; iChain++) {
    solveChain(solutions_[iChain], timeProfile, t1, t2, false, chains_[iChain]);
  }
  const double totalTime = t2 - t1;
  takeAverage(totalTime);
  makeTotal();
}

void RIDecayChains::solve(const TimeProfile& timeProfile,
                          const std::vector<std::pair<double, double>>& measurementWindows)
{
  const std::size_t NumChains = chains_.size();
  for (std::size_t iChain=0; iChain<NumChains; iChain++) {
    for (const auto& w: measurementWindows) {
      solveChain(solutions_[iChain], timeProfile, w.first, w.second, true, chains_[iChain]);
    }
  }
  double totalTime = 0.0;
  for (const auto& w: measurementWindows) {
    totalTime += (w.second - w.first);
  }
  takeAverage(totalTime);
  makeTotal();
}

void RIDecayChains::solveChain(const BatemanSolution& solution,
                               double t,
                               RIDecayChain& chain)
{
  const std::size_t N = chain.size();
  for (std::size_t i=0; i<N; i++) {
    const double s = solution.getSolution(i, t);
    if (verbose_level_ >= 5) {
      std::cout << boost::format("Solution for %14d = %.9e") % chain[i].Isotope().IsotopeID() % s << std::endl;
    }
    chain[i].setRate(s);
  }
}

void RIDecayChains::solveChain(const BatemanSolution& solution,
                               const TimeProfile& timeProfile,
                               double t,
                               RIDecayChain& chain)
{
  const std::size_t N = chain.size();
  for (std::size_t i=0; i<N; i++) {
    double sum = 0.0;
    const std::size_t NumIntervals = timeProfile.NumberOfIntervals();
    for (std::size_t iTI=0; iTI<NumIntervals; iTI++) {
      const TimeInterval interval = timeProfile.getInterval(iTI);
      const double s = solution.getConvolution(i, interval.time1, interval.time2, t);
      if (verbose_level_ >= 6) {
        std::cout << boost::format("Solution for %14d = %.9e at t = %.9e from %.9e %.9e")
          % chain[i].Isotope().IsotopeID() % s % (t/unit::second) % (interval.time1/unit::second) % (interval.time2/unit::second) << std::endl;
      }
      sum += s * interval.rate;
    }
    if (verbose_level_ >= 5) {
      std::cout << boost::format("Solution for %14d = %.9e (sum)") % chain[i].Isotope().IsotopeID() % sum << std::endl;
    }
    chain[i].setRate(sum);
  }
}

void RIDecayChains::solveChain(const BatemanSolution& solution,
                               const TimeProfile& timeProfile,
                               double t1,
                               double t2,
                               bool is_rate_additive,
                               RIDecayChain& chain)
{
  const std::size_t N = chain.size();
  for (std::size_t i=0; i<N; i++) {
    double sum = 0.0;
    const std::size_t NumIntervals = timeProfile.NumberOfIntervals();
    for (std::size_t iTI=0; iTI<NumIntervals; iTI++) {
      const TimeInterval interval = timeProfile.getInterval(iTI);
      const double s = solution.getIntegralConvolution(i, interval.time1, interval.time2, t1, t2);
      if (verbose_level_ >= 6) {
        std::cout << boost::format("Solution for %14d = %.9e at t = %.9e %.9e from %.9e %.9e")
          % chain[i].Isotope().IsotopeID() % s % (t1/unit::second) % (t2/unit::second) % (interval.time1/unit::second) % (interval.time2/unit::second) << std::endl;
      }
      sum += s * interval.rate;
    }
    if (verbose_level_ >= 5) {
      std::cout << boost::format("Solution for %14d = %.9e (sum)") % chain[i].Isotope().IsotopeID() % sum << std::endl;
    }

    if (is_rate_additive) {
      chain[i].addRate(sum);
    }
    else {
      chain[i].setRate(sum);
    }
  }
}

void RIDecayChains::takeAverage(double totalTime)
{
  for (RIDecayChain& chain: chains_) {
    for (RIDecayProperties& decay: chain) {
      const double r = decay.Isotope().Rate();
      decay.setRate(r/totalTime);
    }
  }
}

void RIDecayChains::makeTotal()
{
  total_counts_.clear();
  double sumWeight = 0.0;

  for (const RIDecayChain& chain: chains_) {
    const std::size_t N = chain.size();
    double weight = 1.0;
    for (std::size_t i=1; i<=N-1; i++) {
      weight *= chain[i].BranchingRatio();
    }
    sumWeight += weight;

    for (const RIDecayProperties& decay: chain) {
      const int64_t isotopeID = decay.Isotope().IsotopeID();
      const double count = decay.Isotope().Rate();
      const double weightedCount = weight * count;

      if (total_counts_.count(isotopeID)) {
        total_counts_[isotopeID] += weightedCount;
      }
      else {
        total_counts_[isotopeID] = weightedCount;
      }
    }

    if (verbose_level_ >= 4) {
      std::cout << boost::format("Weight: %.9f") % weight << std::endl;
    }
  }

  if (verbose_level_ >= 4) {
    std::cout << boost::format("Sum of weights: %.9f") % sumWeight << std::endl;
  }

  if (verbose_level_ >= 2) {
    std::cout << "Results:" << std::endl;
    printTotalCounts();
  }
}

void RIDecayChains::printChains() const
{
  for (const RIDecayChain& chain: chains_) {
    std::cout << "+";
    for (const RIDecayProperties& decay: chain) {
      char flags[4] = "*";
      flags[0] = decay.isMerged() ? 'M' : '*';
      std::cout << "--->";
      std::cout <<
        boost::format("(%.3f,%.2e|%d,%d,%.1f,%d|%s)")
        % decay.BranchingRatio()
        % (decay.DecayConstant()*unit::second)
        % decay.Isotope().Z()
        % decay.Isotope().A()
        % (decay.Isotope().Energy()/unit::keV)
        % decay.Isotope().FloatingLevel()
        % flags;
    }
    std::cout << std::endl;
  }
}

void RIDecayChains::printTotalCounts() const
{
  double sum(0.0);
  for (auto& o: total_counts_) {
    std::cout << boost::format("%14d %16.9e") % o.first % o.second << std::endl;
    sum += o.second;
  }
  std::cout << boost::format("Sum: %16.9e") % sum << std::endl;
}

} /* namespace comptonsoft */
