#include <xc.h>
#include <sys/attribs.h>

#include "config.h"
#include "init.h"
#include "audio.h"
#include "delay.h"
#include "timers.h"
#include "uart.h"
#include "adc.h"

int main(void) {
    oscInit();
    codecInit();
    uartInit();
    adcInit();

    // Currently used for blink
    TRISDbits.TRISD4 = 0;

    t1Init();
    // t2Init();

    while (1){
        left_output = 8;
        right_output = 8;
        delay_ms(500);
        LATDbits.LATD4 = 1;

        delay_ms(500);
        LATDbits.LATD4 = 0;
    }

    // Impossible!
    return 0;
}
