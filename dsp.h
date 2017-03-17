/*
 * File:   delay.h
 * Author: bbales
 *
 * Created on September 29, 2016, 9:53 PM
 */

#ifndef DSP_H
#define	DSP_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <dsplib_def.h>

void dspInit();
int32 mul32custom(int32 a, int32 b);

typedef struct DSPDelay{
    int32 (*func)(char, int32);
    int step;
    int length;
    double decay;
    int32 line[48000];
    int32 temp;
} DSPDelay;
int32 DSPdelayFunc(int32 sample);

typedef struct DSPLeakyIntegrator{
    int32 (*func)(char, int32);
    int32 prevOutput;
    double alpha;
} DSPLeakyIntegrator;
int32 DSPLeakyIntegratorFunc(int32 sample);

typedef struct DSPfirFilter{
    int32 (*func)(char, int32);
    int32 line[20];
    int32 coeffs[20];
    int64 acc;
    int32 head;
    int32 current;
    int order;
    int numCoeffs;
    int iterator;
    long ptr;
} DSPfirFilter;
int32 DSPfirFilterFunc(int32 sample);

// Instances of DSP objects
DSPDelay d;
DSPLeakyIntegrator leaky;
DSPfirFilter fir;

#ifdef	__cplusplus
}
#endif

#endif	/* DSP_H */
