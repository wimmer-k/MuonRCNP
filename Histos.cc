#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <signal.h>
#include <sys/time.h>
#include <map>
#include "TFile.h"
#include "TChain.h"
#include "TROOT.h"
#include "TEnv.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TList.h"
#include "Event.hh"
#include "CommandLineInterface.hh"
#include "util.h"
#include "globaldefs.h"

using namespace std;
map<int,int> PLAmap();
map<int,int> BAFmap();
//map<int,int> SCImap();
int main(int argc, char *argv[]){
  double time_start = get_time();
  signal(SIGINT,signalhandler);
  vector<char*> infiles;
  char* outfile = NULL;
  int lastevent = -1;
  int vl = 0;
  //Read in the command line arguments
  CommandLineInterface* interface = new CommandLineInterface();
  interface->Add("-i", "input file(s)", &infiles);
  interface->Add("-o", "output file", &outfile);
  interface->Add("-l", "last event to be read", &lastevent);
  interface->Add("-v", "verbose level", &vl);
  interface->CheckFlags(argc, argv);

  if(infiles.size() == 0){
    cout << "no input file(s) given, exiting " << endl;
    return 99;
  }
  if(outfile==NULL){
    cout <<"no output file, exiting" << endl;
    return 98;
  }
  cout<<"input file(s):"<<endl;
  for(unsigned int i=0; i<infiles.size(); i++){
    cout<<infiles[i]<<endl;
  }
  cout<<"output file: "<<outfile<< endl;
  TChain* tr;
  tr = new TChain("tr");
  for(unsigned int i=0; i<infiles.size(); i++){
    tr->Add(infiles[i]);
  }
  if(tr == NULL){
    cout << "could not find tree tr in any input file " << endl;
    return 97;
  } 
  Event* evt = new Event();
  tr->SetBranchAddress("event",&evt);
  Double_t nentries = tr->GetEntries();
  cout << nentries << " entries in chain";
  if(lastevent>0){
    nentries = lastevent;
    cout << ", reading up to event " << nentries;
  }
  cout << endl;
  cout << "creating histograms" << endl;
  TList *hlist = new TList();
  //multiplicities
  TH2F* hmult[NBOARDS];
  for(int b=0; b<NBOARDS; b++){
    hmult[b] = new TH2F(Form("hmult_bd%d",b),Form("multtplicity board %d",b),16,0,16,10,0,10); hlist->Add(hmult[b]);
  }
  //TS differences
  TH1F *hGe_Pla_TSdiff[NGES][NPLAS];
  TH1F *hGe_gatePla[NGES][NPLAS];
  for(int g=0; g<NGES; g++){
    for(int p=0; p<NPLAS; p++){
      hGe_Pla_TSdiff[g][p] = new TH1F(Form("hGe%d_Pla%d_TSdiff",g,p), Form("TS difference Ge[%d] - Plastic[%d]",g,p), 2000,-400000,400000); hlist->Add(hGe_Pla_TSdiff[g][p]);
      hGe_gatePla[g][p] = new TH1F(Form("hGe%d_gatePla%d",g,p), Form("Ge[%d] gated on Plastic[%d]",g,p), 8000,-0,8000); hlist->Add(hGe_gatePla[g][p]);
    }
  }
  //Pla detectors
  TH1F *hPla_PH[NPLAS];
  TH2F *hPla_PH_sum = new TH2F("hPla_PH_sum", "Plastic max. pulse height summary", NPLAS,0,NPLAS, 2000,0,20000); hlist->Add(hPla_PH_sum);
  TH2F *hPla_TMAX_sum = new TH2F("hPla_TMAX_sum", "Plastic Time at max. pulse height summary", NPLAS,0,NPLAS, 1000,0,1000); hlist->Add(hPla_TMAX_sum);
  TH2F *hPla_LED_sum = new TH2F("hPla_LED_sum", "Plastic Time at max. pulse height summary", NPLAS,0,NPLAS, 1000,0,1000); hlist->Add(hPla_LED_sum);
  for(int p=0; p<NPLAS; p++){
    hPla_PH[p] = new TH1F(Form("hPla_PH_%d",p), Form("Plastic[%d] max. pulse height",p), 2000,0,20000); hlist->Add(hPla_PH[p]);
  }
  
  //BaF detectors
  TH1F *hBaF_PH[NBAFS];
  TH2F *hBaF_PH_sum = new TH2F("hBaF_PH_sum", "BaF max. pulse height summary", NBAFS,0,NBAFS, 2000,0,2000); hlist->Add(hBaF_PH_sum);
  TH2F *hBaF_TMAX_sum = new TH2F("hBaF_TMAX_sum", "BaF Time at max. pulse height summary", NBAFS,0,NBAFS, 1000,0,1000); hlist->Add(hBaF_TMAX_sum);
  TH2F *hBaF_LED_sum = new TH2F("hBaF_LED_sum", "BaF Time at max. pulse height summary", NBAFS,0,NBAFS, 1000,0,1000); hlist->Add(hBaF_LED_sum);
  for(int b=0; b<NBAFS; b++){
    hBaF_PH[b] = new TH1F(Form("hBaF_PH_%d",b), Form("BaF[%d] max. pulse height",b), 2000,0,2000); hlist->Add(hBaF_PH[b]);
  }
  
  //Sci detectors
  TH1F *hSci_PH[NSCINTS];
  TH1F *hSci_IntShort[NSCINTS];
  TH1F *hSci_IntLong[NSCINTS];
  TH2F *hSci_Int[NSCINTS];
  
  TH2F *hSci_PH_sum = new TH2F("hSci_PH_sum", "Sci max. pulse height summary", NSCINTS,0,NSCINTS, 4000,0,4000); hlist->Add(hSci_PH_sum);
  TH2F *hSci_TMAX_sum = new TH2F("hSci_TMAX_sum", "Sci Time at max. pulse height summary", NSCINTS,0,NSCINTS, 1000,0,1000); hlist->Add(hSci_TMAX_sum);
  TH2F *hSci_LED_sum = new TH2F("hSci_LED_sum", "Sci Time at max. pulse height summary", NSCINTS,0,NSCINTS, 1000,0,1000); hlist->Add(hSci_LED_sum);
  for(int b=0; b<NSCINTS; b++){
    hSci_PH[b] = new TH1F(Form("hSci_PH_%d",b), Form("Sci[%d] max. pulse height",b), 4000,0,4000); hlist->Add(hSci_PH[b]);
    hSci_IntShort[b] = new TH1F(Form("hSci_IntShort_%d",b), Form("Sci[%d] short integral",b), 4000,0,4000); hlist->Add(hSci_IntShort[b]);
    hSci_IntLong[b] = new TH1F(Form("hSci_IntLong_%d",b), Form("Sci[%d] long integral",b), 4000,0,40000); hlist->Add(hSci_IntLong[b]);
    hSci_Int[b] = new TH2F(Form("hSci_Int_%d",b), Form("Sci[%d] short vs long integral",b), 2000,0,40000, 2000,0,4000); hlist->Add(hSci_Int[b]);
  }
  
  //Ge detectors
  TH1F *hGe_cal[NGES];
  TH2F *hGe_raw_sum = new TH2F("hGe_raw_sum", "Ge raw pulse height summary",  NGES,0,NGES, 8000,0,8000); hlist->Add(hGe_raw_sum);
  TH2F *hGe_cal_sum = new TH2F("hGe_cal_sum", "Ge calibrated energy summary", NGES,0,NGES, 8000,0,8000); hlist->Add(hGe_cal_sum);
  for(int g=0; g<NGES; g++){
    hGe_cal[g] = new TH1F(Form("hGe_cal_%d",g), Form("Ge[%d] calibrated energy",g), 8000,0,8000); hlist->Add(hGe_cal[g]);
  }

  map<int,int> mapPLA  = PLAmap();
  if(vl>-1){
    cout <<"mapping plastics" << endl;
    for(int i=0;i<5;i++)
      cout << i << "\t" << mapPLA[i] << endl;
  }
  map<int,int> mapBAF  = BAFmap();
  if(vl>-1){
    cout <<"mapping BaF" << endl;
    for(int i=0;i<16;i++)
      cout << i << "\t" << mapBAF[i] << endl;
  }
  // map<int,int> mapSCI  = SCImap();
  // if(vl>-1){
  //   cout <<"mapping Scintillator" << endl;
  //   for(int i=0;i<13;i++)
  //     cout << i << "\t" << mapSCI[i] << endl;
  // }
  
  Int_t nbytes = 0;
  Int_t status;
  Wave *w;
  PHA *p;
  for(int i=0; i<nentries; i++){
    if(signal_received){
      break;
    }
    evt->Clear();
    if(vl>2)
      cout << "getting entry " << i << endl;
    status = tr->GetEvent(i);
    if(vl>2)
      cout << "status " << status << endl;
    if(status == -1){
      cerr<<"Error occured, couldn't read entry "<<i<<" from tree "<<tr->GetName()<<" in file "<<tr->GetFile()->GetName()<<endl;
      return 5;
    }
    else if(status == 0){
      cerr<<"Error occured, entry "<<i<<" in tree "<<tr->GetName()<<" in file "<<tr->GetFile()->GetName()<<" doesn't exist"<<endl;
      return 6;
    }
    nbytes += status;
    //histogram filling here
    //digitized data
    //remember the plastic time stamps to be correlated with the Ge
    unsigned long long int TSpla[NPLAS] = {0};
    int evtNpla[NPLAS] = {0};
    short LEDpla[NPLAS] = {-1};
    short mult[NBOARDS][16] = {0};
    for(int j=0; j<evt->GetNWaves(); j++){
      w = evt->GetWave(j);
      int bd = w->GetBoard();
      int ch = w->GetCh();
      mult[bd][ch]++;
      //Plastics
      if(bd==PLABOARD){
	if(ch==PLA0||ch==PLA1||ch==PLA2||ch==PLA3){
	  TSpla[mapPLA[ch]] = w->GetTS();
	  evtNpla[mapPLA[ch]] = w->GetNEvent();
	  LEDpla[mapPLA[ch]] = w->GetLED();
	  double maxPH = w->GetMaxPH();
	  hPla_PH[mapPLA[ch]]->Fill(maxPH);
	  hPla_PH_sum->Fill(mapPLA[ch],maxPH);
	  if(maxPH>200)
	    hPla_TMAX_sum->Fill(mapPLA[ch],w->GetMaxPHTime());
	  if(LEDpla[mapPLA[ch]]>0)
	    hPla_LED_sum->Fill(mapPLA[ch],LEDpla[mapPLA[ch]]);
	}
      }
      //BaF
      int bafdet=-1;
      if(bd==BAFBOARD0){
	if(ch==BAF0||ch==BAF1||ch==BAF2||ch==BAF3)
	  bafdet = mapBAF[ch];
      }
      if(bd==BAFBOARD1){
	if(ch==BAF4||ch==BAF5||ch==BAF6)
	  bafdet = mapBAF[ch];
      }
      if(bafdet>-1){
      	double maxPH = w->GetMaxPH();
      	short led = w->GetLED();
      	hBaF_PH[bafdet]->Fill(maxPH);
      	hBaF_PH_sum->Fill(bafdet,maxPH);
      	if(maxPH>200)
      	  hBaF_TMAX_sum->Fill(bafdet,w->GetMaxPHTime());
      	if(led>0)
      	  hBaF_LED_sum->Fill(bafdet,led);
      }
      //scintillators
      int scidet = -1;
      if(bd==SCINTBOARD0){
	if(ch>=SCINTCHSTA0 && ch<=SCINTCHEND0)
	  scidet = ch;
      }
      if(bd==SCINTBOARD1){
	if(ch>=SCINTCHSTA1 && ch<=SCINTCHEND1)
	  scidet = ch+NSCINTS0;
      }
      if(scidet>-1){
      	double maxPH = w->GetMaxPH();
      	short led = w->GetLED();
      	hSci_PH[scidet]->Fill(maxPH);
      	hSci_PH_sum->Fill(scidet,maxPH);
      	if(maxPH>200)
      	  hSci_TMAX_sum->Fill(scidet,w->GetMaxPHTime());
      	if(led>0)
      	  hSci_LED_sum->Fill(scidet,led);
	hSci_IntShort[scidet]->Fill(w->GetShortIntegral());
	hSci_IntLong[scidet]->Fill(w->GetLongIntegral());
	hSci_Int[scidet]->Fill(w->GetLongIntegral(), w->GetShortIntegral());
      }
      
    }//Wave fragments
    for(int g=0; g<evt->GetNPHAs(); g++){
      p = evt->GetPHA(g);
      int bd = p->GetBoard();
      int ch = p->GetCh(); 
      mult[bd][ch]++;
      if(bd==GEBOARD && ch<NGES){//Ge
	double raw = p->GetRaw();
	double en = p->GetEn();
	hGe_cal[ch]->Fill(en);
	hGe_cal_sum->Fill(ch,en);
	hGe_raw_sum->Fill(ch,raw);
	for(int j=0;j<NPLAS;j++){
	  if(LEDpla[j]>0 && TSpla[j]>0 && p->GetTS()>0){
	    hGe_Pla_TSdiff[ch][j]->Fill((double)p->GetTS()-(double)TSpla[j]);
	    //cout << "pha = " << p->GetTS() << ",\tpla = " << TSpla[j] << ",\t diff = " << (double)p->GetTS()- (double)TSpla[j] << ",\t eventN pha = " << p->GetNEvent() << ",\tpla = " << evtNpla[j] << ",\t diff = " << p->GetNEvent()- evtNpla[j] << endl;
	    hGe_gatePla[ch][j]->Fill(en);
	  }
	}
      }//germaniums
    }//PHA fragments
    //multiplicities
    for(int b=0; b<NBOARDS; b++){
      for(int c=0; c<16; c++){
	hmult[b]->Fill(c,mult[b][c]);
      }
    }

    if(i%10000 == 0){
      double time_end = get_time();
      cout << setw(5) << setiosflags(ios::fixed) << setprecision(1) << (100.*i)/nentries <<
	" % done\t" << (Float_t)i/(time_end - time_start) << " events/s " << 
	(nentries-i)*(time_end - time_start)/(Float_t)i << "s to go \r" << flush;
    }
  }
  cout << endl;
  cout << "creating outputfile " << endl;
  TFile* ofile = new TFile(outfile,"recreate");
  ofile->cd();
  TH1F* h1;
  TH2F* h2;
  TIter next(hlist);
  while( (h1 = (TH1F*)next()) ){
    if(h1->GetEntries()>0)
      h1->Write("",TObject::kOverwrite);
  }
  while( (h2 = (TH2F*)next()) ){
    if(h2->GetEntries()>0)
      h2->Write("",TObject::kOverwrite);
  }
  ofile->Close();
  double time_end = get_time();
  cout << "Program Run time: " << time_end - time_start << " s." << endl;

  return 0;
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
map<int ,int> BAFmap(){
  map<int ,int> m;
  m.insert(make_pair(BAF0,0));
  m.insert(make_pair(BAF1,1));
  m.insert(make_pair(BAF2,2));
  m.insert(make_pair(BAF3,3));
  m.insert(make_pair(BAF4,4));
  m.insert(make_pair(BAF5,5));
  m.insert(make_pair(BAF6,6));

  return m;
}
//map<int ,int> SCImap(){
//  map<int ,int> m;
//  for(int i=SCINTCHSTA0;i<SCINTCHEND0+1;i++)
//    m.insert(make_pair(i,i));
//  for(int i=SCINTCHSTA1;i<SCINTCHEND1+1;i++)
//    m.insert(make_pair(i,i+NSCINTS0));
//  return m;
//}

