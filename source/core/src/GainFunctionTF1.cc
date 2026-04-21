#include "GainFunctionTF1.hh"
namespace comptonsoft {
GainFunctionTF1::GainFunctionTF1(): func_(nullptr) {}
GainFunctionTF1::~GainFunctionTF1() = default;

double GainFunctionTF1::eval(double x) const {
  return func_->Eval(x);
}

double GainFunctionTF1::RangeMin() const {
  return func_->GetXmin();
}

double GainFunctionTF1::RangeMax() const {
  return func_->GetXmax();
}
} // namespace comptonsoft