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

#ifndef COMPTONSOFT_RealDetectorUnit_H
#define COMPTONSOFT_RealDetectorUnit_H 1

#include <vector>
#include "VDetectorUnit.hh"
#include "DetectorHit_sptr.hh"

namespace comptonsoft {

class OneASICData;
class ChannelTable;
class DetectorHit;

/**
 * A virtual class of a detector unit.
 * This class contains the inforamtion about readout ASICs and hits detected by this detector.
 * @author Hirokazu Odaka
 * @date 2007-12-xx
 * @date 2008-07-29
 * @date 2008-08-25
 * @date 2009-07-09
 */

class RealDetectorUnit : public VDetectorUnit
{
public:
  RealDetectorUnit();
  virtual ~RealDetectorUnit();

  /**
   * register an ASIC data to this detector object.
   * This method adds the ASIC data to the ASIC data vector.
   */
  void registerASICData(OneASICData* chip) { m_ASICDataVector.push_back(chip); }
  
  int NumASIC() const { return m_ASICDataVector.size(); }
  OneASICData* getASICData(int i) { return m_ASICDataVector[i]; }
  
  /**
   * @return the begin iterator of the ASIC data vector.
   */
  std::vector<OneASICData*>::iterator ASICDataBegin()
  { return m_ASICDataVector.begin(); }

  /**
   * @return the end iterator of the ASIC data vector.
   */
  std::vector<OneASICData*>::iterator ASICDataEnd()
  { return m_ASICDataVector.end(); }
  
  /**
   * register an channel table to this detector object.
   */
  void registerChannelTable(const ChannelTable* table)
  { m_ChannelTable = table; }

  /**
   * convert an ASIC channel into a stirp ID pair.
   * @param hit1_chip chip ID of the hit. (the first hit for a double-sided detector.)
   * @param hit1_chanel channel ID of the hit. (the first hit for a double-sided detector.)
   * @param hit2_chip chip ID of the second hit for a double-sided detector. This parameters is neccesary for a double-sided detector.
   * @param hit2_chanel channel ID of the second hit for a double-sided detector. This parameters is neccesary for a double-sided detector.
   */
  StripPair convertChannel2Strip(int hit1_chip,
				 int hit1_channel,
				 int hit2_chip = -1,
				 int hit2_channel = -1) const;

  /**
   * convert an ASIC channel into a stirp ID pair along x-axis.
   * @sa convertChannel2Strip(int, int, int, int) const
   */
  int convertChannel2StripX(int hit1_chip,
			    int hit1_channel,
			    int hit2_chip = -1,
			    int hit2_channel = -1) const;
  
  /**
   * convert an ASIC channel into a stirp ID pair along y-axis.
   * @sa convertChannel2Strip(int, int, int, int) const
   */
  int convertChannel2StripY(int hit1_chip,
			    int hit1_channel,
			    int hit2_chip = -1,
			    int hit2_channel = -1) const;

  /**
   * convert a stirp ID pair into chip/channel IDs.
   * @param strip_x strip ID along x-axis.
   * @param strip_y strip ID along y-axis.
   * @param pChip a pointer to a chip ID.
   * @param pChannel a pointer to a chip ID.
   */
  void findChipAndChannel(int strip_x, int strip_y,
                          int* pChip, int* pChannel) const;

  /**
   * convert a stirp ID pair into chip/channel IDs.
   * @param strip_x strip ID along x-axis.
   * @param strip_y strip ID along y-axis.
   * @return a pair of a chip ID and a channel ID.
   */
  std::pair<int, int> findChipAndChannel(int strip_x, int strip_y) const;

  virtual int selectHits();
  int discriminateHits();
  virtual int analyzeHits() = 0;

  void applyNewPI();
  void assignDetectorInfo();

  void setAnalysisMode(int val) { m_AnalysisMode = val; }
  int AnalysisMode() { return m_AnalysisMode; }

  void insertDetectorHit(DetectorHit_sptr hit)
  { m_DetectorHits.push_back(hit); }
  int NumDetectorHits() const { return m_DetectorHits.size(); }
  DetectorHit_sptr getDetectorHit(int i) { return m_DetectorHits[i]; }
  void clearDetectorHits() { m_DetectorHits.clear(); }

  void insertAnalyzedHit(DetectorHit_sptr hit)
  { m_AnalyzedHits.push_back(hit); }
  int NumAnalyzedHits() const { return m_AnalyzedHits.size(); }
  DetectorHit_sptr getAnalyzedHit(int i) { return m_AnalyzedHits[i]; };
  virtual void clearAnalyzedHits() { m_AnalyzedHits.clear(); }

protected:
  std::vector<DetectorHit_sptr> m_AnalyzedHits;

private:
  std::vector<OneASICData*> m_ASICDataVector;
  const ChannelTable* m_ChannelTable;

  std::vector<DetectorHit_sptr> m_DetectorHits;

  int m_AnalysisMode;
};

typedef std::vector<RealDetectorUnit*> DetectorVector;
typedef DetectorVector::iterator DetectorIter;

}

#endif /* COMPTONSOFT_RealDetectorUnit_H */
