#ifndef COMPTONSOFT_LArRecombinationModelFactory_HH
#define COMPTONSOFT_LArRecombinationModelFactory_HH 1
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
 * @date 2026-07-02 | Shota Arai | Rename
 */
class LArRecombinationModelFactory {
public:
  static std::unique_ptr<VLArRecombinationModel> load(const std::string &filename);
  static std::unique_ptr<VLArRecombinationModel> create(RecombinationModelType type, const std::map<std::string, double> &params);
};
} /* namespace comptonsoft */

#endif //COMPTONSOFT_LArRecombinationModelFactory_HH