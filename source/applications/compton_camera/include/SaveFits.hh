#ifndef ComptonSoftApplications_SaveFits_hh
#define ComptonSoftApplications_SaveFits_hh 1
#include "anlnext/BasicModule.hh"
#include "fitshandle.h"
#include "healpix_map.h"
#include <memory>
#include <string>
namespace comptonsoft {
class SaveFits : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(SaveFits, 1.0);

public:
  SaveFits();
  ~SaveFits();

  template <typename T> void writeMap(const Healpix_Map<T>& map, PDT datatype);
  std::string Filename() const { return filename_; }
  void setFilename(const std::string& v) { filename_ = v; }
  int ColNum() const { return col_num_; }

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_finalize() override;

private:
  std::unique_ptr<fitshandle> handle_;
  std::string filename_;
  int col_num_;
};

template <typename T> void SaveFits::writeMap(const Healpix_Map<T>& map, PDT datatype)
{
  write_Healpix_map_to_fits(*handle_, map, datatype);
  ++col_num_;
}
} // namespace comptonsoft
#endif // ComptonSoftApplications_SaveFits_hh