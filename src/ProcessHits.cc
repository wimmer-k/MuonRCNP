#include "ProcessHits.hh"
#include "stdio.h"
#include "string.h"

//! default constructor (useless)
ProcessHits::ProcessHits(){
}
/*!
  Initialize the ProcessHits. The calibration parameters are read from a settings file, the random generator is initialized
  \param settings the settings file
  \param vl the verbose level
*/
ProcessHits::ProcessHits(char* settings, int vl){
  fvl = vl;
  //set the random
  fRand = new TRandom();
  fRand->SetSeed(0);
  //read settings
  TEnv* set = new TEnv(settings);
  fsamplesforbaseline = set->GetValue("Samples.Baseline",10);
  cout << "fsamplesforbaseline " << fsamplesforbaseline << endl;
  cout << "threshold settings " << set->GetValue("Thresholds","./set/thresholds.dat") << endl;
  TEnv* thresholds = new TEnv(set->GetValue("Thresholds","./set/thresholds.dat"));
  cout << "integration settings " << set->GetValue("IntegrationWindows","./set/integrationwindows.dat") << endl;
  TEnv* integrationwindows = new TEnv(set->GetValue("IntegrationWindows","./set/integrationwindows.dat"));
  for(int b=0; b<NWAVEBOARDS; b++){
    for(int c=0; c<16; c++){
      fthreshold[b][c] = thresholds->GetValue(Form("Board%d.Channel%d",b,c),0);
      if(fvl>0)
	cout << "board["<<b<<"], channel["<<c<<"] threshold = " << fthreshold[b][c] << endl;
      fshortint[b][c][0] = integrationwindows->GetValue(Form("Board%d.Channel%d.Short.Start",b,c),0);
      fshortint[b][c][1] = integrationwindows->GetValue(Form("Board%d.Channel%d.Short.End",b,c),0);
      flongint[b][c][0] = integrationwindows->GetValue(Form("Board%d.Channel%d.Long.Start",b,c),0);
      flongint[b][c][1] = integrationwindows->GetValue(Form("Board%d.Channel%d.Long.End",b,c),0);
      if(fvl>0)
	cout << "integration window short " << fshortint[b][c][0] << " to " << fshortint[b][c][1] << ", long " << flongint[b][c][0] << " to " << flongint[b][c][1] << endl;
    }
  }

  for(int i=0; i<NGES; i++){
    fGegain[i] = set->GetValue(Form("Ge.%d.Gain",i),1.0);
    fGeoffs[i] = set->GetValue(Form("Ge.%d.Offset",i),0.0);
    if(fvl>0)
      cout << "Ge[" << i <<"] gain = " << fGegain[i] << ", offset = " << fGeoffs[i] << endl;
  }
}
/*!
  Analyze the wave forms, calculates the maximum pulse height, and its timing 
  \returns true if LED threshold was crossed, false if wave did not trigger
  \param wave a pointer to the Wave containing the wave form data to be analyzed
*/
bool ProcessHits::AnalyzeWave(Wave* wave){
  short length = wave->GetWave().size();
  short b = wave->GetBoard();
  short c = wave->GetCh();

  double baseline = 0;
  double maxheight = 0;
  short maxtime = 0;
  short leadingedge = -1;
  double shortint = 0;
  double longint = 0;


  //first get the baseline from the first fsamplesforbaseline
  for(short i=0; i<fsamplesforbaseline; i++){
    baseline += (double)wave->GetWave().at(i);
  }
  baseline/=fsamplesforbaseline;
  if(fvl>1)
    cout << "base line at " << baseline << endl;
  for(short i=0; i<length; i++){
    //subtract the baseline, and convert to a positive number
    double data = baseline - wave->GetWave().at(i);
    //find the maximum pusle height and its timing
    if(data > maxheight){
      maxheight = (double)data;
      maxtime = i;
    }
    //if leading edge is not set yet, and data exceeds the threshold, trigger the LED
    if(leadingedge < 0 && data > fthreshold[b][c]){
      if(fvl>11)
	cout <<"leading edge trigger at " << i << " for board["<<b<<"], channel["<<c<<"]" <<endl;
      leadingedge = i;
    }
  }
  if(fvl>1)
    cout << "found the max pulse height of "<< maxheight<<" at " << maxtime <<" for board["<<b<<"], channel["<<c<<"]"<<endl;

  for(short i=min(fshortint[b][c][0],flongint[b][c][0]); i<max(fshortint[b][c][1],flongint[b][c][1]); i++){
    //wrt maxtime, subtract the baseline, and convert to a positive number
    if(i+maxtime<0)
      continue;
    if(i+maxtime==length)
      break;
    double data = baseline - wave->GetWave().at(i+maxtime);
    if(i>=fshortint[b][c][0] && i<=fshortint[b][c][1])
      shortint+=data;
    if(i>=flongint[b][c][0] && i<=flongint[b][c][1])
      longint+=data;
  }

  wave->SetBaseLine(baseline);
  wave->SetMaxPH(maxheight);
  wave->SetMaxPHTime(maxtime);
  wave->SetLED(leadingedge);
  wave->SetShortIntegral(shortint);
  wave->SetLongIntegral(longint);

  if(fvl>0)
    wave->Print();

  if(leadingedge < 0)
    return false;
  return true;
      
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
