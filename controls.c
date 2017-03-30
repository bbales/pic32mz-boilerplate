#include <xc.h>
// #include <stdio.h> // Required for UART stuff

#include "init.h"
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

    // Time Select Switch
    TRISBbits.TRISB11 = 1;
    CNPDBbits.CNPDB11 = 1;
    ANSELBbits.ANSB11 = 0;

    TRISBbits.TRISB10 = 1;
    CNPDBbits.CNPDB10 = 1;
    ANSELBbits.ANSB10 = 0;

    // More LEDs
    TRISECLR = 0b111 << 1;
    LATESET = 0b111 << 1;

    // Subdiv LEDs
    // RG6 : Thirty-second note 8:1
    // RG7 : Sixteenth note 4:1
    // RG8 : Eigth note 2:1
    // RG9 : Quarter note 1:1
    TRISGCLR = 0b1111 << 6;

    // Time knob averaging
    for (avgIndex = 0; avgIndex < TIME_KNOB_AVERAGE_LEN; avgIndex++) { avgBuffer[avgIndex] = 0; }
    avgIndex = 0;
}

// Tap
unsigned long long audioCycles = 0;
int pr2 = 0;
int trueTap = 0;
int subTap = 0;
char tapFlip = 0;
char tapBounce = 1;

void checkTap() {
    // Audio cycles updates 96000 times a second
    if (TAP_SW_R && tapBounce) {
        tapBounce = 0;
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
        tapBounce = 1;
    }

    LATEbits.LATE6 = trueTap >= 0;
    LATEbits.LATE7 = subTap >= 0;
}

// Bypass
unsigned int bypassCount = 0;
char bypassBounce = 1;

void checkBypass() {
    // Bypass Routine
    if (BYPASS_SW_R) {
        if (!bypassCount && bypassBounce) {
            LATBINV = BIT_12;
            bypassBounce = 0;
        }
        bypassCount = 50000;
    } else {
        if (!bypassBounce) bypassCount--;
        if (!bypassCount) bypassBounce = 1;
    }
}

// Subdivision
char subdivBounce = 1;
unsigned int subdivCount = 0;
char subdiv = 1;

short timeState = 0;

void checkSubdiv() {
    // Subdiv switch has been pressed
    if (SUBDIV_SW_R) {
        // Minimum countdown has elapsed
        // + button has returned to rest before being pressed again
        if (!subdivCount && subdivBounce) {
            // Clear all subdiv LEDs
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
            subdivBounce = 0;
        }
        subdivCount = 50000;
    } else {
        if (!subdivBounce) subdivCount--;
        if (!subdivCount) subdivBounce = 1;
    }

    // Time Switch LEDs
    timeState = TIME_SW_R1 + 2 * TIME_SW_R0;
    LATEbits.LATE1 = timeState == 2;
    LATEbits.LATE2 = timeState == 1;
    LATEbits.LATE3 = timeState == 0;
}

// Time knob averaging
unsigned short avgIndex = 0;
unsigned int avgBuffer[TIME_KNOB_AVERAGE_LEN];
unsigned int avg = 0;
unsigned int total = 0;

// Potentiometer stuff
char turn = 0;

// Timer1 handler
void readPots(void) {
    switch (turn) {
    case 0:
        tapeDelay.decay = readFilteredADC(0) / 4096.0;
        turn = 1;
        break;
    case 1:
        leaky.alpha = readFilteredADC(1) / 4096.0;
        leaky.alphaNot = 1.0 - leaky.alpha;
        turn = 2;
        break;
    case 2:
        codec.dry = readFilteredADC(2) / 4096.0;
        codec.wet = 1.0 - codec.dry;
        turn = 3;
        break;
    case 3:
        if (timeState == 2) {
            // Remove the last from the sum
            total = total - avgBuffer[avgIndex];

            // Set newest at index
            avgBuffer[avgIndex] = readFilteredADC(4) >> 2;

            // Add newest to average
            total = total + avgBuffer[avgIndex];

            // Increment Index
            avgIndex++;

            // Reset index on OOB
            if (avgIndex >= TIME_KNOB_AVERAGE_LEN) avgIndex = 0;

            // Calculate average
            avg = total / TIME_KNOB_AVERAGE_LEN;

            // Pad the average to prevent lock-up
            PR2 = avg + 80;
        }
        turn = 0;
        break;
    }

    // Clear interrupt
    IFS0bits.T1IF = 0;
}
