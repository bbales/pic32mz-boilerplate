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
#define SUB_1_W LATBbits.LATB12
#define SUB_2_W LATBbits.LATB13
#define SUB_3_W LATBbits.LATB14
#define SUB_4_W LATBbits.LATB15

#define TIME_KNOB_AVERAGE_LEN 60

// Tap
extern unsigned long long audioCycles;
int trueTap;
int subTap;
char tapFlip;
void checkTap();

// Initialize Controls
void controlsInit();

// Bypass
void checkBypass();

// Subdivision
char subdiv;
void checkSubdiv();

// Time knob averaging
unsigned short avgIndex;
unsigned int avgBuffer[TIME_KNOB_AVERAGE_LEN];
unsigned int avg;
unsigned int total;

// Potentiometer stuff
void readPots(void);
void __ISR_AT_VECTOR(_TIMER_1_VECTOR, IPL6SRS) readPots(void);

#ifndef CONTROLS_H
#define	CONTROLS_H

#ifdef	__cplusplus
}
#endif
#endif
