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
 * @date 2020-10-27 | T. Tamba | redesign
 */
#ifndef COMPTONSOFT_CodedAperture_H
#define COMPTONSOFT_CodedAperture_H 1

#include "VCodedAperture.hh"
//#include <memory>
#include <random>
//#include "TH2.h"
#include "CSTypes.hh"

namespace comptonsoft {

class CodedAperture : public VCodedAperture
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

  void setAperturePattern(const std::shared_ptr<image_t>& pattern) override;
  void setEncodedImage(const std::shared_ptr<image_t>& image) override;
  bool setSkyImage() override;

  void decode() override;

  std::shared_ptr<image_t> DecodedImage() const override { return decoded_image_; }

  void setNumSky(int x, int y) { num_sky_x_ = x; num_sky_y_ = y; }
  int NumSkyX() { return num_sky_x_; }
  int NumSkyY() { return num_sky_y_; }
  void setSkySizeAngle(double x, double y) { sky_size_angle_x_ = x; sky_size_angle_y_ = y; }
  double SkySizeAngleX() { return sky_size_angle_x_; }
  double SkySizeAngleY() { return sky_size_angle_y_; }
  void setDetectorToApertureDistance(double v) { detector_to_aperture_distance_ = v; }
  double DetectorToApertureDistance() { return detector_to_aperture_distance_; }

  double SkyElementAngleX() const override { return sky_element_angle_x_; }
  double SkyElementAngleY() const override { return sky_element_angle_y_; }

  void setDetectorRollAngle(double v) { detector_roll_angle_ = v; }
  double DetectorRollAngle() { return detector_roll_angle_; }
  void setApertureRollAngle(double v) { aperture_roll_angle_ = v; }
  double ApertureRollAngle() { return aperture_roll_angle_; }
  void setApertureOffset(double x, double y) { aperture_offset_.setX(x); aperture_offset_.setY(y); }
  vector2_t ApertureOffset() { return aperture_offset_; }
  void setSkyOffsetAngle(double x, double y) { sky_offset_angle_.setX(x); sky_offset_angle_.setY(y); }
  vector2_t SkyOffsetAngle() { return sky_offset_angle_; }
  void setNumDecodingIteration(int v) { num_decoding_iteration_ = v; }
  int NumDecodingIteration() { return num_decoding_iteration_; }
  void setDecodingMode(int v) { decoding_mode_ = v; }
  int DecodingMode() { return decoding_mode_; }
  
  struct ID {
    int ix = 0;
    int iy = 0;
    bool invalid = false;
    ID() {}
    ID(int ix, int iy, bool invalid=false): ix(ix), iy(iy), invalid(invalid) {}
    ID(bool invalid): invalid(invalid) {}
  };

  ID DetectorID(vector2_t& v);
  ID ApertureID(vector2_t& v);
  ID SkyID(vector2_t& v);
  vector2_t DetectorPosition(ID& id, bool random = false);
  vector2_t AperturePosition(ID& id, bool random = false);
  vector2_t SkyAngle(ID& id, bool random = false);
  
  vector2_t DetectorToAperture(vector2_t v);
  vector2_t ApertureToDetector(vector2_t v);
  vector2_t DetectedAngleToSkyAngle(vector2_t v);
  vector2_t SkyAngleToDetectedAngle(vector2_t v);

  double UniformDistribution(double x1, double x2);


private:
  double detector_element_size_x_ = 1.0;
  double detector_element_size_y_ = 1.0;
  double aperture_element_size_x_ = 1.0;
  double aperture_element_size_y_ = 1.0;
  double sky_element_angle_x_ = 1.0;
  double sky_element_angle_y_ = 1.0;

  int num_sky_x_ = 0;
  int num_sky_y_ = 0;
  double sky_size_angle_x_ = 0.0;
  double sky_size_angle_y_ = 0.0;
  double detector_to_aperture_distance_ = 1.0;

  double detector_roll_angle_ = 0.0;
  double aperture_roll_angle_ = 0.0;
  vector2_t aperture_offset_ = vector2_t(0.0, 0.0);
  vector2_t sky_offset_angle_ = vector2_t(0.0, 0.0);
  int num_decoding_iteration_ = 1;
  int decoding_mode_ = 1;

  std::shared_ptr<image_t> aperture_pattern_;
  std::shared_ptr<image_t> decoder_array_;
  std::shared_ptr<image_t> encoded_image_;
  std::shared_ptr<image_t> decoded_image_;

  void decode_based_on_sky_coordinate();
  void decode_based_on_aperture_coordinate();

  std::mt19937 randomGenerator_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_CodedAperture_H */
