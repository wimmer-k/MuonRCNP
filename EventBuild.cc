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
  int lastfrag = -1;
  int vl = 0;
  int memdepth = 20000;
  int window = 2000;
  char* setfile = NULL;
  int writewave = 1;
  //Read in the command line arguments
  CommandLineInterface* interface = new CommandLineInterface();
  interface->Add("-e", "event building window (in ns)", &window);
  interface->Add("-l", "last fragment to be read", &lastfrag);
  interface->Add("-m", "memory depth for timestamp sorting", &memdepth);
  interface->Add("-r", "run number", &runnr);
  interface->Add("-s", "settings file", &setfile);
  interface->Add("-v", "verbose level", &vl);
  interface->Add("-w", "write the wave form, 0 never, 1 if LED triggered, 2 always, 3 write fragment only if LED triggered (default 1)", &writewave);
  interface->CheckFlags(argc, argv);

  if(runnr<1){
    cout << "invalid run number " << endl;
    return 99;
  }
  if(setfile==NULL){
    cout <<"no settings file, using defaultset.dat" << endl;
    setfile = "./defaultset.dat";
  }

  int disk = runnr%3;
  ifstream *rawin = new ifstream(Form("%s%02d%s%02d/run%04d.dat",baseraw0,disk,baseraw1,disk,runnr), ios::in | ios::binary);
  TFile *rootout = new TFile(Form("%s/run%04d.root",baseroot,runnr), "RECREATE");
  cout << "reading file: " << Form("%s%02d%s%02d/run%04d.dat",baseraw0,disk,baseraw1,disk,runnr) << endl;
  cout << "output file:  " << Form("%s/run%04d.root",baseroot,runnr) << endl;

  ProcessHits* analyzer = new ProcessHits(setfile,vl);
  
  SortHits* sort = new SortHits(rootout);
  sort->SetVerbose(vl);
  sort->SetMemDepth(memdepth);
  sort->SetWindow(window);

  uint32_t id;
  int header[8];
  uint32_t ww;
  long long int bytes_read = 0;
  rawin->read( (char *)&id, sizeof(int) );
  bytes_read += sizeof(int);
  int frags=0;

  TH2F* debugfragTSWave = NULL;
  TH2F* debugfragTSPHA[2];
  if(vl>0){
    debugfragTSWave = new TH2F("debugfragTSWave","debugfragTSWave",1000,0,1e6,1000,0,3e9);
    for(int i=0;i<2;i++)
      debugfragTSPHA[i] = new TH2F(Form("debugfragTSPHA_%d",i),Form("debugfragTSPHA_%d",i),10000,0,1e4,2200,0,2.2e9);
  }
  
  unsigned int tswrapsWave[NWAVEBOARDS]={0};
  unsigned long long int lastTSboardWave[NWAVEBOARDS]={0};
  unsigned int tswrapsPHA[NGES]={0};
  unsigned long long int lastTSchanPHA[NGES]={0};
  unsigned long long int firstTS = 0;
  while(!rawin->eof()){
    if(signal_received){
      break;
    }
    if(frags % 1000 == 0){
      double time_end = get_time();
      cout << "\r" << frags << " fragments read... "<<bytes_read/(1024*1024)<<" MB... "<<frags/(time_end - time_start) << " fragments/s" << flush;
      // if(frags % 1000000 == 0){
      // 	sort->GetTree()->AutoSave();
      // }
    }
    
    if(vl>0)
      cout << "id = "<< id  <<"\t" << (hex) << id  << (dec)<< endl;
    //check if we are at the end of the file
    if(rawin->eof()){
      cout << "end of file" << endl;
      break;
    }
    //check ID of fragment
    if(id==ID_WAVE){
      if(vl>0)
	cout << "\nwave " <<endl;
      // 8 header words
      for(int h=0; h<8; h++){
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
	cout << "timestamp (ticks)" << TS;
	cout << "\tn event " << eventNR;
	cout << "\tboard number " << boardNR;
	cout << "\tchannel " << chanNR << endl;
      }
      if(vl>0)
	debugfragTSWave->Fill(sort->GetFragNr()+1,TS);
      //check for time stamp wrapping
      if(TS < lastTSboardWave[boardNR]){
	if(vl>0)
	  cout << "wrapping on board " << boardNR <<" detected: this time stamp:" << TS << " last one on this board " << lastTSboardWave[boardNR]<< endl;
	//count the number of time stamp wraps
	tswrapsWave[boardNR]++;
      }
      //remember the last time stamp of this board
      lastTSboardWave[boardNR] = header[0]&0x7fffffff;

      //correct for wrapping of time stamps
      TS += (unsigned long long int)pow(2,31)*tswrapsWave[boardNR];
      //convert to ns
      TS*=(unsigned long long int)WAVETSTICK;
      
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
	waveform.push_back(ww&0x0000ffff);
	waveform.push_back((ww&0xffff0000) >> 16);
	isample++;
      }
      wave->SetWave(waveform);

      //analyze wave here, true if LED fired
      bool led = analyzer->AnalyzeWave(wave);
      if(vl>1)
	wave->Print();
      if(writewave==0 || (writewave==1&&!led))//0 never, 1 if LED
	wave->ClearWave();
      
      //add the wave form as a fragment to the sorter
      Fragment *frag = wave;
      if(writewave!=3 || led)
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
      //Ge 0 in chan 0, Ge 2 in chan 2, because scalers are only available for pairs of channels
      chanNR/=2;
      if(vl>0)
	debugfragTSPHA[chanNR]->Fill(sort->GetFragNr()+1,TS);
      //check for time stamp wrapping
      if(TS < lastTSchanPHA[chanNR]){
	if(vl>0)
	  cout << "wrapping on board " << boardNR <<" detected: this time stamp:" << TS << " last one on this board " << lastTSchanPHA[chanNR] << " last event " << sort->GetLastTS() << endl;
	//count the number of time stamp wraps
	tswrapsPHA[chanNR]++;
      }
      //remember the last time stamp of this board
      lastTSchanPHA[chanNR] = TS;

      //correct for wrapping of time stamps
      TS += (unsigned long long int)pow(2,31)*tswrapsPHA[chanNR];
      //convert to ns
      TS*=(unsigned long long int)PHATSTICK;
      
      //remember first timestamp
      if(firstTS ==0)
	firstTS = TS;
      
      PHA* pha = new PHA(TS,eventNR,boardNR,chanNR);
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
    frags++;
    if(lastfrag > 0 && frags >= lastfrag)
      break;
  }//frsgments

  sort->Flush();
  cout << "------------------------------------" << endl;
  cout << "Total of " << frags << " data fragments ("<<bytes_read/(1024*1024)<<" MB) read." << endl;
  cout << sort->GetTree()->GetEntries() << " entries written to tree ("<<sort->GetTree()->GetZipBytes()/(1024*1024)<<" MB)"<< endl;
  cout << "First time stamp: " <<  firstTS << ", last time stamp: " << sort->GetLastTS() << ", data taking time: " << (sort->GetLastTS() - firstTS)*1e-9 << " seconds." << endl;

  if(vl>0){
    debugfragTSWave->Write();
    for(int i=0;i<2;i++)
      debugfragTSPHA[i]->Write();
  }

  rootout->Close();
  double time_end = get_time();

  cout << "Program Run time: " << time_end - time_start << " s." << endl;
  cout << "Unpacked " << frags/(time_end - time_start) << " fragments/s." << endl;

  return 1;
}
