#include <xc.h>
// #include <stdio.h> // Required for UART stuff

#include "controls.h"
#include "adc.h"
#include "dsp.h"

void controlsInit() {
    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB12 = 0;
    TRISBbits.TRISB14 = 1;
    TRISBbits.TRISB15 = 1;
    ANSELBbits.ANSB14 = 0;
    ANSELBbits.ANSB15 = 0;
    TRISBbits.TRISB9 = 0;
    LATBbits.LATB9 = 1;
}

// Tap
unsigned long long audioCycles = 0;
unsigned long long lastTap = 0;

void checkTap() {
    // Audio cycles updates 96000 times a second
    if (PORTBbits.RB14 && audioCycles > 9600) {
        lastTap = audioCycles;
        audioCycles = 0;
        //
        // Formula is ((PBCLK/CLKDIV)/ (tdlen * fcodec)) / lastTap
        // PBCLK = 945000000
        // CLKDIV = 1 -> 256
        // fcodec = 96000
        // tdlen = 48000
        // At clkdiv = 1 => 0.0205 * lastTap
        //
        PR2 = 0.01025 * lastTap;
    }
    // At PR2 = 500, Timer 2 fires at 189kHz

    // Tap LED
    LATBbits.LATB11 = PORTBbits.RB14;
}

// Bypass
int bypassCount = 0;
unsigned int bounceState = 1;

void checkBypass() {
    // Bypass Routine
    if (PORTBbits.RB15) {
        if (!bypassCount && bounceState) {
            LATBINV = 1 << 12;
            bounceState = 0;
        }
        bypassCount = 300000;
    } else {
        if (!bounceState) bypassCount--;
        if (!bypassCount) bounceState = 1;
    }
}

// Potentiometer stuff
double adc1 = 0;
double adc2 = 0;
int turn = 0;

// Timer1 handler
void readPots(void) {
    switch (turn) {
    case 0:
        tapeDelay.decay = readFilteredADC(0) / 4096.0;
        turn = 1;
        break;
    case 1:
        // PR2 = readFilteredADC(1);
        leaky.alpha = readFilteredADC(1) / 4096.0;
        turn = 0;
        break;
    }

    // static char str[20];
    // uartClearScreen();
    // UART Example
    // sprintf(str, "Pot 2: %f", adc2);
    // uartSendString(str);
    // uartNewline();

    // Clear interrupt
    IFS0bits.T1IF = 0;
}
