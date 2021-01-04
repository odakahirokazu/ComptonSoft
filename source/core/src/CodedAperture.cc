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
#include "AstroUnits.hh"
#include "Randomize.hh"

namespace unit = anlgeant4::unit;

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
}

bool CodedAperture::buildSkyImage()
{
  const int Nsx = sky_num_x_;
  const int Nsy = sky_num_y_;
  const double sizex = sky_fov_x_;
  const double sizey = sky_fov_y_;

  const bool number_determined = (Nsx>0) && (Nsy>0);
  const bool size_determined = (sizex>0.0) && (sizey>0.0);
  if (!number_determined) {
    std::cerr << "num_sky_x and num_sky_y should be determined by user." << std::endl;
    return false;
  }

  if (decoded_image_) {
    decoded_image_->resize(boost::extents[Nsx][Nsy]);
  }
  else {
    decoded_image_.reset(new image_t(boost::extents[Nsx][Nsy]));
  }

  if (size_determined) {
    sky_element_angle_x_ = sizex/static_cast<double>(Nsx);
    sky_element_angle_y_ = sizey/static_cast<double>(Nsy);
  }
  else {
    const int Ndx = encoded_image_->shape()[0];
    const int Ndy = encoded_image_->shape()[1];
    const int Nmx = aperture_pattern_->shape()[0];
    const int Nmy = aperture_pattern_->shape()[1];
    const double dist = detector_to_aperture_distance_;

    double xmax = 0.0;
    double ymax = 0.0;
    
    for (int dx=0; dx<Ndx; dx++) {
      for (int dy=0; dy<Ndy; dy++) {
        for (int mx=0; mx<Nmx; mx++) {
          for (int my=0; my<Nmy; my++) {
            CodedAperture::ID id1 = CodedAperture::ID(dx, dy);
            CodedAperture::ID id2 = CodedAperture::ID(mx, my);
            vector2_t v1 = DetectorPosition(id1);
            vector2_t aperture_v = AperturePosition(id2);
            vector2_t v2 = ApertureToDetector(aperture_v);
            vector2_t v3 = v2 - v1;
            vector2_t detected_angle = vector2_t(std::tan(v3.x()/dist), std::tan(v3.y()/dist));
            vector2_t sky_angle = DetectedAngleToSkyAngle(detected_angle);
            xmax = std::max(xmax, std::abs(sky_angle.x()));
            ymax = std::max(ymax, std::abs(sky_angle.y()));
          }
        }
      }
    }
    
    sky_element_angle_x_ = xmax*2.0/static_cast<double>(Nsx);
    sky_element_angle_y_ = ymax*2.0/static_cast<double>(Nsy);
  }

  return true;
}


void CodedAperture::decode()
{
  if (decoding_mode_==1) {
    decode_based_on_sky_coordinate();
  }
  else if (decoding_mode_==2) {
    decode_based_on_aperture_coordinate();
  }
  else {
    std::cerr << "Decoding mode not set appropriately." << std::endl;
  }
}

void CodedAperture::decode_based_on_sky_coordinate()
{
  const int Nsx = decoded_image_->shape()[0];
  const int Nsy = decoded_image_->shape()[1];
  const int Ndx = encoded_image_->shape()[0];
  const int Ndy = encoded_image_->shape()[1];

  const double dist = DetectorToApertureDistance();

  image_t& imageD = *decoded_image_;
  const image_t& imageE = *encoded_image_;
  const image_t& t = *decoder_array_;
  const bool rand = true;
  
  for (int sx=0; sx<Nsx; sx++) {
    for (int sy=0; sy<Nsy; sy++) {
      for (int dx=0; dx<Ndx; dx++) {
        for (int dy=0; dy<Ndy; dy++) {
          if (imageE[dx][dy]<=0.0) {
            continue;
          }
          for (int ti=0; ti<num_decoding_iterations_; ti++) {
            CodedAperture::ID sky_id = CodedAperture::ID(sx, sy);
            CodedAperture::ID detector_id = CodedAperture::ID(dx, dy);
            vector2_t sky_angle = SkyAngle(sky_id, rand);
            vector2_t detected_angle = SkyAngleToDetectedAngle(sky_angle);
            vector2_t v1 = DetectorPosition(detector_id, rand);
            vector2_t v2 = vector2_t(std::tan(dist*detected_angle.x()), std::tan(dist*detected_angle.y()));
            vector2_t v3 = v1 + v2;
            vector2_t aperture_pos = DetectorToAperture(v3);
            CodedAperture::ID aperture_id = ApertureID(aperture_pos);
            if (aperture_id.invalid) {
              continue;
            }
            imageD[sx][sy] += imageE[dx][dy]*t[aperture_id.ix][aperture_id.iy];
          }
        }
      }
    }
  }
}

void CodedAperture::decode_based_on_aperture_coordinate()
{
  const int Ndx = encoded_image_->shape()[0];
  const int Ndy = encoded_image_->shape()[1];
  const int Nmx = decoder_array_->shape()[0];
  const int Nmy = decoder_array_->shape()[1];

  const double dist = DetectorToApertureDistance();

  image_t& imageD = *decoded_image_;
  const image_t& imageE = *encoded_image_;
  const image_t& t = *decoder_array_;
  const bool rand = true;

  for (int dx=0; dx<Ndx; dx++) {
    for (int dy=0; dy<Ndy; dy++) {
      for (int mx=0; mx<Nmx; mx++) {
        for (int my=0; my<Nmy; my++) {
          if (imageE[dx][dy]<=0.0) {
            continue;
          }
          for (int ti=0; ti<num_decoding_iterations_; ti++) {
            CodedAperture::ID id1 = CodedAperture::ID(dx, dy);
            CodedAperture::ID id2 = CodedAperture::ID(mx, my);
            vector2_t v1 = DetectorPosition(id1, rand);
            vector2_t aperture_v = AperturePosition(id2);
            vector2_t v2 = ApertureToDetector(aperture_v);
            vector2_t v3 = v2 - v1;
            vector2_t detected_angle = vector2_t(std::atan(v3.x()/dist), std::atan(v3.y()/dist));
            vector2_t sky_angle = DetectedAngleToSkyAngle(detected_angle);
            CodedAperture::ID sky_id = SkyID(sky_angle);
            if (sky_id.invalid) {
              continue;
            }
            imageD[sky_id.ix][sky_id.iy] += imageE[dx][dy]*t[mx][my];
          }
        }
      }
    }
  }

}

CodedAperture::ID CodedAperture::DetectorID(const vector2_t& v) const
{
  const int nx = encoded_image_->shape()[0];
  const int ny = encoded_image_->shape()[1];
  const double cx = 0.5*(nx-1.0);
  const double cy = 0.5*(ny-1.0);
  const double dx = detector_element_size_x_;
  const double dy = detector_element_size_y_;
  CodedAperture::ID id;
  id.ix = static_cast<int>(std::floor(cx+v.x()/dx+0.5));
  id.iy = static_cast<int>(std::floor(cy+v.y()/dy+0.5));
  if (id.ix<0 || id.ix>=nx || id.iy<0 || id.iy>=ny) { id.invalid = true; }
  return id;
}

vector2_t CodedAperture::DetectorPosition(const CodedAperture::ID& id, bool random/*=false*/)
{
  const int nx = encoded_image_->shape()[0];
  const int ny = encoded_image_->shape()[1];
  const double cx = 0.5*(nx-1.0);
  const double cy = 0.5*(ny-1.0);
  const double dx = detector_element_size_x_;
  const double dy = detector_element_size_y_;
  double rx = 0.0;
  double ry = 0.0;
  if (random) {
    rx = UniformDistribution(-0.5, 0.5);
    ry = UniformDistribution(-0.5, 0.5);
  }
  const double vx = (id.ix-cx+rx)*dx;
  const double vy = (id.iy-cy+ry)*dy;
  return vector2_t(vx, vy);
}

CodedAperture::ID CodedAperture::ApertureID(const vector2_t& v) const
{
  const int nx = aperture_pattern_->shape()[0];
  const int ny = aperture_pattern_->shape()[1];
  const double cx = 0.5*(nx-1.0);
  const double cy = 0.5*(ny-1.0);
  const double dx = aperture_element_size_x_;
  const double dy = aperture_element_size_y_;
  CodedAperture::ID id;
  id.ix = static_cast<int>(std::floor(cx+v.x()/dx+0.5));
  id.iy = static_cast<int>(std::floor(cy+v.y()/dy+0.5));
  if (id.ix<0 || id.ix>=nx || id.iy<0 || id.iy>=ny) { id.invalid = true; }
  return id;
}

vector2_t CodedAperture::AperturePosition(const CodedAperture::ID& id, bool random/*=false*/)
{
  const int nx = aperture_pattern_->shape()[0];
  const int ny = aperture_pattern_->shape()[1];
  const double cx = 0.5*(nx-1.0);
  const double cy = 0.5*(ny-1.0);
  const double dx = aperture_element_size_x_;
  const double dy = aperture_element_size_y_;
  double rx = 0.0;
  double ry = 0.0;
  if (random) {
    rx = UniformDistribution(-0.5, 0.5);
    ry = UniformDistribution(-0.5, 0.5);
  }
  const double vx = (id.ix-cx+rx)*dx;
  const double vy = (id.iy-cy+ry)*dy;
  return vector2_t(vx, vy);
}

CodedAperture::ID CodedAperture::SkyID(const vector2_t& v) const
{
  const int nx = decoded_image_->shape()[0];
  const int ny = decoded_image_->shape()[1];
  const double cx = 0.5*(nx-1.0);
  const double cy = 0.5*(ny-1.0);
  const double dx = sky_element_angle_x_;
  const double dy = sky_element_angle_y_;
  CodedAperture::ID id;
  id.ix = static_cast<int>(std::floor(cx+v.x()/dx+0.5));
  id.iy = static_cast<int>(std::floor(cy+v.y()/dy+0.5));
  if (id.ix<0 || id.ix>=nx || id.iy<0 || id.iy>=ny) id.invalid = true;
  return id;
}

vector2_t CodedAperture::SkyAngle(const CodedAperture::ID& id, bool random/*=false*/)
{
  const int nx = decoded_image_->shape()[0];
  const int ny = decoded_image_->shape()[1];
  const double cx = 0.5*(nx-1.0);
  const double cy = 0.5*(ny-1.0);
  const double dx = sky_element_angle_x_;
  const double dy = sky_element_angle_y_;
  double rx = 0.0;
  double ry = 0.0;
  if (random) {
    rx = UniformDistribution(-0.5, 0.5);
    ry = UniformDistribution(-0.5, 0.5);
  }
  const double vx = (id.ix-cx+rx)*dx;
  const double vy = (id.iy-cy+ry)*dy;
  return vector2_t(vx, vy);
}

vector2_t CodedAperture::DetectorToAperture(vector2_t v)
{
  v -= aperture_offset_;
  const double ang = aperture_roll_angle_;
  v.rotate(ang);
  vector2_t res;
  res.setX(-v.x());
  res.setY(v.y());
  return res;
}

vector2_t CodedAperture::ApertureToDetector(vector2_t v)
{
  v += aperture_offset_;
  const double ang = aperture_roll_angle_;
  v.rotate(ang);
  vector2_t res;
  res.setX(-v.x());
  res.setY(v.y());
  return res;
}

vector2_t CodedAperture::DetectedAngleToSkyAngle(vector2_t v)
{
  v -= sky_offset_;
  const double ang = -detector_roll_angle_;
  v.rotate(ang);
  vector2_t res;
  res.setX(-v.x());
  res.setY(v.y());
  return res;
}

vector2_t CodedAperture::SkyAngleToDetectedAngle(vector2_t v)
{
  v += sky_offset_;
  const double ang = -detector_roll_angle_;
  v.rotate(ang);
  vector2_t res;
  res.setX(-v.x());
  res.setY(v.y());
  return res;
}

double CodedAperture::UniformDistribution(double x1, double x2) {
  std::uniform_real_distribution<double> distribution(x1, x2);
  const double t = distribution(randomGenerator_);
  
  return t;
}

} /* namespace comptonsoft */
