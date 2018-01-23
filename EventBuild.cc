#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <algorithm> 
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TEnv.h"
#include "CommandLineInterface.hh"
#include "SortHits.hh"
#include "ProcessHits.hh"
#include "util.h"
#include "globaldefs.h"

using namespace std;

int main(int argc, char *argv[]){
  double time_start = get_time();
  signal(SIGINT,signalhandler);
  int runnr = 0;
  int lastbuffer = -1;
  int vl = 0;
  int memdepth = 1000;
  char* setfile = NULL;
  //Read in the command line arguments
  CommandLineInterface* interface = new CommandLineInterface();
  interface->Add("-lb", "last buffer to be read", &lastbuffer);
  interface->Add("-r", "run number", &runnr);
  interface->Add("-v", "verbose level", &vl);
  interface->Add("-s", "settings file", &setfile);
  interface->Add("-m", "memory depth for timestamp sorting", &memdepth);
  interface->CheckFlags(argc, argv);

  if(runnr<1){
    cout << "invalid run number " << endl;
    return 99;
  }
  if(setfile==NULL){
    cout <<"no settings file, using defaultset.dat" << endl;
    setfile = "./defaultset.dat";
  }
  //read settings file
  TEnv* set = new TEnv(setfile);
  //read constant fixed baselines
  short fixed_baseline[NBOARDS][16];
  cout << "baseline settings " << set->GetValue("Baselines","./set/baseline.dat") << endl;
  TEnv* baselines = new TEnv(set->GetValue("Baselines","./set/baseline.dat"));
  for(int b=0;b<NBOARDS;b++){
    for(int c=0;c<16;c++){
      fixed_baseline[b][c] = baselines->GetValue(Form("Board%d.Channel%d",b,c),16384);
    }
  }

  int disk = runnr%3;
  ifstream *rawin = new ifstream(Form("%s%02d%s%02d/run%04d.dat",baseraw0,disk,baseraw1,disk,runnr), ios::in | ios::binary);
  TFile *rootout = new TFile(Form("%s/run%04d.root",baseroot,runnr), "RECREATE");
  cout << "reading file: " << Form("%s%02d%s%02d/run%04d.dat",baseraw0,disk,baseraw1,disk,runnr) << endl;
  cout << "output file:  " << Form("%s/run%04d.root",baseroot,runnr) << endl;


  ProcessHits* analyzer = new ProcessHits(setfile);
  analyzer->SetVerbose(vl);
  
  SortHits* sort = new SortHits(rootout);
  sort->SetVerbose(vl);
  sort->SetMemDepth(memdepth);
  sort->SetWindow(2000);
  uint32_t id;
  int header[8];
  uint32_t ww;
  long long int bytes_read = 0;
  rawin->read( (char *)&id, sizeof(int) );
  bytes_read += sizeof(int);
  int buffers=0;
  
  unsigned int tswraps[NBOARDS]={0};
  unsigned long long int lastTSboard[NBOARDS]={0};
  unsigned long long int firstTS = 0;
  while(!rawin->eof()){
   if(signal_received){
      break;
    }
    if(buffers % 1000 == 0){
      double time_end = get_time();
      cout << "\r" << buffers << " buffers read... "<<bytes_read/(1024*1024)<<" MB... "<<buffers/(time_end - time_start) << " buffers/s" << flush;
      if(buffers % 10000 == 0){
	sort->GetTree()->AutoSave();
      }
    }
    
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
	cout << "\nwave " <<endl;
      // 8 header words
      for(int h=0;h<8;h++){
	rawin->read( (char *)&header[h], sizeof(int) );
	bytes_read += sizeof(int);  
	if(vl>1)
	  cout << "header["<<h<<"] = " << header[h] << endl;
      }
      unsigned long long int TS = header[0]&0x7fffffff; // TimeStamp has 31 bit
      int eventNR = header[4];
      int boardNR = header[7];
      int chanNR = header[3];
      if(vl>0){
	cout << "timestamp " << TS;
	cout << "\tn event " << eventNR;
	cout << "\tboard number " << boardNR;
	cout << "\tchannel " << chanNR << endl;
      }
      //check for timestamp jumps
      if(TS < lastTSboard[boardNR]){
	if(vl>0)
	  cout << "wrapping on board " << boardNR <<" detected: this time stamp:" << TS << " last one on this board " << lastTSboard[boardNR]<< endl;
	//count the number of time stamp wraps
	tswraps[boardNR]++;
      }
      //correct for wrapping of time stamps
      TS += (unsigned long long int)pow(2,31)*tswraps[boardNR];
      //remember the last time stamp of this board
      lastTSboard[boardNR] = header[0]&0x7fffffff;
      //remember first timestamp
      if(firstTS ==0)
	firstTS = TS;
      //initialyze the wave
      Wave* wave = new Wave(TS,eventNR,boardNR,chanNR);
      int isample =0;
      vector<short> waveform;
      while(!rawin->eof() && isample<MAXSAMPLES){
	rawin->read( (char *)&ww, sizeof(int) );
	bytes_read += sizeof(int);
	//cout << "ww = "<< ww  <<"\t" << (hex) << ww  << (dec)<< endl;
	if(ww==ID_WAVE || ww==ID_PHA){
	  id = ww;
	  break;
	}
	// order of waveform samples 1 0, 3 2, ...
	waveform.push_back( (ww&0x0000ffff)        - fixed_baseline[boardNR][chanNR]);
	waveform.push_back(((ww&0xffff0000) >> 16) - fixed_baseline[boardNR][chanNR]);
	isample++;
      }
      wave->SetWave(waveform);

      //analyze wave here
      analyzer->AnalyzeWave(wave);
      if(vl>1)
	wave->Print();
      //add the wave form as a fragment to the sorter
      Fragment *frag = wave;
      sort->Add(frag);
    }//waveform
    else if(id==ID_PHA){
      if(vl>1)
    	cout << "\nGe " <<endl;
      unsigned long long TS,format;
      unsigned short extras, rawen;
      unsigned int extras2, chanNR, eventNR, boardNR, eventNRS;
      rawin->read( (char *)&TS, sizeof(unsigned long long) );
      bytes_read += sizeof(unsigned long long);  
      rawin->read( (char *)&format, sizeof(unsigned long long) );
      bytes_read += sizeof(unsigned long long);
      rawin->read( (char *)&extras, sizeof(unsigned short) );
      bytes_read += sizeof(unsigned short);  
      rawin->read( (char *)&extras2, sizeof(unsigned int) );
      bytes_read += sizeof(unsigned int);  
      rawin->read( (char *)&chanNR, sizeof(unsigned int) );
      bytes_read += sizeof(unsigned int);  
      rawin->read( (char *)&eventNR, sizeof(unsigned int) );
      bytes_read += sizeof(unsigned int);  
      rawin->read( (char *)&boardNR, sizeof(unsigned int) );
      bytes_read += sizeof(unsigned int);  
      rawin->read( (char *)&eventNRS, sizeof(unsigned int) );
      bytes_read += sizeof(unsigned int);        
      rawin->read( (char *)&rawen, sizeof(unsigned short) );
      bytes_read += sizeof(unsigned short);  
      if(vl>1){
	cout << "TS = " << TS << endl;
	cout << "format = " << format << endl;
	cout << "extras = " << extras << endl;
	cout << "extras2 = " << extras2 << endl;
	cout << "chanNR = " << chanNR << endl;
	cout << "eventNR = " << eventNR << endl;
	cout << "boardNR = " << boardNR << endl;
	cout << "eventNRS = " << eventNRS << endl;
	cout << "rawen = " << rawen << endl;
      }
      PHA* pha = new PHA(TS,eventNR,boardNR,chanNR/2);
      pha->SetRaw(rawen);
      //analyze pha data here
      analyzer->AnalyzePHA(pha);
      if(vl>1){
	//sort->SetVerbose(1);
	pha->Print();
      }
      //add the pulse height data as a fragment to the sorter
      Fragment *frag = pha;
      sort->Add(frag);
      
      rawin->read( (char *)&ww, sizeof(int) );
      bytes_read += sizeof(int);
      if(ww==ID_WAVE || ww==ID_PHA){
	id = ww;
      }
      else{
	cout << "ww = "<< ww  <<"\t" << (hex) <<"0x"<< ww  << (dec)<< endl;
	cout << "should be " << ID_WAVE << " or " << ID_PHA << endl;
	break;
      }
    }//ge
    buffers++;
    if(lastbuffer > 0 && buffers >= lastbuffer)
      break;
  }//buffers

  sort->Flush();
  cout << "------------------------------------" << endl;
  cout << "Total of " << buffers << " data buffers ("<<bytes_read/(1024*1024)<<" MB) read." << endl;
  cout << sort->GetTree()->GetEntries() << " entries written to tree ("<<sort->GetTree()->GetZipBytes()/(1024*1024)<<" MB)"<< endl;
  cout << "First time-stamp: " <<  firstTS << ", last time-stamp: " << sort->GetLastTS() << ", data taking time: " << (sort->GetLastTS() - firstTS)*8e-9 << " seconds." << endl;
  rootout->Close();
  double time_end = get_time();
  cout << "Program Run time: " << time_end - time_start << " s." << endl;
  cout << "Unpacked " << buffers/(time_end - time_start) << " buffers/s." << endl;
  return 1;
}
