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

#include "CSException.hh"
#include <boost/format.hpp>


int comptonsoft::CSException::VerboseLevel = 1;

comptonsoft::CSException::CSException(const std::string& message)
{
  *this << CSErrorInfo(message);
}

void comptonsoft::CSException::setMessage(const std::string& message)
{
  *this << CSErrorInfo(message);
}

std::string comptonsoft::CSException::getMessage() const
{
  const std::string* message = boost::get_error_info<CSErrorInfo>(*this);
  if (message==nullptr) {
    return std::string();
  }
  return *message;
}

std::string comptonsoft::CSException::toString() const
{
  if (VerboseLevel == 1) {
    std::ostringstream oss;
    const std::string* message = boost::get_error_info<CSErrorInfo>(*this);
    if (message) { oss << *message << "\n"; }
    return oss.str();
  }
  else if (VerboseLevel >= 2) {
    return boost::diagnostic_information(*this);
  }

  return "";
}
