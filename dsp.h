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

#include <sys/attribs.h>
#include <dsplib_def.h>


#define TAPE_ENABLED 1
#define DELAY_ENABLED 0

//
// Routine for initializing all DSP
//

void dspInit();

//
// Synchronous buffered delay line
//

typedef struct DSPDelay{
    int32 (*func)(int32);
    int step;
    double decay;
    int length;
    int32 line[48000];
    int32 temp;
} DSPDelay;

#if DELAY_ENABLED==1
DSPDelay delay;
#endif

//
// Asynchronous buffered delay line
//

typedef struct DSPTapeDelay{
    int32 (*func)(int32);
    int step;
    int32 sample;
    double decay;
    double swell;
    int length;
    int32 line[48000];
    int32 temp;
} DSPTapeDelay;

#if TAPE_ENABLED==1

DSPTapeDelay tapeDelay;
void DSPTapeDelayTimerFunc(void);
void __ISR_AT_VECTOR(_TIMER_2_VECTOR, IPL7SRS) DSPTapeDelayTimerFunc(void);
#endif

//
// The simplest and least computationally expensive filtering
//

typedef struct DSPLeakyIntegrator{
    int32 (*func)(struct DSPLeakyIntegrator, int32);
    int32 prevOutput;
    double alpha;
    double alphaNot;
} DSPLeakyIntegrator;

DSPLeakyIntegrator l1;
DSPLeakyIntegrator l2;

//
// Finite Impulse Response Filter
//

typedef struct DSPFIRFilter{
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
} DSPFIRFilter;

DSPFIRFilter fir;

//
// Wavetable based amplitude modulation
//

typedef struct DSPAmplitudeModulation{
    int32 (*func)(int32);
    double * wave;
    double step;
    double depth;
} DSPAmplitudeModulation;

DSPAmplitudeModulation mod;


#ifdef	__cplusplus
}
#endif

#endif	/* DSP_H */
