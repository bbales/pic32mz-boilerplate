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
    int step;
    int length;
    int decayNum;
    int decayDenom;
    signed long line[48000];
    signed long (*func)(int, signed long);
    signed long temp;
} DSPDelay;

DSPDelay d;

#ifdef	__cplusplus
}
#endif

#endif	/* DSP_H */
