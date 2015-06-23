#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "types.h"
#include "common.h"
#include "fft4g.h"
#include "dct.h"
#include "calc.h"

extern float_t ecgdata[];
extern dct_data_t dct_data;

float_t timedata[ECGREC_LENGTH + 1];
float_t freqdata[ECGREC_LENGTH + 1];
float_t decompdata[ECGREC_LENGTH + 1];
float_t ac_wkdata[ECGREC_LENGTH + 1];

static void dct_dct(float_t *timedata, int binlength) {

    int ip[NMAXSQRT + 2];
    float_t w[NMAX * 5 / 4];

    ip[0] = 0;
    ddct(binlength, -1, timedata, ip, w);
}

static void dct_idct(float_t *freqdata, int binlength) {
    int j;
    int ip[NMAXSQRT + 2];
    float_t w[NMAX * 5 / 4];

    freqdata[0] *= 0.5;
    ddct(binlength, 1, freqdata, ip, w);
    for (j = 0; j <= binlength - 1; j++) {
        freqdata[j] *= 2.0 / binlength;
    }
}

void dct_ac_wk_dct(void) {
    int i;


    // timedata <- original ecg data
    calc_copy(timedata, dct_data.ts, 0, dct_data.offset, dct_data.n);

    // timedata <- normalized ecg data
    calc_norm(timedata, dct_data.n);

    // timedata <- differentiated normalized ecg data
    calc_differentiate(timedata, dct_data.n, 2);

    // transform timedata then -> timedata
    dct_dct(timedata, dct_data.n);
    // freqdata <- timedata
    calc_copy(freqdata, timedata, 0, 0, dct_data.n);

    // Dismiss some values
    for (i = 0; i < dct_data.highpass; i++)
     freqdata[i] = 0.00;
    for (i = dct_data.lowpass; i < dct_data.n; i++)
        freqdata[i] = 0.00;


    // inverse transform timedata -> timedata
    dct_idct(timedata, dct_data.n);
    // decompdata <- inverse transformed timedata (to store it)
    calc_copy(decompdata, timedata, 0, 0, dct_data.n);

    // calc_abs(freqdata, dct_data.n);
    calc_pow2(freqdata, dct_data.n);
    dct_idct(freqdata, dct_data.n);
    calc_copy(ac_wkdata, freqdata, 0, 0, dct_data.n);   
}

void dct_ac_wk_dct2(void) {
    int i;


    // timedata <- original ecg data
    calc_copy(timedata, dct_data.ts, 0, dct_data.offset, dct_data.n);
    // transform timedata then -> timedata
    dct_dct(timedata, dct_data.n);
    // freqdata <- timedata
    calc_copy(freqdata, timedata, 0, 0, dct_data.n);

    // Dismiss some values
    for (i = 0; i < dct_data.highpass; i++)
     freqdata[i] = 0.00;
    for (i = dct_data.lowpass; i < dct_data.n; i++)
        freqdata[i] = 0.00;
    // inverse transform timedata -> timedata
    dct_idct(timedata, dct_data.n);
    // decompdata <- inverse transformed timedata
    calc_copy(decompdata, timedata, 0, 0, dct_data.n);

    calc_abs(freqdata, dct_data.n);
    dct_idct(freqdata, dct_data.n);
    calc_copy(ac_wkdata, freqdata, 0, 0, dct_data.n);   
}