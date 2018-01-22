#ifndef __PROCESSHITS_H 
#define __PROCESSHITS_H
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
};
#endif
