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

/**
 * virtual class of TTree format
 * @author Hirokazu Odaka
 * @date 2012-04-13
 */

#include "TTree.h"

namespace comptonsoft {

class VTreeFormat
{
public:
  VTreeFormat() {}
  virtual ~VTreeFormat() {}

  virtual InitializeForWrite() = 0;
  virtual InitializeForRead() = 0;

protected:
  TBranch* Branch(const char* name, void* address, const char* leaflist,
                  Int_t bufsize = 32000)
  { return tree->Branch(name, address, leaflist, bufsize); }

  Int_t SetBranchAddress(const char* bname, void* address,
                         TBranch** ptr = 0)
  { return tree->SetBranchAddress(bname, address, ptr); }

private:
  TTree* tree;
};

}
