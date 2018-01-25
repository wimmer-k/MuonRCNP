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
    fLED = 0;
    fshortint = sqrt(-1);
    flongint = sqrt(-1);
  }
  //! clearing data
  void Clear(Option_t *option = ""){
    Fragment::Clear(option);
    fbaseline = sqrt(-1);
    fwave.clear();
    fmaxPH = sqrt(-1);
    fmaxPHtime = 0;
    fLED = 0;
    fshortint = sqrt(-1);
    flongint = sqrt(-1);
  }
  //! set the waveform a sample n
  void SetWave(unsigned short n, short wave){fwave.at(n) = wave;}
  //! set the waveform
  void SetWave(vector<short> wave){
    fwave = wave;
    fwavelength = wave.size();
  }
  //! set baseline (determined from the first few samples)
  void SetBaseLine(double baseline){ fbaseline = baseline;}
  //! set maximum pulse height 
  void SetMaxPH(double maxPH){ fmaxPH = maxPH;}
  //! set the time at which the maximum pulse height occured 
  void SetMaxPHTime(short maxPHtime){ fmaxPHtime = maxPHtime;}
  //! set the leading edge timing
  void SetLED(short led){ fLED = led;}
  //! set the short integral
  void SetShortIntegral(double shortint){ fshortint = shortint;}
  //! set the long integral
  void SetLongIntegral(double longint){ flongint = longint;}

  //! clear the waveform
  void ClearWave(){
    fwave.clear();
  }
  
  //! returns the waveform
  vector <short> GetWave(){ return fwave;}
  //! returns the lenght of the waveform
  unsigned short GetLength(){ return fwavelength;}
  //! returns the baseline
  double GetBaseLine(){ return fbaseline;}
  //! returns the maximum pulse height
  double GetMaxPH(){ return fmaxPH;}
  //! returns the time at which the maximum pulse height occured
  short GetMaxPHTime(){ return fmaxPHtime;}
  //! returns the leading edge timing
  short GetLED(){ return fLED;}
  //! returns the short integral
  double GetShortIntegral(){ return fshortint;}
  //! returns the long integral
  double GetLongIntegral(){ return flongint;}
  
  //! print the fragment information
  void Print(){
    Fragment::Print();
    cout << "baseline = " << fbaseline;
    cout << ", wave length = " << fwavelength;
    cout << ", maximum PH " << fmaxPH;
    cout << ", at time " << fmaxPHtime;
    cout << ", LED " << fLED;
    cout << ", short integral " << fshortint;
    cout << ", long integral " << flongint << endl;
  }
protected:
  //! the baseline, derived from the first samples
  double fbaseline;
  //! the length of the wave form
  unsigned short fwavelength;
  //! the maximum of the pulse height (converted to positive)
  double fmaxPH;
  //! the timing of the maximum pulse height
  short fmaxPHtime;
  //! the leading edge timing
  short fLED;
  //! the short integral
  double fshortint;
  //! the long integral
  double flongint;
  //! the wave form, only written to file, if selected (otherwise cleared)
  vector <short> fwave;

  /// \cond CLASSIMP
  ClassDef(Wave,1);
  /// \endcond
};

#endif
