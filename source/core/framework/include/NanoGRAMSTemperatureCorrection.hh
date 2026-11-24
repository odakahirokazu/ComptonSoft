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

#ifndef COMPTONSOFT_NanoGRAMSTemperatureCorrection_hh
#define COMPTONSOFT_NanoGRAMSTemperatureCorrection_hh 1
#include <vector>
namespace comptonsoft {

/**
 * Temperature (gain drift) correction of NanoGRAMS FECs.
 * The correction factor multiplied to PHA (ADC) values is
 *   factor = (reference test-pulse ADC) / (measured test-pulse ADC),
 * where the reference is the test-pulse ADC when the gain functions (ADC2C) were measured
 * (the test-pulse time series interpolated at the specified reference time, or ccal2ADC),
 * and the measured value is given as a fixed value or as a time series interpolated at the event time.
 * - If no measured value is given, the factor is 1.
 * - If the event time is out of the range of the time series, the factor is NaN.
 * The calibration data are read by the application side and given to this class.
 * @date 2026-09-24 | rewritten from TPCProperty (applications/missions/nanograms)
 */
class NanoGRAMSTemperatureCorrection
{
public:
  explicit NanoGRAMSTemperatureCorrection(int numFECs);
  ~NanoGRAMSTemperatureCorrection();

  int NumberOfFECs() const { return referenceADC_.size(); }

  // test-pulse ADC value when the gain functions were measured
  void setReferenceADC(int fec, double v) { referenceADC_.at(fec) = v; }
  double ReferenceADC(int fec) const { return referenceADC_.at(fec); }

  // measured test-pulse ADC value independent of time
  void setFixedTestPulseADC(int fec, double v);

  // measured test-pulse ADC value at the unixtime [s]; the values are sorted by time internally
  void addTestPulseADC(int fec, double unixTime, double v);

  /**
   * correction factor of the FEC at the unixtime [s]
   */
  double factor(int fec, double unixTime) const;

private:
  double measuredTestPulseADC(int fec, double unixTime) const;

private:
  std::vector<double> referenceADC_;
  std::vector<double> fixedTestPulseADC_;
  std::vector<std::vector<double>> testPulseTimes_;
  std::vector<std::vector<double>> testPulseADCs_;
};

} // namespace comptonsoft
#endif // COMPTONSOFT_NanoGRAMSTemperatureCorrection_hh
