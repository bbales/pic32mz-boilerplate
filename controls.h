#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/attribs.h>

#define TAP_SW_R PORTBbits.RB14
#define BYPASS_SW_R PORTBbits.RB15
#define SUBDIV_SW_R PORTBbits.RB13

#define TAP_LIGHT_W LATEbits.LATE6
#define TAP_LIGHT_TRUE_W LATEbits.LATE7
#define BYPASS_W LATBbits.LATB12

// Tap
extern unsigned long long audioCycles;
void checkTap();
int trueTap;
int subTap;
int tapFlip;

// Initialize Controls
void controlsInit();

// Bypass
int bypassCount;
unsigned int bounceState;
void checkBypass();

// Subdivision
int bypassCount;
unsigned short subdiv;
unsigned int subdivState;
void checkSubdiv();

// Potentiometer stuff
int turn;
void readPots(void);
void __ISR_AT_VECTOR(_TIMER_1_VECTOR, IPL6SRS) readPots(void);

#ifndef CONTROLS_H
#define	CONTROLS_H

#ifdef	__cplusplus
}
#endif
#endif
