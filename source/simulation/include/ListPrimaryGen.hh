#ifndef COMPTONSOFT_ListPrimaryGen_H
#define COMPTONSOFT_ListPrimaryGen_H 1

#include "BasicPrimaryGen.hh"
#include <memory>

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
 */
class ListPrimaryGen : public anlgeant4::BasicPrimaryGen
{
  DEFINE_ANL_MODULE(ListPrimaryGen, 5.0);
public:
  ListPrimaryGen();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  void makePrimarySetting() override;

private:
  std::string filename_;
  std::unique_ptr<TFile> file_;
  TTree* tree_ = nullptr;

  double x_ = 0.0;
  double y_ = 0.0;
  double z_ = 0.0;
  double energy_ = 0.0;
  double dirx_ = 0.0;
  double diry_ = 0.0;
  double dirz_ = 0.0;

  int numEntries_ = 0;
  int index_ = 0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ListPrimaryGen_H */
