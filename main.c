#include <xc.h>
#include <sys/attribs.h>
#include "config.h"
#include "init.h"
#include "audio.h"
#include "delay.h"
#include "timers.h"
#include "uart.h"


int main(void) {
    initOscillator();
    codecInit();
    // initMem();

    // RE5 is currently used in
    // TRISEbits.TRISE5 = 0;
    // LATEbits.LATE5 = 1;

    TRISDbits.TRISD4 = 0;

    //  t1Init();
    //  t2Init();
    //  uartInit();

    while (1){
        // left_output = 8;
        // right_output = 8;
        delay_ms(500);
        LATDbits.LATD4 = 1;

        delay_ms(500);
        LATDbits.LATD4 = 0;
    }

    // Impossible!
    return 0;
}
