%module decayChains
%{
#include <csignal>
#include <cstring>
#include <iostream>
#include "RIDecayCalculation.hh"
#include "IsotopeDatabaseAccess.hh"
%}

%include "std_string.i"

namespace comptonsoft {

class RIDecayCalculation
{
public:
  RIDecayCalculation();
  virtual ~RIDecayCalculation();

  void setVerboseLevel(int v);
  void setInputFiles(const std::string& production_rate,
                     const std::string& irradiation);

  void setMeasurementTime(double t);
  double MeasurememtTime() const;

  void addMeasurementWindow(double t1, double t2);
  void clearMeasurememtWindows();
  double getStartingTimeOfWindow(std::size_t i) const;
  double getEndingTimeOfWindow(std::size_t i) const;

  void setCountThreshold(double v);
  double CountThreshold() const;
  void setDecayConstantThreshold(double v);
  double DecayConstantThreshold() const;
  void setBranchingRatioThreshold(double v);
  double BranchingRatioThreshold() const;

  void setOutputFile(const std::string& decay_rate);

  %extend {
    bool run()
    {
      if ($self->initialize()) {
        struct sigaction sa;
        struct sigaction sa_org;
        std::memset(&sa, 0, sizeof(sa));
        std::memset(&sa_org, 0, sizeof(sa_org));
        sa.sa_handler = SIG_DFL;
        sa.sa_flags |= SA_RESTART;
        if ( sigaction(SIGINT, &sa, &sa_org) != 0 ) {
          std::cout << "sigaction(2) error!" << std::endl;
          return false;
        }
      
        $self->perform();
        $self->output();

        if ( sigaction(SIGINT, &sa_org, 0) != 0 ) {
          std::cout << "sigaction(2) error!" << std::endl;
          return false;
        }
        
        return true;
      }

      return false;
    }
  }
};

class IsotopeDatabaseAccess
{
public:
  IsotopeDatabaseAccess();
  virtual ~IsotopeDatabaseAccess();

  void initialize_data();
  void retrive_isotope(int z, int a, double energy);
  double get_lifetime() const;
  double get_halflife() const;
};

} /* namespace comptonsoft */
