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
    fbaseline = sqrt(-1);
    fwave.clear();
    fmaxPH = sqrt(-1);
    fmaxPHtime = 0;
  }
  //! clearing data
  void Clear(Option_t *option = ""){
    Fragment::Clear(option);
    fbaseline = sqrt(-1);
    fwave.clear();
    fmaxPH = sqrt(-1);
    fmaxPHtime = 0;
  }
  //! set the waveform a sample n
  void SetWave(unsigned short n, short wave){fwave.at(n) = wave;}
  //! set the waveform
  void SetWave(vector<short> wave){fwave = wave;}
  //! set baseline (determined from the first few samples)
  void SetBaseLine(double baseline){ fbaseline = baseline;}
  //! set maximum pulse height (and subtract baseline )
  void SetMaxPH(double maxPH){ fmaxPH = -(maxPH - fbaseline);}
  //! set the time at which the maximum pulse height occured 
  void SetMaxPHTime(short maxPHtime){ fmaxPHtime = maxPHtime;}
  
  //! returns the waveform
  vector <short> GetWave(){ return fwave;}
  //! returns the lenght of the waveform
  unsigned short GetLength(){ return fwave.size();}
  //! returns the baseline
  double GetBaseLine(){ return fbaseline;}
  //! returns the maximum pulse height
  double GetMaxPH(){ return fmaxPH;}
  //! returns the time at which the maximum pulse height occured
  short GetMaxPHTime(){ return fmaxPHtime;}

  void Print(){
    Fragment::Print();
    cout << "baseline = " << fbaseline;
    cout << ", maximum PH " << fmaxPH;
    cout << ", at time " << fmaxPHtime << endl;
  }
protected:

  double fbaseline;
  double fmaxPH;
  short fmaxPHtime;
  
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
