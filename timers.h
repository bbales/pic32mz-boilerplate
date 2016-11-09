#ifndef TIMERS_H
#define TIMERS_H

int adc1;
long mini;
long maxi;

void t1Init();
void t2Init();
void t1Handler();

// Timer1 interrupt subroutine
void __ISR_AT_VECTOR(_TIMER_1_VECTOR, IPL6SRS) t1Handler(void);

#endif
