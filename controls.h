#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/attribs.h>

#define PCLK_FREQ 94500000

// Inputs
/* These are configured as pull-down, hence the (!) */
#define TAP_SW_R !PORTEbits.RE7
#define BYPASS_SW_R !PORTEbits.RE6
#define SUBDIV_SW_R !PORTCbits.RC12
#define SW1 !PORTEbits.RE1 + 2*!PORTEbits.RE2
#define SW2 !PORTEbits.RE3 + 2*!PORTEbits.RE4

// Outputs
#define TAP_LIGHT_TRUE_W LATCbits.LATC15
#define BYPASS_W LATEbits.LATE5
#define BYPASS_FLIP LATEINV = BIT_5
#define RELAY_FLIP LATBINV = BIT_8
#define SUB_1_W LATBbits.LATB12
#define SUB_2_W LATBbits.LATB13
#define SUB_3_W LATBbits.LATB14
#define SUB_4_W LATBbits.LATB15
//
// Initialize Controls
//

void controlsInit();

//
// Debounce
//

typedef struct Debouncer{
    void (*func)(void);
    char hasBounced;
    unsigned int count;
    unsigned int max;
} Debouncer;

void debounce(Debouncer * d, char trigger);

Debouncer tapDebounce;
Debouncer bypassDebounce;
Debouncer subdivDebounce;

//
// Tap
//

void readControls();

typedef struct Tap {
    unsigned long long audioCycles, sum;
    unsigned int true, sub, avg;
    double period;
    char flip, subdiv, state;
} Tap;

Tap tap;

void checkTap();
void checkBypass();
void checkSubdiv();

//
// Potentiometers
//

void readPots(void);
void __ISR_AT_VECTOR(_TIMER_1_VECTOR, IPL6SRS) readPots(void);

#ifndef CONTROLS_H
#define	CONTROLS_H

#ifdef	__cplusplus
}
#endif
#endif
