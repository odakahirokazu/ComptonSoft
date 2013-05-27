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

#ifndef COMPTONSOFT_RealDetectorUnit2DStrip_H
#define COMPTONSOFT_RealDetectorUnit2DStrip_H 1

#include "RealDetectorUnit.hh"

namespace comptonsoft {

/**
 * A class of a 2D-strip semicondutor detector unit.
 * @author Hirokazu Odaka
 * @date 2008-08-25
 */

class RealDetectorUnit2DStrip : public RealDetectorUnit
{
public:
  RealDetectorUnit2DStrip();
  virtual ~RealDetectorUnit2DStrip();

  virtual std::string Type() { return std::string("Detector2DStrip"); }

  virtual int analyzeHits();
  virtual void clearAnalyzedHits();
  
  void setPriorityToAnodeSide(bool v=true)   { m_PriorityToAnodeSide = v; }
  void setPriorityToCathodeSide(bool v=true) { m_PriorityToAnodeSide = !v; }
  void setPriorityToNSide(bool v=true) { m_PriorityToAnodeSide = v; }
  void setPriorityToPSide(bool v=true) { m_PriorityToAnodeSide = !v; }
  
  bool PriorityToAnodeSide() const   { return m_PriorityToAnodeSide; }
  bool PriorityToCathodeSide() const { return !m_PriorityToAnodeSide; }
  bool PriorityToNSide() const { return m_PriorityToAnodeSide; }
  bool PriorityToPSide() const { return !m_PriorityToAnodeSide; }

  int NumCathodeSideHits() const { return m_CathodeSideHits.size(); }
  DetectorHit_sptr getCathodeSideHit(int i) { return m_CathodeSideHits[i]; }
  int NumAnodeSideHits() const { return m_AnodeSideHits.size(); }
  DetectorHit_sptr getAnodeSideHit(int i) { return m_AnodeSideHits[i]; }

protected:
  void prepareDoubleSideHits();
  virtual void cluster();

protected:
  std::vector<DetectorHit_sptr> m_CathodeSideHits;
  std::vector<DetectorHit_sptr> m_AnodeSideHits;

private:
  void analyzeMaxPHAHit();
  void analyzeSingleHit();

private:
  bool m_PriorityToAnodeSide;
};

}

#endif /* COMPTONSOFT_RealDetectorUnit2DStrip_H */
