#include <sys/time.h>
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TAxis.h"
#include "/home/daq/work/seamine2018_daq/kathrin/inc/globaldefs.h"
#include "/home/daq/work/seamine2018_daq/kathrin/inc/Event.hh"
char * ffilename = (char*)"/home/daq/work/seamine2018_daq/kathrin/root/run0011.root";
void SetFile(char* filename){
  ffilename = filename;
}
void ViewBaF(int nstart, double thresh = 200, int nevents=9){
  TCanvas *c = new TCanvas("c","c",1200,600);
  c->Divide(3,3);
  TFile *f = new TFile(ffilename);
  TTree* tr = (TTree*)f->Get("tr");
  Event* evt = new Event();
  tr->SetBranchAddress("event",&evt);
  Wave *w;
  int data[MAXSAMPLES];
  int x[MAXSAMPLES];
  vector<TGraph*> gv;
  for(int n=nstart;n<nstart+nevents;n++){
    Int_t status = tr->GetEvent(n);
    cout << "event = " << n << endl;
    //cout << "event wave length " << evt->GetNWaves() << endl;
    for(int i=0; i<evt->GetNWaves(); i++){
      w = evt->GetWave(i);
      if(w->GetBoard()!=NBAFBOARD)
	continue;
      int ch = w->GetCh();
      if(ch<BAFCHSTA || ch >=BAFCHSTA+NBAFS)
	continue;
      if(w->GetMaxPH()<thresh)
	continue;
      cout << "board: " <<w->GetBoard() <<", ch: " << w->GetCh() << ", PH: " << w->GetMaxPH()<< endl;
      int data[MAXSAMPLES];
      int x[MAXSAMPLES];
      TGraph* g;
      int length = w->GetLength();
      if(length<1 || w->GetWave().size()<1)
	continue;
      for(int i=0;i<length;i++){
	x[i] = i;
	data[i] = (int)w->GetWave()[i];
	//cout << x[i] << "\t" << data[i] << endl;
      }
      c->cd(n-nstart+1);
      g = new TGraph(length,x,data);
      g->SetTitle(Form("BaF wave event %d, channel %d, pusle %f",n,ch,w->GetMaxPH()));
      gv.push_back(g);
      gv.back()->Draw("AL");
    }//event length
  }//events
}
void ViewWave(int n, int board, int ch){
  TCanvas *c = new TCanvas("c","c",1200,600);  
  TFile *f = new TFile(ffilename);
  TTree* tr = (TTree*)f->Get("tr");
  Event* evt = new Event();
  tr->SetBranchAddress("event",&evt);
  Int_t status = tr->GetEvent(n);
  Wave *w;
  cout << "event wave length " << evt->GetNWaves() << endl;
  if(evt->GetNWaves()<1)
    return;
  for(int i=0; i<evt->GetNWaves(); i++){
    w = evt->GetWave(i);
    //cout << w->GetBoard() <<"\t" << w->GetCh() << endl;
    if(w->GetBoard()==board && w->GetCh()==ch)
      break;
  }
  int data[MAXSAMPLES];
  int x[MAXSAMPLES];
  TGraph* g;
  w->Print();
  int length = w->GetLength();
  if(length<1 || w->GetWave().size()<1)
    return;
  for(int i=0;i<length;i++){
    x[i] = i;
    data[i] = (int)w->GetWave()[i];
    //cout << x[i] << "\t" << data[i] << endl;
  }
  c->cd();
  g = new TGraph(length,x,data);
  g->SetTitle(Form("Wave event %d, board %d, channel %d",n,board,ch));
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
  cout << "event wave length " << evt->GetNWaves() << endl;
  for(int i=0; i<evt->GetNWaves(); i++){
    w = evt->GetWave(i);
    cout << w->GetBoard() <<"\t" << w->GetCh() << endl;
    if(w->GetBoard()!=board)
      continue;
    int ch = w->GetCh();
    TGraph* g;
    int length = w->GetLength();
    if(length<1 || w->GetWave().size()<1)
      continue;
    for(int j=0;j<length;j++){
      x[j] = j;
      data[j] = (int)w->GetWave()[j];
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
  Event* evt = new Event();
  tr->SetBranchAddress("event",&evt);
  Wave* w = NULL;
  Int_t status = tr->GetEvent(n);
  int data[MAXSAMPLES];
  int x[MAXSAMPLES];
  vector<TGraph*> gv;
  if(evt->GetNWaves()>16){
    cout << "too many waves to plot" << endl;
    return;
  }
  if(evt->GetNWaves()>12)
    c->Divide(4,4);
  if(evt->GetNWaves()>9)
    c->Divide(4,4);
  if(evt->GetNWaves()>6)
    c->Divide(3,3);
  if(evt->GetNWaves()>4)
    c->Divide(3,2);
  if(evt->GetNWaves()>2)
    c->Divide(2,2);
  if(evt->GetNWaves()>1)
    c->Divide(2,1);

  //cout << "event wave length " << evt->GetNWaves() << endl;
  for(int i=0; i<evt->GetNWaves(); i++){
    w = evt->GetWave(i);
    int bd = w->GetBoard();
    int ch = w->GetCh();
    TGraph* g;
    int length = w->GetLength();
    if(length<1 || w->GetWave().size()<1)
      continue;
    for(int j=0;j<length;j++){
      x[j] = j;
      data[j] = (int)w->GetWave()[j];
      //cout << x[i] << "\t" << data[i] << endl;
    }
    c->cd(ch+1);
    g = new TGraph(length,x,data);
    g->SetTitle(Form("Wave event %d,board %d, channel %d",n,bd,ch));
    gv.push_back(g);
    gv.back()->Draw("AL");
  }
}

