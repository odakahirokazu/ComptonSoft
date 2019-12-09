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

#ifndef COMPTONSOFT_AssignSXIGrade_H
#define COMPTONSOFT_AssignSXIGrade_H 1

#include "VCSModule.hh"
#include <list>
#include "SXIXrayEvent.hh"


namespace comptonsoft {

class CSHitCollection;


/**
 * @author Tsubasa Tamba
 * @date 2019-12-09
 */
class AssignSXIGrade : public VCSModule
{
  DEFINE_ANL_MODULE(AssignSXIGrade, 1.0);
public:
  AssignSXIGrade();
  ~AssignSXIGrade();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

protected:
  void resetImage(image_t& image);

private:
  CSHitCollection* hitCollection_ = nullptr;
  image_t image_;
  const int outerSize_ = 5;

  double eventThreshold_ = 0.0;
  double splitThreshold_ = 0.0;
  double outerSplitThreshold_ = 0.0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_AssignSXIGrade_H */
