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

#ifndef COMPTONSOFT_RadioactivationTreeIO_H
#define COMPTONSOFT_RadioactivationTreeIO_H 1

#include <cstdint>
#include <tuple>
#include "G4ThreeVector.hh"
#include "IsotopeInfo.hh"

class TTree;

namespace comptonsoft {

/**
 *
 * @author Hirokazu Odaka
 * @date 2026-06-24
 */
class RadioactivationTreeIO
{
public:
  RadioactivationTreeIO();
  virtual ~RadioactivationTreeIO();

  virtual void setTree(TTree* tree)
  { tree_ = tree; }

  virtual void defineBranches();
  virtual void setBranchAddresses();

  void fill(const IsotopeInfo& isotope, int volume, const G4ThreeVector& position);
  std::tuple<IsotopeInfo, int, G4ThreeVector> retrieve() const;

private:
  TTree* tree_ = nullptr;

  /*
   * tree contents
   */
  int32_t Z_ = 0;
  int32_t A_ = 0;
  double E_ = 0.0;
  int32_t floating_level_ = 0;
  int32_t volume_ = 0;
  float posx_ = 0.0;
  float posy_ = 0.0;
  float posz_ = 0.0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RadioactivationTreeIO_H */
