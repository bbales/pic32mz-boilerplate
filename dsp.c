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

int32 clip24bit(int32 a) {
    if (a > 8388606) return 8388606;
    if (a < -8388606) return -8388606;
    return a;
}

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

void DSPDelayInit() {
    delay = (DSPDelay){.func = DSPDelayFunc, .length = 20000, .decay = 0.6, .line = {0}};
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
    asm volatile("di");

    tapeDelay.line[tapeDelay.step] = clip24bit(
        tapeDelay.sample + (tapeDelay.decay + tapeDelay.swell) * tapeDelay.line[tapeDelay.step]);

    tapeDelay.step++;

    // Decrement counters for tap LEDs
    if (Tap.sub) Tap.sub--;
    if (Tap.true) Tap.true --;

    // At the end of the tape, go back to the beginning
    if (tapeDelay.step >= tapeDelay.length) {
        tapeDelay.step = 0;

        // The LED should be illuminated for 20% of the tape length
        Tap.sub = tapeDelay.length * 0.2;

        // Tap.flip is flipped every tape length
        if (Tap.flip >= Tap.subdiv - 1) {
            Tap.flip = 0;

            // Tap.true gets reset once all subdivisions have occurred
            Tap.true = Tap.sub * Tap.subdiv;
        } else {
            Tap.flip++;
        }
    }

    // Clear interrupt
    asm volatile("ei");
    IFS0bits.T2IF = 0;
}

void DSPTapeDelaySetTap() {
    // Clock divider
    unsigned int timerClockDiv = 1;

    // Calculates loop period in seconds
    double period = (double) Tap.avg / (double) codec.sampleRate;

    // Desired timer frequency 1/(period/length) = length/period
    unsigned int timerFrequency = tapeDelay.length / period;

    unsigned int _PR2 = (PCLK_FREQ / timerClockDiv) / timerFrequency;

    PR2 = _PR2;
}

void DSPTapeDelayInit() {
    tapeDelay = (DSPTapeDelay){.line = {0}};
    tapeDelay.func = DSPTapeDelayFunc;
    tapeDelay.length = 48000;
    tapeDelay.decay = 0.6;
    tapeDelay.stepSize = 2;
}

#endif

//
// Leaky integrator implementation
//

int32 DSPLeakyIntegratorFunc(struct DSPLeakyIntegrator l, int32 sample) {
    l.prevOutput = l.alpha * l.prevOutput + l.alphaNot * sample;
    return l.prevOutput;
}

void DSPLeakyIntegratorInit() {
    l1 = (DSPLeakyIntegrator){.func = DSPLeakyIntegratorFunc, .alpha = 0.2, .alphaNot = 0.8};
    l2 = (DSPLeakyIntegrator){.func = DSPLeakyIntegratorFunc, .alpha = 0.2, .alphaNot = 0.8};
}

//
// FIR implementation
//

int32 DSPFIRFilterFunc(int32 sample) {
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

void DSPFIRFilterInit() {
    fir = (DSPFIRFilter){.func = DSPFIRFilterFunc,
                         .order = 12,
                         .numCoeffs = 11,
                         .coeffs = {-20858415, -45336999, -48513960, 4602059, 134600231, 311017954,
                                    468218759, 530064262, 468218759, 311017954, 134600231, 4602059,
                                    -48513960, -45336999, -20858415},
                         .line = {0}};
}

//
// Amplitude Modulation
//

int32 DSPAmplitudeModulationFunc(int32 sample) {
    mod.step += 0.0125 * 10;
    if (mod.step >= 600.0) { mod.step = 0.0; }
    return (int32)(sample * (1 - mod.depth + mod.depth * mod.wave[(int) mod.step]));
}

void DSPAmplitudeModulationInit() {
    mod = (DSPAmplitudeModulation){
        .step = 0.0, .wave = sinew, .depth = 0.4, .func = DSPAmplitudeModulationFunc};
}

//
// Resonant Filter
//

int32 DSPResonantFilterFunc(int32 sample) {
    res.fb = res.q + res.q / (1.0 - res.f);
    res.buf0 = clip24bit(res.buf0 + res.f * (sample - res.buf0 + res.fb * (res.buf0 - res.buf1)));
    res.buf1 = clip24bit(res.buf1 + res.f * (res.buf0 - res.buf1));
    return res.buf1;
}

void DSPResonantFilterInit() { res = (DSPResonantFilter){.func = DSPResonantFilterFunc}; }

//
// Single Pole, Single Zero LP HP
//

void DSPPZFilterSetLP(double fCut, double fSampling) {
    pzf.a0 = fCut / (fSampling + fCut);
    pzf.a1 = pzf.a0;
    pzf.b1 = (fSampling - fCut) / (fSampling + fCut);
}

void DSPPZFilterSetHP(double fCut, double fSampling) {
    pzf.a0 = fSampling / (fSampling + fCut);
    pzf.a1 = -pzf.a0;
    pzf.b1 = (fSampling - fCut) / (fSampling + fCut);
}

int32 DSPPZFilterFunc(int32 sample) {
    pzf.in1 = pzf.in0;
    pzf.in0 = sample;
    pzf.out1 = pzf.out0;
    pzf.out0 = pzf.in0 * pzf.a0 + pzf.in1 * pzf.a1 + pzf.out1 * pzf.b1;
    return pzf.out0;
}

void DSPPZFilterInit() {
    pzf = (DSPPZFilter){};
    pzf.func = DSPPZFilterFunc;
    pzf.a0 = pzf.a1 = pzf.b1 = 0.0;
    pzf.in0 = pzf.in1 = pzf.out0 = pzf.out1 = 0;
}

//
// Initialization
//

void dspInit() {

#if DELAY_ENABLED == 1
    DSPDelayInit();
#endif

#if TAPE_ENABLED == 1
    DSPTapeDelayInit();
#endif

    DSPLeakyIntegratorInit();
    DSPFIRFilterInit();
    DSPAmplitudeModulationInit();
    DSPResonantFilterInit();
    DSPPZFilterInit();
}
