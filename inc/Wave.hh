#ifndef __WAVE_HH
#define __WAVE_HH

#include <iostream>
#include <vector>
#include <cstdlib>
#include <math.h>

#include "Fragment.hh"

using namespace std;
/*!
  Container for the information of a wave event
*/
class Wave : public Fragment {
public:
  //! default constructor
  Wave() : Fragment(){ Clear();  }
  //! constructor with the header information
  Wave(long long int ts, int nevent, int board, int ch) : Fragment(0, ts,nevent,board,ch){
    fwave.clear();
  }
  //! clearing data
  void Clear(Option_t *option = ""){
    Fragment::Clear(option);
    fwave.clear();
  }
  //! set the waveform a sample n
  void SetWave(unsigned short n, short wave){fwave.at(n) = wave;}
  //! set the waveform
  void SetWave(vector<short> wave){fwave = wave;}
  
  //! returns the waveform
  vector <short> GetWave(){ return fwave;}
  //! returns the lenght of the waveform
  unsigned short GetLength(){ return fwave.size();}
  
protected:

#ifdef WRITE_WAVE
  vector <short> fwave;
#else
  vector <short> fwave; //!
#endif

  /// \cond CLASSIMP
  ClassDef(Wave,1);
  /// \endcond
};

#endif
