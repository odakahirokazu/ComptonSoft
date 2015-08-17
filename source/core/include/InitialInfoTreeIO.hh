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

#ifndef COMPTONSOFT_InitialInfoTreeIO_H
#define COMPTONSOFT_InitialInfoTreeIO_H 1

#include "CSTypes.hh"

class TTree;

namespace comptonsoft {

/**
 * 
 * @author Hirokazu Odaka
 * @date 2014-12-02
 */
class InitialInfoTreeIO
{
public:
  InitialInfoTreeIO();
  virtual ~InitialInfoTreeIO();

  virtual void setTree(TTree* tree) { tree_ = tree; }

  void enableInitialInfoRecord() { enabled_ = true; }
  void disableInitialInfoRecord() { enabled_ = false; }

  virtual void defineBranches();
  virtual void setBranchAddresses();

  double getInitialEnergy() const
  { return ini_energy_; }

  vector3_t getInitialDirection() const
  { return vector3_t(ini_dirx_, ini_diry_, ini_dirz_); }

  double getInitialTime() const
  { return ini_time_; }

  vector3_t getInitialPosition() const
  { return vector3_t(ini_posx_, ini_posy_, ini_posz_); }
  
  vector3_t getInitialPolarization() const
  { return vector3_t(ini_polarx_, ini_polary_, ini_polarz_); }

  double getWeight() const
  { return weight_; }

  void setInitialInfo(double energy,
                      const vector3_t& direction,
                      double time,
                      const vector3_t& position,
                      const vector3_t& polarization)
  {
    setInitialEnergy(energy);
    setInitialDirection(direction);
    setInitialTime(time);
    setInitialPosition(position);
    setInitialPolarization(polarization);
  }

  void setInitialInfo(double energy,
                      const vector3_t& direction,
                      double time,
                      const vector3_t& position)
  {
    setInitialEnergy(energy);
    setInitialDirection(direction);
    setInitialTime(time);
    setInitialPosition(position);
    setInitialPolarization(vector3_t(0.0, 0.0, 0.0));
  }

  void setWeight(double v)
  { weight_ = v; }

  void setInitialEnergy(double v)
  { ini_energy_ = v; }

  void setInitialDirection(double x, double y, double z)
  {
    ini_dirx_ = x;
    ini_diry_ = y;
    ini_dirz_ = z;
  }

  void setInitialDirection(const vector3_t& v)
  { setInitialDirection(v.x(), v.y(), v.z()); }
  
  void setInitialTime(double v)
  { ini_time_ = v; }

  void setInitialPosition(double x, double y, double z)
  {
    ini_posx_ = x;
    ini_posy_ = y;
    ini_posz_ = z;
  }
  
  void setInitialPosition(const vector3_t& v)
  { setInitialPosition(v.x(), v.y(), v.z()); }

  void setInitialPolarization(double x, double y, double z)
  {
    ini_polarx_ = x;
    ini_polary_ = y;
    ini_polarz_ = z;
  }
  
  void setInitialPolarization(const vector3_t& v)
  { setInitialPolarization(v.x(), v.y(), v.z()); }

private:
  TTree* tree_;
  bool enabled_;

  /*
   * tree contents
   */
  float ini_energy_ = 0.0;
  float ini_dirx_ = 0.0;
  float ini_diry_ = 0.0;
  float ini_dirz_ = 0.0;
  float ini_time_ = 0.0;
  float ini_posx_ = 0.0;
  float ini_posy_ = 0.0;
  float ini_posz_ = 0.0;
  float ini_polarx_ = 0.0;
  float ini_polary_ = 0.0;
  float ini_polarz_ = 0.0;
  float weight_ = 1.0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_InitialInfoTreeIO_H */
