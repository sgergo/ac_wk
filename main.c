#include <stdint.h>
#include <stdio.h>
#include "types.h"
#include "common.h"
#include "dct.h"
#include "acorr.h"

extern float_t ecgdata[];

extern float_t timedata[ECGREC_LENGTH + 1];
extern float_t freqdata[ECGREC_LENGTH + 1];
extern float_t decompdata[ECGREC_LENGTH + 1];
extern float_t ac_wkdata[ECGREC_LENGTH + 1];
extern float_t acresult[ECGREC_LENGTH + 1];


acorr_data_t acorr_data;
dct_data_t dct_data;

int main(void) {
	unsigned int i;
	FILE *ofp;

	printf("\n\n 1-D DCT ACF and statistical ACF test\n\n");

	dct_data.wkresult = ac_wkdata;
	dct_data.ts = ecgdata;
	dct_data.offset = ECG_OFFSET;
	dct_data.lowpass = LOWPASS;
	dct_data.highpass = HIGHPASS;
	dct_data.n = ECGCALC_LENGTH;
	dct_ac_wk_dct();
	// Init ACF parameters
	acorr_data.acresult = acresult;
	acorr_data.ts = ecgdata;
	acorr_data.offset = ECG_OFFSET;
	acorr_data.maxlag = ECGCALC_LENGTH-1;
	acorr_data.n = ECGCALC_LENGTH;
	// Calculate ACF
	acorr_calculate(&acorr_data);

	// Output result
	ofp = fopen("result_ch3.csv", "w");

	for (i = 0; i < ECGREC_LENGTH; i++) {
		printf("%d: %f\t%f\t%f\t%f\n", i, ecgdata[i], decompdata[i], acresult[i], ac_wkdata[i]); 
		fprintf(ofp, "%f, %f, %f, %f\n", ecgdata[i], decompdata[i], acresult[i],  ac_wkdata[i]);
	}

	fclose(ofp);
	return 0;
}
