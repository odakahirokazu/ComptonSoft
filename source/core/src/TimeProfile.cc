/**
 * @file TimeProfile.cc
 * @brief source file of class TimeProfile
 * @author Hirokazu Odaka
 * @date 2016-05-04
 */

#include "TimeProfile.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include "CLHEP/Units/SystemOfUnits.h"

namespace comptonsoft
{

TimeProfile::~TimeProfile() = default;

bool TimeProfile::readFile(const std::string& filename)
{
  clear();
  
  std::ifstream fin(filename.c_str());
  if (!fin) {
    std::cout << "Error in TimeProfile::loadFile(): opening file failed: " << filename << std::endl;
    return false;
  }
  
  while (1) {
    double x1(0.0), x2(0.0), x3(0.0);

    constexpr int BUFSIZE = 256;
    char buf[BUFSIZE];
    fin.getline(buf, BUFSIZE);

    if (fin.gcount() >= BUFSIZE) {
      std::cout << "Error in TimeProfile::loadFile(): the line length may exceed the buffer size." << std::endl;
      return false;
    }

    if (fin.eof()) { break; }
    if (buf[0] == '#') { continue; }

    std::istringstream iss(buf);
    iss >> x1 >> x2 >> x3;
    const double time1 = x1 * CLHEP::second;
    const double time2 = x2 * CLHEP::second;
    const double rate = x3 / CLHEP::second;
    push(time1, time2, rate);
  }

  fin.close();
  return true;
}

} /* namespace comptonsoft */
