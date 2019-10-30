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

#include <fstream>
#include <iostream>
#include <cstdint>
#include <list>
#include <algorithm>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <cmath>

namespace comptonsoft
{

CodedAperture::~CodedAperture() = default;

void CodedAperture::setup()
{
  const int nsx = NumDecodedImageX();
  const int nsy = NumDecodedImageY();
  const int ndx = NumEncodedImageX();
  const int ndy = NumEncodedImageY();
  const int nmx = NumMaskX();
  const int nmy = NumMaskY();
  pattern_.resize(boost::extents[nmx][nmy]);
  decoder_array_.resize(boost::extents[nmx][nmy]);
  encoded_image_.resize(boost::extents[ndx][ndy]);
  decoded_image_.resize(boost::extents[nsx][nsy]);
  for (int ix=0; ix<nmx; ix++) {
    for (int iy=0; iy<nmy; iy++) {
      pattern_[ix][iy] = 0.0;
      decoder_array_[ix][iy] = 0.0;
    }
  }
  for (int ix=0; ix<ndx; ix++) {
    for (int iy=0; iy<ndy; iy++) {
      encoded_image_[ix][iy] = 0.0;
    }
  }
  for (int ix=0; ix<nsx; ix++) {
    for (int iy=0; iy<nsy; iy++) {
      decoded_image_[ix][iy] = 0.0;
    }
  }
}

void CodedAperture::calculateApertureRatio()
{
  const int nmx = NumMaskX();
  const int nmy = NumMaskY();
  double num_aperture = 0.0;
  for (int mx=0; mx<nmx; mx++) {
    for (int my=0; my<nmy; my++) {
      if (pattern_[mx][my]>0.0) {
        num_aperture += 1.0;
      }
    }
  }
  aperture_ratio_ = num_aperture/static_cast<double>(nmx*nmy);
}

void CodedAperture::makeDecoderArray()
{	
  const int nmx = NumMaskX();
  const int nmy = NumMaskY();
  const double c = -1.0*ApertureRatio() / ( 1.0 - ApertureRatio());
	for(int mx=0; mx < nmx; mx++){ 		
			for(int my=0; my < nmy; my++){
					if(pattern_[mx][my]>0){
						decoder_array_[mx][my] = 1.0;
					}	else{
						decoder_array_[mx][my] = c;
					}
			}
	}
}

void CodedAperture::calculateFieldOfView()
{
  const double l = SourceToMaskDistance();
  const double h = MaskToDetectorDistance();
  const double mask_corner_x = 0.5*MaskElementSize()*NumMaskX();
  const double mask_corner_y = 0.5*MaskElementSize()*NumMaskY();
  const double detector_corner_x = -0.5*DetectorElementSize()*NumEncodedImageX();
  const double detector_corner_y = -0.5*DetectorElementSize()*NumEncodedImageY();
  const double sky_corner_x = detector_corner_x + (mask_corner_x - detector_corner_x) * (l+h) / h;
  const double sky_corner_y = detector_corner_y + (mask_corner_y - detector_corner_y) * (l+h) / h;
  sky_element_size_x_ = sky_corner_x * 2.0 / NumDecodedImageX();
  sky_element_size_y_ = sky_corner_y * 2.0 / NumDecodedImageY();
}

void CodedAperture::decode()
{	
  const int nsx = NumDecodedImageX();
  const int nsy = NumDecodedImageY();
  const int ndx = NumEncodedImageX();
  const int ndy = NumEncodedImageY();
  const int nmx = NumMaskX();
  const int nmy = NumMaskY();
	for(int sx =0; sx < nsx; sx++){
			for(int sy =0; sy < nsy; sy++){
				decoded_image_[sx][sy] = 0.0;  
			}
	}

  const double l = SourceToMaskDistance();
  const double h = MaskToDetectorDistance();
	for(int dx=0; dx < ndx; dx++){
			for(int dy=0; dy < ndy; dy++){
				const double dx0 = dx -  (NumEncodedImageX()-1.0) * 0.5;
				const double dy0 = dy -  (NumEncodedImageY()-1.0) * 0.5;
				for (int mx=0; mx < nmx; mx++){
						for(int my=0; my < nmy; my++){
								const double mx0 = mx -  (NumMaskX() - 1.0) * 0.5;
								const double my0 = my -  (NumMaskY() - 1.0) * 0.5;
                const double x1 = dx0 * DetectorElementSize();
                const double y1 = dy0 * DetectorElementSize();
                const double x2 = mx0 * MaskElementSize();
                const double y2 = my0 * MaskElementSize();
                const double sx0 = (x1 + (x2-x1) * (l+h) / h) / SkyElementSizeX();
                const double sy0 = (y1 + (y2-y1) * (l+h) / h) / SkyElementSizeY(); 
								const double sx = sx0 + (NumDecodedImageX() - 1.0) *0.5;
								const double sy = sy0 + (NumDecodedImageY() - 1.0) *0.5;
                if (sx>=0 && sx<NumDecodedImageX() && sy>=0 && sy<NumDecodedImageY()) {
                    decoded_image_[sx][sy] += encoded_image_[dx][dy] * decoder_array_[mx][my];
                  }
						}
				}
			}
	}

}

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

} /* namespace comptonsoft */
