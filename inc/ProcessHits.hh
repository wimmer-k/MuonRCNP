#ifndef __PROCESSHITS_HH 
#define __PROCESSHITS_HH
#include <iostream>
#include "TEnv.h"
#include "Wave.hh"
using namespace std;

class ProcessHits{
 public:
  ProcessHits();
  ProcessHits(char *settings);
  void AnalyzeWave(Wave* wave);
  void SetVerbose(int vl){fvl = vl;}
 private:
  int fvl;
  int fsamplesforbaseline;
};
#endif
