#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "types.h"
#include "common.h"
#include "fft4g.h"
#include "calc.h"

extern float_t ecgdata[];

float_t timedata[ECGREC_LENGTH + 1];
float_t freqdata[ECGREC_LENGTH + 1];
float_t decompdata[ECGREC_LENGTH + 1];
float_t ac_wkdata[ECGREC_LENGTH + 1];


static void calc_copy(float_t *dst, float_t *src, int dstindex, int srcindex, int length) {
	// Add offset
	dst +=dstindex;
	src +=srcindex;

	while(length--) {
		*dst = *src;
		dst++;
		src++; 
	}
}

static void calc_pow2(float_t *freqdata, int binlength) {
    int i;
    float_t buf;

    for (i = 0; i < binlength; i++) {
        buf = freqdata[i] * freqdata[i];
        freqdata[i] = sqrt(buf); 
    }
}

static void calc_rdft(float_t *timedata, int binlength) {

	int ip[NMAXSQRT + 2];
    float_t w[NMAX * 5 / 4];

    ip[0] = 0;
    rdft(binlength, 1, timedata, ip, w);
}

static void calc_irdft(float_t *freqdata, int binlength) {
	int j;
	int ip[NMAXSQRT + 2];
    float_t w[NMAX * 5 / 4];

	// freqdata[0] *= 0.5;
    // ip[0] = 0;
    rdft(binlength, -1, freqdata, ip, w);

    for (j = 0; j <= binlength - 1; j++) {
        freqdata[j] *= 2.0 / binlength;
    }
}

static void calc_cdft(float_t *timedata, int binlength) {

    int ip[NMAXSQRT + 2];
    float_t w[NMAX * 5 / 4];

    ip[0] = 0;
    rdft(2 * binlength, -1, timedata, ip, w);
}

static void calc_icdft(float_t *freqdata, int binlength) {
    int j;
    int ip[NMAXSQRT + 2];
    float_t w[NMAX * 5 / 4];

    // freqdata[0] *= 0.5;
    // ip[0] = 0;
    rdft(2 * binlength, 1, freqdata, ip, w);

    for (j = 0; j <= binlength - 1; j++) {
        freqdata[j] *= 1.0 / binlength;
    }
}

static void calc_dct(float_t *timedata, int binlength) {

    int ip[NMAXSQRT + 2];
    float_t w[NMAX * 5 / 4];

    ip[0] = 0;
    ddct(binlength, -1, timedata, ip, w);
}

static void calc_idct(float_t *freqdata, int binlength) {
    int j;
    int ip[NMAXSQRT + 2];
    float_t w[NMAX * 5 / 4];

    freqdata[0] *= 0.5;
    ddct(binlength, 1, freqdata, ip, w);
    for (j = 0; j <= binlength - 1; j++) {
        freqdata[j] *= 2.0 / binlength;
    }
}

void calc_ac_wk_rdft(void) {
    
    // timedata <- original ecg data
    calc_copy(timedata, ecgdata, 0, 0, ECGREC_LENGTH);
    // transform timedata then -> timedata
    calc_rdft(timedata, ECGREC_LENGTH);
    // freqdata <- timedata
    calc_copy(freqdata, timedata, 0, 0, ECGREC_LENGTH);
    // inverse transform timedata -> timedata
    calc_irdft(timedata, ECGREC_LENGTH);
    // decompdata <- inverse transformed timedata
    calc_copy(decompdata, timedata, 0, 0, ECGREC_LENGTH);

    calc_pow2(freqdata, ECGREC_LENGTH);
    calc_irdft(freqdata, ECGREC_LENGTH);
    calc_copy(ac_wkdata, freqdata, 0, 0, ECGREC_LENGTH);
    
}

void calc_ac_wk_cdft(void) {
    
    // timedata <- original ecg data
    calc_copy(timedata, ecgdata, 0, 0, ECGREC_LENGTH);
    // transform timedata then -> timedata
    calc_cdft(timedata, ECGREC_LENGTH/2);
    // freqdata <- timedata
    calc_copy(freqdata, timedata, 0, 0, ECGREC_LENGTH);
    // inverse transform timedata -> timedata
    calc_icdft(timedata, ECGREC_LENGTH/2);
    // decompdata <- inverse transformed timedata
    calc_copy(decompdata, timedata, 0, 0, ECGREC_LENGTH);

    calc_pow2(freqdata, ECGREC_LENGTH);
    calc_icdft(freqdata, ECGREC_LENGTH/2);
    calc_copy(ac_wkdata, freqdata, 0, 0, ECGREC_LENGTH);   
}

void calc_ac_wk_dct(void) {
    int i;
    // timedata <- original ecg data
    calc_copy(timedata, ecgdata, 0, 0, ECGREC_LENGTH);
    // transform timedata then -> timedata
    calc_dct(timedata, ECGREC_LENGTH);
    // freqdata <- timedata
    calc_copy(freqdata, timedata, 0, 0, ECGREC_LENGTH);

    // Dismiss some values
    for (i = 0; i < HIGHPASS; i++)
     freqdata[i] = 0.00;
    for (i = LOWPASS; i < ECGREC_LENGTH; i++)
        freqdata[i] = 0.00;
    // inverse transform timedata -> timedata
    calc_idct(timedata, ECGREC_LENGTH);
    // decompdata <- inverse transformed timedata
    calc_copy(decompdata, timedata, 0, 0, ECGREC_LENGTH);

    calc_pow2(freqdata, ECGREC_LENGTH);
    calc_idct(freqdata, ECGREC_LENGTH);
    calc_copy(ac_wkdata, freqdata, 0, 0, ECGREC_LENGTH);   
}