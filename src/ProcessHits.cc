#include "ProcessHits.hh"
#include "stdio.h"
#include "string.h"

ProcessHits::ProcessHits(){
}
ProcessHits::ProcessHits(char* settings){
  TEnv* set = new TEnv(settings);
}
void ProcessHits::AnalyzeWave(Wave* wave){

}
