#include <sys/time.h>
#include <map>
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

map<int,int> PLAmap();
char * ffilename = (char*)"/home/daq/work/seamine2018_daq/kathrin/root/run0011.root";
//change the filename
void SetFile(char* filename){
  ffilename = filename;
}
//change run number (if standard name is chosen)
void SetRun(int run){
  ffilename = Form("/home/daq/work/seamine2018_daq/kathrin/root/run%04d.root",run);
}
//waits for user input
void PressEnterToContinue(){
  int c;
  printf( "Press ENTER to continue... " );
  fflush( stdout );
  do c = getchar(); 
  while ((c != '\n') && (c != EOF));
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
      
      if(w->GetBoard()!=BAFBOARD0 && w->GetBoard()!=BAFBOARD1)
	continue;
      int ch = -1;
      if(w->GetBoard()==BAFBOARD0 && (w->GetCh()==BAF0 ||w->GetCh()==BAF1 ||w->GetCh()==BAF2 ||w->GetCh()==BAF3) )
	ch = w->GetCh();
      if(w->GetBoard()==BAFBOARD1 && (w->GetCh()==BAF4 ||w->GetCh()==BAF5 ||w->GetCh()==BAF6) )
	ch = w->GetCh();
      
      if(ch==-1)
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
      if(w->GetBoard()!=RFBOARD || w->GetCh()!=RFCH)
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
// this function plots the Plastic waves for event nevt
void ViewPlastic(int nevt){
  TCanvas *c = new TCanvas("c","c",1200,600);
  c->Divide(2,2);
  map<int,int> mapPLA  = PLAmap();
  cout <<"mapping plastics" << endl;
  for(int i=0;i<5;i++)
    cout << i << "\t" << mapPLA[i] << endl;

  TFile *f = new TFile(ffilename);
  TTree* tr = (TTree*)f->Get("tr");
  Event* evt = new Event();
  tr->SetBranchAddress("event",&evt);
  Wave *w;
  int data[MAXSAMPLES];
  int x[MAXSAMPLES];
  vector<TGraph*> gv;
  bool found = false;
  for(int n=nevt;n<tr->GetEntries();n++){
    Int_t status = tr->GetEvent(n);
    for(int i=0; i<evt->GetNWaves(); i++){
      w = evt->GetWave(i);
      if(w->GetBoard()!=PLABOARD)
	continue;
      if(w->GetCh()!=PLA0 && w->GetCh()!=PLA1 && w->GetCh()!=PLA2 && w->GetCh()!=PLA3)
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
      found = true;
      for(int i=0;i<length;i++){
	x[i] = i;
	data[i] = (int)w->GetWave()[i];
	//cout << x[i] << "\t" << data[i] << endl;
      }
      c->cd(mapPLA[w->GetCh()]+1);
      g = new TGraph(length,x,data);
      g->SetTitle(Form("Plastic[%d] wave event %d, pulse %f, LED %d",mapPLA[w->GetCh()],n,w->GetMaxPH(),w->GetLED()));
      gv.push_back(g);
      gv.back()->Draw("AL");
      gv.back()->GetYaxis()->SetRangeUser(0,17000);
      TLine *ll = new TLine(w->GetLED(),0,w->GetLED(),17000);
      ll->SetLineColor(2);
      ll->Draw();
    }//event length
    if(found)
      break;
  }//entries
  c->Modified();
  c->Update();
}
// this function plots the Plastic waves for all events (PressEnterToContinue)
void ViewPlastic(){
  TCanvas *c = new TCanvas("c","c",1200,600);
  c->Divide(2,2);
  map<int,int> mapPLA  = PLAmap();
  cout <<"mapping plastics" << endl;
  for(int i=0;i<5;i++)
    cout << i << "\t" << mapPLA[i] << endl;

  TFile *f = new TFile(ffilename);
  TTree* tr = (TTree*)f->Get("tr");
  Event* evt = new Event();
  tr->SetBranchAddress("event",&evt);
  Wave *w;
  int data[MAXSAMPLES];
  int x[MAXSAMPLES];
  vector<TGraph*> gv;
  bool found = false;
  for(int n=0;n<tr->GetEntries();n++){
    Int_t status = tr->GetEvent(n);
    gv.clear();
    for(int i=0; i<evt->GetNWaves(); i++){
      w = evt->GetWave(i);
      if(w->GetBoard()!=PLABOARD)
	continue;
      if(w->GetCh()!=PLA0 && w->GetCh()!=PLA1 && w->GetCh()!=PLA2 && w->GetCh()!=PLA3)
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
      found = true;
      for(int i=0;i<length;i++){
	x[i] = i;
	data[i] = (int)w->GetWave()[i];
	//cout << x[i] << "\t" << data[i] << endl;
      }
	 c->cd(mapPLA[w->GetCh()]+1);
      g = new TGraph(length,x,data);
      g->SetTitle(Form("Plastic[%d] wave event %d, pulse %f, LED %d",mapPLA[w->GetCh()],n,w->GetMaxPH(),w->GetLED()));
      gv.push_back(g);
      gv.back()->Draw("AL");
      gv.back()->GetYaxis()->SetRangeUser(0,17000);
      TLine *ll = new TLine(w->GetLED(),0,w->GetLED(),17000);
      ll->SetLineColor(2);
      ll->Draw();
    }//event length
    if(found){
      c->Modified();
      c->Update();
      PressEnterToContinue();
    }
  }//entries
}
// this function plots the board=board channel=chan waves for 100 events 
void ViewChan(int board, int chan, int run =97){
  SetRun(run);
  TCanvas *c = new TCanvas("c","c",1200,600);

  TFile *f = new TFile(ffilename);
  TTree* tr = (TTree*)f->Get("tr");
  Event* evt = new Event();
  tr->SetBranchAddress("event",&evt);
  Wave *w;
  int data[MAXSAMPLES];
  int x[MAXSAMPLES];
  vector<TGraph*> gv;
  int ctr =0;
  for(int n=0;n<tr->GetEntries();n++){
    Int_t status = tr->GetEvent(n);
    for(int i=0; i<evt->GetNWaves(); i++){
      w = evt->GetWave(i);
      if(w->GetBoard()!=board)
	continue;
      if(w->GetCh()!=chan)
	continue;
      int length = w->GetLength();
      if(length<1 || w->GetWave().size()<1)
	continue;
      w->Print();
      int data[MAXSAMPLES];
      int x[MAXSAMPLES];
      TGraph* g;
      // if(w->GetLED()<0)
      // 	continue;
      for(int i=0;i<length;i++){
	x[i] = i;
	data[i] = (int)w->GetWave()[i];
	//cout << x[i] << "\t" << data[i] << endl;
      }
      c->cd();
      g = new TGraph(length,x,data);
      g->SetTitle(Form("board %d, channel %d",board, chan));
      gv.push_back(g);
      gv.back()->SetLineColor(ctr%5+1);
      if(ctr==0){
	gv.back()->Draw("AL");
	gv.back()->GetYaxis()->SetRangeUser(0,17000);
      }
      gv.back()->Draw("L");
      ctr++;
    }//event length
    if(ctr==100)
      break;
  }//entries
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
map<int,int> PLAmap(){
  map<int,int> m;
  m.insert(make_pair(PLA0,0));
  m.insert(make_pair(PLA1,1));
  m.insert(make_pair(PLA2,2));
  m.insert(make_pair(PLA3,3));
  // m[PLA0] = 0;
  // m[PLA1] = 1;
  // m[PLA2] = 2;
  // m[PLA3] = 3;
  return m;
}

