/*************************************************************************
 *                                                                       *
 * Copyright (c) 2019 Hirokazu Odaka                                     *
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
 * CodedAperture
 *
 * @author Tsubasa Tamba, Satoshi Takashima, Hirokazu Odaka
 * @date 2019-11-01
 * @date 2019-11-15 | H. Odaka | redesign
 */
#ifndef COMPTONSOFT_VCodedAperture_H
#define COMPTONSOFT_VCodedAperture_H 1

#include "XrayEvent.hh"
#include <memory>

namespace comptonsoft {

class VCodedAperture
{
public:
  VCodedAperture();
  virtual ~VCodedAperture();

  virtual void setAperturePattern(const std::shared_ptr<image_t>& pattern) = 0;
  virtual void setEncodedImage(const std::shared_ptr<image_t>& image) = 0;
  virtual void decode() = 0;
  virtual std::shared_ptr<image_t> DecodedImage() const = 0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_VCodedAperture_H */
