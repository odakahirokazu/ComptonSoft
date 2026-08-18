/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 *************************************************************************/

#ifndef COMPTONSOFT_NanoGRAMSChargeToEnergySpline_H
#define COMPTONSOFT_NanoGRAMSChargeToEnergySpline_H 1

#include <memory>

class TSpline3;
namespace comptonsoft
{

class ChargeToEnergySpline
{
public:
  ChargeToEnergySpline();
  ~ChargeToEnergySpline();

  void setElectricField(double efield_v_cm);
  double evaluate(double charge_coulomb) const;

private:
  std::unique_ptr<TSpline3> spline_;
  double minimum_charge_ = 0.0;
  double maximum_charge_ = 0.0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSChargeToEnergySpline_H */
