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

#ifndef ANLGEANT4_VANLGeometry_H
#define ANLGEANT4_VANLGeometry_H 1

#include <anlnext/BasicModule.hh>
#include "SDAssignment.hh"

class G4VUserDetectorConstruction;


namespace anlgeant4
{

/**
 * Virtual geometry module
 * @author Hirokazu Odaka
 * @date xxxx-xx-xx
 * @date 2012-05-30
 * @date 2013-08-18 | H. Odaka | length unit, surface check
 * @date 2017-07-03 | 4.2 | H. Odaka | length unit is fixed to cm
 * @date 2026-04-17 | 5.0 | H. Odaka | SD map
 */
class VANLGeometry : public anlnext::BasicModule, public SDAssignment
{
  DEFINE_ANL_MODULE(VANLGeometry, 5.0);

public:
  VANLGeometry();

  virtual G4VUserDetectorConstruction* create() = 0;

  double get_length_unit() const { return length_unit_; }
  std::string get_length_unit_name() const { return length_unit_name_; }

  bool surface_check() const { return surface_check_; }

  anlnext::ANLStatus mod_define() override;

private:
  const double length_unit_;
  const std::string length_unit_name_;
  bool surface_check_;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_VANLGeometry_H */
