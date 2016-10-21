/*
* @Author: bbales
* @Date:   2016-03-08 17:42:00
* @Last Modified by:   bbales
* @Last Modified time: 2016-03-08 17:44:28
*/
#include <xc.h>
#include <sys/attribs.h>
#include "dsp.h"
#include "audio.h"

signed long delayLine[48000] = {0};

signed long DSPd(int channel, signed long sample){
    if(!channel) return 0;
    if(d.step >= d.length) d.step = 0;
    d.temp = delayLine[d.step] = sample + d.decayNum*delayLine[d.step]/d.decayDenom;
    d.step++;
    return d.temp;
}

void initDSP(){
    d = (DSPDelay) {.step = 0, .length = 5000, .decayNum = 97, .decayDenom = 100, .temp = 0};
    d.func = DSPd;
}
