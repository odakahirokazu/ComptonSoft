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

#ifndef COMPTONSOFT_VDetectorUnitFactory_H
#define COMPTONSOFT_VDetectorUnitFactory_H 1

#include <string>

namespace comptonsoft {

class VRealDetectorUnit;

/**
 * An abstract factory class for detector units.
 * @author Hirokazu Odaka
 * @date 2014-11-14
 */
class VDetectorUnitFactory
{
public:
  VDetectorUnitFactory() = default;
  virtual ~VDetectorUnitFactory();

  virtual VRealDetectorUnit* createDetectorUnit(const std::string& type);
  virtual VRealDetectorUnit* createDetectorUnit2DPixel() = 0;
  virtual VRealDetectorUnit* createDetectorUnit2DStrip() = 0;
  virtual VRealDetectorUnit* createDetectorUnitScintillator() = 0;
  virtual VRealDetectorUnit* createDetectorUnit3DVoxel() = 0;

private:
  VDetectorUnitFactory(const VDetectorUnitFactory&) = delete;
  VDetectorUnitFactory(VDetectorUnitFactory&&) = delete;
  VDetectorUnitFactory& operator=(const VDetectorUnitFactory&) = delete;
  VDetectorUnitFactory& operator=(VDetectorUnitFactory&&) = delete;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_VDetectorUnitFactory_H */
