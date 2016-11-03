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

void dspInit();

typedef struct DSPDelay{
    long (*func)(char, long);
    int step;
    int length;
    long decayNum;
    long decayDenom;
    long line[48000];
    long temp;
} DSPDelay;

DSPDelay d;

typedef struct DSPLeakyIntegrator{
    long (*func)(char, long);
    long prevOutput;
    long alpha;
    long denom;
} DSPLeakyIntegrator;

DSPLeakyIntegrator leaky;

long DSPLeakyIntegratorFunc(char channel, long sample);
long DSPdelayFunc(char channel, long sample);

#ifdef	__cplusplus
}
#endif

#endif	/* DSP_H */
