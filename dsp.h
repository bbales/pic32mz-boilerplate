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

#define TAPE_ENABLED 1
#define DELAY_ENABLED 0

void dspInit();
int32 mul32custom(int32 a, int32 b);

typedef struct DSPDelay{
    int32 (*func)(int32);
    int step;
    int length;
    double decay;
    int32 line[48000];
    int32 temp;
} DSPDelay;

typedef struct DSPTapeDelay{
    int32 (*func)(int32);
    int step;
    int32 sample;
    double decay;
    int32 line[48000];
    int32 temp;
} DSPTapeDelay;


typedef struct DSPLeakyIntegrator{
    int32 (*func)(int32);
    int32 prevOutput;
    double alpha;
} DSPLeakyIntegrator;
int32 DSPLeakyIntegratorFunc(int32 sample);

typedef struct DSPfirFilter{
    int32 (*func)(int32);
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
#if DELAY_ENABLED==1
DSPDelay delay;
int32 DSPDelayFunc(int32 sample);
#endif

#if TAPE_ENABLED==1
int32 DSPTapeDelayFunc(int32 sample);
void DSPTapeDelayTimerFunc(void);

DSPTapeDelay tapeDelay;
void __ISR_AT_VECTOR(_TIMER_2_VECTOR, IPL1SRS) DSPTapeDelayTimerFunc(void);
#endif

DSPLeakyIntegrator leaky;
DSPfirFilter fir;

#ifdef	__cplusplus
}
#endif

#endif	/* DSP_H */
