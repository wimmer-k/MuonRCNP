#ifndef __EVENT_HH
#define __EVENT_HH

#include <iostream>
#include <vector>
#include <cstdlib>
#include <math.h>

#include "Wave.hh"
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
    fwaves.clear();
    flastTS  = -1;
  }
  //! add a new fragment
  void Add(Fragment* add){
    if(add->GetID()==0)
      AddWave((Wave*)add);
  }
  //! add a new wave hit
  void AddWave(Wave* add){
    fwaves.push_back(add);
    flastTS = add->GetTS();
  }
  //! returns all the wave data
  vector <Wave*> GetWaves(){ return fwaves;}
  //! returns one wave data fragment
  Wave* GetWave(short n){ return fwaves.at(n);}
  //! returns the number of waves in the event
  unsigned short GetLength(){ return fwaves.size();}
  //! returns the last added timestamp
  long long int GetLastTS() { return flastTS;}

  void PrintEvent(){
    cout << "last time stamp added to the event " << flastTS << endl;
    cout << "number of waveforms " << fwaves.size() << endl;
  }
  
protected:
  vector <Wave*> fwaves;
  long long int flastTS;
  /// \cond CLASSIMP
  ClassDef(Event,1);
  /// \endcond
};

#endif
