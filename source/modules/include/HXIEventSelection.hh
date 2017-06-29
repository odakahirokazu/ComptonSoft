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

#ifndef COMPTONSOFT_HXIEventSelection_H
#define COMPTONSOFT_HXIEventSelection_H 1

#include "VCSModule.hh"

namespace comptonsoft {

class CSHitCollection;

/**
 * HXI event selection.
 * @author Hirokazu Odaka
 * @date 2011-xx-xx
 */
class HXIEventSelection : public VCSModule
{
  DEFINE_ANL_MODULE(HXIEventSelection, 1.1);
public:
  HXIEventSelection() = default;
  ~HXIEventSelection() = default;

  anl::ANLStatus mod_init() override;
  anl::ANLStatus mod_ana() override;

private:
  CSHitCollection* m_HitCollection = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_HXIEventSelection_H */
