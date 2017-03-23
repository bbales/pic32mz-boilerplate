#include <xc.h>
// #include <stdio.h> // Required for UART stuff

#include "controls.h"
#include "adc.h"
#include "audio.h"
#include "dsp.h"

void controlsInit() {
    // Tap LEDs
    TRISEbits.TRISE6 = 0;
    TRISEbits.TRISE7 = 0;

    // Bypass LED
    TRISBbits.TRISB12 = 0;

    // Subdiv switch
    TRISBbits.TRISB13 = 1;
    CNPDBbits.CNPDB13 = 1;
    ANSELBbits.ANSB13 = 0;
    ETHCON1bits.ON = 0;

    // Tap switch
    TRISBbits.TRISB14 = 1;
    CNPDBbits.CNPDB14 = 1;
    ANSELBbits.ANSB14 = 0;

    // Bypass Switch
    TRISBbits.TRISB15 = 1;
    CNPDBbits.CNPDB15 = 1;
    ANSELBbits.ANSB15 = 0;

    // Subdiv LEDs
    // RG6 : Thirty-second note 8:1
    // RG7 : Sixteenth note 4:1
    // RG8 : Eigth note 2:1
    // RG9 : Quarter note 1:1
    TRISGCLR = 0b1111 << 6;
    LATGSET = 0b0001 << 6;
}

// Tap
unsigned long long audioCycles = 0;
int pr2 = 0;
int tapHasBeenUp = 1;
int tapFlip = 0;
int trueTap = 0;
int subTap = 0;

void checkTap() {
    // Audio cycles updates 96000 times a second
    if (TAP_SW_R && tapHasBeenUp) {
        tapHasBeenUp = 0;
        if (audioCycles > 20000) {
            pr2 = 0.00512 * audioCycles;
            audioCycles = 0;
            //
            // Formula is ((PBCLK/CLKDIV)/ (tdlen * fcodec)) * lastTap
            // PBCLK = 945000000
            // CLKDIV = 1 -> 256
            // fcodec = 96000
            // tdlen = 48000
            // At clkdiv = 1 => 0.0205 * lastTap
            //
            PR2 = pr2 / subdiv;
            LATESET = 3 << 6;
            tapFlip = 0;
        }
    } else {
        tapHasBeenUp = 1;
    }

    LATEbits.LATE6 = trueTap >= 0;
    LATEbits.LATE7 = subTap >= 0;
}

// Bypass
int bypassCount = 0;
unsigned int bounceState = 1;

void checkBypass() {
    // Bypass Routine
    if (BYPASS_SW_R) {
        if (!bypassCount && bounceState) {
            LATBINV = 1 << 12;
            bounceState = 0;
        }
        bypassCount = 50000;
    } else {
        if (!bounceState) bypassCount--;
        if (!bypassCount) bounceState = 1;
    }
}

// Subdivision
int subdivCount = 0;
unsigned int subdivState = 1;
unsigned short subdiv = 1;

void checkSubdiv() {
    if (SUBDIV_SW_R) {
        if (!subdivCount && subdivState) {
            LATGCLR = 0b1111 << 6;
            switch (subdiv) {
            case 1:
                subdiv = 2;
                LATGbits.LATG7 = 1;
                break;
            case 2:
                subdiv = 4;
                LATGbits.LATG8 = 1;
                break;
            case 4:
                subdiv = 8;
                LATGbits.LATG9 = 1;
                break;
            case 8:
                subdiv = 1;
                LATGbits.LATG6 = 1;
                break;
            }
            PR2 = pr2 / subdiv;
            subdivState = 0;
        }
        subdivCount = 50000;
    } else {
        if (!subdivState) subdivCount--;
        if (!subdivCount) subdivState = 1;
    }
}

// Potentiometer stuff
int turn = 0;
double wet = 0.0;
double dry = 0.0;

// Timer1 handler
void readPots(void) {
    switch (turn) {
    case 0:
        tapeDelay.decay = readFilteredADC(0) / 4096.0;
        turn = 1;
        break;
    case 1:
        leaky.alpha = readFilteredADC(1) / 4096.0;
        turn = 2;
        break;
    case 2:
        dry = readFilteredADC(2) / 4096.0;
        wet = 1 - dry;
        turn = 0;
        break;
    }

    // Clear interrupt
    IFS0bits.T1IF = 0;
}
