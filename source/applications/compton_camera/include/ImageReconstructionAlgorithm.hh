#ifndef COMPTONSOFT_ImageReconstructionAlgorithm_hh
#define COMPTONSOFT_ImageReconstructionAlgorithm_hh 1
#include "BasicComptonEvent.hh"
#include "CSTypes.hh"
#include "anlnext/BasicSubModule.hh"
#include "healpix_cxx/healpix_base.h"
#include <vector>

namespace comptonsoft {

/**
 * HEALPix image space in a sky frame defined by (xaxis, yaxis, zaxis).
 * The convention is the same as BackProjectionHealPix:
 *   theta = acos(u.yaxis), phi = atan2(u.xaxis, u.zaxis)
 * so that the HEALPix vector is (u.zaxis, u.xaxis, u.yaxis).
 */
class HealPixImageSpace
{
public:
  HealPixImageSpace(int nside, const vector3_t& xaxis, const vector3_t& yaxis, const vector3_t& zaxis)
    : base_(nside, RING, SET_NSIDE),
      xaxis_(xaxis),
      yaxis_(yaxis),
      zaxis_(zaxis)
  {
  }

  const Healpix_Base& base() const { return base_; }
  int num_pixels() const { return base_.Npix(); }

  /** convert a direction in the world frame into a HEALPix vector */
  vec3 to_healpix_vector(const vector3_t& u) const { return vec3(u.dot(zaxis_), u.dot(xaxis_), u.dot(yaxis_)); }

  /** convert a HEALPix vector into a direction in the world frame */
  vector3_t to_world_vector(const vec3& v) const { return v.x * zaxis_ + v.y * xaxis_ + v.z * yaxis_; }

private:
  Healpix_Base base_;
  vector3_t xaxis_;
  vector3_t yaxis_;
  vector3_t zaxis_;
};

/**
 * Base class of image reconstruction algorithms (submodule of ImageReconstructionModule).
 * An algorithm holds its own state during one ANL run.
 * One ANL run corresponds to one update of the image.
 */
class ImageReconstructionAlgorithm : public anlnext::BasicSubModule
{
  DEFINE_ANL_SUBMODULE(ImageReconstructionAlgorithm, 1.0);

public:
  ImageReconstructionAlgorithm() = default;
  ~ImageReconstructionAlgorithm() = default;

  /**
   * called at the beginning of the run.
   * @param space image space (must be alive until the end of the run)
   * @param image current (prior) image
   * @param sensitivity sensitivity map
   */
  virtual void initialize(const HealPixImageSpace& /* space */, const std::vector<double>& /* image */,
                          const std::vector<double>& /* sensitivity */)
  {
  }

  /**
   * called for each physical event.
   * @param events reconstructed Compton events (hypotheses) of one physical event.
   *        Each hypothesis is weighted by ReconstructionFraction().
   */
  virtual void calculate(const std::vector<BasicComptonEvent_sptr>& /* events */) {}

  /**
   * called at the end of the run.
   * @param updated_image updated image is stored.
   */
  virtual void update(std::vector<double>& /* updated_image */) {}

  virtual double log_likelihood() const { return 0.0; }
  virtual long int num_events() const { return 0; }
  virtual long int num_skipped_events() const { return 0; }
};

} // namespace comptonsoft
#endif // COMPTONSOFT_ImageReconstructionAlgorithm_hh
