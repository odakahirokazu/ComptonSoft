#include "SaveFits.hh"

using namespace anlnext;
namespace comptonsoft {
SaveFits::SaveFits()
  : handle_(nullptr),
    filename_("out.fits"),
    col_num_(0)
{
}
SaveFits::~SaveFits() = default;

ANLStatus SaveFits::mod_define()
{
  define_parameter("filename", &mod_class::filename_);
  return AS_OK;
}
ANLStatus SaveFits::mod_initialize()
{
  handle_ = std::make_unique<fitshandle>();
  handle_->create("!" + filename_);
  return AS_OK;
}
ANLStatus SaveFits::mod_finalize()
{
  std::cout << module_id() << ": " << col_num_ << "maps written" << std::endl;
  handle_->close();
  handle_.reset();
  return AS_OK;
}

} // namespace comptonsoft