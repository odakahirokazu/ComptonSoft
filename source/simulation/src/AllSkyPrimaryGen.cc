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

#include "AllSkyPrimaryGen.hh"
#include "Randomize.hh"
#include "AstroUnits.hh"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft
{

AllSkyPrimaryGen::AllSkyPrimaryGen()
  : filename_("allsky.fits"),
    hdu_index_map_(2),
    hdu_index_energy_(3),
    origin_longitude_(0.0),
    origin_latitude_(0.0),
    pole_longitude_(0.0),
    pole_latitude_(90.0*unit::degree)
//    source_flux_(1.0)
{
  add_alias("AllSkyPrimaryGen");
}

AllSkyPrimaryGen::~AllSkyPrimaryGen() = default;

ANLStatus AllSkyPrimaryGen::mod_define()
{
  ANLStatus status = IsotropicPrimaryGen::mod_define();
  if (status != AS_OK) {
    return status;
  }
 
  define_parameter("filename", &mod_class::filename_);
  define_parameter("hdu_index_map", &mod_class::hdu_index_map_);
  define_parameter("hdu_index_energy", &mod_class::hdu_index_energy_);
  define_parameter("origin_longitude_", &mod_class::origin_longitude_);
  define_parameter("origin_latitude_", &mod_class::origin_latitude_);
  define_parameter("pole_longitude_", &mod_class::pole_longitude_);
  define_parameter("pole_latitude_", &mod_class::pole_latitude_);

  return AS_OK;
}

ANLStatus AllSkyPrimaryGen::mod_initialize()
{
  ANLStatus status = IsotropicPrimaryGen::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  std::unique_ptr<fitshandle> fits(new fitshandle);
  fits->open(filename_);
  fits->goto_hdu(hdu_index_map_);
  fits->get_key("MAPMODE", map_mode_);
  fits->get_key("NMAP", num_maps_);

#if 0
  std::cout << "MAPMODE : " << map_mode_ << std::endl;
  std::cout << "NMAP: " << num_maps_ << std::endl;
#endif

  if (map_mode_ == "multiband") {
    loadMultiBandImages(fits.get(), num_maps_, status);
  }
#if 0
  else if (map_mode_ == "mono") {
    loadMonochromaticImage(fits.get(), num_maps_, status);
  }
  else if (map_mode_ == "powerlaw") {
    loadPowerLawImage(fits.get(), num_maps_, status);
  }
#endif
  else {
    std::cout << "Undefined map mode: " << map_mode_ << std::endl;
    status = AS_QUIT_ERROR;
  }
  fits->close();
  if (status != AS_OK) { return status; }

  constructMaps(status); 
  if (status != AS_OK) { return status; }

  calculateMapIntegrals(status);
  if (status != AS_OK) { return status; }

  setCoordinate(status);
  if (status != AS_OK) { return status; }

  return status;
}

void AllSkyPrimaryGen::makePrimarySetting()
{
  using std::cos;
  using std::sin;
  using std::sqrt;

  const int band_index_ = sampleBandIndex();
  const int ipix_ = samplePixel(band_index_);

  const double theta = image_theta[ipix_];
  const double phi = image_phi[ipix_];

  const double d = Distance();
  G4ThreeVector centerDirection = CenterDirection();
  const G4ThreeVector centerPosition = CenterPosition();
  const double radius = Radius();

  G4ThreeVector v( d * sin(theta) * cos(phi), 
                   d * sin(theta) * sin(phi), 
                   d * cos(theta) );

  G4ThreeVector v2 = v.orthogonal();
  v2.setMag( radius * sqrt(G4UniformRand()) );
  const G4double chi = CLHEP::twopi * G4UniformRand();
  v2.rotate(chi, v);
  G4ThreeVector position = centerPosition + v + v2;

  // set spectrum
  const double photon_index_ = band_maps_[band_index_][ipix_].photon_index; // photon index at the current pix
  const double emin_ = band_maps_[band_index_][ipix_].emin;
  const double emax_ = band_maps_[band_index_][ipix_].emax;
  const double energy = sampleFromPowerLaw( photon_index_, emin_, emax_ );

  G4ThreeVector direction = (-v).unit();

  setPrimary(position, energy, direction);
  return;

#if 0
  setUnpolarized();
#endif
}

ANLStatus AllSkyPrimaryGen::mod_end_run()
{
  double totalPhotonFlux_ = 0;
  for (int i = 0; i < num_bands_; i++) {
    for (int ipix = 0; ipix < num_pixel_; ipix++) {
      totalPhotonFlux_ += band_maps_[i][ipix].integrated_intensity * (1.0/unit::cm2/unit::s);
    }
  }
  const double radius = Radius();
  const double area = CLHEP::pi*radius*radius;
  const double realTime = Number()/(totalPhotonFlux_*area);

  setRealTime(realTime);

  std::cout.setf(std::ios::scientific);
  std::cout << "AllSkyPrimaryGen::mod_end_run \n"
            << "  Input file: " << filename_ << "\n"
            << "  Number (event number; photon number): " << Number() << "\n"
            << "  Total Energy (=sum of sampled photon energy): " << TotalEnergy()/unit::keV << " keV = "
            << TotalEnergy()/unit::erg << " erg\n"
            << "  Area: " << area/unit::cm2 << " cm2\n"
            << "  Real time : " << realTime/unit::s << " s\n"
            << "  Photon flux in " << band_maps_[0][0].emin / unit::keV << " to " << band_maps_[num_bands_-1][0].emax / unit::keV << " keV: " << totalPhotonFlux_/(1.0/unit::cm2/unit::s) << " photons/cm2/s\n"
            << std::endl;
  std::cout.unsetf(std::ios::scientific);

  return AS_OK;
}

void AllSkyPrimaryGen::loadMultiBandImages(fitshandle* fits, int num_maps_, ANLStatus& status)
{
  //read allsky map
  maps_.resize(num_maps_);
  energies_.resize(num_maps_);

  fits->goto_hdu(hdu_index_map_);
  for (int i=0; i<num_maps_; i++) {
    read_Healpix_map_from_fits(*fits, maps_[i], i+1);
#if 1
  std::cout << "Read all sky map at index " << i << std::endl;
  std::cout << "  colname = " << fits->colname(i+1) << std::endl;
  std::cout << "  unit = " << fits->colunit(i+1) << std::endl;
  std::cout << "  column = " << i << std::endl;
  std::cout << "  hdunum = " << hdu_index_map_ << std::endl;
  std::cout << "  Nside = " << maps_[i].Nside() << std::endl;
  std::cout << "  Npix  = " << maps_[i].Npix() << std::endl;
  std::cout << std::endl;
#endif
  }

  //read energy range
  /*
  //for the old format
  fits->goto_hdu(hdu_index_map_);
  for (int i=0; i<num_maps_; i++) {
    double this_energy;
    fits->get_key("ENE"+std::to_string(i+1), this_energy);
    energies_[i] = this_energy * unit::keV;
#if 1
    std::cout << "Energy at index " << i << " = " << energies_[i] / unit::keV << " keV" << std::endl;
    std::cout << std::endl;
#endif
  }
  */

  //for the new format
  fits->goto_hdu(hdu_index_energy_);
  fits->read_column(1, energies_);
  for (int i=0; i<num_maps_; i++) {
    energies_[i] *= unit::keV;
  }

#if 1
  std::cout << "Read the energy range in the map file" << std::endl;
  for (int i=0; i<num_maps_; i++) {
    std::cout << "  Energy at index " << i << " = " << energies_[i] / unit::keV << " keV" << std::endl;
  }
  std::cout << std::endl;
#endif

  //remove the maps outside the defined energy range
  auto it_maps_ = maps_.end();
  auto it_energies_ = energies_.end();
  for (int i=0; i<num_maps_; i++) {
    it_maps_ -= 1;
    it_energies_ -= 1;
    if( *(it_energies_) < getEnergyMin() || getEnergyMax() < *(it_energies_) )
    {
      maps_.erase(it_maps_);
      energies_.erase(it_energies_);
    }
  }
  maps_.shrink_to_fit();
  energies_.shrink_to_fit();

  //check the number of the filtered maps
  if( maps_.size() <= 1 )
  {
    std::cerr << "Error: The number of the maps in the user-defined energy range is less than 2." << std::endl;
    status = AS_QUIT_ERROR;
    return;
  }

  //determine the basic quantities
  num_bands_ = maps_.size() - 1;
  num_pixel_ = maps_[0].Npix();
  num_side_ = maps_[0].Nside();
  pixel_area_ = 4 * M_PI / num_pixel_;
#if 1
  std::cout << "The information about the filtered maps" << std::endl;
  std::cout << "  the number of the energy bands = " << num_bands_ << std::endl;
  std::cout << "  the energy range:" << std::endl;
  for (int i = 0; i < num_bands_; i++) {
    double emin_ = energies_[i];
    double emax_ = energies_[i+1];
    std::cout << "    " << i << " : " << emin_/unit::keV << " - " << emax_/unit::keV << " (keV)" << std::endl;
  }
  std::cout << std::endl;
#endif

  status = AS_OK;
}

void AllSkyPrimaryGen::constructMaps(ANLStatus& status)
{
  if (map_mode_ == "multiband") {
    constructMapsMultiBand(status);
  }
#if 0
  else if (map_mode_ == "mono") {
    //TBD
  }
  else if (map_mode_ == "powerlaw") {
    //TBD
  }
#endif
  else {
    std::cout << "Undefined map mode: " << map_mode_ << std::endl;
    status = AS_QUIT_ERROR;
  }
}

void AllSkyPrimaryGen::constructMapsMultiBand(ANLStatus& status)
{
  band_maps_.resize(num_bands_);

  for (int i = 0; i < num_bands_; i++) {
    double emin_ = energies_[i];
    double emax_ = energies_[i+1];
    band_maps_[i].SetNside(num_side_, RING); 

    for (int ipix = 0; ipix < num_pixel_; ipix++) {
      double nmin_ = maps_[i][ipix];
      double nmax_ = maps_[i+1][ipix];

      double photon_index_ = -1 * log(nmax_/nmin_) / log(emax_/emin_);

      double intg_1 = nmin_ * emin_ / (-photon_index_+1);
      double intg_2 = nmin_ * pow ( 1 / emin_, (-1.0)*photon_index_) * pow (emax_, -photon_index_+1) / (-photon_index_+1);
      double integrated_intensity_ = (intg_2 - intg_1) * pixel_area_ * (unit::MeV / unit::keV);
      
      band_maps_[i][ipix] = {emin_, emax_, photon_index_, integrated_intensity_};
    }
  }

  status = AS_OK;
}

void AllSkyPrimaryGen::calculateMapIntegrals(ANLStatus& status)
{
  band_integrals_.resize(num_bands_ + 1);
  band_integrals_[0] = 0.0;
  pixel_integrals_.resize(num_bands_);

  for(int i_band = 0; i_band < num_bands_; ++i_band){
    pixel_integrals_[i_band].resize(num_pixel_+1);
    pixel_integrals_[i_band][0] = 0.0;

    for(int ipix = 0; ipix < num_pixel_; ++ipix){
      pixel_integrals_[i_band][ipix + 1] += pixel_integrals_[i_band][ipix] + band_maps_[i_band][ipix].integrated_intensity;
    }

    const double pixel_integrals_norm = pixel_integrals_[i_band].back();
    band_integrals_[i_band + 1] += band_integrals_[i_band] + pixel_integrals_norm;

    for (auto& v: pixel_integrals_[i_band] ) {
      v /= pixel_integrals_norm;
    }
  }

  const double band_integrals_norm = band_integrals_.back();
  for (auto& v: band_integrals_) {
    v /= band_integrals_norm;
  }

#if 0
  std::cout << "band_integrals_: " << band_integrals_.front() << " " << band_integrals_.back() << " " 
                                   << band_integrals_.size() << std::endl;
  std::cout << "pixel_integrals_: " << pixel_integrals_.size() << std::endl;
  std::cout << "pixel_integrals_[0]: " << pixel_integrals_[0].front() << " " << pixel_integrals_[0].back() << " " 
                                   << pixel_integrals_[0].size() << std::endl;
#endif

  status = AS_OK;
}

int AllSkyPrimaryGen::sampleBandIndex()
{
  const double r = G4UniformRand();
  const std::vector<double>::const_iterator it = std::upper_bound(band_integrals_.begin(), 
                                                                  band_integrals_.end(), r);
  const int r0 = it - band_integrals_.begin() - 1;
  return r0;
}

int AllSkyPrimaryGen::samplePixel(int band_index_)
{
  const double r = G4UniformRand();
  const std::vector<double>::const_iterator it = std::upper_bound(pixel_integrals_[band_index_].begin(), 
                                                                  pixel_integrals_[band_index_].end(), r);
  const int r0 = it - pixel_integrals_[band_index_].begin() - 1;
  return r0;
}

void AllSkyPrimaryGen::setCoordinate(ANLStatus& status)
{
  using std::sin;
  using std::cos;

  double z_ ;
  double phi_ ;
  double theta_ ;

  G4RotationMatrix r;
  G4ThreeVector newX( sin(TMath::Pi()/2 - origin_latitude_) * cos(origin_longitude_),
                      sin(TMath::Pi()/2 - origin_latitude_) * sin(origin_longitude_),
                      cos(TMath::Pi()/2 - origin_latitude_) );
  G4ThreeVector newZ( sin(TMath::Pi()/2 - pole_latitude_) * cos(pole_longitude_),
                      sin(TMath::Pi()/2 - pole_latitude_) * sin(pole_longitude_),
                      cos(TMath::Pi()/2 - pole_latitude_) );
  if( abs(newX.dot(newZ)) > threshold_zero_angle )
  {
    std::cerr << "Error: The origin is not perpendicular to the pole." << std::endl;
    status = AS_QUIT_ERROR;
    return;
  }
  G4ThreeVector newY = newZ.cross(newX);
  r.rotateAxes(newX, newY, newZ);

  image_phi.resize(num_pixel_);
  image_theta.resize(num_pixel_);
  
  for(int ipix_ = 0; ipix_ < num_pixel_; ++ipix_){
  	maps_[0].pix2zphi( ipix_, z_, phi_ );
  	theta_ = acos( z_ );
    
    G4ThreeVector v( sin(theta_) * cos(phi_),
                     sin(theta_) * sin(phi_),
                     cos(theta_) );
    G4ThreeVector new_v = convertCoordinate( r, v );

  	image_theta[ipix_] = new_v.getTheta();
    image_phi[ipix_] = new_v.getPhi();
  }

  status = AS_OK;
}

G4ThreeVector AllSkyPrimaryGen::convertCoordinate(const G4RotationMatrix r, G4ThreeVector vector)
{
  return r * vector; 
}

} /* namespace comptonsoft */
