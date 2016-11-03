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

void initDSP();


typedef struct DSPDelay{
    signed long (*func)(int, signed long);
    int step;
    int length;
    signed long decayNum;
    signed long decayDenom;
    signed long line[48000];
    signed long temp;
} DSPDelay;

DSPDelay d;

typedef struct DSPLeakyIntegrator{
    signed long (*func)(int, signed long);
    signed long prevOutput;
    signed long alpha;
    signed long denom;
} DSPLeakyIntegrator;

DSPLeakyIntegrator leaky;

signed long DSPLeakyIntegratorFunc(int channel, signed long sample);
signed long DSPdelayFunc(int channel, signed long sample);

#ifdef	__cplusplus
}
#endif

#endif	/* DSP_H */
