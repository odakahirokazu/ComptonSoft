/**
 * @file RateData.cc
 * @brief source file of class RateData
 * @author Hirokazu Odaka
 * @date 2016-05-04
 */

#include "RateData.hh"
#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/format.hpp>
#include "AstroUnits.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

RateData::~RateData() = default;

bool RateData::readFile(const std::string& filename)
{
  data_.clear();
  
  std::ifstream fin(filename.c_str());
  if (!fin) {
    std::cout << "Error in RateData::loadFile(): opening file failed: " << filename << std::endl;
    return false;
  }

  bool numEventsValid = false;
  double numEvents(0.0);
  
  while (1) {
    std::string category;
    std::string volume;
    int64_t isotopeID(0);
    int z(0);
    int a(0);
    double energy(0.0);
    int floatingLevel(0);
    double count(0.0);

    constexpr int BUFSIZE = 1024;
    char buf[BUFSIZE];
    fin.getline(buf, BUFSIZE);

    if (fin.gcount() >= BUFSIZE) {
      std::cout << "Error in RateData::loadFile(): the line length may exceed the buffer size." << std::endl;
      return false;
    }

    if (fin.eof()) { break; }
    if (buf[0] == '#') { continue; }

    std::istringstream iss(buf);
    iss >> category;
    if (category=="NumberOfEvents") {
      iss >> numEvents;
      if (numEventsValid) {
        std::cout << "Error: read NumberOfEvents again." << std::endl;
        return false;
      }
      else {
        numEventsValid = true;
      }
    }
    if (category.substr(0, 6)=="Volume") {
      iss >> volume;
      data_.push_back(std::make_pair(volume, RateVector()));
    }
    else if (category=="Isotope") {
      iss >> isotopeID >> z >> a >> energy >> floatingLevel >> count;
      double productionEfficiency = 0.0;

      if (numEventsValid) {
        productionEfficiency = count/numEvents;
      }
      else {
        std::cout << "Error: NumberOfEvents is not valid." << std::endl;
        return false;
      }

      if (count >= CountThreshold()) {
        IsotopeInfo isotope(z, a, energy*unit::keV, floatingLevel);
        isotope.setRate(productionEfficiency);
        data_.back().second.push_back(isotope);
      }
    }
  }

  fin.close();
  return true;
}

bool RateData::writeFile(const std::string& filename)
{
  std::ofstream fout(filename.c_str());
  if (!fout) {
    std::cout << "Error in RateData::loadFile(): opening file failed: " << filename << std::endl;
    return false;
  }

  fout << "NumberOfEvents " << -1 << '\n' << std::endl;

  for (std::size_t i=0; i<data_.size(); i++) {
    const std::string volumeName = data_[i].first;
    const RateVector& rateVector = data_[i].second;
    
    fout << "Volume[" << i << "] " << volumeName << std::endl;
    
    for (const IsotopeInfo& isotope: rateVector) {
      fout << boost::format("Isotope %16d %3d %3d %15.9e %2d %15.6e\n")
        % isotope.IsotopeID()
        % isotope.Z()
        % isotope.A()
        % (isotope.Energy()/unit::keV)
        % isotope.FloatingLevel()
        % (isotope.Rate()*unit::second);
    }
    fout << std::endl;
  }
  fout.close();

  return true;
}

} /* namespace comptonsoft */
