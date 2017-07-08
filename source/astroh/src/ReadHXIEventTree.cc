/*************************************************************************
 *                                                                       *
 * Copyright (c) 2013 Hirokazu Odaka                                     *
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

#include "ReadHXIEventTree.hh"
#include "HXIEvent.hpp"
#include "HXIEventTree.hpp"
#include "DetectorReadoutModule.hh"
#include "MultiChannelData.hh"

using namespace anl;

namespace comptonsoft
{

ReadHXIEventTree::ReadHXIEventTree()
  : anlgeant4::InitialInformation(false),
    event_tree(0), num_events(0), index(0)
{
  add_alias("InitialInformation");
}

ANLStatus ReadHXIEventTree::mod_define()
{
  register_parameter(&file_names, "file_list", "seq", "hxi_event.root");
  register_parameter(&tree_names, "trees", "seq", "event_tree");

  return AS_OK;
}

ANLStatus ReadHXIEventTree::mod_initialize()
{
  if (file_names.size()==0) {
    std::cout << "Event file to be read is not specified." << std::endl;
    return AS_QUIT;
  }
  
  if (tree_names.size()==0) {
    tree_names.resize(file_names.size(), "event_tree");
  }

  event_tree = new TChain(tree_names[0].c_str());
  for(size_t i=0; i<file_names.size(); i++) {
    event_tree->AddFile(file_names[i].c_str(),
                        TChain::kBigNumber,
                        tree_names[i].c_str());
  }
  
  num_events = event_tree->GetEntries();
  std::cout << "Total events: " << num_events << std::endl;

  return AS_OK;
}

ANLStatus ReadHXIEventTree::mod_analyze()
{
  if (index==num_events) {
    return AS_QUIT;
  }

  typedef std::vector<DetectorReadModule*>::iterator ModuleIter;
  typedef std::vector<MultiChannelData*>::iterator ASICIter;
  std::vector<DetectorReadModule*> modules
    = GetDetectorManager()->getReadModuleVector();
  for (ModuleIter im=modules.begin(); im!=modules.end(); ++im) {
    ASICIter ia = (*im)->ASICDataBegin();
    ASICIter iaEnd = (*im)->ASICDataEnd();
    for (; ia!=iaEnd; ++ia) {
      (*ia)->resetRawADC();
    }
  }

  hxi::Event event;
  hxi::EventTree::restoreOneEvent(event_tree, event, index);
  const int EventID = index;
  setEventID(EventID);

  const size_t NumHitASICs = event.getASICID().size();
  for (size_t i=0; i<NumHitASICs; i++) {
    const unsigned int ASICID = event.getASICID()[i];
    const unsigned int ASICIndex = ASICID/16;
    const unsigned int TrayIndex = ASICID%16;
    const uint16_t CommonModeNoise = event.getCommonModeNoise()[i];
    const uint16_t Reference = event.getReferenceChannel()[i];
    MultiChannelData* ASICData = modules[TrayIndex]->getASICData(ASICIndex);
    ASICData->setCommonModeNoise(CommonModeNoise);
    ASICData->setReference(Reference);
  }

  const size_t NumHits = event.getChannelID().size();
  for (size_t i=0; i<NumHits; i++) {
    const int ASICID = event.getASICIDOfEachChannel()[i];
    const unsigned int ASICIndex = ASICID/16;
    const unsigned int TrayIndex = ASICID%16;
    const int ChannelID = event.getChannelID()[i];
    const uint16_t ADCValue = event.getADCValue()[i];
    MultiChannelData* ASICData = modules[TrayIndex]->getASICData(ASICIndex);
    ASICData->setRawADC(ChannelID, ADCValue);
  }

  index++;
  return AS_OK;
}

} /* namespace comptonsoft */
