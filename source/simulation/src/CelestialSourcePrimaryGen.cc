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

#include "CelestialSourcePrimaryGen.hh"
#include "Randomize.hh"
#include "AstroUnits.hh"

namespace cfitsio
{
extern "C" {
#include "fitsio.h"
}
}

namespace
{

/**
 * rotate v as uz will be a new Z-axis.
 * this function is an inverse of G4ThreeVector::rotateUz(G4ThreeVector),
 * which rotates v as the current Z-axis into a uz.
 * The rotation matrix is given as the transpose of G4ThreeVector::rotateUz().
 */
void rotateCoordinateZ(G4ThreeVector& v, G4ThreeVector uz);

} /* anonymous namespace */


using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft
{

CelestialSourcePrimaryGen::CelestialSourcePrimaryGen()
  : fitsFilenameI_("imageI.fits"),
    fitsFilenameQ_("imageQ.fits"),
    fitsFilenameU_("imageU.fits")
{
  add_alias("CelestialSourcePrimaryGen");
}

CelestialSourcePrimaryGen::~CelestialSourcePrimaryGen() = default;

ANLStatus CelestialSourcePrimaryGen::mod_define()
{
  ANLStatus status = IsotropicPrimaryGen::mod_define();
  if (status != AS_OK) {
    return status;
  }
 
  define_parameter("fits_filename_I", &mod_class::fitsFilenameI_);
  set_parameter_description("Polarization I image file.");
  define_parameter("fits_filename_Q", &mod_class::fitsFilenameQ_);
  set_parameter_description("Polarization Q image file.");
  define_parameter("fits_filename_U", &mod_class::fitsFilenameU_);
  set_parameter_description("Polarization U image file.");
  define_parameter("input_image_rotation_angle", &mod_class::inputImageRotationAngle_, unit::degree, "degree");
  set_parameter_description("Rotation angle of the input image");
  define_parameter("detector_roll_angle", &mod_class::detectorRollAngle_, unit::degree, "degree");
  set_parameter_description("Roll angle of the detector with respect to north direction.");
  define_parameter("set_polarization", &mod_class::setPolarization_);
  set_parameter_description("If true, polarization is on.");

  undefine_parameter("flux");
  define_parameter("flux", &mod_class::sourceFlux_, (unit::erg/unit::cm2/unit::s), "erg/cm2/s");
  set_parameter_description("Total flux of the source. This parameter is used only for calculating real time correspoing to a simulation.");

  return AS_OK;
}

ANLStatus CelestialSourcePrimaryGen::mod_initialize()
{
  ANLStatus status = IsotropicPrimaryGen::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  inputImage(fitsFilenameI_, imageI_, &status);

  if (status != AS_OK) {
    return status;
  }
  pixelX_ = imageI_.shape()[0];
  pixelY_ = imageI_.shape()[1];

  if (setPolarization_) {
    inputImage(fitsFilenameQ_, imageQ_, &status);
    if (status != AS_OK) {
      return status;
    }
    inputImage(fitsFilenameU_, imageU_, &status);
    if (status != AS_OK) {
      return status;
    }
    const int qx = imageQ_.shape()[0];
    const int qy = imageQ_.shape()[1];
    const int ux = imageU_.shape()[0];
    const int uy = imageU_.shape()[1];
    if (qx!=pixelX_ || qy!=pixelY_ || ux!=pixelX_ || uy!=pixelY_) {
      std::cout << "ERROR: The sizes of 3 images are not same." << std::endl;
      return AS_QUIT_ERROR;
    }
  }
  else {
    imageQ_.resize(boost::extents[pixelX_][pixelY_]);
    imageU_.resize(boost::extents[pixelX_][pixelY_]);
    for (int ix=0; ix<pixelX_; ix++) {
      for (int iy=0; iy<pixelY_; iy++) {
        imageQ_[ix][iy] = 0.0;
        imageU_[ix][iy] = 0.0;
      }
    }
  }

  makePolarizationMap(&status);
  if (status != AS_OK) {
    std::cout << "ERROR: Polarization map not correct." << std::endl;
    return status;
  }

  setCoordinate(&status);
  if (status != AS_OK) {
    return status;
  }
  
  buildPixelIntegral();

  return AS_OK;
}

void CelestialSourcePrimaryGen::makePrimarySetting()
{
  using std::cos;
  using std::sin;
  using std::sqrt;
  using std::atan2;

  std::pair<int, int> currentPixel = samplePixel();
  const int ix = currentPixel.first;
  const int iy = currentPixel.second;
  const double d = Distance();
  G4ThreeVector centerDirection = CenterDirection();
  const G4ThreeVector centerPosition = CenterPosition();
  const double radius = Radius();

  const double cosTheta = cos(-imageDec_[ix][iy] + 0.5*CLHEP::pi);
  const double sinTheta = sqrt(1.0-cosTheta*cosTheta);
  const double phi = imageRA_[ix][iy];
  G4ThreeVector v(d*sinTheta*cos(phi), d*sinTheta*sin(phi), d*cosTheta);
  G4ThreeVector initialPosition = v;
  rotateCoordinateZ(v, -centerDirection);

  G4ThreeVector v2 = v.orthogonal();
  v2.setMag( radius * sqrt(G4UniformRand()) );
  const G4double chi = CLHEP::twopi * G4UniformRand();
  v2.rotate(chi, v);
  G4ThreeVector position = centerPosition + v + v2;

  const double energy = sampleEnergy();
  G4ThreeVector direction = (-v).unit();


  G4ThreeVector uz(0.0, 0.0, 1.0);
  G4ThreeVector uy(0.0, 1.0, 0.0);
  G4ThreeVector newUz = uz;
  rotateCoordinateZ(newUz, -centerDirection);
  G4ThreeVector north(newUz.x(), newUz.y(), 0.0);
  north = north.unit();
  G4ThreeVector dety = north.rotate(detectorRollAngle_, -uz);
  const double detAngle = atan2(dety.y(), dety.x()) - 0.5*CLHEP::pi;
  position.rotate(-detAngle, uz);
  direction.rotate(-detAngle, uz);

  if (!setPolarization_) {
    setPrimary(position, energy, direction);
    return;
  }

  const double polDegree = polarizationDegree_[ix][iy];
  const double polAngle = polarizationAngle_[ix][iy];

#if 0
  std::cout << "position: (" << position.x() << ", " << position.y() << ", " << position.z() << ")" << std::endl;
  std::cout << "direction: (" << direction.x() << ", " << direction.y() << ", " << direction.z() << ")" << std::endl; 
#endif

  G4ThreeVector polarizationVector(sin(phi), -cos(phi), 0.0);
  polarizationVector.rotate(inputImageRotationAngle_ + polAngle, -initialPosition);
  rotateCoordinateZ(polarizationVector, -centerDirection);
  polarizationVector.rotate(-detAngle, uz);

#if 0
  std::cout << "polarization degree: " << polDegree << std::endl;
  std::cout << polAngle << std::endl;
  std::cout << "polarization: (" << polarizationVector.x() << ", " << polarizationVector.y() << ", " << polarizationVector.z() << ")" << std::endl; 
#endif

  if (G4UniformRand() < polDegree) {
    setPrimary(position, energy, direction, polarizationVector);
  }
  else {
    setPrimary(position, energy, direction);
    setUnpolarized();
  }
}

ANLStatus CelestialSourcePrimaryGen::mod_end_run()
{
  const double radius = Radius();
  const double area = CLHEP::pi*radius*radius;
  const double realTime = TotalEnergy()/(sourceFlux_*area);
  const double pflux = Number()/area/realTime;

  setRealTime(realTime);

  std::cout.setf(std::ios::scientific);
  std::cout << "CelestialSourcePrimaryGen::mod_end_run \n"
            << "  Number: " << Number() << "\n"
            << "  Flux: " << sourceFlux_/(unit::erg/unit::cm2/unit::s) << " erg/cm2/s\n"
            << "  Total Energy: " << TotalEnergy()/unit::keV << " keV = "
            << TotalEnergy()/unit::erg << " erg\n"
            << "  Area: " << area/unit::cm2 << " cm2\n"
            << "  Real time: " << realTime/unit::s << " s\n"
            << "  Photon flux: " << pflux/(1.0/unit::cm2/unit::s) << " photons/cm2/s\n"
            << std::endl;
  std::cout.unsetf(std::ios::scientific);

  return AS_OK;
}

void CelestialSourcePrimaryGen::inputImage(std::string filename, image_t& image, ANLStatus* status)
{
  cfitsio::fitsfile* fitsFile = nullptr;
  int fitsStatus = 0;
  *status = AS_OK;
  int maxdim = 2;
  long naxes[2] = {0, 0};
  long startPixel[2] = {1, 1};
  int numPixels = 1;
  int nulval = 0;
  int anynul = 0;
  std::vector<double> array;

  cfitsio::fits_open_image(&fitsFile, filename.c_str(), READONLY, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    *status = AS_QUIT_ERROR;
    return;
  }
  cfitsio::fits_get_img_size(fitsFile, maxdim, naxes, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    *status = AS_QUIT_ERROR;
    return;
  }
  image.resize(boost::extents[naxes[0]][naxes[1]]);
  numPixels = naxes[0] * naxes[1];
  array.resize(numPixels, 0.0);

  cfitsio::fits_read_pix(fitsFile, TDOUBLE, startPixel, numPixels, &nulval, &array[0], &anynul, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    *status = AS_QUIT_ERROR;
    return;
  }

  int i = 0;
  for (int ix=0; ix<naxes[0]; ix++) {
    for (int iy=0; iy<naxes[1]; iy++) {
      image[ix][iy] = array[i];
      i++;
    }
  }

  cfitsio::fits_close_file(fitsFile, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    *status = AS_QUIT_ERROR;
    return;
  }
}

void CelestialSourcePrimaryGen::makePolarizationMap(ANLStatus* status)
{
  polarizationDegree_.resize(boost::extents[pixelX_][pixelY_]);
  polarizationAngle_.resize(boost::extents[pixelX_][pixelY_]);
  for (int ix=0; ix<pixelX_; ix++) {
    for (int iy=0; iy<pixelY_; iy++) {
      polarizationDegree_[ix][iy] = 0.0;
      polarizationAngle_[ix][iy] = 0.0;
    }
  }
  *status = AS_OK;
  const double eps = 1E-9;
  for (int ix=0; ix<pixelX_; ix++) {
    for (int iy=0; iy<pixelY_; iy++) {
      const double I = imageI_[ix][iy];
      const double Q = imageQ_[ix][iy];
      const double U = imageU_[ix][iy];
      if (I*I+eps<Q*Q+U*U) {
        *status = AS_QUIT_ERROR;
        std::cout << "x=" << ix << " y=" << iy << std::endl;
        std::cout << "I=" << I << " Q=" << Q << " U=" << U << std::endl;
        return;
      }
      if (I==0.0) continue;
      polarizationDegree_[ix][iy] = std::sqrt(Q*Q + U*U) / I;
      if (U==0.0 && Q==0.0) continue;
      polarizationAngle_[ix][iy] = 0.5 * std::atan2(U, Q);
    }
  }
}

void CelestialSourcePrimaryGen::setCoordinate(ANLStatus* status)
{
  using std::sin;
  using std::cos;

  *status = AS_OK;
  imageRA_.resize(boost::extents[pixelX_][pixelY_]);
  imageDec_.resize(boost::extents[pixelX_][pixelY_]);
  cfitsio::fitsfile* fitsFile = nullptr;
  int fitsStatus = 0;
  std::vector<std::string> key = {"CRPIX1", "CRPIX2", "CRVAL1", "CRVAL2", "CDELT1", "CDELT2"};
  const int numkey = key.size();
  std::vector<double> keyvalue = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  cfitsio::fits_open_image(&fitsFile, fitsFilenameI_.c_str(), READONLY, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    *status = AS_QUIT_ERROR;
    return;
  }

  for (int i=0; i<numkey; i++) {
    cfitsio::fits_read_key(fitsFile, TDOUBLE, key[i].c_str(), &keyvalue[i], nullptr, &fitsStatus);
    if (fitsStatus) {
      cfitsio::fits_report_error(stderr, fitsStatus);
      *status = AS_QUIT_ERROR;
      return;
    }
  }

  cfitsio::fits_close_file(fitsFile, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    *status = AS_QUIT_ERROR;
    return;
  }

  const int refX = static_cast<int>(keyvalue[0]);
  const int refY = static_cast<int>(keyvalue[1]);
  const double refRA = keyvalue[2] * unit::degree;
  const double refDec = keyvalue[3] * unit::degree;
  const double dx = keyvalue[4] * unit::degree;
  const double dy = keyvalue[5] * unit::degree;
  const double rollAngle = inputImageRotationAngle_;
  degPixelX_ = dx;
  degPixelY_ = dy;
  
  for (int ix=0; ix<pixelX_; ix++) {
    for (int iy=0; iy<pixelY_; iy++) {
      double ra = refRA + (ix-refX) * dx * cos(rollAngle) + (iy-refY) * dy * sin(rollAngle);
      double dec = refDec + (ix-refX) * dx * sin(rollAngle) + (iy-refY) * dy * cos(rollAngle);
      imageRA_[ix][iy] = ra;
      imageDec_[ix][iy] = dec;
    }
  }

}

void CelestialSourcePrimaryGen::buildPixelIntegral()
{
  const int nbins = pixelX_*pixelY_;
  pixelIntegral_.resize(nbins+1);
  pixelIntegral_[0] = 0.0;
  int i = 0;
  for (int ix=0; ix<pixelX_; ix++) {
    for (int iy=0; iy<pixelY_; iy++) {
      pixelIntegral_[i+1] = pixelIntegral_[i] + imageI_[ix][iy];
      i++;
    }
  }
  const double norm = pixelIntegral_.back();
  for (auto& v: pixelIntegral_) {
    v /= norm;
  }
}

std::pair<int, int> CelestialSourcePrimaryGen::samplePixel()
{
  std::pair<int, int> p;
  const double r = G4UniformRand();
  const std::vector<double>::const_iterator it = std::upper_bound(pixelIntegral_.begin(), pixelIntegral_.end(), r);
  const int r0 = it - pixelIntegral_.begin();
  p.first = r0%pixelX_;
  p.second = r0/pixelX_;
  return p;
}

} /* namespace comptonsoft */

namespace
{

void rotateCoordinateZ(G4ThreeVector& v, G4ThreeVector uz)
{
  uz = uz.unit();
  const double u1 = uz.x();
  const double u2 = uz.y();
  const double u3 = uz.z();
  const double up2 = u1*u1 + u2*u2;

  if (up2>0) {
    const double up = std::sqrt(up2);
    const double vx = v.x();
    const double vy = v.y();
    const double vz = v.z();
    v.setX( u1*u3*vx/up + u2*u3*vy/up - up*vz);
    v.setY(-u2*vx/up    + u1*vy/up           );
    v.setZ( u1*vx       + u2*vy       + u3*vz);
  }
  else if (u3 < 0.) {
    v.setX(-v.x());
    v.setZ(-v.z());
  }
}

} /* anonymous namespace */
