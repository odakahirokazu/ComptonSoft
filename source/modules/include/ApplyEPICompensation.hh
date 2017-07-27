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

#ifndef COMPTONSOFT_ApplyEPICompensation_H
#define COMPTONSOFT_ApplyEPICompensation_H 1

#include "VCSModule.hh"

namespace comptonsoft {

class CSHitCollection;

/**
 * @author Hirokazu Odaka
 * @date 2015-05-14 | rename from SetSimGainCorrection.
 * @date 2016-10-07 | for version 5.2
 */
class ApplyEPICompensation : public VCSModule
{
  DEFINE_ANL_MODULE(ApplyEPICompensation, 3.0);
public:
  ApplyEPICompensation();
  ~ApplyEPICompensation();

  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  CSHitCollection* m_HitCollection = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ApplyEPICompensation_H */
