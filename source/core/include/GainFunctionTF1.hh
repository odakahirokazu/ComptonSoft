#ifndef COMPTONSOFT_GainFunctionTF1_H
#define COMPTONSOFT_GainFunctionTF1_H 1
#include "VGainFunction.hh"
#include "TF1.h"

class TF1;

namespace comptonsoft {
/**
 * A class of gain function based on TF1.
 * @author Shota Arai
 * @date 2026-04-18
 */
class GainFunctionTF1 : public VGainFunction {
public:
  GainFunctionTF1();
  virtual ~GainFunctionTF1();
  GainFunctionTF1(const GainFunctionTF1&) = default;
  GainFunctionTF1(GainFunctionTF1&&) = default;
  GainFunctionTF1& operator=(const GainFunctionTF1&) = default;
  GainFunctionTF1& operator=(GainFunctionTF1&&) = default;

  double eval(double x) const override;
  double RangeMin() const override;
  double RangeMax() const override;
  
  void set(const TF1 *func) { func_ = func; }

private:
  const TF1* func_;
};
} // namespace comptonsoft

#endif //COMPTONSOFT_GainFunctionTF1_H