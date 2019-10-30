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
 */
#ifndef COMPTONSOFT_CodedAperture_H
#define COMPTONSOFT_CodedAperture_H 1

#include "XrayEvent.hh"

namespace comptonsoft {

class CodedAperture
{
public:
  CodedAperture() = default;
  virtual ~CodedAperture();
    //virtual CodedAperture() = default;

  // setters & getters

  virtual void decode();
  void setup();
  void calculateApertureRatio();
  void makeDecoderArray();
  void calculateFieldOfView();
  void mirrorDecodedImage();

  int NumEncodedImageX() { return num_encoded_image_x_; }
  int NumEncodedImageY() { return num_encoded_image_y_; }
  int NumDecodedImageX() { return num_decoded_image_x_; }
  int NumDecodedImageY() { return num_decoded_image_y_; }
  int NumMaskX() { return num_mask_x_; }
  int NumMaskY() { return num_mask_y_; }
  double SkyElementSizeX() { return sky_element_size_x_; }
  double SkyElementSizeY() { return sky_element_size_y_; }
  double DetectorElementSize() { return detector_element_size_; }
  double MaskElementSize() { return mask_element_size_; }
  double SourceToMaskDistance() { return source_to_mask_distance_; }
  double MaskToDetectorDistance() { return mask_to_detector_distance_; }
  double ApertureRatio(){ return aperture_ratio_; }
  image_t& DecodedImage() { return decoded_image_; }
  
  void setEncodedImage(const image_t& v) { encoded_image_ = v; }
  void setPattern(const image_t& v) { pattern_ = v; }
  void setDetectorElementSize(double v) { detector_element_size_ = v; }
  void setMaskElementSize(double v) { mask_element_size_ = v; }
  void setMaskToDetectorDistance(double v) { mask_to_detector_distance_ = v; }
  void setSourceToMaskDistance(double v) { source_to_mask_distance_ = v; }
  void setNumEncodedImageX(int v) { num_encoded_image_x_ = v; }
  void setNumEncodedImageY(int v) { num_encoded_image_y_ = v; }
  void setNumDecodedImageX(int v) { num_decoded_image_x_ = v; }
  void setNumDecodedImageY(int v) { num_decoded_image_y_ = v; }
  void setNumMaskX(int v) { num_mask_x_ = v; }
  void setNumMaskY(int v) { num_mask_y_ = v; }

private:
  double sky_element_size_x_ = 1.0;
  double sky_element_size_y_ = 1.0;
  double detector_element_size_ = 1.0;
  double mask_element_size_ = 1.0;
  double mask_to_detector_distance_ = 1.0;
  double source_to_mask_distance_ = 1.0;

  image_t pattern_; // aperture pattern
  image_t decoder_array_; // decoder pattern
  image_t encoded_image_; // image on the focal plane
  image_t decoded_image_; // reconstructed image

  int num_encoded_image_x_ = 1;
  int num_encoded_image_y_ = 1;
  int num_decoded_image_x_ = 1;
  int num_decoded_image_y_ = 1;
  int num_mask_x_ = 1;
  int num_mask_y_ = 1;

  double aperture_ratio_ = 0.0; // open ratio of mask
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_CodedAperture_H */
