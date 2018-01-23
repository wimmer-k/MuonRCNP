#ifndef __FRAGMENT_HH
#define __FRAGMENT_HH

#include <iostream>
#include <vector>
#include <cstdlib>
#include <math.h>

#include "TObject.h"

using namespace std;
/*!
  Container for the information of a fragment, wave or Ge hit. This is not supposed to be used alone but Wave and Ge inherit from this
*/
class Fragment : public TObject {
public:
  //! default constructor
  Fragment(){
    Clear();
  }
  //! constructor with the header information
  Fragment(int id, long long int ts, int nevent, int board, int ch){
    Clear();
    fID = id;
    fTS = ts;
    fnevent = nevent;
    fboard = board;
    fch = ch;
  }
  //! destructor
  virtual ~Fragment(){
  }
  //! clearing data
  void Clear(Option_t *option = ""){
    fTS = 0;
    fID = -1;
    fnevent = -1;
    fboard = -1;
    fch = -1;
  }
  
  //getter
  //! returns the timestamp
  long long int GetTS(){ return fTS;}
  //! returns the id, 0 for waveform, 1 for Ge
  int GetID(){ return fID;}
  //! returns the event number
  int GetNEvent(){ return fnevent;}
  //! returns the board number
  int GetBoard(){ return fboard;}
  //! returns the channel number
  int GetCh(){ return fch;}
  //! prints the fragment information
  void Print(){
    cout << "TS = " << fTS;
    if(fID==0)
      cout << ", wave form";
    else if (fID==1)
      cout << ", germanium";     
    cout << " data, event # " << fnevent;
    cout << ", board " << fboard;
    cout << ", channel " << fch << endl;
  }
  
protected:
  //! the timestamp
  long long int fTS;
  //! the ID, 0 for waveform, 1 for Ge
  int fID;
  //! the internal event number
  int fnevent;
  //! the board number
  int fboard;
  //! the channel number
  int fch;

  /// \cond CLASSIMP
  ClassDef(Fragment,1);
  /// \endcond
};

#endif
