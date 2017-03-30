#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/attribs.h>

#define TAP_SW_R PORTBbits.RB14
#define BYPASS_SW_R PORTBbits.RB15
#define SUBDIV_SW_R PORTBbits.RB13
#define TIME_SW_R1 PORTBbits.RB11
#define TIME_SW_R0 PORTBbits.RB10

#define TAP_LIGHT_W LATEbits.LATE6
#define TAP_LIGHT_TRUE_W LATEbits.LATE7
#define BYPASS_W LATBbits.LATB12

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
