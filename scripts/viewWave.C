#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TAxis.h"
#include "/home/daq/work/seamine2018_daq/kathrin/inc/defaults.h"
#include "/home/daq/work/seamine2018_daq/kathrin/inc/Event.hh"
char * ffilename = (char*)"/home/daq/work/seamine2018_daq/kathrin/root/run0018.root";
void SetFile(char* filename){
  ffilename = filename;
}
void ViewWave(int n, int board, int ch){
  TCanvas *c = new TCanvas("c","c",1200,600);  
  TFile *f = new TFile(ffilename);
  TTree* tr = (TTree*)f->Get("tr");
  Event* evt = new Event();
  tr->SetBranchAddress("event",&evt);
  Int_t status = tr->GetEvent(n);
  Wave *w;
  //cout << "event length " << evt->GetLength() << endl;
  for(int i=0; i<evt->GetLength(); i++){
    w = evt->GetWave(i);
    //cout << w->GetBoard() <<"\t" << w->GetCh() << endl;
    if(w->GetBoard()==board && w->GetCh()==ch)
      break;
  }
  int data[MAXSAMPLES];
  int x[MAXSAMPLES];
  TGraph* g;
  int length = w->GetWave().size();
  w->Print();
  for(int i=0;i<length;i++){
    x[i] = i;
    data[i] = (int)w->GetWave()[i];
    //cout << x[i] << "\t" << data[i] << endl;
  }
  c->cd();
  g = new TGraph(length,x,data);
  g->SetTitle(Form("Wave event %d,board %d, channel %d",n,board,ch));
  g->Draw("AL");
}
void ViewWave(int n, int board){
  TCanvas *c = new TCanvas("c","c",1200,600);
  c->Divide(4,4);
  TFile *f = new TFile(ffilename);
  TTree* tr = (TTree*)f->Get("tr");
  Event* evt = new Event();
  tr->SetBranchAddress("event",&evt);
  Int_t status = tr->GetEvent(n);
  Wave *w;
  int data[MAXSAMPLES];
  int x[MAXSAMPLES];
  vector<TGraph*> gv;
  //cout << "event length " << evt->GetLength() << endl;
  for(int i=0; i<evt->GetLength(); i++){
    w = evt->GetWave(i);
    //cout << w->GetBoard() <<"\t" << w->GetCh() << endl;
    if(w->GetBoard()!=board)
      continue;
    int ch = w->GetCh();
    int data[MAXSAMPLES];
    int x[MAXSAMPLES];
    TGraph* g;
    int length = w->GetWave().size();
    for(int i=0;i<length;i++){
      x[i] = i;
      data[i] = (int)w->GetWave()[i];
      //cout << x[i] << "\t" << data[i] << endl;
    }
    c->cd(ch+1);
    g = new TGraph(length,x,data);
    g->SetTitle(Form("Wave event %d,board %d, channel %d",n,board,ch));
    gv.push_back(g);
    gv.back()->Draw("AL");
    
  }

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
