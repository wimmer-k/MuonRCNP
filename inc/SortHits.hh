#ifndef __SORTHITS_HH 
#define __SORTHITS_HH
#include <iostream>
#include <list>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "Event.hh"
#include "Fragment.hh"
using namespace std;

class SortHits{
 public:
  SortHits();
  SortHits(TFile *out);
  void SetRootFile(TFile *out);
  bool Add(Fragment* addme);
  bool FullList(Fragment* addme);
  bool Insert(Fragment* insertme);
  void WriteFragment(Fragment* writeme);
  void CloseEvent();
  void PrintList();
  void Flush();
  void Status();
  void SetMemDepth(int memdepth){fmemdepth = memdepth;}
  void SetWindow(int window){fwindow = window;}
  void SetVerbose(int vl){fvl = vl;}
  TTree* GetTree(){return ftr;}
  long long int GetLastTS(){return fevent->GetLastTS();}
private:
  list<Fragment*> flist;
  Event* fevent;
  int ffragnr;
  int fevtnr;
  int fdiscarded;
  TFile *frootfile;
  int fmemdepth;
  int fwindow;
  int fvl;
  TTree *ftr;
  TH1F* fhTSdiff;
  TH2F* fhfragTS;
  TH2F* fheventTS;
};
#endif
