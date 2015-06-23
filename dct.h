#ifndef DCT_H
#define DCT_H

typedef struct {
	float_t *ts;
	int offset;
	float_t *wkresult;
	int lowpass;
	int highpass;
	int n;
}dct_data_t;

void dct_ac_wk_dct(void);



#endif