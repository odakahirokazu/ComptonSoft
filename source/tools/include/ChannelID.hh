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

namespace comptonsoft {

/**
 * A class of a readout channel ID
 * @author Hirokazu Odaka
 */

class ChannelID
{
public:
  ChannelID(int a=-1, int b=-1, int c=-1)
  {
    module = a;
    chip = b;
    channel = c;
  }

  void setModule(int r) { module = r; }
  void setChip(int r) { chip = r; }
  void setChannel(int r) { channel = r; }

  int Module() { return module; }
  int Chip() { return chip; }
  int Channel() { return channel; }

private:
  int module;
  int chip;
  int channel;
};

}

#endif /* COMPTONSOFT_ChannelID_H */
