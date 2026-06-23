#ifndef COMPTONSOFT_ListPrimaryGenerator_H
#define COMPTONSOFT_ListPrimaryGenerator_H 1

#include "BasicPrimaryGenerator.hh"
#include <memory>
#include <mutex>

class TFile;
class TTree;

namespace comptonsoft {

/*
 * Primary generation module according to a photon list.
 *
 * @author Hirokazu Odaka
 * @date 2012-11-20
 * @date 2017-07-14 | Hirokazu Odaka | 4.1, makePrimarySetting()
 * @date 2020-11-24 | Hirokazu Odaka | 5.0, copied from monaco
 * @date 2026-06-20 | Hirokazu Odaka | 7.0 Geant4 MT
 */
class ListPrimaryGenerator : public anlgeant4::BasicPrimaryGenerator
{
  DEFINE_ANL_MODULE(ListPrimaryGenerator, 5.0);
public:
  ListPrimaryGenerator();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  anlgeant4::PrimarySetting make_primary_setting() const override;

private:
  std::string filename_;

  double x_ = 0.0;
  double y_ = 0.0;
  double z_ = 0.0;
  double energy_ = 0.0;
  double dirx_ = 0.0;
  double diry_ = 0.0;
  double dirz_ = 0.0;

  std::unique_ptr<TFile> file_;
  TTree* tree_ = nullptr;
  int num_entries_ = 0;

  mutable std::mutex mutex_;
  mutable int index_ = 0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ListPrimaryGenerator_H */
