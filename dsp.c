/*
* @Author: bbales
* @Date:   2016-03-08 17:42:00
* @Last Modified by:   bbales
* @Last Modified time: 2016-03-08 17:44:28
*/
#include <xc.h>
#include <sys/attribs.h>
#include <dsplib_def.h>
#include <float.h>
#include "dsp.h"
#include "audio.h"

int32 DSPLeakyIntegratorFunc(char channel, int32 sample){
    if(!channel) return 0;
    leaky.prevOutput = leaky.alpha*(leaky.prevOutput/leaky.denom) + (leaky.denom - leaky.alpha)*(sample/leaky.denom);
    return leaky.prevOutput;
}

int32 DSPdelayFunc(char channel, int32 sample){
    if(!channel) return 0;
    if(d.step >= d.length) d.step = 0;
    d.temp = d.line[d.step] = sample + d.decayNum*d.line[d.step]/d.decayDenom;
    d.step++;
    return d.temp;
}

int32 DSPfirFilterDelayLine(int32 x){
    int32 y = fir.line[fir.ptr]; /* read operation */
    fir.line[fir.ptr++] = x;      /* write operation */
    if (fir.ptr >= fir.order) { fir.ptr -= fir.order; } /* wrap ptr */
    return y;
}

int32 DSPfirFilterFunc(char channel, int32 sample){
    int32 current = sample;
    if(!channel) return 0;
    fir.acc = 0;

    for(fir.iterator = 0; fir.iterator < fir.order; fir.iterator++){
        current = DSPfirFilterDelayLine(current);
        fir.acc += mul32(current, fir.coeffs[fir.iterator]);
    }

    return fir.acc;
}

void dspInit(){
    // Delay
    d = (DSPDelay) {
        .func = DSPdelayFunc,
        .step = 0,
        .length = 10000,
        .decayNum = 80,
        .decayDenom = 100,
        .temp = 0,
        .line = {0}
    };

    leaky = (DSPLeakyIntegrator) {
        .func = DSPLeakyIntegratorFunc,
        .denom = 100,
        .alpha = 0,
        .prevOutput = 0
    };

    fir = (DSPfirFilter) {
        .func = DSPfirFilterFunc,
        .order = 10,
        .coeffs = {0},
        .line = {0},
        .ptr = 0,
        .acc = 0,
        .iterator = 0
    };
}
