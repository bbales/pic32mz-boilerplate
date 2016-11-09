#include <xc.h>
#include <sys/attribs.h>

#include "config.h"
#include "init.h"
#include "audio.h"
#include "delay.h"
#include "timers.h"
#include "uart.h"
#include "adc.h"
#include "dsp.h"

int main(void) {
    // Enable multi-vectored mode
    INTCONbits.MVEC = 1;

    // Initializations
    dspInit();
    codecInit();
    uartInit();
    adcInit();
    t1Init();
    // t2Init();

    // Currently used for blink
    TRISDbits.TRISD4 = 0;

    while (1){
        delay_ms(500);
        LATDbits.LATD4 = 1;
        delay_ms(500);
        LATDbits.LATD4 = 0;
    }

    // Impossible!
    return 0;
}
