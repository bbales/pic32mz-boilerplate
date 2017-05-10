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

#define PCLK_FREQ 94500000

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
    int32 sample, temp;
    int32 line[96000];
    double decay, swell;
    unsigned int length, step, stepSize;
} DSPTapeDelay;

#if TAPE_ENABLED==1
DSPTapeDelay tapeDelay;
void DSPTapeDelaySetTap();
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

//
// Resonant Filter
//

typedef struct DSPResonantFilter{
    int32 buf0,buf1;
    int32 (*func)(int32);
    double fb,f,q;
} DSPResonantFilter;

DSPResonantFilter res;

int32 DSPResonantFilterFunc(int32 sample);

//
// Pole Zero Filter LP + HP
//

typedef struct DSPPZFilter{
    int32 (*func)(int32);
    double a0,a1,b1;
    int32 in0,in1,out0,out1;
} DSPPZFilter;

void DSPPZFilterSetHP(double fCut, double fSampling);
void DSPPZFilterSetLP(double fCut, double fSampling);

DSPPZFilter pzf;


#ifdef	__cplusplus
}
#endif

#endif	/* DSP_H */
