#ifndef __WAVE_HH
#define __WAVE_HH

#include <iostream>
#include <vector>
#include <cstdlib>
#include <math.h>

#include "TObject.h"

using namespace std;
/*!
  Container for the information of a wave event
*/
class Wave : public TObject {
public:
  //! default constructor
  Wave(){
    Clear();
  }
  //! constructor with the header information
  Wave(long long int ts, int nevent, int board, int ch){
    Clear();
    fTS = ts;
    fnevent = nevent;
    fboard = board;
    fch = ch;
  }
  //! clearing data
  void Clear(Option_t *option = ""){
    fTS = 0;
    fnevent = -1;
    fboard = -1;
    fch = -1;
    fWave.clear();
  }
  //! set the waveform a sample n
  void SetWave(unsigned short n, short wave){fWave.at(n) = wave;}
  //! set the waveform
  void SetWave(vector<short> wave){fWave = wave;}

  
  //getter
  //! returns the timestamp
  long long int GetTS(){ return fTS;}
  //! returns the event number
  int GetNEvent(){ return fnevent;}
  //! returns the board number
  int GetBoard(){ return fboard;}
  //! returns the channel number
  int GetCh(){ return fch;}
  //! returns the waveform
  vector <short> GetWave(){ return fWave;}
  //! returns the lenght of the waveform
  unsigned short GetLength(){ return fWave.size();}
  
protected:
  //! the timestamp
  long long int fTS;
  //! the internal event number
  int fnevent;
  //! the board number
  int fboard;
  //! the channel number
  int fch;

#ifdef WRITE_WAVE
  vector <short> fWave;
#else
  vector <short> fWave; //!
#endif

  /// \cond CLASSIMP
  ClassDef(Wave,1);
  /// \endcond
};

#endif
