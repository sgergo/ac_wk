#include <stdint.h>
#include <stdio.h>
#include "types.h"
#include "common.h"
#include "calc.h"
#include "acorr.h"

extern float_t ecgdata[];
extern float_t timedata[ECGBIN_LENGTH + 1];
extern float_t freqdata[ECGBIN_LENGTH + 1];
extern float_t ac_wkdata[ECGREC_LENGTH + 1];
extern int hrdata[250];

// float_t compressedecgdata[ECGREC_LENGTH + 1];
// float_t decompressedecgdata[ECGREC_LENGTH + 1];
// int qrsdata[ECGREC_LENGTH + 1];
// float_t acresult[ECGREC_LENGTH + 1];

acorr_data_t acorr_data;

int main(void) {
	unsigned int i, j;
	FILE *ofp;

	printf("\n\n 1-D DCT test\n\n");

	// Transform-dismiss-inverse transform
	// calc_compress_ecgdata();
	// calc_ac_wk_dct();
	// Init ACF parameters
	// acorr_data.acresult = acresult;
	// acorr_data.ts = ecgdata;
	// acorr_data.maxlag = ECGREC_LENGTH;
	// acorr_data.n = ECGREC_LENGTH + 1;
	// Calculate ACF
	// acorr_calculate(&acorr_data);

	// Output result
	ofp = fopen("result_ch3.csv", "w");

	// fprintf(ofp, "Original, iRDFT, ACF, AC_WK\n");
	// printf("Original\tiRDFT\tACF\tAC_WK\n");
	// for (i = 0; i < ECGREC_LENGTH; i++) {
	// 	printf("%d: %f\t%f\t%f\t%f\n", i, ecgdata[i], decompressedecgdata[i], acresult[i], ac_wkdata[i]); 
	// 	fprintf(ofp, "%f, %f, %f, %f\n", ecgdata[i], decompressedecgdata[i], acresult[i],  ac_wkdata[i]);
		
	// }

	fprintf(ofp, "HR\n");
	printf("HR\n");

	j = calc_continuous_ac();
	for (i = 0; i < j; i++) {
		printf("%d: %d\n", i, hrdata[i]); 
		fprintf(ofp, "%d\n", hrdata[i]);		
	}
	fclose(ofp);
	return 0;
}
