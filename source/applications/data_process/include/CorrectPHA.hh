/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
 *************************************************************************/

#ifndef COMPTONSOFT_CorrectPHA_H
#define COMPTONSOFT_CorrectPHA_H 1

#include "VCSModule.hh"
#include <memory>

namespace comptonsoft {

/**
 *
 * @author Hirokazu Odaka 
 * @date 2007-12-xx 
 * @date 2009-06-22
 * @date 2014-09-09
 * @date 2016-05-02 | PHA randomization
 */
class CorrectPHA : public VCSModule
{
  DEFINE_ANL_MODULE(CorrectPHA, 3.0)
public:
  enum class CMNSubtractionMode {
    No, Given, Median, Mean
  };

public:
  CorrectPHA();
  ~CorrectPHA();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

private:
  bool m_PHARandomization;
  bool m_PedestalCorrection;
  CMNSubtractionMode m_CMNSubtraction;
  bool m_GainCorrection;

  std::string m_PedestalFileName;
  int m_CMNSubtractionInteger;
  std::string m_GainFileName;
  std::unique_ptr<TFile> m_GainFile;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_CorrectPHA_H */
