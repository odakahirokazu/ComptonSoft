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

#ifndef ComptonSoft_CSException_H
#define ComptonSoft_CSException_H 1

#include <stdexcept>
#include <string>
#include <boost/exception/all.hpp>

namespace comptonsoft
{

typedef boost::error_info<struct tag_CSError, std::string> CSErrorInfo;

/**
 * Exception class for ComptonSoft.
 * 
 * @author Hirokazu Odaka
 * @date 2016-08-22
 */
struct CSException : virtual std::exception, virtual boost::exception
{
  static int VerboseLevel;

  CSException() = default;
  explicit CSException(const std::string& message);

  void setMessage(const std::string& message);
  std::string getMessage() const;
  std::string toString() const;
};

} /* namespace comptonsoft */

#endif /* ComptonSoft_CSException_H */
