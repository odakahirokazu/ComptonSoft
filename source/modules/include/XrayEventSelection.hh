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

#ifndef COMPTONSOFT_XrayEventSelection_H
#define COMPTONSOFT_XrayEventSelection_H 1

#include "XrayEventCollection.hh"

namespace comptonsoft {
/**
 * @author Hirokazu Odaka
 * @date 2019-10-30
 */
class XrayEventSelection : public XrayEventCollection
{
  DEFINE_ANL_MODULE(XrayEventSelection, 1.0);
public:
  XrayEventSelection();
  ~XrayEventSelection();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

protected:
  virtual bool select(const XrayEvent_sptr& e) const;

private:
  int ixMin_ = 0;
  int ixMax_ = 100000;
  int iyMin_ = 0;
  int iyMax_ = 100000;
  int rankMin_ = 0;
  int rankMax_ = 100;
  int weightMin_ = 0;
  int weightMax_ = 100;
  double sumPHMin_ = 0.0;
  double sumPHMax_ = 100000.0;
  
  const XrayEventCollection* collection_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_XrayEventSelection_H */
