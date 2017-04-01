#include <xc.h>
// #include <stdio.h> // Required for UART stuff

#include "init.h"
#include "controls.h"
#include "adc.h"
#include "audio.h"
#include "dsp.h"

void controlsInit() {
    // Tap LED
    TRISCbits.TRISC15 = 0;

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

    // Time LEDs
    // TRISECLR = 0b111 << 1;
    // LATESET = 0b111 << 1;

    // Pots
    TRISBSET = 0b11111;
    ANSELBSET = 0b11111;

    CM2CONbits.ON = 0;
    RPD2R = 0;
    CM1CONbits.ON = 0;
    RPD4R = 0;

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
            // Calculate non-subdivided period and clear cycles
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

            // Set timer 2 period to reflect tap
            PR2 = pr2 / subdiv;

            // Turn both tap LEDs
            TAP_LIGHT_TRUE_W = 1;
            SUB_1_W = 1;
            SUB_2_W = 1;
            SUB_3_W = 1;
            SUB_4_W = 1;

            // Clear tapFlip flag
            tapFlip = 0;
        }
    } else {
        tapBounce = 1;
    }

    // Control Tap LEDs
    TAP_LIGHT_TRUE_W = trueTap >= 0;
    SUB_1_W = subdiv == 1 && subTap >= 0;
    SUB_2_W = subdiv == 2 && subTap >= 0;
    SUB_3_W = subdiv == 4 && subTap >= 0;
    SUB_4_W = subdiv == 8 && subTap >= 0;
}

// Bypass
unsigned int bypassCount = 0;
char bypassBounce = 1;

void checkBypass() {
    // Bypass Routine
    if (BYPASS_SW_R) {
        if (!bypassCount && bypassBounce) {
            LATEINV = BIT_5;
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
            if (subdiv == 8) {
                subdiv = 1;
            } else {
                subdiv *= 2;
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
    // timeState = TIME_SW_R1 + 2 * TIME_SW_R0;
    // LATEbits.LATE1 = timeState == 2;
    // LATEbits.LATE2 = timeState == 1;
    // LATEbits.LATE3 = timeState == 0;
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
        // if (timeState == 2) {
        //     // Remove the last from the sum
        //     total = total - avgBuffer[avgIndex];
        //
        //     // Set newest at index
        //     avgBuffer[avgIndex] = readFilteredADC(4) >> 2;
        //
        //     // Add newest to average
        //     total = total + avgBuffer[avgIndex];
        //
        //     // Increment Index
        //     avgIndex++;
        //
        //     // Reset index on OOB
        //     if (avgIndex >= TIME_KNOB_AVERAGE_LEN) avgIndex = 0;
        //
        //     // Calculate average
        //     avg = total / TIME_KNOB_AVERAGE_LEN;
        //
        //     // Pad the average to prevent lock-up
        //     PR2 = avg + 80;
        // }
        turn = 0;
        break;
    }

    // Clear interrupt
    IFS0bits.T1IF = 0;
}
