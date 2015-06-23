#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "types.h"
#include "common.h"

void calc_copy(float_t *dst, float_t *src, int dstindex, int srcindex, int length) {
	// Add offset
	dst +=dstindex;
	src +=srcindex;

	while(length--) {
		*dst = *src;
		dst++;
		src++; 
	}
}

void calc_pow2(float_t *dst, int length) {
    int i;
    float_t buf;

    for (i = 0; i < length; i++) {
        buf = dst[i] * dst[i];
        // dst[i] = sqrt(buf);
        dst[i] = buf; 
    }
}

void calc_abs(float_t *dst, int length) {
    int i;
    float_t buf;

    for (i = 0; i < length; i++) {
        buf = fabs(dst[i]);
        dst[i] = buf; 
    }
}

void calc_norm(float_t *dst, int length) {
	int i;
	float_t max;
	float_t buf;

	max = 0;
	for (i = 0; i < length; i++)
		if (dst[i] > max)
			max = dst[i];

	for (i = 0; i < length; i++) {
		buf = dst[i] / max;
		dst[i] = buf;
	}
}

void calc_differentiate(float_t *dst, int length, int constant) {
	int i;
	float_t buf;

	for (i = 0; i < length - 1; i++) {
		buf = (dst[i+1] - dst[i]) * constant;
		dst[i] = buf;
	}
}