#include <xc.h>

#include "config.h"
#include "init.h"
#include "controls.h"
#include "audio.h"
#include "delay.h"
#include "dsp.h"
#include "timers.h"
#include "uart.h"
#include "adc.h"

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
    controlsInit();

    while (1) {
        checkBypass();
        checkSubdiv();
        checkTap();
    }

    // Impossible!
    return 0;
}
