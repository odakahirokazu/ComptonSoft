#include "ListPrimaryGen.hh"
#include "TFile.h"
#include "TTree.h"
#include "AstroUnits.hh"

using namespace anlnext;
using namespace anlgeant4;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

ListPrimaryGen::ListPrimaryGen()
  : filename_("photon_list.root")
{
  add_alias("ListPrimaryGen");
}

ANLStatus ListPrimaryGen::mod_define()
{
  BasicPrimaryGen::mod_define();
  register_parameter(&filename_, "filename");
  return AS_OK;
}

ANLStatus ListPrimaryGen::mod_initialize()
{
  BasicPrimaryGen::mod_initialize();

  file_ = std::make_unique<TFile>(filename_.c_str());
  tree_ = static_cast<TTree*>(file_->Get("primary_list"));
  numEntries_ = tree_->GetEntries();
  tree_->SetBranchAddress("x", &x_);
  tree_->SetBranchAddress("y", &y_);
  tree_->SetBranchAddress("z", &z_);
  tree_->SetBranchAddress("energy", &energy_);
  tree_->SetBranchAddress("dirx", &dirx_);
  tree_->SetBranchAddress("diry", &diry_);
  tree_->SetBranchAddress("dirz", &dirz_);

  return AS_OK;
}

ANLStatus ListPrimaryGen::mod_analyze()
{
  if (index_ == numEntries_) {
    return AS_QUIT;
  }
  return AS_OK;
}

void ListPrimaryGen::makePrimarySetting()
{
  tree_->GetEntry(index_++);

  const G4double energy = energy_ * unit::keV;
  const G4ThreeVector direction(dirx_, diry_, dirz_);
  const G4ThreeVector position(x_*unit::cm, y_*unit::cm, z_*unit::cm);
  setPrimary(position, energy, direction);
  setUnpolarized();
}

} /* namespace comptonsoft */
