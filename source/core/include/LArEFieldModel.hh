#ifndef COMPTONSOFT_LArEFieldModel_hh
#define COMPTONSOFT_LArEFieldModel_hh 1
#include "EFieldModel.hh"
namespace comptonsoft {
class LArEFieldModel: public EFieldModel {
public:
  LArEFieldModel() = default;
  ~LArEFieldModel() override = default;
  LArEFieldModel(const LArEFieldModel &) = default;
  LArEFieldModel(LArEFieldModel &&) = default;
  LArEFieldModel &operator=(const LArEFieldModel &) = default;
  LArEFieldModel &operator=(LArEFieldModel &&) = default;

  double CCE(double z) const override;
};
inline double LArEFieldModel::CCE(double z) const {
  const double cce = CCEOfElectron(z);
  return cce;
}
} // namespace comptonsoft

#endif //COMPTONSOFT_LArEFieldModel_hh