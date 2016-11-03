/*
* @Author: bbales
* @Date:   2016-03-08 17:42:00
* @Last Modified by:   bbales
* @Last Modified time: 2016-03-08 17:44:28
*/
#include <xc.h>
#include <sys/attribs.h>
#include <float.h>
#include "dsp.h"
#include "audio.h"

long DSPLeakyIntegratorFunc(char channel, long sample){
    if(!channel) return 0;
    leaky.prevOutput = leaky.alpha*(leaky.prevOutput/leaky.denom) + (leaky.denom - leaky.alpha)*(sample/leaky.denom);
    return leaky.prevOutput;
}

long DSPdelayFunc(char channel, long sample){
    if(!channel) return 0;
    if(d.step >= d.length) d.step = 0;
    d.temp = d.line[d.step] = sample + d.decayNum*d.line[d.step]/d.decayDenom;
    d.step++;
    return d.temp;
}

void dspInit(){
    // Delay
    d = (DSPDelay) {
        .func = DSPdelayFunc,
        .step = 0,
        .length = 5000,
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
}
