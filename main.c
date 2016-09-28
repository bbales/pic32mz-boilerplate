#include <xc.h>
#include <sys/attribs.h>
#include "config.h"
#include "init.h"
#include "audio.h"
#include "timers.h"
#include "uart.h"

int main(void) {
    initOscillator();
    initReferenceClocks();
    initCodec();
    // initMem();

    // RE5 is currently used in
    TRISEbits.TRISE5 = 0;
    LATEbits.LATE5 = 1;

    //  t1Init();
    //  t2Init();
    //  uartInit();

    while (1){
        LATEbits.LATE5 = !PORTEbits.RE5;
        rwCodec();
        left_output = 8;
        right_output = 8;
    }

    // Impossible!
    return 0;
}
