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

#ifndef COMPTONSOFT_XrayEventCollection_H
#define COMPTONSOFT_XrayEventCollection_H 1

#include <anlnext/BasicModule.hh>
#include "XrayEvent.hh"

namespace comptonsoft {
/**
 * @author Hirokazu Odaka
 * @date 2019-10-30
 */
class XrayEventCollection : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(XrayEventCollection, 1.0);
public:
  XrayEventCollection();
  ~XrayEventCollection();

  XrayEventContainer& getEvents() { return events_; }
  const XrayEventContainer& getEvents() const { return events_; }
  
  virtual void initializeEvent();
  void insertEvent(const XrayEvent_sptr& e) { events_.push_back(e); }

  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

private:
  XrayEventContainer events_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_XrayEventCollection_H */
