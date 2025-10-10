#ifndef COMPTONSOFT_VLArRecombinationModelFactory_HH
#define COMPTONSOFT_VLArRecombinationModelFactory_HH 1
#include "VLArRecombinationModel.hh"

namespace comptonsoft {
enum class RecombinationModelType {
  Birks,
  ModifiedBox,
  None
};
/**
 * A factory class to create LAr recombination models.
 * @author Shota Arai
 * @date 2025-10-10
 */
class VLArRecombinationModelFactory {
public:
  static std::unique_ptr<VLArRecombinationModel> load(const std::string &filename);
  static std::unique_ptr<VLArRecombinationModel> create(RecombinationModelType type, const std::map<std::string, double> &params);
};
} /* namespace comptonsoft */

#endif //COMPTONSOFT_VLArRecombinationModelFactory_HH