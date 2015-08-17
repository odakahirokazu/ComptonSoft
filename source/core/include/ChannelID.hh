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

#ifndef COMPTONSOFT_ChannelID_H
#define COMPTONSOFT_ChannelID_H 1

#include <string>

namespace comptonsoft {

/**
 * A class of a readout channel ID
 * @author Hirokazu Odaka
 * @date 2014-11-11
 */
class ChannelID
{
public:
  constexpr static int Undefined = -1;

public:
  ChannelID(int section=Undefined, int index=Undefined);
  ~ChannelID();
  ChannelID(const ChannelID&) = default;
  ChannelID(ChannelID&&) = default;
  ChannelID& operator=(const ChannelID&) = default;
  ChannelID& operator=(ChannelID&&) = default;

  void set(int section, int index)
  {
    section_ = section;
    index_ = index;
  }

  int Section() const { return section_; }
  int Index() const { return index_; }

private:
  int section_ = Undefined;
  int index_ = Undefined;
};

class DetectorChannelID
{
public:
  DetectorChannelID(int detector=ChannelID::Undefined,
                    int section=ChannelID::Undefined,
                    int index=ChannelID::Undefined);
  ~DetectorChannelID();
  DetectorChannelID(const DetectorChannelID&) = default;
  DetectorChannelID(DetectorChannelID&&) = default;
  DetectorChannelID& operator=(const DetectorChannelID&) = default;
  DetectorChannelID& operator=(DetectorChannelID&&) = default;

  void set(int detector, int section, int index)
  {
    detector_ = detector;
    channel_.set(section, index);
  }

  int Detector() const { return detector_; }
  int Section() const { return channel_.Section(); }
  int Index() const { return channel_.Index(); }
  ChannelID Channel() const { return channel_; }

  std::string toString() const;

private:
  int detector_ = ChannelID::Undefined;
  ChannelID channel_;
};

class ReadoutChannelID
{
public:
  ReadoutChannelID(int readoutModule=ChannelID::Undefined,
                   int section=ChannelID::Undefined,
                   int index=ChannelID::Undefined);
  ~ReadoutChannelID();
  ReadoutChannelID(const ReadoutChannelID&) = default;
  ReadoutChannelID(ReadoutChannelID&&) = default;
  ReadoutChannelID& operator=(const ReadoutChannelID&) = default;
  ReadoutChannelID& operator=(ReadoutChannelID&&) = default;

  void set(int readoutModule, int section, int index)
  {
    readoutModule_ = readoutModule;
    channel_.set(section, index);
  }

  int ReadoutModule() const { return readoutModule_; }
  int Section() const { return channel_.Section(); }
  int Index() const { return channel_.Index(); }
  ChannelID Channel() const { return channel_; }

  std::string toString() const;

private:
  int readoutModule_ = ChannelID::Undefined;
  ChannelID channel_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ChannelID_H */
