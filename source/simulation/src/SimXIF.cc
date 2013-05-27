/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Authors of SIMX, Hirokazu Odaka                    *
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

#include "SimXIF.hh"

#include <algorithm>

#include "Randomize.hh"
#include "G4ThreeVector.hh"
#include "AstroUnit.hh"
#include "PhaseSpaceVector.hh"

#include "RealDetectorUnit.hh"
#include "DetectorManager.hh"
#include "ConstructDetector_Sim.hh"

#include "fitsio.h"

namespace simx {

struct SimputCatalog;
#include "simx.h"

}

using namespace anl;
using namespace comptonsoft;


extern "C"
void get_input_data(int argc, char *argv[],
                    simx::Param_File_Type *p_input,
                    struct simx::PARAMETERS *params,
                    int *status);

/***** SimX version <= 1 ****
extern "C"
void initialize(struct simx::PARAMETERS *params,
                struct simx::Source_Type *src, 
                int *status);
*****/

extern "C"
void initialize(struct simx::PARAMETERS *params,
                struct simx::Response_Type **rsp,
                struct simx::Source_Type *src, 
                simx::SimputCatalog **simcat,
                int *status);

extern "C"
int fits_output(struct simx::PARAMETERS *params,
                struct simx::Source_Type *src,
                struct simx::Event_Type *evt,
                int *status);

extern "C"
double timeset(struct simx::PARAMETERS *params, int *idum, int *status);

extern "C"
void specmod(double *specin, int Nspec, int *idum, int *status);

extern "C"
void get_position(struct simx::PARAMETERS *params,
                  struct simx::Source_Type *src, 
                  int image_size, double image_prob[], double *xs, double *ys,
                  double *costheta, double *sintheta, int *idum, int *status);

namespace {

/*****
void generate_primaries(struct simx::PARAMETERS *params,
                        struct simx::Source_Type *src,
                        double area,
                        std::vector<PhaseSpaceVector>* primaries);
*****/

void generate_primaries(struct simx::PARAMETERS *params,
                        struct simx::Response_Type *response,
                        struct simx::Source_Type *src,
                        double area,
                        std::vector<PhaseSpaceVector>* primaries);

void output_primaries(const std::vector<PhaseSpaceVector>* primaries,
                      const char* file_name);

bool compair_time(const PhaseSpaceVector& left, const PhaseSpaceVector& right)
{
  return left.Time() < right.Time();
}

}

SimXIF::SimXIF()
  : m_SimXParameters(0),
    m_Source(0),
    m_Response(0),
    m_EventList(0),
    m_Event(0),
    m_SimXStatus(0),
    m_DetectorManager(0)
{
  add_alias("SimXIF");

  m_SimXParameters = new simx::PARAMETERS;
  m_Source = new simx::Source_Type;
  m_Response = 0; // be careful about memory leak
  m_PrimaryIter = m_Primaries.begin();
}


SimXIF::~SimXIF()
{
  delete m_Source;
  delete m_SimXParameters;

  struct simx::Event_Type* eventList = m_EventList;
  struct simx::Event_Type* event = eventList;
  while (event != 0) {
    eventList = event->next;
    delete event;
    event = eventList;
  }
  m_Event = 0;
  m_EventList = 0;
}


ANLStatus SimXIF::mod_prepare()
{
  std::cout << "*******************************\n"
            << "***      SimX 2.0 IF        ***\n"
            << "***   ASTRO-H simulation    ***\n"
            << "*******************************" << std::endl;
  
  /* Get needed inputs: outfile, pointing, source parameters 
     (including flat/ptsrc/image file; position of source, spectrum)
  */
  int argc = 1;
  char simx_str[] = "simx";
  char* argv[1];
  argv[0] = simx_str;
  get_input_data(argc, argv, NULL, m_SimXParameters, &m_SimXStatus);
  if (m_SimXStatus != 0) {
    std::cout << "simx: Error reading input data.  Exiting without running." << std::endl;
    return AS_QUIT_ERR;
  }

  std::string filter("SimXIF:FilterName:");
  filter += m_SimXParameters->FilterName;
  EvsDef(filter);
  
  return AS_OK;
}


ANLStatus SimXIF::mod_init()
{
  /* Read in files, set things up, etc */ 
  simx::SimputCatalog *simcat = 0;
  initialize(m_SimXParameters, &m_Response, m_Source, &simcat, &m_SimXStatus);
  if (m_SimXStatus != 0) {
    std::cout << "simx: Error doing initialization.  Exiting without running." << std::endl;
    return AS_QUIT_ERR;
  }

  off();

  if (ModuleExist("ConstructDetector_Sim")) {
    m_DetectorManager = GetANLModuleNC<ConstructDetector_Sim>("ConstructDetector_Sim")->GetDetectorManager();
  }
  
  return AS_OK;
}


ANLStatus SimXIF::mod_ana()
{
  if (m_PrimaryIter == m_Primaries.end()) return AS_QUIT;

  return AS_OK;
}


void SimXIF::generatePrimaries(double area)
{
  /* Apply identical scales to convert SKY into DET */
  const double tcdlt2 = m_SimXParameters->tcdlt2;
  const double tcdlt3 = m_SimXParameters->tcdlt3;
  m_SimXParameters->tcdlt2 = 1.0;
  m_SimXParameters->tcdlt3 = 1.0;

  generate_primaries(m_SimXParameters, m_Response, m_Source,
                     area/cm2, &m_Primaries);

  m_SimXParameters->tcdlt2 = tcdlt2;
  m_SimXParameters->tcdlt3 = tcdlt3;

  m_PrimaryIter = m_Primaries.begin();
  on();
}


PhaseSpaceVector SimXIF::takePrimary()
{
  return *(m_PrimaryIter++);
}


void SimXIF::addEvent(double time, double energy, int stripx, int stripy,
                      int detector_id)
{
  vector3_t xaxis(1., 0., 0.);
  vector3_t yaxis(0., 1., 0.);
  vector3_t offset(0., 0., 0.);
  double widthx(0.0);
  double widthy(0.0);

  if (m_DetectorManager==0 && ModuleExist("ConstructDetector_Sim")) {
    m_DetectorManager = GetANLModuleNC<ConstructDetector_Sim>("ConstructDetector_Sim")->GetDetectorManager();
  }
  
  if (m_DetectorManager) {
    RealDetectorUnit* det = m_DetectorManager->getDetectorByID(detector_id);
    if (det) {
      double offsetx = det->getCenterPositionX() / det->getPixelPitchX();
      double offsety = det->getCenterPositionY() / det->getPixelPitchY();
      offset.set(offsetx, offsety, 0.0);
      xaxis = det->getDirectionX();
      yaxis = det->getDirectionY();
      widthx = det->getWidthX() / det->getPixelPitchX();
      widthy = det->getWidthY() / det->getPixelPitchY();
    }
  }
  
  simx::Event_Type* event = new simx::Event_Type;
  event->Time = time/second;
  double localx = stripx;
  double localy = stripy;
  vector3_t position = offset + (localx-0.5*widthx)*xaxis + (localy-0.5*widthy)*yaxis;
 
  event->x = m_SimXParameters->NumPixelsX - position.x() + (G4UniformRand()-0.5);
  event->y = 1.0 + position.y() + (G4UniformRand()-0.5);
  event->ix = static_cast<int>(event->x+0.5);
  event->iy = static_cast<int>(event->y+0.5);
  event->Energy = energy/keV;
  event->pi = findPI(energy);
  
  event->resflag = HIGHRES;
  event->background = FALSE;
  event->next = 0;
  
  if (m_EventList == 0) {
    m_EventList = event;
    m_Event = m_EventList;
  }
  else {
    m_Event->next = event;
    m_Event = m_Event->next;
  }
}


void SimXIF::outputEvents()
{
  fits_output(m_SimXParameters, m_Source, m_EventList, &m_SimXStatus);
}


void SimXIF::outputPrimaries(const std::string& file_name)
{
  std::string file("!");
  file += file_name;
  output_primaries(&m_Primaries, file.c_str());
}


int SimXIF::findPI(double energy)
{
  const int length = m_Response->rmf_Nchannels;
  float* array = m_Response->rmf_emin;
  float* ptr = std::lower_bound(array, array+length, energy/keV) - 1;
  int index = (ptr-1) - array; 
  int channel = -1;
  if (index>=0) channel = (m_Response->rmf_channel)[index];
  return channel;
}


namespace {

void generate_primaries(struct simx::PARAMETERS *params,
                        struct simx::Response_Type *response,
                        struct simx::Source_Type *src,
                        double area,
                        std::vector<PhaseSpaceVector> *primaries)
{
  // this function is based on simx/generate_events.c
  int idum;
  if (params->RandomSeed == -1) {
    idum = (int) time(NULL);
  }
  else {
    idum = params->RandomSeed;
  }
  if (idum > 0) idum = -idum;
  if (idum == 0) idum = -1;

  double exposure = params->Exposure;
  double* specin = (double *) malloc((src->Nspec)*sizeof(double));
  // simx::Response_Type* response = &(src->rsp);
  for (int ii=0; ii<src->Nspec; ii++) {
    double eBinWidth = response->arf_ebin_hi[ii] - response->arf_ebin_lo[ii];
    specin[ii] = src->spectrum[ii] * eBinWidth * area * exposure;
#if 0
    std::cout << ii << "  "
              << src->energy[ii] << "  " << eBinWidth << "  "
              << src->spectrum[ii] << "  " << specin[ii] << std::endl;
#endif
  }
  
  int image_size = -1;
  double* image_prob = NULL;

  if (params->SourceImageType == IMAGE) {
    image_size = src->naxes[0]*src->naxes[1];
    image_prob = (double *) malloc(image_size*sizeof(double));
    
    /* Generate integrated 2d image, for probability use. */
    image_prob[0] = src->image[0];
    for (int ii=1; ii<image_size; ii++) {
      image_prob[ii] = src->image[ii] + image_prob[ii-1];
    }
  }

  /* Handle poisson errors */
  int specStatus = 0;
  int* status = &specStatus;
  specmod(specin,src->Nspec,&idum,status);

  int numEvent = 0;
  for(int ii=0; ii<src->Nspec; ii++) {
    if(specin[ii] < 0) specin[ii] = 0;
    numEvent += (int) specin[ii];
  }

  primaries->resize(numEvent);

  int i = 0;
  for (int ii=0; ii<src->Nspec; ii++) {
    /* Q: Is this OK if specin[ii] was reset by specmod? */
    int is = (int) specin[ii];
    for (int iEvent=0; iEvent<is; iEvent++) {
      /* Get the raw event position */
      double xs=0.0, ys=0.0, costheta=0.0, sintheta=0.0;
      get_position(params, src, image_size, image_prob, &xs, &ys, 
                   &costheta, &sintheta, &idum, status);
      
      double xsrad = xs*DEG2RAD;
      double ysrad = ys*DEG2RAD;
      PhaseSpaceVector primary(src->energy[ii]*keV,
                               -sin(xsrad),
                               -cos(xsrad)*sin(ysrad),
                               -cos(xsrad)*cos(ysrad));
      primary.setTime(timeset(params,&idum,status)*second);
      
      // std::cout << xs << " " << ys << std::endl;
      primaries->at(i++) = primary;
    }
  }

  std::sort(primaries->begin(), primaries->end(), compair_time);

  free(specin);
  specin = 0;
  if (image_prob) free(image_prob);
  image_prob = 0;
}


void output_primaries(const std::vector<PhaseSpaceVector>* primaries,
                      const char* file_name)
{
  int fitsStatus = 0;
  fitsfile *fits;
  fits_create_file(&fits, file_name, &fitsStatus);
  if (fitsStatus) {
    fits_report_error(stderr, fitsStatus);
    return;
  }

  // fits_create_hdu(fits, &fitsStatus);
  
  const char* ttype[] = {"energy", "theta", "phi"};
  const char* tform[] = {"F16.3", "F16.12", "F16.12"};
  const char* tunit[] = {"keV", "", ""};
  const char* extname = "sky";
  fits_create_tbl(fits, ASCII_TBL, 0, 3,
                  const_cast<char**>(ttype),
                  const_cast<char**>(tform),
                  const_cast<char**>(tunit),
                  const_cast<char*>(extname),
                  &fitsStatus);
  
  size_t nRows = primaries->size();
  double* energyArray = new double[nRows];
  double* thetaArray = new double[nRows];
  double* phiArray = new double[nRows];

  for (size_t i=0; i<nRows; i++) {
    double energy = primaries->at(i).Energy()/keV;
    G4ThreeVector vec(-primaries->at(i).DirectionX(),
                      -primaries->at(i).DirectionY(),
                      -primaries->at(i).DirectionZ());
    double theta = vec.theta();
    double phi = vec.phi();
    energyArray[i] = energy;
    thetaArray[i] = theta;
    phiArray[i] = phi;
  }
  
  fits_write_col_dbl(fits, 1, 1, 0, nRows, energyArray, &fitsStatus);
  fits_write_col_dbl(fits, 2, 1, 0, nRows, thetaArray, &fitsStatus);
  fits_write_col_dbl(fits, 3, 1, 0, nRows, phiArray, &fitsStatus);

  // std::string keyName("NAXIS2");
  // std::string comment("");
  // fits_write_key_lng(fits, const_cast<char*>(keyName.c_str()), 100000000,
  //                    const_cast<char*>(comment.c_str()), &fitsStatus);
  
  fits_close_file(fits, &fitsStatus);
  if (fitsStatus) {
    fits_report_error(stderr, fitsStatus);
    return;
  }

  delete[] energyArray;
  delete[] thetaArray;
  delete[] phiArray;
}

}
