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

#ifndef COMPTONSOFT_FlagDefinition_H
#define COMPTONSOFT_FlagDefinition_H 1

namespace comptonsoft {

namespace process {
constexpr uint32_t PhotoelectricAbsorption = 0x00000001u;
constexpr uint32_t ComptonScattering       = 0x00000002u;
constexpr uint32_t RayleighScattering      = 0x00000004u;
constexpr uint32_t GammaConversion         = 0x00000008u;

constexpr uint32_t NucleusHit = 0x00010000u;
}

namespace flag {
constexpr uint64_t BadChannel   = 0x0001u;
constexpr uint64_t AnodeSide    = 0x0002u;
constexpr uint64_t CathodeSide  = 0x0004u;
constexpr uint64_t NSide = AnodeSide;
constexpr uint64_t PSide = CathodeSide;
constexpr uint64_t PrioritySide = 0x0008u;

constexpr uint64_t AdjacentClustered = 0x0010u;
constexpr uint64_t DistantClustered  = 0x0020u;
constexpr uint64_t FluorescenceHit   = 0x0040u;
constexpr uint64_t ElectronEscape    = 0x0080u;

constexpr uint64_t LowZHit           = 0x0100u;
constexpr uint64_t HighZHit          = 0x0200u;
constexpr uint64_t AntiCoincidence   = 0x0400u;
}

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_FlagDefinition_H */ 
