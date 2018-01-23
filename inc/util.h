#ifndef __UTIL_HH
#define __UTIL_HH
#include <signal.h>
#include <sys/time.h>

bool signal_received = false;
void signalhandler(int sig);
double get_time();

void signalhandler(int sig){
  if (sig == SIGINT){
    signal_received = true;
  }
}

double get_time(){
    struct timeval t;
    gettimeofday(&t, NULL);
    double d = t.tv_sec + (double) t.tv_usec/1000000;
    return d;
}
#endif
