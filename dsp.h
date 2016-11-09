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

typedef struct DSPDelay{
    int32 (*func)(char, int32);
    int step;
    int length;
    int32 decayNum;
    int32 decayDenom;
    int32 line[48000];
    int32 temp;
} DSPDelay;
int32 DSPdelayFunc(char channel, int32 sample);

typedef struct DSPLeakyIntegrator{
    int32 (*func)(char, int32);
    int32 prevOutput;
    int32 alpha;
    int32 denom;
} DSPLeakyIntegrator;
int32 DSPLeakyIntegratorFunc(char channel, int32 sample);

typedef struct DSPfirFilter{
    int32 (*func)(char, int32);
    int32 line[10];
    int32 coeffs[10];
    int32 acc;
    int order;
    int iterator;
    long ptr;
} DSPfirFilter;
int32 DSPfirFilterFunc(char channel, int32 sample);
int32 DSPfirFilterDelayLine(int32 x);

// Instances of DSP objects
DSPDelay d;
DSPLeakyIntegrator leaky;
DSPfirFilter fir;

#ifdef	__cplusplus
}
#endif

#endif	/* DSP_H */
