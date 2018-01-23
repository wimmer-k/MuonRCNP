#include "ProcessHits.hh"
#include "stdio.h"
#include "string.h"

//! default constructor (useless)
ProcessHits::ProcessHits(){
}
/*!
  Initialize the ProcessHits. The calibration parameters are read from a settings file, the random generator is initialized
  \param settings the settings file
*/
ProcessHits::ProcessHits(char* settings){
  //set the random
  fRand = new TRandom();
  fRand->SetSeed(0);
  //read settings
  TEnv* set = new TEnv(settings);
  fsamplesforbaseline = set->GetValue("Samples.Baseline",10);
  cout << "fsamplesforbaseline " << fsamplesforbaseline << endl;
  for(int i=0;i<NGES;i++){
    fGegain[i] = set->GetValue(Form("Ge.%d.Gain",i),1.0);
    fGeoffs[i] = set->GetValue(Form("Ge.%d.Offset",i),0.0);
    cout << "Ge[" << i <<"] gain = " << fGegain[i] << ", offset = " << fGeoffs[i] << endl;
  }
}
/*!
  Analyze the wave forms, calculates the maximum pulse height, and its timing 
  \param wave a pointer to the Wave containing the wave form data to be analyzed
*/
void ProcessHits::AnalyzeWave(Wave* wave){
  double baseline = 0;
  double maxheight = 0;
  short maxtime = 0;

  int length = wave->GetWave().size();
  for(int i=0;i<length;i++){
    short data = wave->GetWave().at(i);
    if(i<fsamplesforbaseline){
      baseline = baseline + (double)data;
      if(fvl>2)
	cout << " this sample: " << (double)data << " current baseline: " << baseline << endl;
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
/*!
  Analyze the pulse height data, calibrates the Ge raw pusle height signals
  \param pha a pointer to the PHA containing the pulse height data to be analyzed
*/
void ProcessHits::AnalyzePHA(PHA* pha){
  //randomize the short raw data
  double en = pha->GetRaw()-0.5+fRand->Uniform(0,1);
  en = fGegain[pha->GetCh()]*en + fGeoffs[pha->GetCh()];
  pha->SetEn(en);
}
