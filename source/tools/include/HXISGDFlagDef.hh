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

//
// ComptonCamera Hit Data Flag Definition
// HXISGDFlagDef.hh
// 2008-08-23 Hirokazu Odaka
// 2013-07-08 Yuto Ichinohe
//

#ifndef COMPTONSOFT_HXISGDFlagDef_H
#define COMPTONSOFT_HXISGDFlagDef_H 1

namespace comptonsoft {
// process
const unsigned int PROC_PHOTOABS        = 0x00000001u;
const unsigned int PROC_COMPTON         = 0x00000002u;
const unsigned int PROC_RAYLEIGH        = 0x00000004u;
const unsigned int PROC_GAMMACONVERSION = 0x00000008u;

// const unsigned int PROC_FLUOR = 0x00000010u;
// const unsigned int PROC_BREMS = 0x00000020u;
// const unsigned int PROC_0ENE = 0x00000080u;

// const unsigned int ORIG_INCIDENT = 0x00000100u;
// const unsigned int ORIG_FLUOR = 0x00000200u;
// const unsigned int ORIG_BREMS = 0x00000400u;

// const unsigned int EV_RAYL = 0x00001000u;
// const unsigned int EV_PASSIVE = 0x00002000u;
// const unsigned int EV_ESCAPE = 0x00004000u;
// const unsigned int EV_GUARDRING = 0x00008000u;
// const unsigned int EV_RAYLONLYSD = 0x00010000u;

const unsigned int QUENCHING = 0x10000000u;

// grade
const int GRADE1        = 1;
const int GRADE2        = 2;
const int GRADE3        = 3;
const int GRADE4        = 4;
const int GRADE5        = 5;
const int GRADE6        = 6;
const int GRADE7        = 7;
const int GRADE8        = 8;

// flag
const unsigned int BGO_HIT    = 0x00000001u;
const unsigned int SI_HIT     = 0x00000002u;
const unsigned int CDTE_HIT   = 0x00000004u;
const unsigned int CDTE_FLUOR = 0x00000008u;

const unsigned int BGO_VETO     = 0x00000010u;
const unsigned int SI_TRIGGER   = 0x00000020u;
const unsigned int CDTE_TRIGGER = 0x00000040u;

const unsigned int CLUSTERED      = 0x00000100u;
const unsigned int PRIORITY_SIDE  = 0x00000200u;
const unsigned int N_SIDE         = 0x00000400u;
const unsigned int ANODE_SIDE     = N_SIDE;

const unsigned int BAD_CHANNEL     = 0x00001000u;
const unsigned int BAD_TIME_ORDER = 0x00002000u;
const unsigned int HI_ENERGY = 0x00004000u;

// strip
const int NO_STRIP       = -1;
const int GUARD_RING     = -2;

// time group
const int NOTIMEGROUP   = -1;
}

#endif /* COMPTONSOFT_HXISGDFlagDef_H */
