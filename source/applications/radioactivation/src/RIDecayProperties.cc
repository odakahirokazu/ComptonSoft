/**
 * @file RIDecayProperties.cc
 * @brief source file of class RIDecayProperties
 * @author Hirokazu Odaka
 * @date 2016-05-04
 */

#include "RIDecayProperties.hh"

namespace comptonsoft
{

RIDecayProperties::RIDecayProperties(double decay_constant,
                                     double branching_ratio,
                                     IsotopeInfo isotope)
  : decay_constant_(decay_constant),
    branching_ratio_(branching_ratio),
    isotope_(isotope),
    merged_(false)
{
}

RIDecayProperties::RIDecayProperties(double decay_constant,
                                     double branching_ratio,
                                     IsotopeInfo&& isotope)
  : decay_constant_(decay_constant),
    branching_ratio_(branching_ratio),
    isotope_(isotope),
    merged_(false)
{
}

RIDecayProperties::~RIDecayProperties() = default;

} /* namespace comptonsoft */
