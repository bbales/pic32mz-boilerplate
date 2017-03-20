/*
* @Author: bbales
* @Date:   2016-03-08 17:42:00
* @Last Modified by:   bbales
* @Last Modified time: 2016-03-08 17:44:28
*/
#include <xc.h>
#include <dsplib_def.h>
#include <float.h>

#include "dsp.h"
#include "audio.h"

int32 mul32custom(int32 a, int32 b) {
    int64 y = (int64) a * (int64) b;
    return ((int32) y >> 31);
}

int32 DSPLeakyIntegratorFunc(int32 sample) {
    leaky.prevOutput = leaky.alpha * (leaky.prevOutput) + (1.0 - leaky.alpha) * (sample);
    return leaky.prevOutput;
}

#if DELAY_ENABLED == 1
int32 DSPDelayFunc(int32 sample) {
    if (delay.step >= delay.length) delay.step = 0;
    delay.temp = delay.line[delay.step] = sample + delay.decay * delay.line[delay.step];
    delay.step++;
    return delay.temp;
}
#endif

#if TAPE_ENABLED == 1
int32 DSPTapeDelayFunc(int32 sample) {
    tapeDelay.sample = sample;
    return tapeDelay.line[tapeDelay.step];
}

void DSPTapeDelayTimerFunc(void) {
    // if (tapeDelay.step == 1) LATBINV = 1 << 9;
    tapeDelay.line[tapeDelay.step] =
        tapeDelay.sample + tapeDelay.decay * tapeDelay.line[tapeDelay.step];
    tapeDelay.step++;
    if (tapeDelay.step >= tapeDelay.length) tapeDelay.step = 0;
    // Clear interrupt
    IFS0bits.T2IF = 0;
}
#endif

int32 DSPfirFilterFunc(int32 sample) {
    fir.current = sample;
    fir.acc = 0;
    for (fir.iterator = 0; fir.iterator < fir.order; fir.iterator++) {
        fir.head = fir.line[fir.ptr];
        fir.line[fir.ptr] = fir.current;
        fir.ptr++;
        if (fir.ptr >= fir.order) fir.ptr = 0;
        fir.acc += (int64) fir.head * (int64) fir.coeffs[fir.numCoeffs - fir.iterator];
        fir.current = fir.head;
    }
    return (int32)(fir.acc >> 31);
}

void dspInit() {
// Delay - Standard fixed rate buffer delay
#if DELAY_ENABLED == 1
    delay = (DSPDelay){
        .func = DSPDelayFunc, .step = 0, .length = 20000, .decay = 0.6, .temp = 0, .line = {0}};
#endif

// Tape Delay - variable rate buffer delay
#if TAPE_ENABLED == 1
    tapeDelay = (DSPTapeDelay){.func = DSPTapeDelayFunc,
                               .step = 0,
                               .length = 48000,
                               .sample = 0,
                               .decay = 0.6,
                               .temp = 0,
                               .line = {0}};
#endif

    // Leaky integrator
    leaky = (DSPLeakyIntegrator){.func = DSPLeakyIntegratorFunc, .alpha = 0.2, .prevOutput = 0};

    // FIR filter
    fir = (DSPfirFilter){.func = DSPfirFilterFunc,
                         .order = 12,
                         .numCoeffs = 11,
                         .coeffs = {-20858415, -45336999, -48513960, 4602059, 134600231, 311017954,
                                    468218759, 530064262, 468218759, 311017954, 134600231, 4602059,
                                    -48513960, -45336999, -20858415},
                         .line = {0},
                         .ptr = 0,
                         .head = 0,
                         .acc = 0,
                         .iterator = 0,
                         .current = 0};
}
