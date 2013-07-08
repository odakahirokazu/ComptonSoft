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

#ifndef COMPTONSOFT_DetectorHit_H
#define COMPTONSOFT_DetectorHit_H 1

// If you use boost::pool library for fast memory allocation,
// set DetectorHit_BoostPool as 1; otherwise 0.
#define DetectorHit_BoostPool 1

#if DetectorHit_BoostPool
#include <boost/pool/pool.hpp>
#endif

#include "cs_globals.hh"
#include "StripPair.hh"
#include "HXISGDFlagDef.hh"

#include <string>

namespace comptonsoft {


/**
 * A class of a hit in a semiconductor or scintillation detector.
 * This class contains an energy deposited, position, time, detector ID, and so on.
 *
 * @author Hirokazu Odaka
 * @date 2008-08-22
 * @date 2012-06-29 | rename DetectorID; add Instrument ID
 * @author Yuto Ichinohe
 * @data 2013-07-08 | addflag(), set/getTrackID(), set/getParticleType()
 */
class DetectorHit
{
public:
  // constructor and destructor
  DetectorHit();
  ~DetectorHit() {}
  
  // setter methods
  void setRealPosX(double val) { realposx = val; }
  void setRealPosY(double val) { realposy = val; }
  void setRealPosZ(double val) { realposz = val; }
  void setRealPos(double x, double y, double z)
  { realposx = x; realposy = y; realposz = z; }

  void setLocalPosX(double val) { localposx = val; }
  void setLocalPosY(double val) { localposy = val; }
  void setLocalPosZ(double val) { localposz = val; }
  void setLocalPos(double x, double y, double z)
  { localposx = x; localposy = y; localposz = z; }

  void setPosX(double val) { posx = val; }
  void setPosY(double val) { posy = val; }
  void setPosZ(double val) { posz = val; }
  void setPos(double x, double y, double z)
  { posx = x; posy = y; posz = z; }
  
  void setEdep(double val) { edep = val; }
  void setPHA(double val) { e_pha = val; }
  void setPI(double val) { e_pi = val; }
  
  void setEnergy(double val) { energy = val; }

  void setTime(double val) { time = val; }

  void setProcess(int val) { process = val; }
  void addProcess(int val) { process |= val; }
  
  void setGrade(unsigned int val) { grade = val; }

  void setInstrumentID(int val) { instrument_id = val; }
  void setDetectorID(int val) { detector_id = val; }
  void setChipID(int val) { chipid = val; }
  void setChannel(int val) { channel = val; }
  void setStripX(int val) { stripx = val; }
  void setStripY(int val) { stripy = val; }
  void setStrip(int x, int y) { stripx = x; stripy = y; }
  void setStrip(const StripPair& sp) { stripx = sp.X(); stripy = sp.Y(); }
  
  void setTimeGroup(int val) { time_group = val; }

  void setFlag(unsigned int v) { flag = v; }
  
  void setTrackID(int val) { trackid = val; }
  void setParticleType(std::string val) { particletype = val; }

  // getter methods
  double getRealPosX() const { return realposx; }
  double getRealPosY() const { return realposy; }
  double getRealPosZ() const { return realposz; }
  vector3_t getRealPos() const 
  { return vector3_t(realposx, realposy, realposz); }

  double getLocalPosX() const { return localposx; }
  double getLocalPosY() const { return localposy; }
  double getLocalPosZ() const { return localposz; }
  vector3_t getLocalPos() const
  { return vector3_t(localposx, localposy, localposz); }
  
  double getPosX() const { return posx; }
  double getPosY() const { return posy; }
  double getPosZ() const { return posz; }
  vector3_t getPos() const { return vector3_t(posx, posy, posz); }
  
  double getEdep() const { return edep; }
  double getPHA() const { return e_pha; }
  double getPI() const { return e_pi; }
  
  double getEnergy() const { return energy; }

  double getTime() const { return time; }

  unsigned int getProcess() const { return process; }
  
  int getGrade() const { return grade; }

  int getInstrumentID() const { return instrument_id; }
  int getDetectorID() const { return detector_id; }
  int getChipID() const { return chipid; }
  int getChannel() const { return channel; }
  int getStripX() const { return stripx; }
  int getStripY() const { return stripy; }
  StripPair getStrip() const { return StripPair(stripx, stripy); }
  
  int getTimeGroup() const { return time_group; }

  unsigned int getFlag() const { return flag; }

  int getTrackID() const { return trackid; }
  std::string getParticleType() const { return particletype; }

  // utility
  bool areAdjacent(const DetectorHit& r, int mode = 1) const;
  void addAdjacent(const DetectorHit& r);
  
  void addFlag(unsigned int f) { flag |= f; }
  void clearFlag(unsigned int f) { flag &= ~f; }
  bool isFlag(unsigned int f) const { return (flag&f)==f; }
  bool isProcess(unsigned int f) const { return (process&f)==f; }
  
  double distance(const DetectorHit& r) const { return distance(r.getPos()); }
  double distance(const vector3_t& v) const { return (getPos()-v).mag(); }

  bool isXSide() const { return getStripX()>=0 && getStripY()==comptonsoft::NO_STRIP; }
  bool isYSide() const { return getStripY()>=0 && getStripX()==comptonsoft::NO_STRIP; }

  bool isNSide() const { return isFlag(comptonsoft::N_SIDE); }
  bool isPSide() const { return !isNSide(); }
  bool isAnodeSide() const { return isNSide(); }
  bool isCathodeSide() const { return !isNSide(); }
  
  // operator
  bool operator==(const DetectorHit& r) const;
  bool operator!=(const DetectorHit& r) const;
  const DetectorHit& operator+=(const DetectorHit& r);

  // override new/delete operators
  // by using boost::pool library for fast memory allocation
#if DetectorHit_BoostPool
  void* operator new(size_t);
  void operator delete(void*);
#endif


private:
  double realposx;
  double realposy;
  double realposz;

  double localposx;
  double localposy;
  double localposz;

  double posx;
  double posy;
  double posz;

  double edep;
  double e_pha;
  double e_pi;
  
  double energy;
  
  double time;

  unsigned int process;
  
  int grade;
  
  int instrument_id;
  int detector_id;
  int chipid;
  int channel;

  int stripx;
  int stripy;

  int time_group;

  unsigned int flag;

  int trackid;
  std::string particletype;
};


inline DetectorHit::DetectorHit()
  : realposx(0.0), realposy(0.0), realposz(0.0),
    localposx(0.0), localposy(0.0), localposz(0.0),
    posx(0.0), posy(0.0), posz(0.0),
    edep(0.0), e_pha(0.0), e_pi(0.0),
    energy(0.0),
    time(0.0),
    process(0),
    grade(0),
    instrument_id(0), detector_id(-1), chipid(-1), channel(-1),
    stripx(-1), stripy(-1),
    time_group(-1),
    flag(0)
{
}


inline
bool DetectorHit::operator==(const DetectorHit& r) const
{
  return ( instrument_id == r.instrument_id 
           && detector_id == r.detector_id 
	   && stripx == r.stripx && stripy == r.stripy 
	   && time_group == r.time_group);
}


inline
bool DetectorHit::operator!=(const DetectorHit& r) const
{
  return !( instrument_id == r.instrument_id 
            && detector_id == r.detector_id 
	    && stripx == r.stripx && stripy == r.stripy
	    && time_group == r.time_group);
}


// override new/delete operators
// by using boost::pool library for fast memory allocation
#if DetectorHit_BoostPool

extern boost::pool<> DetectorHitAllocator;

inline
void* DetectorHit::operator new(size_t)
{
  void *aHit = (void*)DetectorHitAllocator.malloc();
  return aHit;
}

inline
void DetectorHit::operator delete(void *aHit)
{
  if (aHit != 0) DetectorHitAllocator.free(aHit);
}

#endif

}

#endif /* COMPTONSOFT_DetectorHit_H */
