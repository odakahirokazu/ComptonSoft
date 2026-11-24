#ifndef COMPTONSOFTApplications_BackProjectionHealPix_hh
#define COMPTONSOFTApplications_BackProjectionHealPix_hh 1
#include "CSTypes.hh"
#include "anlnext/BasicModule.hh"
#include <memory>

template <typename T> class Healpix_Map;

namespace comptonsoft {
class EventReconstruction;
class SaveFits;

class BackProjectionHealPix : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(BackProjectionHealPix, 1.0);

public:
  BackProjectionHealPix();
  ~BackProjectionHealPix();

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

  const EventReconstruction* getEventReconstructionModule() const { return event_reconstruction_module_; }

  std::size_t NumPoints() const { return static_cast<std::size_t>(num_points_); }
  double ARMSpread() const { return arm_spread_; }

private:
  std::unique_ptr<Healpix_Map<double>> map_;
  int nside_;
  int num_points_;
  double arm_spread_;

  double image_center_theta_;
  double image_center_phi_;
  double image_yaxis_theta_;
  double image_yaxis_phi_;

  vector3_t xaxis_;
  vector3_t yaxis_;
  vector3_t zaxis_;

  const EventReconstruction* event_reconstruction_module_; 
  SaveFits* save_fits_module_;
  std::string fits_filename_ = "Backprojection.fits";
};
} // namespace comptonsoft
#endif // COMPTONSOFTApplications_BackProjectionHealPix_hh