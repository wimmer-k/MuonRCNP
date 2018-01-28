#ifndef __EVENT_HH
#define __EVENT_HH

#include <iostream>
#include <vector>
#include <cstdlib>
#include <math.h>

#include "Wave.hh"
#include "PHA.hh"
#include "TObject.h"

using namespace std;
/*!
  Container for the information of an event containing waveforms and Ge data
*/
class Event : public TObject {
public:
  //! default constructor
  Event(){
    Clear();
  }
  //! clearing data
  void Clear(Option_t *option = ""){
    for(vector<Wave*>::iterator w=fwaves.begin(); w!=fwaves.end(); w++){
      delete *w;
    }
    fwaves.clear();
    for(vector<PHA*>::iterator p=fphas.begin(); p!=fphas.end(); p++){
      delete *p;
    }
    fphas.clear();
    flastTS  = 0;
    flastWaveTS  = 0;
    flastPHATS  = 0;
  }
  //! add a new fragment
  void Add(Fragment* add){
    if(add->GetID()==0)
      AddWave((Wave*)add);
    else if(add->GetID()==1)
      AddPHA((PHA*)add);
    //record the last time stamp added to that event
    flastTS = add->GetTS();
  }
  //! add a new wave hit
  void AddWave(Wave* add){
    fwaves.push_back(add);
    flastWaveTS = add->GetTS();
  }
  //! add a new pulse height hit
  void AddPHA(PHA* add){
    fphas.push_back(add);
    flastPHATS = add->GetTS();
  }
  //! returns all the wave data
  vector <Wave*> GetWaves(){ return fwaves;}
  //! returns one wave data fragment
  Wave* GetWave(short n){ return fwaves.at(n);}
  //! returns the number of wave form hits in the event
  unsigned short GetNWaves(){ return fwaves.size();}

  //! returns all the pulse height data
  vector <PHA*> GetPHAs(){ return fphas;}
  //! returns one wave data fragment
  PHA* GetPHA(short n){ return fphas.at(n);}
  //! returns the number of pusle height hits in the event
  unsigned short GetNPHAs(){ return fphas.size();}

  //! returns the last added time stamp
  unsigned long long int GetLastTS() { return flastTS;}
  //! returns the last added time stamp for a Wave
  unsigned long long int GetLastWaveTS() { return flastWaveTS;}
  //! returns the last added time stamp for a PHA
  unsigned long long int GetLastPHATS() { return flastPHATS;}
  //! prints the basic information of the event
  void PrintEvent(){
    cout << "last time stamp added to the event " << flastTS << endl;
    cout << "number of wave forms " << fwaves.size() << endl;
    for(unsigned short i=0; i<fwaves.size();i++)
      cout << i <<"\t" << fwaves.at(i)->GetTS() << endl;
    cout << "number of pulse heights " << fphas.size() << endl;
    for(unsigned short i=0; i<fphas.size();i++)
      cout << i <<"\t" << fphas.at(i)->GetTS() << endl;
  }
  
protected:
  //! a vector with the wave form data
  vector <Wave*> fwaves;
  //! a vector with the pulse height data
  vector <PHA*> fphas;
  //! the last time stamp that was added to this event
  unsigned long long int flastTS;
  //! the last time stamp of a Wave that was added to this event
  unsigned long long int flastWaveTS;
  //! the last time stamp of a PHA that was added to this event
  unsigned long long int flastPHATS;

  /// \cond CLASSIMP
  ClassDef(Event,1);
  /// \endcond
};

#endif
