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

/**
 * @file IsotopeDatabaseAccess.hh
 * @brief header file of class IsotopeDatabaseAccess
 * @author Hirokazu Odaka
 * @date 2016-05-04
 */

#ifndef COMPTONSOFT_IsotopeDatabaseAccess_H
#define COMPTONSOFT_IsotopeDatabaseAccess_H 1

#include "RIDecayCalculation.hh"
#include "G4Ions.hh"

namespace comptonsoft {

class IsotopeDatabaseAccess : private RIDecayCalculation
{
public:
  IsotopeDatabaseAccess() = default;
  virtual ~IsotopeDatabaseAccess();

  void initialize_data();
  void retrive_isotope(int z, int a, double energy, int floating_level=0);
  double get_lifetime() const;
  double get_halflife() const;
  
private:
  const G4Ions* ions_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_IsotopeDatabaseAccess_H */
