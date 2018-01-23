#ifndef __PROCESSHITS_HH 
#define __PROCESSHITS_HH
#include <iostream>
#include "TEnv.h"
#include "TRandom.h"
#include "Wave.hh"
#include "PHA.hh"
#include "globaldefs.h"

using namespace std;
/*!
  processes the hits, applies calibration and wave form analysis
*/
class ProcessHits{
 public:
  //! default constructor
  ProcessHits();
  //! standard constructor with a settings file
  ProcessHits(char *settings);
  //! analyze the waveforms
  void AnalyzeWave(Wave* wave);
  //! analyse the pulse height data
  void AnalyzePHA(PHA* pha);
  //! set the verbose level
  void SetVerbose(int vl){fvl = vl;}
 private:
  //! random number generator for the calibration
  TRandom* fRand;
  //! the verbose level 
  int fvl;
  //read from settings file
  //! the number of samples taken to estimate the baseline
  int fsamplesforbaseline;
  //! the gains of the germaniums
  double fGegain[2];
  //! the offsets of the germaniums
  double fGeoffs[2];
};
#endif
