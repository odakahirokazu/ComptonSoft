#ifndef COMPTONSOFT_LMMLEM_hh
#define COMPTONSOFT_LMMLEM_hh 1
#include "ImageReconstructionAlgorithm.hh"
#include <utility>
#include <vector>

namespace comptonsoft {

/**
 * List-mode maximum likelihood expectation maximization (LM-MLEM).
 * One ANL run performs one iteration:
 *   lambda_j <- lambda_j / s_j * sum_i t_ij / (sum_k t_ik lambda_k)
 * t_ij is an analytic Gaussian kernel of ARM with a fixed width.
 */
class LMMLEM : public ImageReconstructionAlgorithm
{
  DEFINE_ANL_SUBMODULE(LMMLEM, 1.0);

public:
  LMMLEM();
  ~LMMLEM();

  void define_parameters() override;

  void initialize(const HealPixImageSpace& space,
                  const std::vector<double>& image,
                  const std::vector<double>& sensitivity) override;
  void calculate(const std::vector<BasicComptonEvent_sptr>& events) override;
  void update(std::vector<double>& updated_image) override;

  double log_likelihood() const override { return likelihood_; }
  long int num_events() const override { return num_events_; }
  long int num_skipped_events() const override { return num_skipped_events_; }

private:
  /**
   * append the system response (pixel index, t_ij) of one hypothesis.
   * @return false if no pixel has non-zero response.
   */
  bool fillResponse(const HealPixImageSpace& space,
                    const BasicComptonEvent& event,
                    double weight,
                    std::vector<std::pair<int, double>>& response) const;

private:
  // parameters
  double arm_sigma_;
  double cutoff_sigma_;

  // state
  const HealPixImageSpace* space_;
  std::vector<double> image_;          // current (prior) image lambda_j
  std::vector<double> sensitivity_;    // sensitivity s_j
  std::vector<double> backprojection_; // accumulated back projection
  double likelihood_;
  long int num_events_;
  long int num_skipped_events_;
};

} // namespace comptonsoft
#endif // COMPTONSOFT_LMMLEM_hh
