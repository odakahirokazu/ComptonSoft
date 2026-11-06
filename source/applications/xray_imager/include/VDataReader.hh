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

#ifndef COMPTONSOFT_VDataReader_H
#define COMPTONSOFT_VDataReader_H 1

#include <string>

namespace comptonsoft {

/**
 * VDataReader
 *
 * @author Hirokazu Odaka
 * @date 2019-11-12
 */
class VDataReader
{
public:
  virtual void add_file(const std::string& filename) = 0;
  virtual bool has_file(const std::string& filename) const = 0;
  virtual bool is_done() const = 0;

  virtual std::string current_filename() = 0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_VDataReader_H */
