#ifndef COMPTONSOFT_ImageReconstructionModule_hh
#define COMPTONSOFT_ImageReconstructionModule_hh 1
#include "ImageReconstructionAlgorithm.hh"
#include "anlnext/BasicModule.hh"
#include <memory>
#include <string>
#include <vector>

template <typename T> class Healpix_Map;

namespace comptonsoft {

class EventReconstruction;

/**
 * Iterative image reconstruction on a HEALPix map.
 * One ANL run performs one iteration: the prior image is read from a FITS file
 * (or uniform if not given) and the updated image is written to a FITS file.
 * The algorithm is given as a submodule "algorithm" (default: LMMLEM).
 */
class ImageReconstructionModule : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(ImageReconstructionModule, 1.0);

public:
  ImageReconstructionModule();
  ~ImageReconstructionModule();

  void set_submodule_by_key(const std::string& name, const std::string& key) override;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

private:
  bool readMap(const std::string& filename, std::vector<double>& values) const;
  void writeImage(const std::vector<double>& values) const;

private:
  // parameters
  int nside_;
  double image_center_theta_;
  double image_center_phi_;
  double image_yaxis_theta_;
  double image_yaxis_phi_;
  std::string input_image_;
  std::string sensitivity_map_;
  std::string output_image_;
  int iteration_;

  // state
  const EventReconstruction* event_reconstruction_;
  ImageReconstructionAlgorithm* algorithm_;
  std::unique_ptr<HealPixImageSpace> space_;
};

} // namespace comptonsoft
#endif // COMPTONSOFT_ImageReconstructionModule_hh
