/**
 * Calculation of a charge collection efficiency map in a detector pixel.
 *
 * @author Hiromasa Suzuki and Hirokazu Odaka
 * 
 * @date 2020-02-26 | Hiromasa Suzuki | implementation for Suzuki et al., JATIS, submitted
 * @date 2020-04-21 | Hirokazu Odaka | pack into classes in ComptonSoft
 */

#include <memory>
#include <vector>
#include "AstroUnits.hh"
#include "RectangularGoalRegion.hh"
#include "CCECalculation.hh"

namespace unit = anlgeant4::unit;

std::vector<double> make_xedges(int m, int n, double alpha, double width);
std::vector<double> make_zedges(int m, int n, double alpha, double width);
std::unique_ptr<comptonsoft::VGoalRegion> make_goal(double pixelSize, double thickness, double nodeDepth);

int main()
{
  const std::string efield_file("efield.root");
  const std::string output_file("cce.root");

  const double pixelSize = 36.0*unit::um;
  const double thickness = 500.0*unit::um;
  const double nodeDepth = 0.1*unit::um;
  const double stepLength = 1.0*unit::um;
  const bool carrierPositive = true; // holes run to pixel electrodes
  const double mobility = 5.0e+2 * (unit::cm2/unit::s/unit::volt);
  const double lifetime = 10.0 * unit::us;
  const double temperature = 273.15*unit::kelvin;
  const double spreadFactor = 1.0;

  const std::vector<double> edgesx = make_xedges(10, 10, 0.1, pixelSize);
  const std::vector<double> edgesy = make_xedges(10, 10, 0.1, pixelSize);
  const std::vector<double> edgesz = make_zedges(20, 20, 0.98, thickness);
  const int numParticles = 1000;
  const double endingTime = 100.0 * unit::us;
  std::unique_ptr<comptonsoft::VGoalRegion> goal = make_goal(pixelSize, thickness, nodeDepth);
  auto cceBuilder = std::make_unique<comptonsoft::CCECalculation>();
  cceBuilder->setUpsideReadout(true);
  cceBuilder->setPixelDimensions(pixelSize, pixelSize, thickness);
  cceBuilder->setSegmentation(edgesx, edgesy, edgesz);
  cceBuilder->setStepLength(stepLength);
  cceBuilder->initializeCCEMap(output_file);
  cceBuilder->setSemiconductorProperties(carrierPositive, mobility, lifetime, temperature, spreadFactor);
  cceBuilder->unsetCarrierVelocitySaturation();
  cceBuilder->loadEField(efield_file);
  cceBuilder->useAccurateWeightingPotential();
  cceBuilder->registerGoal(std::move(goal));
  cceBuilder->setEndingTime(endingTime);
  cceBuilder->run(numParticles);
  cceBuilder->write();
	return 0;
}

std::vector<double> make_zedges(int m, int n, double alpha, double width)
{
  std::vector<double> edges(m+n+1);
  const double start = -0.5*width;
  for (int i=0; i<=m; i++) {
    edges[i] = start + width*alpha*static_cast<double>(i)/m;
  }
  for (int i=1; i<=n; i++) {
    edges[m+i] = start + width*alpha + width*(1.0-alpha)*static_cast<double>(i)/n;
  }
  return edges;
}

std::vector<double> make_xedges(int m, int n, double alpha, double width)
{
  std::vector<double> edges(2*(m+n)+1);
  const double x0 = -0.5*width;
  const double x1 = +0.5*width;
  for (int i=0; i<=m; i++) {
    edges[i]         = x0 + 0.5*width*alpha*static_cast<double>(i)/m;
    edges[2*(m+n)-i] = x1 - 0.5*width*alpha*static_cast<double>(i)/m;
  }
  for (int i=1; i<n; i++) {
    edges[m+i]           = x0 + 0.5*width*alpha + 0.5*width*(1.0-alpha)*static_cast<double>(i)/n;
    edges[2*(m+n)-(m+i)] = x1 - 0.5*width*alpha - 0.5*width*(1.0-alpha)*static_cast<double>(i)/n;
  }
  edges[m+n] = 0.0;
  return edges;
}

std::unique_ptr<comptonsoft::VGoalRegion> make_goal(double pixelSize, double thickness, double nodeDepth)
{
  auto goal = std::make_unique<comptonsoft::RectangularGoalRegion>();
  goal->addRegion(-3.0*pixelSize, +3.0*pixelSize,
                  -3.0*pixelSize, +3.0*pixelSize,
                  0.5*thickness-nodeDepth, 0.5*thickness+100*nodeDepth);
  return goal;
}
