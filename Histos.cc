#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <signal.h>
#include <sys/time.h>
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
  //TS differences

  TH1F *hGe_Pla_Tdiff[NGES][NPLAS];
  cout << "check code ... work in progress" << endl;
  //implementing Ge - Pla TS differences
  //return;
  for(int g=0; g<NGES; g++){
    for(int p=0; p<NPLAS; p++){
      hGe_Pla_Tdiff[g][p] = new TH1F(Form("hGe%d_Pla%d_Tdiff",g,p), Form("TS difference Ge[%d] - Plastic[%d]",g,p), 2000,0,2000); 
      //hlist->Add(hPla_PH[i]);
    }
  }
  //Pla detectors
  TH1F *hPla_PH[NPLAS];
  TH2F *hPla_PH_sum = new TH2F("hPla_PH_sum", "Plastic max. pulse height summary", NPLAS,0,NPLAS, 2000,0,2000); hlist->Add(hPla_PH_sum);
  TH2F *hPla_TMAX_sum = new TH2F("hPla_TMAX_sum", "Plastic Time at max. pulse height summary", NPLAS,0,NPLAS, 2000,0,2000); hlist->Add(hPla_TMAX_sum);
  for(int p=0; p<NPLAS; p++){
    hPla_PH[p] = new TH1F(Form("hPla_PH_%d",p), Form("Plastic[%d] max. pulse height",p), 2000,0,2000); hlist->Add(hPla_PH[p]);
  }
  //BaF detectors
  TH1F *hBaF_PH[NBAFS];
  TH2F *hBaF_PH_sum = new TH2F("hBaF_PH_sum", "BaF max. pulse height summary", NBAFS,0,NBAFS, 2000,0,2000); hlist->Add(hBaF_PH_sum);
  TH2F *hBaF_TMAX_sum = new TH2F("hBaF_TMAX_sum", "BaF Time at max. pulse height summary", NBAFS,0,NBAFS, 2000,0,2000); hlist->Add(hBaF_TMAX_sum);
  for(int b=0; b<NBAFS; b++){
    hBaF_PH[b] = new TH1F(Form("hBaF_PH_%d",b), Form("BaF[%d] max. pulse height",b), 2000,0,2000); hlist->Add(hBaF_PH[b]);
  }
  //Ge detectors
  TH1F *hGe_cal[NGES];
  TH2F *hGe_raw_sum = new TH2F("hGe_raw_sum", "Ge raw pulse height summary",  NGES,0,NGES, 8000,0,8000); hlist->Add(hGe_raw_sum);
  TH2F *hGe_cal_sum = new TH2F("hGe_cal_sum", "Ge calibrated energy summary", NGES,0,NGES, 8000,0,8000); hlist->Add(hGe_cal_sum);
  for(int g=0; g<NGES; g++){
    hGe_cal[g] = new TH1F(Form("hGe_cal_%d",g), Form("Ge[%d] calibrated energy",g), 8000,0,8000); hlist->Add(hGe_cal[g]);
  }

  
  
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
    //BaF data
    for(int j=0; j<evt->GetNWaves(); j++){
      w = evt->GetWave(j);
      int bd = w->GetBoard();
      int ch = w->GetCh();

      if(bd==BAFBOARD && ch>=BAFCHSTA && ch<BAFCHSTA+NBAFS){//BaFs
	//cout << "board: " << bd <<", ch: " << ch << ", PH: " << w->GetMaxPH()<< endl;
	double maxPH = w->GetMaxPH();
	hBaF_PH[ch-BAFCHSTA]->Fill(maxPH);
	hBaF_PH_sum->Fill(ch-BAFCHSTA,maxPH);
	if(maxPH>200)
	  hBaF_TMAX_sum->Fill(ch-BAFCHSTA,w->GetMaxPHTime());
      }
    }
    for(int g=0; g<evt->GetNPHAs(); g++){
      p = evt->GetPHA(g);
      int bd = p->GetBoard();
      int ch = p->GetCh(); 
      if(bd==GEBOARD && ch<NGES){//BaFs
	double raw = p->GetRaw();
	double en = p->GetEn();
	hGe_cal[ch]->Fill(en);
	hGe_cal_sum->Fill(ch,en);
	hGe_raw_sum->Fill(ch,raw);
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
