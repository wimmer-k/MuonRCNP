#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <algorithm> 
#include <signal.h>
#include <sys/time.h>
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TEnv.h"
#include "Wave.hh"
#include "CommandLineInterface.hh"
#include "defaults.h"

using namespace std;
bool signal_received = false;
void signalhandler(int sig);
double get_time();

int main(int argc, char *argv[]){
  double time_start = get_time();
  signal(SIGINT,signalhandler);
  int runnr = 0;
  int lastbuffer = 2;
  int vl = 0;
  char* setfile = NULL;
  //Read in the command line arguments
  CommandLineInterface* interface = new CommandLineInterface();
  interface->Add("-lb", "last buffer to be read", &lastbuffer);
  interface->Add("-r", "run number", &runnr);
  interface->Add("-v", "verbose level", &vl);
  interface->Add("-s", "settings file", &setfile);
  interface->CheckFlags(argc, argv);

  if(runnr<1){
    cout << "invalid run number " << endl;
    return 99;
  }
  if(setfile==NULL){
    cout <<"no settings file, using default.dat" << endl;
    setfile = "./defaultset.dat";
  }
  //read settings file
  TEnv* set = new TEnv(setfile);
  //read constant fixed baselines
  short fixed_baseline[2][16];
  cout << "baseline settings " << set->GetValue("Baselines","./set/baseline.dat") << endl;
  TEnv* baselines = new TEnv(set->GetValue("Baselines","./set/baseline.dat"));
  for(int b=0;b<2;b++){
    for(int c=0;c<16;c++){
      fixed_baseline[b][c] = baselines->GetValue(Form("Board%d.Channel%d",b,c),16384);
    }
  }
  
  ifstream *rawin = new ifstream(Form("%s/run%04d.dat",baseraw,runnr), ios::in | ios::binary);
  TFile *rootout = new TFile(Form("%s/run%04d.root",baseroot,runnr), "RECREATE");
  cout << "reading file: " << Form("%s/run%04d.dat",baseraw,runnr) << endl;
  cout << "output file:  " << Form("%s/run%04d.root",baseroot,runnr) << endl;


  Wave* wave = new Wave();
  TTree *tr = new TTree("tr","waveforms");
  tr->Branch("wave",&wave,320000);
  //tr->BranchRef();
  
  uint32_t id;
  int header[8];
  rawin->read( (char *)&id, sizeof(int) );
  for(int i=0;i<lastbuffer;i++){
    if(vl>0)
      cout << "id = "<< id  <<"\t" << (hex) << id  << (dec)<< endl;
    //check if we are at the end of the file
    if(rawin->eof()){
      cout << "end of file" << endl;
      break;
    }
    //check ID of buffer
    if(id==ID_WAVE){
      if(vl>0)
	cout << "wave " <<endl;
      // 8 header words
      for(int h=0;h<8;h++){
	rawin->read( (char *)&header[h], sizeof(int) );
	if(vl>1)
	  cout << "header["<<h<<"] = " << header[h] << endl;
      }
      header[0] = header[0]&0x7fffffff; // TimeStamp has 31 bit
      if(vl>0){
	cout << "timestamp " << header[0];
	cout << "\tn event " << header[4];
	cout << "\tboard number " << header[7];
	cout << "\tchannel " << header[3] << endl;
      }
      
      wave = new Wave(header[0],header[4],header[7],header[3]);
      uint32_t ww;
      int isample =0;
      vector<short> waveform;
      while(!rawin->eof() && isample<MAXSAMPLES){
	rawin->read( (char *)&ww, sizeof(int) );
	//cout << "ww = "<< ww  <<"\t" << (hex) << ww  << (dec)<< endl;
	if(ww==ID_WAVE || ww==ID_GE){
	  id = ww;
	  break;
	}
	// order of waveform samples 1 0, 3 2, ...
	waveform.push_back( (ww&0x0000ffff)        - fixed_baseline[header[7]][header[3]]);
	waveform.push_back(((ww&0xffff0000) >> 16) - fixed_baseline[header[7]][header[3]]);
	isample++;
      }
#ifdef WRITE_WAVE
      wave->SetWave(waveform);
#endif
      //analyze wave here
      tr->Fill();
    }//waveform
  }//buffers
  rootout->cd();
  tr->Write("",TObject::kOverwrite);
  rootout->Close();
}
void signalhandler(int sig){
  if (sig == SIGINT){
    signal_received = true;
  }
}

double get_time(){
    struct timeval t;
    gettimeofday(&t, NULL);
    double d = t.tv_sec + (double) t.tv_usec/1000000;
    return d;
}
