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
#ifndef COMPTONSOFT_CodedAperture_H
#define COMPTONSOFT_CodedAperture_H 1

#include "XrayEvent.hh"
#include <memory>

namespace comptonsoft {

class CodedAperture
{
public:
  CodedAperture();
  virtual ~CodedAperture();

  void setElementSizes(double detector_x,
                       double detector_y,
                       double aperture_x,
                       double aperture_y)
  {
    detector_element_size_x_ = detector_x;
    detector_element_size_y_ = detector_y;
    aperture_element_size_x_ = aperture_x;
    aperture_element_size_y_ = aperture_y;
  }

  void setAperturePattern(const std::shared_ptr<image_t>& pattern);
  void setEncodedImage(const std::shared_ptr<image_t>& image);

  virtual void decode();
  // void mirrorDecodedImage();

  std::shared_ptr<image_t> DecodedImage() const { return decoded_image_; }

private:
  double detector_element_size_x_ = 1.0;
  double detector_element_size_y_ = 1.0;
  double aperture_element_size_x_ = 1.0;
  double aperture_element_size_y_ = 1.0;
  double desampling_factor_ = 0.125;

  std::shared_ptr<image_t> aperture_pattern_;
  std::shared_ptr<image_t> decoder_array_;
  std::shared_ptr<image_t> encoded_image_;
  std::shared_ptr<image_t> decoded_image_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_CodedAperture_H */
