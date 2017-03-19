#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/attribs.h>

// Tap
extern unsigned long long audioCycles;
extern unsigned long long lastTap;
void checkTap();

// Initialize Controls
void controlsInit();

// Bypass
int bypassCount;
unsigned int bounceState;
void checkBypass();

// Potentiometer stuff
extern double adc1;
extern double adc2;
int turn;
void readPots(void);
void __ISR_AT_VECTOR(_TIMER_1_VECTOR, IPL5SRS) readPots(void);

#ifndef CONTROLS_H
#define	CONTROLS_H

#ifdef	__cplusplus
}
#endif
#endif
