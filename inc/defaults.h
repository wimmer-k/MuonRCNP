char* baseraw[2] = {"/data","/RCNP1801_"};
char* baseroot = "./root";

const uint32_t ID_WAVE=0xffffffff;
const uint32_t ID_PHA =0xefefefef;

const int MAXSAMPLES = 2000;
const int NBOARDS = 4;

const int NBAFBOARD = 0;
const int NBAFS = 7;
const int BAFCHSTA = 4;
const int BAFCHEND = 10;


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
