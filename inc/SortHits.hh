#ifndef __SORTHITS_H 
#define __SORTHITS_H
#include <iostream>
#include <list>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "Wave.hh"
using namespace std;
struct item{
  long long TS;
  vector<Wave*> waves;
};
class SortHits{
 public:
  SortHits();
  SortHits(TFile *out);
  void SetRootFile(TFile *out);
  bool Add(item* addme);
  bool FullList(item* addme);
  bool Insert(item* insertme);
  void WriteFile(item* writeme);
  void PrintList();
  void Flush();
  void Status();
  void SetMemDepth(int memdepth){fmemdepth = memdepth;}
 private:
  list<item*> flist;
  int fevtnr;
  int fdiscarded;
  TFile *frootfile;
  int fmemdepth;
  long long flastts;
  TTree *ftr;
  long long fts;
  TH1F* fhtdiff;
};

class TSComparer {
public:
  bool operator() ( item *lhs, item *rhs) {
    return (*rhs).TS < (*lhs).TS;
  }
};

#endif
