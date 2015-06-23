#ifndef COMMON_H_
#define COMMON_H_

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef NMAX
#define NMAX 8192
#define NMAXSQRT 64
#endif

// #define ECGBIN_LENGTH 512
#define ECGREC_LENGTH 4096
#define ECGCALC_LENGTH 4096
#define ECG_OFFSET 0

#define LOWPASS 300
#define HIGHPASS 5


#endif
