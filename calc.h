#ifndef CALC_H
#define CALC_H

void calc_copy(float_t *dst, float_t *src, int dstindex, int srcindex, int length);
void calc_pow2(float_t *dst, int length);
void calc_abs(float_t *dst, int length);
void calc_norm(float_t *dst, int length);
void calc_differentiate(float_t *dst, int length, int constant);
void calc_foldedcorr(float_t *src, float_t *dst, int length);

#endif