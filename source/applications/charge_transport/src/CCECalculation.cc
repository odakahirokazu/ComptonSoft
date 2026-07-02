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

#include "CCECalculation.hh"
#include "AstroUnits.hh"
#include "PointCharge.hh"

namespace unit = anlgeant4::unit;
namespace constant = anlgeant4::constant;

namespace comptonsoft {

CCECalculation::CCECalculation()
  : field_(new SemiconductorModel)
{
}

CCECalculation::~CCECalculation() = default;

void CCECalculation::setUpsideReadout(bool v)
{
  field_->setUpsideReadout(v);
}

void CCECalculation::setPixelDimensions(double x, double y, double z)
{
  pixelWidthX_ = x;
  pixelWidthY_ = y;
  thickness_ = z;
  field_->setPixelDimensions(x, y, z);
}

void CCECalculation::setSegmentation(const std::vector<double>& xs,
                                     const std::vector<double>& ys,
                                     const std::vector<double>& zs)
{
  segmentationX_ = xs;
  segmentationY_ = ys;
  segmentationZ_ = zs;
}

std::vector<double> CCECalculation::
extendedEdges(const std::vector<double>& xs, double pixelWidth)
{
  const int numBins = xs.size() - 1;
  const int extension = 3;
  const int numExtendedBins = numBins * extension;

  std::vector<double> exs(numExtendedBins+1, 0.0);
  for (int i=0; i<numBins; i++) {
    exs[i]           = xs[i] - pixelWidth;
    exs[i+numBins]   = xs[i];
    exs[i+2*numBins] = xs[i] + pixelWidth;
  }
  exs[3*numBins] = xs[numBins] + pixelWidth;

  return exs;
}

void CCECalculation::initializeCCEMap(const std::string& filename)
{
  std::vector<double> edgesX = extendedEdges(segmentationX_, pixelWidthX_);
  std::vector<double> edgesY = extendedEdges(segmentationY_, pixelWidthY_);
  std::vector<double> edgesZ = segmentationZ_;

  for (auto& x: edgesX) { x /= unit::cm; }
  for (auto& x: edgesY) { x /= unit::cm; }
  for (auto& x: edgesZ) { x /= unit::cm; }
  
  outFile_ = std::make_unique<TFile>(filename.c_str(), "recreate");
  cceMap_ = new TH3D("cce", "CCE map",
                     edgesX.size()-1, &edgesX[0],
                     edgesY.size()-1, &edgesY[0],
                     edgesZ.size()-1, &edgesZ[0]);
}

void CCECalculation::
setSemiconductorProperties(bool carrier_positive,
                           double mobility,
                           double lifetime,
                           double temperature,
                           double spreadFactor)
{
  field_->setProperties(carrier_positive, mobility, lifetime, temperature, spreadFactor);
}

void CCECalculation::setCarrierVelocitySaturation(double efield)
{
  field_->setVelocitySaturation(efield);
}

void CCECalculation::unsetCarrierVelocitySaturation()
{
  field_->unsetVelocitySaturation();
}

void CCECalculation::loadEField(const std::string& filename)
{
  field_->load(filename);
}

void CCECalculation::useAccurateWeightingPotential()
{
  const std::vector<double> edgesX = extendedEdges(segmentationX_, pixelWidthX_);
  const std::vector<double> edgesY = extendedEdges(segmentationY_, pixelWidthY_);
  const std::vector<double> edgesZ = segmentationZ_;
  field_->useAccurateWeightingPotential(edgesX, edgesX, edgesZ);
}

void CCECalculation::setRandomSeed(uint32_t seed)
{
  field_->setRandomSeed(seed);
}

void CCECalculation::run(const int numParticles)
{
  const int nx = segmentationX_.size() - 1;
  const int ny = segmentationY_.size() - 1;
  const int nz = segmentationZ_.size() - 1;
  for (int ix=0; ix<nx; ix++) {
    std::cout << "ix: " << ix << std::endl;
    for (int iy=0; iy<ny; iy++) {
      std::cout << " iy: " << iy << " => ";
      for (int iz=0; iz<nz; iz++) {
        std::cout << "*";
        std::cout.flush();
        calculate(ix, iy, iz, numParticles);
      }
      std::cout << std::endl;
    }
  }
}

void CCECalculation::calculate(const int ix, const int iy, const int iz, const int numParticles)
{
  // pixel-electrode side
  for (int j=0; j<numParticles; j++) {
    transport(ix, iy, iz, 1.0/numParticles);
  }

  // flat-electrode side
  calculateForOppositeSide(ix, iy, iz);

#if 0
  const int nx = segmentationX_.size() - 1;
  const int ny = segmentationY_.size() - 1;
  const int binx = 1 + nx + ix;
  const int biny = 1 + ny + iy;
  const int binz = 1 + iz;
  std::cout << "CCE= " << cceMap_->GetBinContent(binx, biny, binz) << std::endl;
#endif
}

void CCECalculation::transport(const int ix, const int iy, const int iz, const double charge)
{
  const int nx = segmentationX_.size() - 1;
  const int ny = segmentationY_.size() - 1;
  const int binx = 1 + nx + ix;
  const int biny = 1 + ny + iy;
  const int binz = 1 + iz;
  const double x = cceMap_->GetXaxis()->GetBinCenter(binx) * unit::cm;
  const double y = cceMap_->GetYaxis()->GetBinCenter(biny) * unit::cm;
  const double z = cceMap_->GetZaxis()->GetBinCenter(binz) * unit::cm;
  const vector3_t position(x, y, z);

  auto particle = std::make_unique<PointCharge>();
  particle->setPosition(position);
  particle->setGoal(goal_.get());
  particle->setEffectiveQ(charge);
  particle->setPixelSeparations(pixelWidthX_, pixelWidthY_);
  particle->setField(field_.get());
  particle->setEndingTime(endingTime_);

  const boost::multi_array<double, 2> resultCCE = particle->transport(stepLength_);
  addCCE(binx-nx, biny-ny, binz, resultCCE[0][0]);
  addCCE(binx-nx, biny,    binz, resultCCE[0][1]);
  addCCE(binx-nx, biny+ny, binz, resultCCE[0][2]);
  addCCE(binx,    biny-ny, binz, resultCCE[1][0]);
  addCCE(binx,    biny,    binz, resultCCE[1][1]);
  addCCE(binx,    biny+ny, binz, resultCCE[1][2]);
  addCCE(binx+nx, biny-ny, binz, resultCCE[2][0]);
  addCCE(binx+nx, biny,    binz, resultCCE[2][1]);
  addCCE(binx+nx, biny+ny, binz, resultCCE[2][2]);
}

void CCECalculation::calculateForOppositeSide(const int ix, const int iy, const int iz)
{
  const int nx = segmentationX_.size() - 1;
  const int ny = segmentationY_.size() - 1;
  const int binx = 1 + nx + ix;
  const int biny = 1 + ny + iy;
  const int binz = 1 + iz;
  const double x = cceMap_->GetXaxis()->GetBinCenter(binx) * unit::cm;
  const double y = cceMap_->GetYaxis()->GetBinCenter(biny) * unit::cm;
  const double z = cceMap_->GetZaxis()->GetBinCenter(binz) * unit::cm;

  for (int px=-1; px<=+1; px++) {
    for (int py=-1; py<=+1; py++) {
      const double x_shifted = x + px*pixelWidthX_;
      const double y_shifted = y + py*pixelWidthY_;
      const vector3_t position(x_shifted, y_shifted, z);
      const double wp0 = field_->WeightingPotential(position);
      const double wp1 = 0.0;
      const double signal = wp0 - wp1;
      const int binx_shifted = binx + px*nx;
      const int biny_shifted = biny + py*ny;
      addCCE(binx_shifted, biny_shifted, binz, signal);
    }
  }
}

void CCECalculation::addCCE(const int binx, const int biny, const int binz, const double value)
{
  const double v0 = cceMap_->GetBinContent(binx, biny, binz);
  cceMap_->SetBinContent(binx, biny, binz, v0+value);
}

void CCECalculation::write()
{
  outFile_->Write();
  outFile_->Close();
}

} /* namespace comptonsoft */
