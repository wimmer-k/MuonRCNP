#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TAxis.h"
#include "/home/daq/work/seamine2018_daq/kathrin/inc/defaults.h"
#include "/home/daq/work/seamine2018_daq/kathrin/inc/Wave.hh"
char * ffilename = (char*)"/home/daq/work/seamine2018_daq/kathrin/root/run0004.root";
void SetFile(char* filename){
  ffilename = filename;
}
void ViewWave(int n){
  TCanvas *c = new TCanvas("c","c",1200,600);
  
  TFile *f = new TFile(ffilename);
  TTree* tr = (TTree*)f->Get("tr");
  Wave* w = new Wave();
  tr->SetBranchAddress("wave",&w);
  Int_t status = tr->GetEvent(n);
  int data[MAXSAMPLES];
  int x[MAXSAMPLES];
  TGraph* g;
  int length = w->GetWave().size();
  for(int i=0;i<length;i++){
    x[i] = i;
    data[i] = (int)w->GetWave()[i];
    //cout << x[i] << "\t" << data[i] << endl;
  }
  c->cd();
  g = new TGraph(length,x,data);
  g->SetTitle(Form("Wave event %d,board %d, channel %d",n,w->GetBoard(),w->GetCh()));
  g->Draw("AL");
}
