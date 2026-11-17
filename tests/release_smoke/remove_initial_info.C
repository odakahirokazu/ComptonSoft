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

#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>

void remove_initial_info(const char* input, const char* output)
{
  TFile source(input);
  auto* tree = source.Get<TTree>("eventtree");
  if (!tree) { gSystem->Exit(1); return; }
  tree->SetBranchStatus("ini_*", false);
  tree->SetBranchStatus("weight", false);
  TFile destination(output, "CREATE");
  if (destination.IsZombie()) { gSystem->Exit(1); return; }
  auto* copy = tree->CloneTree(32);
  copy->Write();
}
