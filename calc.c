#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "types.h"
#include "common.h"
#include "fft4g.h"
#include "calc.h"

extern float_t ecgdata[];
float_t timedata[ECGBIN_LENGTH + 1];
float_t freqdata[ECGBIN_LENGTH + 1];

float_t compressedecgdata[ECGREC_LENGTH + 1];
float_t decompressedecgdata[ECGREC_LENGTH + 1];

float_t ac_wkdata[ECGREC_LENGTH + 1];
int hrdata[250];

// int qrsdata[ECGREC_LENGTH + 1];

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

void calc_ac_wk_rdft(void) {
    
    // timedata <- original ecg data
    calc_copy(timedata, ecgdata, 0, 0, ECGREC_LENGTH);
    // transform timedata then -> timedata
    calc_rdft(timedata, ECGREC_LENGTH);
    // freqdata <- timedata
    calc_copy(freqdata, timedata, 0, 0, ECGREC_LENGTH);
    // inverse transform timedata -> timedata
    calc_irdft(timedata, ECGREC_LENGTH);
    // decompressedecgdata <- inverse transformed timedata
    calc_copy(decompressedecgdata, timedata, 0, 0, ECGREC_LENGTH);

    calc_pow2(freqdata, ECGREC_LENGTH);
    calc_irdft(freqdata, ECGREC_LENGTH);
    calc_copy(ac_wkdata, freqdata, 0, 0, ECGREC_LENGTH);
    
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

void calc_ac_wk_cdft(void) {
    
    // timedata <- original ecg data
    calc_copy(timedata, ecgdata, 0, 0, ECGREC_LENGTH);
    // transform timedata then -> timedata
    calc_cdft(timedata, ECGREC_LENGTH/2);
    // freqdata <- timedata
    calc_copy(freqdata, timedata, 0, 0, ECGREC_LENGTH);
    // inverse transform timedata -> timedata
    calc_icdft(timedata, ECGREC_LENGTH/2);
    // decompressedecgdata <- inverse transformed timedata
    calc_copy(decompressedecgdata, timedata, 0, 0, ECGREC_LENGTH);

    calc_pow2(freqdata, ECGREC_LENGTH);
    calc_icdft(freqdata, ECGREC_LENGTH/2);
    calc_copy(ac_wkdata, freqdata, 0, 0, ECGREC_LENGTH);   
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

void calc_compress_ecgdata(void) {
	int num_dctbins_in_ecgdata;
	int i, j;
    int index_of_nextbin;

	num_dctbins_in_ecgdata = ECGREC_LENGTH / ECGBIN_LENGTH;

	for (j = 0; j < num_dctbins_in_ecgdata; j++) {
		index_of_nextbin = (ECGBIN_LENGTH * j);
		printf("Iteration: %d, Bin index: %d\n", j, index_of_nextbin);
		
		// Copy original data into working buffer
		calc_copy(timedata, ecgdata, 0, index_of_nextbin, ECGBIN_LENGTH);
		// Calculate 1-D DCT and put everything back to working buffer
		calc_dct(timedata, ECGBIN_LENGTH);

		// timedata now contains frequency domain values
		// Copy those values into freqdata buffer
		calc_copy(freqdata, timedata, 0, 0, ECGBIN_LENGTH);
		
		// Dismiss some values
		// for (i = 0; i < HIGHPASS; i++)
		// 	freqdata[i] = 0.00;
		for (i = LOWPASS; i < ECGBIN_LENGTH; i++)
			freqdata[i] = 0.00;

		// Freqdata contains frequency domain data from timedata with some values dismissed
		// Add those values into compressedecgdata buffer
		calc_copy(compressedecgdata, freqdata, index_of_nextbin, 0, ECGBIN_LENGTH);

		// Calculate inverse transform
		calc_idct(freqdata, ECGBIN_LENGTH);

		// Freqdata contains inverse transformed data from freqdata
		// Add those values into decompressedecgdata buffer
		calc_copy(decompressedecgdata, freqdata, index_of_nextbin, 0, ECGBIN_LENGTH);
	} 	
}

void calc_ac_wk_dct(void) {
    
    // timedata <- original ecg data
    calc_copy(timedata, ecgdata, 0, 0, ECGREC_LENGTH);
    // transform timedata then -> timedata
    calc_dct(timedata, ECGREC_LENGTH);
    // freqdata <- timedata
    calc_copy(freqdata, timedata, 0, 0, ECGREC_LENGTH);
    // inverse transform timedata -> timedata
    calc_idct(timedata, ECGREC_LENGTH);
    // decompressedecgdata <- inverse transformed timedata
    calc_copy(decompressedecgdata, timedata, 0, 0, ECGREC_LENGTH);

    calc_pow2(freqdata, ECGREC_LENGTH);
    calc_idct(freqdata, ECGREC_LENGTH);
    calc_copy(ac_wkdata, freqdata, 0, 0, ECGREC_LENGTH);   
}

static int calc_findHR(float_t *data, int binlength) {
    int i;
    float_t max = 0;
    int maxindex = 0;


    for (i = 150; i < binlength; i++) {
        if (data[i] > max) {
            max = data[i];
            maxindex = i;
        }
    }
    
    
    return maxindex;
}

int calc_continuous_ac(void) {
    int i;
    int nextbin;
    int hr;

    i = 0;
    nextbin = 0;

    while (true) {
        calc_copy(timedata, ecgdata, 0, nextbin, ECGREC_LENGTH);
        calc_dct(timedata, ECGREC_LENGTH);
        calc_pow2(timedata, ECGREC_LENGTH);
        calc_idct(timedata, ECGREC_LENGTH);
        hr = calc_findHR(timedata, ECGREC_LENGTH);
        hrdata[i] = hr;
        printf("hr: %d hrdata[i]: %d\n", hr, hrdata[i]);
        nextbin += 1024;

        if ((nextbin +  ECGREC_LENGTH) > ACFREC_LENGTH)
            break;
        i++;
    }

    return i;
}

