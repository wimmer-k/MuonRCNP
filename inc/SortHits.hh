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
/*!
  sorts the hits according to their time stamps, build events
*/
class SortHits{
 public:
  //! default constructor
  SortHits();
  //! constructor with the root output file
  SortHits(TFile *out);
  //! explicitely set the root output file
  void SetRootFile(TFile *out);
  //! add a fragment to the list
  bool Add(Fragment* addme);
  //! if the list is full remove the oldest
  bool FullList(Fragment* addme);
  //! insert a fragment into the list
  bool Insert(Fragment* insertme);
  //! write out the fragment to the event
  void WriteFragment(Fragment* writeme);
  //! close the event, write it to rootfile, and clear
  void CloseEvent();
  //! print the list of fragments currently in memory
  void PrintList();
  //! flush the list, at the end, write everything to events, and files
  void Flush();
  //! print the status of the event builder
  void Status();
  //! set the memory depth of the list
  void SetMemDepth(int memdepth){fmemdepth = memdepth;}
  //! set the eventbuilding window
  void SetWindow(unsigned int window){fwindow = window;}
  //! set the verbose level
  void SetVerbose(int vl){fvl = vl;}
  //! returns the tree
  TTree* GetTree(){return ftr;}
  //! returns the last timestamp added to the last event
  unsigned long long int GetLastTS(){return flastTS;}
  //! returns the number of fragments
  int GetFragNr(){return ffragnr;}
private:
  //! the fragment list
  list<Fragment*> flist;
  //! the event container
  Event* fevent;
  //! the number of fragments processed
  int ffragnr;
  //! the number of events built
  int fevtnr;
  //! the number of fragments discarded
  int fdiscarded;
  //! the output root file
  TFile *frootfile;
  //! the memory depth
  int fmemdepth;
  //! the length of the eventbuilding window
  unsigned int fwindow;
  //! the time stamp of the last written event
  unsigned long long int flastTS;
  //! the verbose level
  int fvl;
  //! the output tree
  TTree *ftr;
  //! basic diagnostic histogram, difference of subsequent timestamps
  TH1F* fhTSdiff;
  //! basic diagnostic histogram, fragment timestamp as function of event number
  TH2F* fhfragTS;
  //! basic diagnostic histogram, event (last fragment) timestamp as function of event number
  TH2F* fheventTS;
};
#endif
