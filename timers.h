#ifndef TIMERS_H
#define TIMERS_H

void t1Init();
void t1Handler();
void t2Init();
void t2Handler(void);

// Timer1 interrupt subroutine
void __ISR_AT_VECTOR(_TIMER_1_VECTOR, IPL6SRS) t1Handler(void);
void __ISR_AT_VECTOR(_TIMER_2_VECTOR, IPL1SRS) t2Handler(void);

#endif
