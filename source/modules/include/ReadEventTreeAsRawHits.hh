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

#ifndef COMPTONSOFT_ReadEventTreeAsRawHits_H
#define COMPTONSOFT_ReadEventTreeAsRawHits_H 1

#include "ReadEventTree.hh"

namespace comptonsoft {

/**
 * @author Hitokazu Odaka
 * @date 2015-11-14
 */
class ReadEventTreeAsRawHits : public ReadEventTree
{
  DEFINE_ANL_MODULE(ReadEventTreeAsRawHits, 2.0);
public:
  ReadEventTreeAsRawHits();
  ~ReadEventTreeAsRawHits() = default;
  
protected:
  void insertHit(const DetectorHit_sptr& hit);
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ReadEventTreeAsRawHits_H */
