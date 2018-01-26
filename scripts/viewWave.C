#include <sys/time.h>
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TAxis.h"
#include "TLine.h"
#include "/home/daq/work/seamine2018_daq/kathrin/inc/globaldefs.h"
#include "/home/daq/work/seamine2018_daq/kathrin/inc/Event.hh"
char * ffilename = (char*)"/home/daq/work/seamine2018_daq/kathrin/root/run0011.root";
//change the filename
void SetFile(char* filename){
  ffilename = filename;
}
//change run number (if standard name is chosen)
void SetRun(int run){
  ffilename = Form("/home/daq/work/seamine2018_daq/kathrin/root/run%04d.root",run);
}
// this function plots the BaF waves (up to "nwaves") (if written to file) that have a valid LED for events starting from "nstart"  
void ViewBaF(int nstart, double thresh = 200, int nwaves=9){
  if(nwaves>16){
    cout << "plot only up to 16 events" << endl;
    nwaves = 16;
  }    
  TCanvas *c = new TCanvas("c","c",1200,600);
  if(nwaves>12)
    c->Divide(4,4);
  else if(nwaves>9)
    c->Divide(4,3);
  else if(nwaves>6)
    c->Divide(3,3);
  else if(nwaves>4)
    c->Divide(3,2);
  else if(nwaves>2)
    c->Divide(2,2);
  else if(nwaves>1)
    c->Divide(2,1);

  TFile *f = new TFile(ffilename);
  TTree* tr = (TTree*)f->Get("tr");
  Event* evt = new Event();
  tr->SetBranchAddress("event",&evt);
  Wave *w;
  int data[MAXSAMPLES];
  int x[MAXSAMPLES];
  vector<TGraph*> gv;
  int ctr=0;
  for(int n=nstart;n<tr->GetEntries();n++){
    Int_t status = tr->GetEvent(n);
    //cout << "event = " << n << endl;
    //cout << "event wave length " << evt->GetNWaves() << endl;
    for(int i=0; i<evt->GetNWaves(); i++){
      w = evt->GetWave(i);
      if(w->GetBoard()!=BAFBOARD)
	continue;
      int ch = w->GetCh();
      if(ch<BAFCHSTA || ch >=BAFCHSTA+NBAFS)
	continue;
      if(w->GetLED()<0)
	continue;
      if(w->GetMaxPH()<thresh)
	continue;
      w->Print();
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
      ctr++;
      cout << "ctr = " << ctr << endl;
      c->cd(ctr);
      g = new TGraph(length,x,data);
      g->SetTitle(Form("BaF wave event %d, channel %d, pulse %f",n,ch,w->GetMaxPH()));
      gv.push_back(g);
      gv.back()->Draw("AL");
      gv.back()->GetYaxis()->SetRangeUser(0,17000);
      if(ctr==nwaves)
	break;
    }//event length
    if(ctr==nwaves)
      break;
  }//events
  c->Modified();
  c->Update();
}
// this function plots the RF waves (up to "nwaves") (if written to file) for events starting from "nstart"  
void ViewRF(int nstart, int nwaves=9){
  if(nwaves>16){
    cout << "plot only up to 16 events" << endl;
    nwaves = 16;
  }    
  TCanvas *c = new TCanvas("c","c",1200,600);
  if(nwaves>12)
    c->Divide(4,4);
  else if(nwaves>9)
    c->Divide(4,3);
  else if(nwaves>6)
    c->Divide(3,3);
  else if(nwaves>4)
    c->Divide(3,2);
  else if(nwaves>2)
    c->Divide(2,2);
  else if(nwaves>1)
    c->Divide(2,1);

  TFile *f = new TFile(ffilename);
  TTree* tr = (TTree*)f->Get("tr");
  Event* evt = new Event();
  tr->SetBranchAddress("event",&evt);
  Wave *w;
  int data[MAXSAMPLES];
  int x[MAXSAMPLES];
  vector<TGraph*> gv;
  int ctr=0;
  for(int n=nstart;n<tr->GetEntries();n++){
    Int_t status = tr->GetEvent(n);
    //cout << "event = " << n << endl;
    //cout << "event wave length " << evt->GetNWaves() << endl;
    for(int i=0; i<evt->GetNWaves(); i++){
      w = evt->GetWave(i);
      if(w->GetBoard()!=RFBOARD && w->GetCh()!=RFCH)
	continue;
      w->Print();
      int data[MAXSAMPLES];
      int x[MAXSAMPLES];
      TGraph* g;
      int length = w->GetLength();
      if(length<1 || w->GetWave().size()<1)
	continue;
      if(w->GetLED()<0)
	continue;
      for(int i=0;i<length;i++){
	x[i] = i;
	data[i] = (int)w->GetWave()[i];
	//cout << x[i] << "\t" << data[i] << endl;
      }
      ctr++;
      cout << "ctr = " << ctr << endl;
      c->cd(ctr);
      g = new TGraph(length,x,data);
      g->SetTitle(Form("RF wave event %d, pulse %f, time %d",n,w->GetMaxPH(),w->GetMaxPHTime()));
      gv.push_back(g);
      gv.back()->Draw("AL");
      gv.back()->GetYaxis()->SetRangeUser(0,17000);
      TLine *ll = new TLine(w->GetMaxPHTime(),0,w->GetMaxPHTime(),17000);
      ll->SetLineColor(2);
      ll->Draw();
      if(ctr==nwaves)
	break;
    }//event length
    if(ctr==nwaves)
      break;
  }//events
  c->Modified();
  c->Update();
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
  g->GetYaxis()->SetRangeUser(0,17000);
}
// this function plots all waves (if written to file) for event "n" and board "board"
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
    //cout << w->GetBoard() <<"\t" << w->GetCh() << endl;
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
    gv.back()->GetYaxis()->SetRangeUser(0,17000);
  }

}
// this function plots all waves (if written to file) that have a valid LED for event "n" 
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
  for(int i=0; i<evt->GetNWaves(); i++){
    w = evt->GetWave(i);
    int bd = w->GetBoard();
    int ch = w->GetCh();
    TGraph* g;
    int length = w->GetLength();
    if(length<1 || w->GetWave().size()<1 || w->GetLED()<0)
      continue;
    w->Print();
    for(int j=0;j<length;j++){
      x[j] = j;
      data[j] = (int)w->GetWave()[j];
      //cout << x[i] << "\t" << data[i] << endl;
    }
    g = new TGraph(length,x,data);
    g->SetTitle(Form("Wave event %d,board %d, channel %d",n,bd,ch));
    gv.push_back(g);
  }
  
  if(gv.size()>16){
    cout << "too many waves to plot" << gv.size() << endl;
    return;
  }
  if(gv.size()>12)
    c->Divide(4,4);
  else if(gv.size()>9)
    c->Divide(4,3);
  else if(gv.size()>6)
    c->Divide(3,3);
  else if(gv.size()>4)
    c->Divide(3,2);
  else if(gv.size()>2)
    c->Divide(2,2);
  else if(gv.size()>1)
    c->Divide(2,1);

  for(unsigned short i=0;i<gv.size();i++){
    c->cd(1+i);
    gv.at(i)->Draw("AL");
    gv.at(i)->GetYaxis()->SetRangeUser(0,17000);
  }
  
}

