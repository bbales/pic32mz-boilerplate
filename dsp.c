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
#include "waves.h"
#include "audio.h"
#include "controls.h"

//
// Sync Delay implementation
//

#if DELAY_ENABLED == 1
int32 DSPDelayFunc(int32 sample) {
    if (delay.step >= delay.length) delay.step = 0;
    delay.temp = delay.line[delay.step] = sample + delay.decay * delay.line[delay.step];
    delay.step++;
    return delay.temp;
}
#endif

//
// Async Delay implementation
//

#if TAPE_ENABLED == 1
int32 DSPTapeDelayFunc(int32 sample) {
    tapeDelay.sample = sample;
    return tapeDelay.line[tapeDelay.step];
}

void DSPTapeDelayTimerFunc(void) {
    trueTap--;
    subTap--;
    tapeDelay.line[tapeDelay.step] =
        tapeDelay.sample + tapeDelay.decay * tapeDelay.line[tapeDelay.step];
    tapeDelay.step++;
    if (tapeDelay.step >= tapeDelay.length) {
        tapeDelay.step = 0;

        // Tap LEDs
        subTap = tapeDelay.length * 0.2;
        if (tapFlip >= subdiv - 1) {
            tapFlip = 0;
            trueTap = subTap * subdiv;
        } else {
            tapFlip++;
        }
    }

    // Clear interrupt
    IFS0bits.T2IF = 0;
}
#endif

//
// Leaky integrator implementation
//

int32 DSPLeakyIntegratorFunc(int32 sample) {
    leaky.prevOutput = leaky.alpha * (leaky.prevOutput) + (1.0 - leaky.alpha) * (sample);
    return leaky.prevOutput;
}

//
// FIR implementation
//

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

int32 DSPAmplitudeModulationFunc(int32 sample) {
    mod.step += 0.0125 * 10;
    if (mod.step >= 600.0) { mod.step = 0.0; }
    return (int32)(sample * (1 - mod.depth + mod.depth * mod.wave[(int) mod.step]));
}

//
// Initialization
//

void dspInit() {

#if DELAY_ENABLED == 1
    // Delay - Standard fixed rate buffer delay
    delay = (DSPDelay){
        .func = DSPDelayFunc, .step = 0, .length = 20000, .decay = 0.6, .temp = 0, .line = {0}};
#endif

#if TAPE_ENABLED == 1
    // Tape Delay - variable rate buffer delay
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

    // amplitude modulation
    mod = (DSPAmplitudeModulation){
        .step = 0.0, .wave = sinew, .depth = 0.4, .func = DSPAmplitudeModulationFunc};
}
