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

namespace ChannelID
{
constexpr static int Undefined = -1;
}


/**
 * A class of section-channel pair
 * @author Hirokazu Odaka
 * @date 2014-11-11
 * @date 2016-08-18
 */
class SectionChannelPair
{
public:
  SectionChannelPair(int section=ChannelID::Undefined, int channel=ChannelID::Undefined);
  ~SectionChannelPair();
  SectionChannelPair(const SectionChannelPair&) = default;
  SectionChannelPair(SectionChannelPair&&) = default;
  SectionChannelPair& operator=(const SectionChannelPair&) = default;
  SectionChannelPair& operator=(SectionChannelPair&&) = default;

  void set(int section, int channel)
  {
    section_ = section;
    channel_ = channel;
  }

  int Section() const { return section_; }
  int Channel() const { return channel_; }

private:
  int section_ = ChannelID::Undefined;
  int channel_ = ChannelID::Undefined;
};

/**
 * A class of detector-scoped channel ID
 * @author Hirokazu Odaka
 * @date 2014-11-11
 * @date 2016-08-18
 */
class DetectorBasedChannelID
{
public:
  DetectorBasedChannelID(int detector=ChannelID::Undefined,
                         int section=ChannelID::Undefined,
                         int channel=ChannelID::Undefined);
  ~DetectorBasedChannelID();
  DetectorBasedChannelID(const DetectorBasedChannelID&) = default;
  DetectorBasedChannelID(DetectorBasedChannelID&&) = default;
  DetectorBasedChannelID& operator=(const DetectorBasedChannelID&) = default;
  DetectorBasedChannelID& operator=(DetectorBasedChannelID&&) = default;

  void set(int detector, int section, int channel)
  {
    detector_ = detector;
    sc_.set(section, channel);
  }

  int Detector() const { return detector_; }
  int Section() const { return sc_.Section(); }
  int Channel() const { return sc_.Channel(); }
  SectionChannelPair SectionChannel() const { return sc_; }

  std::string toString() const;

private:
  int detector_ = ChannelID::Undefined;
  SectionChannelPair sc_;
};

/**
 * A class of readout-scoped channel ID
 * @author Hirokazu Odaka
 * @date 2014-11-11
 * @date 2016-08-18
 */
class ReadoutBasedChannelID
{
public:
  ReadoutBasedChannelID(int readoutModule=ChannelID::Undefined,
                        int section=ChannelID::Undefined,
                        int channel=ChannelID::Undefined);
  ~ReadoutBasedChannelID();
  ReadoutBasedChannelID(const ReadoutBasedChannelID&) = default;
  ReadoutBasedChannelID(ReadoutBasedChannelID&&) = default;
  ReadoutBasedChannelID& operator=(const ReadoutBasedChannelID&) = default;
  ReadoutBasedChannelID& operator=(ReadoutBasedChannelID&&) = default;

  void set(int readoutModule, int section, int channel)
  {
    readoutModule_ = readoutModule;
    sc_.set(section, channel);
  }

  int ReadoutModule() const { return readoutModule_; }
  int Section() const { return sc_.Section(); }
  int Channel() const { return sc_.Channel(); }
  SectionChannelPair SectionChannel() const { return sc_; }

  std::string toString() const;

private:
  int readoutModule_ = ChannelID::Undefined;
  SectionChannelPair sc_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ChannelID_H */
