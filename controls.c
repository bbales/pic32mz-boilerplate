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
    tapDebounce.count = 0;
    tapDebounce.hasBounced = 1;

    bypassDebounce.func = checkBypass;
    bypassDebounce.count = 0;
    bypassDebounce.hasBounced = 1;

    subdivDebounce.func = checkSubdiv;
    subdivDebounce.count = 0;
    subdivDebounce.hasBounced = 1;
}

void debounce(Debouncer *d, char trigger) {
    if (trigger) {
        if (!d->count && d->hasBounced) {
            d->func();
            d->hasBounced = 0;
        }
        d->count = 50000;
    } else {
        if (!d->hasBounced) d->count--;
        if (!d->count) d->hasBounced = 1;
    }
}

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
    TAP_LIGHT_TRUE_W = trueTap >= 0;
    SUB_1_W = subdiv == 1 && subTap >= 0;
    SUB_2_W = subdiv == 2 && subTap >= 0;
    SUB_3_W = subdiv == 4 && subTap >= 0;
    SUB_4_W = subdiv == 8 && subTap >= 0;
}

//
// Tap
//

unsigned long long audioCycles = 0;
int trueTap = 0;
int subTap = 0;
char tapFlip = 0;
int TAP_PERIOD = 0;
char TAP_STATE = 0;
unsigned long long TAP_SUM = 0;

void checkTap() {
    // If there was a pause greater than 2 seconds, restart
    if (audioCycles > 96000 * 2) {
        TAP_SUM = 0;
        TAP_STATE = 0;
    }

    // For last 3 taps, add to sum
    if (TAP_STATE > 0) { TAP_SUM += audioCycles; }

    // Reset count
    audioCycles = 0;

    if (TAP_STATE == 3) {
        // Calculate non-subdivided period and clear cycles
        TAP_PERIOD = (0.00512 * TAP_SUM) / 3.0;

        // Reset the sum and state
        TAP_SUM = 0;
        TAP_STATE = 0;

        // Formula is ((PBCLK/CLKDIV)/ (tdlen * fcodec)) * lastTap
        // PBCLK = 945000000
        // CLKDIV = 1 -> 256
        // fcodec = 96000
        // tdlen = 48000
        // At clkdiv = 1 => 0.0205 * lastTap

        // Set timer 2 period to reflect tap
        PR2 = TAP_PERIOD / subdiv;

        // Clear tapFlip flag
        tapFlip = 0;
    } else {
        // Increment the state
        TAP_STATE++;
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

char subdiv = 1;

void checkSubdiv() {
    if (subdiv == 8) {
        subdiv = 1;
    } else {
        subdiv *= 2;
    }
    PR2 = TAP_PERIOD / subdiv;
}

//
// Potentiometer stuff
//

char turn = 0;

// Timer1 handler
void readPots(void) {
    switch (turn) {
    case 0:
        tapeDelay.decay = readFilteredADC(0) / 4096.0;
        turn = 1;
        break;
    case 1:
        l1.alpha = readFilteredADC(1) / 4096.0;
        l1.alphaNot = 1.0 - l1.alpha;
        l2.alpha = l1.alpha;
        l2.alphaNot = l2.alphaNot;
        turn = 2;
        break;
    case 2:
        codec.dry = readFilteredADC(2) / 4096.0;
        codec.wet = 1.0 - codec.dry;
        turn = 3;
        break;
    case 3:
        turn = 0;
        break;
    }

    // Clear interrupt
    IFS0bits.T1IF = 0;
}
