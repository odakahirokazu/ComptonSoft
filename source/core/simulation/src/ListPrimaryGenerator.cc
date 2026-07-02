#include "ListPrimaryGenerator.hh"
#include "TFile.h"
#include "TTree.h"
#include "AstroUnits.hh"

using namespace anlnext;
using namespace anlgeant4;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

ListPrimaryGenerator::ListPrimaryGenerator()
  : filename_("photon_list.root")
{
  add_alias("ListPrimaryGenerator");
}

ANLStatus ListPrimaryGenerator::mod_define()
{
  BasicPrimaryGenerator::mod_define();
  define_parameter("filename", &mod_class::filename_);
  return AS_OK;
}

ANLStatus ListPrimaryGenerator::mod_initialize()
{
  BasicPrimaryGenerator::mod_initialize();

  file_ = std::make_unique<TFile>(filename_.c_str());
  tree_ = static_cast<TTree*>(file_->Get("primary_list"));
  num_entries_ = tree_->GetEntries();
  tree_->SetBranchAddress("x", &x_);
  tree_->SetBranchAddress("y", &y_);
  tree_->SetBranchAddress("z", &z_);
  tree_->SetBranchAddress("energy", &energy_);
  tree_->SetBranchAddress("dirx", &dirx_);
  tree_->SetBranchAddress("diry", &diry_);
  tree_->SetBranchAddress("dirz", &dirz_);

  return AS_OK;
}

ANLStatus ListPrimaryGenerator::mod_analyze()
{
  if (index_ == num_entries_) {
    return AS_QUIT;
  }
  return AS_OK;
}

PrimarySetting ListPrimaryGenerator::make_primary_setting() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  tree_->GetEntry(index_++);

  const G4double energy = energy_ * unit::keV;
  const G4ThreeVector direction(dirx_, diry_, dirz_);
  const G4ThreeVector position(x_*unit::cm, y_*unit::cm, z_*unit::cm);
  return make_primary(position, energy, direction, unpolarized_vector(direction));
}

} /* namespace comptonsoft */
