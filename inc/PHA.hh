#ifndef __PHA_HH
#define __PHA_HH

#include <iostream>
#include <vector>
#include <cstdlib>
#include <math.h>

#include "Fragment.hh"

using namespace std;
/*!
  Container for the information of a pulse height (PHA) event
*/
class PHA : public Fragment {
public:
  //! default constructor
  PHA() : Fragment(){ Clear();  }
  //! constructor with the header information
  PHA(long long int ts, int nevent, int board, int ch) : Fragment(1, ts,nevent,board,ch){
    fraw = sqrt(-1);
    fen = sqrt(-1);
  }
  //! clearing data
  void Clear(Option_t *option = ""){
    Fragment::Clear(option);
    fraw = sqrt(-1);
    fen = sqrt(-1);
  }
  //! set the raw energy
  void SetRaw(unsigned short raw){fraw = raw;}
  //! set the calibrated energy
  void SetEn(unsigned short en){fen = en;}
  
  //! returns the raw energy
  unsigned short GetRaw(){ return fraw;}
  //! returns the calibrated
  double GetEn(){ return fen;}
  //! print the fragment information
  void Print(){
    Fragment::Print();
    cout << "raw = " << fraw;
    cout << ", en = " << fen << endl;
  }
protected:
  //! the raw energy
  unsigned short fraw;
  //! the calibrated energy
  double fen;  

  /// \cond CLASSIMP
  ClassDef(PHA,1);
  /// \endcond
};

#endif
