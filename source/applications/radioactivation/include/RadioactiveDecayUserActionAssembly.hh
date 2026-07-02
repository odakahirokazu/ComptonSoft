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

#ifndef COMPTONSOFT_RadioactiveDecayUserActionAssembly_H
#define COMPTONSOFT_RadioactiveDecayUserActionAssembly_H 1

#include "StandardUserActionAssembly.hh"

namespace comptonsoft {


/**
 * UserActionAssembly for radioactive decay.
 *
 * @author Hirokazu Odaka
 * @date 2008-08-27
 * @date 2011-04-08
 * @date 2016-06-29 | rename the module name.
 * @date 2017-06-29 | new design of UserActionAssembly
 * @date 2024-02-24 | process name as a member
 */
class RadioactiveDecayUserActionAssembly : public anlgeant4::StandardUserActionAssembly
{
  DEFINE_ANL_MODULE(RadioactiveDecayUserActionAssembly, 6.0);
  ENABLE_PARALLEL_RUN();
public:
  RadioactiveDecayUserActionAssembly();

  anlnext::ANLStatus mod_define() override;

  void stepping_action(const G4Step* aStep) override;

  void set_termination_time(double v) { termination_time_ = v; }
  double termination_time() const { return termination_time_; }

  double first_decay_time() const { return first_decay_time_; }

private:
  double termination_time_;
  std::string radioactive_decay_process_name_;

  double first_decay_time_;

};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RadioactiveDecayUserActionAssembly_H */
