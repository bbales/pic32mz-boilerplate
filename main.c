#include <xc.h>
#include <sys/attribs.h>

#include "config.h"
#include "init.h"
#include "audio.h"
#include "delay.h"
#include "dsp.h"
#include "timers.h"
#include "uart.h"
#include "adc.h"

// Bypass vars
int bypassCount = 0;
unsigned int bounceState = 1;

int main(void) {
    // Enable multi-vectored mode
    INTCONbits.MVEC = 1;

    // Initializations
    dspInit();
    codecInit();
    uartInit();
    adcInit();
    t1Init();
    t2Init();

    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB12 = 0;
    TRISBbits.TRISB9 = 0;
    TRISBbits.TRISB14 = 1;
    TRISBbits.TRISB15 = 1;
    ANSELBbits.ANSB14 = 0;
    ANSELBbits.ANSB15 = 0;
    LATBbits.LATB9 = 1;

    while (1){
        // Bypass Routine
        if(PORTBbits.RB15){
            if(!bypassCount && bounceState) {
                LATBINV = 1 << 12;
                bounceState = 0;
            }
            bypassCount = 300000;
        }else{
            if(!bounceState) bypassCount--;
            if(!bypassCount) bounceState = 1;
        }

        // Tap LED
        LATBbits.LATB11 = PORTBbits.RB14;
    }

    // Impossible!
    return 0;
}
