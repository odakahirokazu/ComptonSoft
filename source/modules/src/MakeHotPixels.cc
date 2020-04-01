/*************************************************************************
 *                                                                       *
 * Copyright (c) 2019 Hirokazu Odaka                                     *
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

#include "MakeHotPixels.hh"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "FrameData.hh"

using namespace anlnext;

namespace comptonsoft{

MakeHotPixels::MakeHotPixels()
  : filename_("hotpix.xml")
{
}

ANLStatus MakeHotPixels::mod_define()
{
  define_parameter("filename", &mod_class::filename_);
  return AS_OK;
}

ANLStatus MakeHotPixels::mod_end_run()
{
  namespace xp = boost::property_tree::xml_parser;
  using boost::property_tree::ptree;

  ptree pt;
  ptree& pt1 = pt.add("channel_properties", "");
  pt1.add("name", "");
  ptree& data_node = pt1.add("data", "");

  auto& detectors = getDetectorManager()->getDetectors();
  for (auto& detector: detectors) {
    if (detector->hasFrameData()) {
      ptree& detector_node = data_node.add("detector", "");
      detector_node.add("<xmlattr>.id", detector->getID());
      ptree& frame_node = detector_node.add("frame", "");

      const FrameData* frame = detector->getFrameData();
      const int nx = frame->NumPixelsX();
      const int ny = frame->NumPixelsY();
      for (int ix=0; ix<nx; ix++) {
        for (int iy=0; iy<ny; iy++) {
          if (frame->isDisabledPixel(ix, iy)) {
            ptree& pixel_node = frame_node.add("pixel", "");
            pixel_node.add("<xmlattr>.x", ix);
            pixel_node.add("<xmlattr>.y", iy);
            pixel_node.add("disable.<xmlattr>.status", 1);
          }
        }
      }
    }
  }

  const int indent = 2;
  xp::write_xml(filename_,
                pt,
                std::locale(),
                boost::property_tree::xml_writer_make_settings<std::string>(' ', indent));
  
  return AS_OK;
}

} /* namespace comptonsoft */
