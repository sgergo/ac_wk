#include <stdint.h>
#include <string.h>
#include <math.h>
#include "types.h"
#include "common.h"
#include "acorr.h"

float_t acresult[ECGREC_LENGTH + 1];

static void acorr_compute_mean(acorr_data_t *ads) {
  int      i;

  ads->mean = 0.0;
  for (i = ads->offset; i < (ads->n + ads->offset); i++)
    ads->mean = ads->mean + (ads->ts[i] / ads->n);
}

static void acorr_compute_variance(acorr_data_t *ads) {
  int      i;

  ads->variance = 0.0;
  for (i = ads->offset; i < (ads->n + ads->offset); i++)
    ads->variance = ads->variance + (pow((ads->ts[i] - ads->mean), 2.0) / ads->n);
}

static float_t acorr_compute_autoc(acorr_data_t *ads, int lag) {
  float_t   autocv;      // autocovariance 
  float_t   ac_value;    
  int      i;

  autocv = 0.0;
  for (i = ads->offset; i < ((ads->n + ads->offset) - lag); i++)
    autocv = autocv + ((ads->ts[i] - ads->mean) * \
    	(ads->ts[i + lag] - ads->mean) / (ads->n - lag));

  // AC is autocovariance divided by variance
  ac_value = autocv / ads->variance;

  return ac_value;
}

void acorr_calculate(acorr_data_t *ads) {
	int i;
	acorr_compute_mean(ads);
	acorr_compute_variance(ads);

	for (i = 0; i < ads->maxlag; i++) {
		ads->acresult[i] = acorr_compute_autoc(ads, i);
	}
}

