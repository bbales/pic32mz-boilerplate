#include <xc.h>
// #include <stdio.h> // Required for UART stuff

#include "init.h"
#include "controls.h"
#include "adc.h"
#include "audio.h"
#include "dsp.h"

Debouncer tapDebounce;
Debouncer bypassDebounce;
Debouncer subdivDebounce;

void controlsInit() {
    // True Tap LED
    TRISCbits.TRISC15 = 0;
    TAP_LIGHT_TRUE_W = 1;

    // Bypass LED
    TRISBbits.TRISB8 = 0;
    ANSELBbits.ANSB5 = 0;

    // Tap switch
    TRISEbits.TRISE7 = 1;
    CNPUEbits.CNPUE7 = 1;
    ANSELEbits.ANSE7 = 0;

    // Bypass Switch
    TRISEbits.TRISE6 = 1;
    CNPUEbits.CNPUE6 = 1;
    ANSELEbits.ANSE6 = 0;
    PMCON = 0;
    ETHCON1bits.ON = 0;

    // Bypass Relay Enable
    TRISEbits.TRISE5 = 0;
    ANSELEbits.ANSE5 = 0;
    RPE5R = 0;

    // Subdiv switch
    TRISCbits.TRISC12 = 1;
    CNPUCbits.CNPUC12 = 1;

    // Toggle Switches
    TRISESET = 0b1111 << 1;
    CNPUESET = 0b1111 << 1;
    SQI1CON = 0;
    ANSELEbits.ANSE4 = 0;

    // Subdiv LEDs
    /*
       RB12 : Thirty-second note 8:1
       RB13 : Sixteenth note 4:1
       RB14 : Eigth note 2:1
       RB15 : Quarter note 1:1
    */
    TRISBCLR = 0b1111 << 12;

    // Pots
    TRISBSET = 0b11111;
    ANSELBSET = 0b11111;

    CM2CONbits.ON = 0;
    RPD2R = 0;
    CM1CONbits.ON = 0;
    RPD4R = 0;

    tapDebounce.func = checkTap;
    tapDebounce.hasBounced = 1;
    tapDebounce.max = 1000;

    bypassDebounce.func = checkBypass;
    bypassDebounce.hasBounced = 1;
    bypassDebounce.max = 50000;

    subdivDebounce.func = checkSubdiv;
    subdivDebounce.hasBounced = 1;
    subdivDebounce.max = 50000;

    // Configure tap struct
    Tap = (struct Tap){};
    Tap.subdiv = 1;
    Tap.avg = 48000;

    Pots = (struct Pots){.turn = 0};
}

void debounce(Debouncer *d, char trigger) {
    if (trigger) {
        if (!d->count && d->hasBounced) {
            d->func();
            d->hasBounced = 0;
        }
        d->count = d->max;
    } else {
        if (!d->hasBounced) d->count--;
        if (!d->count) d->hasBounced = 1;
    }
}

unsigned int oldStepSize = 0;
unsigned int oldAvg = 0;
void readControls() {
    debounce(&bypassDebounce, BYPASS_SW_R);
    debounce(&tapDebounce, TAP_SW_R);
    debounce(&subdivDebounce, SUBDIV_SW_R);

    if (TAP_SW_R) {
        tapeDelay.swell += 0.00001;
    } else {
        tapeDelay.swell -= 0.01;
        if (tapeDelay.swell < 0.0) tapeDelay.swell = 0.0;
    }
    if (tapeDelay.swell + tapeDelay.decay > 1.2) tapeDelay.swell = 1.2 - tapeDelay.decay;

    // Control Tap LEDs
    TAP_LIGHT_TRUE_W = Tap.true > 0;
    SUB_1_W = Tap.subdiv == 1 && Tap.sub > 0;
    SUB_2_W = Tap.subdiv == 2 && Tap.sub > 0;
    SUB_3_W = Tap.subdiv == 4 && Tap.sub > 0;
    SUB_4_W = Tap.subdiv == 8 && Tap.sub > 0;
}

//
// Tap
//

void checkTap() {
    // If there was a pause greater than 2 seconds, restart
    if (Tap.audioCycles > 96000) {
        Tap.sum = 0;
        Tap.state = 0;
    }

    // For last 3 taps, add to sum
    if (Tap.state > 0) { Tap.sum += Tap.audioCycles; }

    // Reset count
    Tap.audioCycles = 0;

    if (Tap.state == 3) {
        // The tap sum is the number of cycles between 3 taps
        Tap.avg = Tap.sum / 3;

        // Tap avg must be less than the tapeDelayLine length
        Tap.avg = Tap.avg >= tapeDelay.length - 1 ? tapeDelay.length - 1 : Tap.avg;

        DSPTapeDelaySetTap();

        // Reset the sum and state
        Tap.sum = 0;
        Tap.state = 0;

        // Set timer 2 period to reflect tap
        // PR2 = Tap.period / Tap.subdiv;

        // Clear Tap.flip flag
        Tap.flip = 0;
    } else {
        // Increment the state
        Tap.state++;
    }
}

//
// Bypass
//

void checkBypass() {
    RELAY_FLIP;
    BYPASS_FLIP;
}

//
// Subdivision
//

void checkSubdiv() {
    if (Tap.subdiv == 8) {
        Tap.subdiv = 1;
    } else {
        Tap.subdiv *= 2;
    }
    // PR2 = Tap.period / Tap.subdiv;
}

//
// Potentiometer stuff
//

void readPots(void) {
    switch (Pots.turn) {
    case 0:
        Pots.pot0 = readFilteredADC(0) / 4096.0;
        Pots.turn++;
        break;
    case 1:
        Pots.pot1 = readFilteredADC(1) / 4096.0;
        Pots.turn++;
        break;
    case 2:
        Pots.pot2 = readFilteredADC(2) / 4096.0;
        Pots.turn++;
        break;
    case 3:
        Pots.pot3 = readFilteredADC(3) / 4096.0;
        Pots.turn = 0;
        break;
    }

    // l1.alpha = Pots.pot1;
    // l1.alphaNot = 1.0 - Pots.pot1;
    // l2.alpha = Pots.pot1;
    // l2.alphaNot = l2.alphaNot;

    codec.dry = Pots.pot1;
    codec.wet = 1.0 - Pots.pot1;

    tapeDelay.decay = Pots.pot0;

    // DSPPZFilterSetLP(Pots.pot2, Pots.pot3);
    // pzf.q = Pots.pot3;

    res.f = 0.05 + 0.5 * Pots.pot2;
    res.q = 0.5 + 0.5 * Pots.pot3 - 0.05;

    // Clear interrupt
    IFS0bits.T1IF = 0;
}
