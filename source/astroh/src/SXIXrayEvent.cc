/*************************************************************************
 *                                                                       *
 * Copyright (c) 2019 Hirokazu Odaka, Tsubasa Tamba                      *
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

#include "SXIXrayEvent.hh"

#include <iostream>
#include <cmath>

namespace comptonsoft
{

SXIXrayEvent::SXIXrayEvent(int size)
  : XrayEvent(size)
{
}

SXIXrayEvent::~SXIXrayEvent() = default;

void SXIXrayEvent::determineAscaGrade()
{
  const image_t& data = Data();
  const int size = data.shape()[0];
  const int halfSize = size/2;

  const int canv[5][5] = {{12,11,10,9,24}, {13,6,4,1,23}, {14,7,0,2,22}, {15,8,5,3,21}, {16,17,18,19,20}};
  const int lookup[256] = {  0,1,2,5,1,1,5,7,3,5,8,6,3,5,7,7,
                             4,4,8,7,5,5,6,7,7,7,7,7,7,7,7,7,
                             1,1,2,5,1,1,5,7,5,7,7,7,5,7,7,7,
                             4,4,8,7,5,5,6,7,7,7,7,7,7,7,7,7,
                             2,2,7,7,2,2,7,7,8,7,7,7,8,7,7,7,
                             8,8,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             5,5,7,7,5,5,7,7,6,7,7,7,6,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             1,1,2,5,1,1,5,7,3,5,8,6,3,5,7,7,
                             5,5,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             1,1,2,5,1,1,5,7,5,7,7,7,5,7,7,7,
                             5,5,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             5,5,7,7,5,5,7,7,7,7,7,7,7,7,7,7,
                             6,6,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7  };

  sxiTotalGrade_ = 0x000000;
  dataVector_.resize(size*size);
 
  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      const double v = data[i][j];
      dataVector_[canv[i][j]] = v;
      if (i==halfSize && j==halfSize) {
        ;
      }
      else if (std::abs(i-halfSize)<=1 && std::abs(j-halfSize)<=1) {
        if (v>=SplitThreshold()) {
          sxiTotalGrade_ = sxiTotalGrade_ | (0x000001 << (canv[i][j]-1));
        }
      }
      else if (std::abs(i-halfSize)<=2 && std::abs(j-halfSize)<=2) {
        if (v>=OuterSplitThreshold()) {
          sxiTotalGrade_ = sxiTotalGrade_ | (0x000001 << (canv[i][j]-1));
        }
      }
    }
  }

  sxiInnerGrade_ = sxiTotalGrade_ & 0x0000FF;
  sxiOuterGrade_ = sxiTotalGrade_ >> 8;
                   
  setGrade(lookup[sxiInnerGrade_]);
}

int SXIXrayEvent::makeOuterMask (int innerGrade)
{
  int outermask = 0;
  const int mark [9] = {0x0000, 0x4001, 0x2000, 0x1400, 0x0002, 0x0200, 0x0014, 0x0020, 0x0140};
  int crosspix[4] = {2, 4, 5, 7};
  for (int i=0; i<4; i++) {
    int ipix = crosspix[i];
    if ((innerGrade & (0x000001 << (ipix-1))) != 0) {
      outermask = outermask|mark[ipix];
    }
  }
  if ( (innerGrade & (0x000B)) == 0x000B ) { outermask = outermask | mark[1]; }
  if ( (innerGrade & (0x0016)) == 0x0016 ) { outermask = outermask | mark[3]; }
  if ( (innerGrade & (0x0068)) == 0x0068 ) { outermask = outermask | mark[6]; }
  if ( (innerGrade & (0x00D0)) == 0x00D0 ) { outermask = outermask | mark[8]; }

  return outermask;
}

void SXIXrayEvent::classifyGrade()
{
  sxiOuterMaskedGrade_ = sxiOuterGrade_ & outerMask_;

  if (Grade()==6) {
    switch (sxiInnerGrade_) {
      case 0x0b: case 0x8b:
        if ( dataVector_[1] > dataVector_[2] && dataVector_[1] > dataVector_[4]) {
          setGrade(11);
        }
        break;
      case 0x16: case 0x36:
        if ( dataVector_[3] > dataVector_[2] && dataVector_[3] > dataVector_[5]) {
          setGrade(11);
        }
        break;
      case 0x68: case 0x6c:
        if ( dataVector_[6] > dataVector_[4] && dataVector_[6] > dataVector_[7]) {
          setGrade(11);
        }
        break;
      case 0xd0: case 0xd1:
        if ( dataVector_[8] > dataVector_[5] && dataVector_[8] > dataVector_[7]) {
          setGrade(11);
        }
        break;
      default:
        setGrade(6);
        break;
    }
  }

  switch (Grade()) {
    case 0: case 1:
      break;
    case 2: case 3: case 4: case 5: case 6: case 8:
      if (sxiOuterMaskedGrade_ == 0) {
        break;
      }
      else{
        setGrade(10);
        break;
      }
    case 7:
      if (sxiOuterMaskedGrade_ == 0){
        setGrade(9);
        break;
      }
      else{
        break;
      }
    case 11:
      break;
    default:
      break;
  }
}

double SXIXrayEvent::calculateSxiSumPH()
{
  double sumph;
  sumph = CenterPH();

  for (int i=2; i<8; i++) {
    if (dataVector_[i] >= SplitThreshold()) {
      switch (i) {
        case 2: case 4: case 5: case 7:
          sumph += dataVector_[i];
          break;
        default:
          break;
      }
    }
  }

  if (Grade()==6 || Grade()==7 || Grade()==8) {
    if( (dataVector_[2]>=SplitThreshold()) && (dataVector_[4]>=SplitThreshold()) ) {
      sumph += dataVector_[1];
    }
    if( (dataVector_[2]>=SplitThreshold()) && (dataVector_[5]>=SplitThreshold()) ) {
      sumph += dataVector_[3];
    }
    if( (dataVector_[4]>=SplitThreshold()) && (dataVector_[7]>=SplitThreshold()) ) {
      sumph += dataVector_[6];
    }
    if( (dataVector_[5]>=SplitThreshold()) && (dataVector_[7]>=SplitThreshold()) ) {
      sumph += dataVector_[8];
    }
  }

  return sumph;
}

void SXIXrayEvent::calculateRank()
{
  if (Grade()==0){
    setRank(0);
  }
  else if (Grade()==7 || Grade()==10){
    setRank(2);
  }
  else{
    setRank(1);
  }
}

void SXIXrayEvent::calculateWeightAndModifyData()
{
  const image_t& data = Data();
  const int size = data.shape()[0];
  const int halfSize = size/2;
  int weight = 0;

  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      const double v = data[i][j];
      if (i==halfSize && j==halfSize) {
        weight += 1;
      }
      else if (std::abs(i-halfSize)<=1 && std::abs(j-halfSize)<=1) {
        if (v>=SplitThreshold()) {
          weight += 1;
        }
        else{
          setData(i, j, 0.0);
        }
      }
      else if (std::abs(i-halfSize)<=2 && std::abs(j-halfSize)<=2) {
        if (v>=OuterSplitThreshold()) {
          weight += 1;
        }
        else{
          setData(i, j, 0.0);
        }
      }
    }
  }

  setWeight(weight);
}

void SXIXrayEvent::reduce()
{
  determineAscaGrade();
  outerMask_ = makeOuterMask(sxiInnerGrade_);
  classifyGrade();
  setSumPH(calculateSxiSumPH());
  calculateRank();
  calculateWeightAndModifyData();
}

} /* namespace comptonsoft */
