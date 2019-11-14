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

#include "CodedAperture.hh"
#include <iostream>

namespace comptonsoft
{

CodedAperture::CodedAperture()
{
}

CodedAperture::~CodedAperture() = default;

void CodedAperture::setAperturePattern(const std::shared_ptr<image_t>& pattern)
{
  aperture_pattern_ = pattern;

  const int Nmx = (*aperture_pattern_).shape()[0];
  const int Nmy = (*aperture_pattern_).shape()[1];
  double integral = 0.0;
  for (int mx=0; mx<Nmx; mx++) {
    for (int my=0; my<Nmy; my++) {
      integral += (*aperture_pattern_)[mx][my];
    }
  }
  const double apertureFraction = integral/(Nmx*Nmy);
  const double decodingCoefficient = -1.0*apertureFraction/(1.0-apertureFraction);
  decoder_array_.reset(new image_t(boost::extents[Nmx][Nmy]));
	for (int mx=0; mx<Nmx; mx++) {
    for(int my=0; my<Nmy; my++) {
      if ((*aperture_pattern_)[mx][my] > 0.0) {
        (*decoder_array_)[mx][my] = 1.0;
      }
      else {
        (*decoder_array_)[mx][my] = decodingCoefficient;
      }
    }
	}
}

void CodedAperture::setEncodedImage(const std::shared_ptr<image_t>& image)
{
  encoded_image_ = image;

  const int Ndx = encoded_image_->shape()[0];
  const int Ndy = encoded_image_->shape()[1];
  const int Nmx = decoder_array_->shape()[0];
  const int Nmy = decoder_array_->shape()[1];

  const double samplingScaleX = aperture_element_size_x_/detector_element_size_x_;
  const double samplingScaleY = aperture_element_size_y_/detector_element_size_y_;
  const double NmxScaled = samplingScaleX * Nmx;
  const double NmyScaled = samplingScaleY * Nmy;

  const int Nsx = desampling_factor_*(Ndx + NmxScaled - 1);
  const int Nsy = desampling_factor_*(Ndy + NmyScaled - 1);
  if (decoded_image_) {
    decoded_image_->resize(boost::extents[Nsx][Nsy]);
  }
  else {
    decoded_image_.reset(new image_t(boost::extents[Nsx][Nsy]));
  }
}

void CodedAperture::decode()
{	
  const int Nsx = decoded_image_->shape()[0];
  const int Nsy = decoded_image_->shape()[1];
  const int Ndx = encoded_image_->shape()[0];
  const int Ndy = encoded_image_->shape()[1];
  const int Nmx = decoder_array_->shape()[0];
  const int Nmy = decoder_array_->shape()[1];

  const double scaleXInverse = detector_element_size_x_/aperture_element_size_x_;
  const double scaleYInverse = detector_element_size_y_/aperture_element_size_y_;

  image_t& imageD = *decoded_image_;
  const image_t& imageE = *encoded_image_;
  const image_t& t = *decoder_array_;

  const double skyOriginalScale = 1.0/desampling_factor_;
  for (int sx=0; sx<Nsx; sx++) {
    for (int sy=0; sy<Nsy; sy++) {
      double pixelValue = 0.0;
      const double sxOriginal = sx*skyOriginalScale;
      for (int dx=0; dx<Ndx; dx++) {
        const int mxScaled = dx + sxOriginal + 1 - Ndx;
        const int mx = static_cast<int>(scaleXInverse*mxScaled);
        if (mx<0 || Nmx<=mx) { continue; }
        for (int dy=0; dy<Ndy; dy++) {
          const double syOriginal = sy*skyOriginalScale;
          const int myScaled = dy + syOriginal + 1 - Ndy;
          const int my = static_cast<int>(scaleYInverse*myScaled);
          if (my<0 || Nmy<=my) { continue; }
          if (imageE[dx][dy]>0.0) {
            pixelValue += imageE[dx][dy]*t[mx][my];
          }
        }
      }
      imageD[sx][sy] = pixelValue;
    }
  }
}

#if 0
void CodedAperture::mirrorDecodedImage()
{
  const int nsx = NumDecodedImageX();
  const int nsy = NumDecodedImageY();
  image_t image;
  image.resize(boost::extents[nsx][nsy]);
  for (int ix=0; ix<nsx; ix++) {
    for (int iy=0; iy<nsy; iy++) {
      image[ix][iy] = decoded_image_[ix][iy];
    }
  }
  for (int ix=0; ix<nsx; ix++) {
    for (int iy=0; iy<nsy; iy++) {
      decoded_image_[ix][iy] = image[nsx-ix-1][iy];
    }
  }
}
#endif

} /* namespace comptonsoft */
