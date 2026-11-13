/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
 *************************************************************************/

#ifndef COMPTONSOFT_NanoGRAMSCorrectEnergyCharge_H
#define COMPTONSOFT_NanoGRAMSCorrectEnergyCharge_H 1

#include <cstdint>
#include <string>
#include <vector>

#include "VCSModule.hh"

namespace comptonsoft {

class CSHitCollection;

/**
 * Per-FEC correction of the charge-equivalent energy (echarge) of NanoGRAMS hits read from a hit tree,
 * and recalculation of the energy from the corrected charge.
 *
 * For each NanoGRAMS hit (FEC = DetectorSection):
 *   echarge' = echarge x echarge_correction_factors[FEC]
 *   EPI      = echarge'
 *   energy   = recombination correction of the detector unit (mode 4: charge-to-energy spline
 *              of the electric field given by NanoGRAMSLoadConfig), i.e. the same calculation as the data reduction.
 * EPIError is scaled in the same way (the stored EPIError is that after the recombination correction).
 *
 * The factors are given by echarge_correction_factors (4 values), or read from the key
 * "echarge_correction_factors" of parameter_file (yaml) if parameter_file is not empty.
 *
 * Chain: CSHitCollection, ConstructDetector, NanoGRAMSLoadConfig, ReadHitTree, (this), WriteHitTree.
 *
 * @date 2026-11-14
 */
class NanoGRAMSCorrectEnergyCharge : public VCSModule
{
  DEFINE_ANL_MODULE(NanoGRAMSCorrectEnergyCharge, 1.0);

public:
  NanoGRAMSCorrectEnergyCharge();
  ~NanoGRAMSCorrectEnergyCharge() override;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

private:
  std::string parameterFile_;
  std::vector<double> factors_;
  CSHitCollection* hitCollection_ = nullptr;

  int64_t numHits_ = 0;
  int64_t numCorrectedHits_ = 0;
};

} // namespace comptonsoft

#endif // COMPTONSOFT_NanoGRAMSCorrectEnergyCharge_H
