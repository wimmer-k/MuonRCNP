#include "ProcessHits.hh"
#include "stdio.h"
#include "string.h"

ProcessHits::ProcessHits(){
}
ProcessHits::ProcessHits(char* settings){
  TEnv* set = new TEnv(settings);
  fsamplesforbaseline = set->GetValue("Samples.Baseline",10);
  cout << "fsamplesforbaseline " << fsamplesforbaseline << endl;
}
void ProcessHits::AnalyzeWave(Wave* wave){
  double baseline = 0;
  double maxheight = 0;
  short maxtime = 0;

  int length = wave->GetWave().size();
  for(int i=0;i<length;i++){
    short data = wave->GetWave().at(i);
    if(i<fsamplesforbaseline){
      baseline = baseline + (double)data;
      //cout << " this sample: " << (double)data << " current baseline: " << baseline << endl;
    }
    if(data < maxheight){
      maxheight = (double)data;
      maxtime = i;
    } 
  }

  wave->SetBaseLine(baseline/fsamplesforbaseline);
  wave->SetMaxPH(maxheight);
  wave->SetMaxPHTime(maxtime);
  
}
