#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/attribs.h>

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

typedef struct Debouncer{
    void (*func)(void);
    char hasBounced;
    unsigned int count;
} Debouncer;

void debounce(Debouncer * d, char trigger);

// Debouncers
Debouncer tapDebounce;
Debouncer bypassDebounce;
Debouncer subdivDebounce;

// Initialize Controls
void controlsInit();

// Tap
extern unsigned long long audioCycles;
void readControls();
int trueTap;
int subTap;
char tapFlip;

// Subdivision
char subdiv;
void checkTap();
void checkBypass();
void checkSubdiv();

// Potentiometer stuff
void readPots(void);
void __ISR_AT_VECTOR(_TIMER_1_VECTOR, IPL6SRS) readPots(void);

#ifndef CONTROLS_H
#define	CONTROLS_H

#ifdef	__cplusplus
}
#endif
#endif
