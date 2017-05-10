/*
* @Author: bbales
* @Date:   2016-03-14 17:20:57
* @Last Modified by:   bbales
* @Last Modified time: 2016-03-15 22:57:23
*/
#include <xc.h>

#include "timers.h"

//
// Timer 1 - Sampling
//

void t1Init() {
    asm volatile("di"); // Disable interrupts

    T1CONbits.ON = 0;    // Disable timer 1
    T1CONbits.SIDL = 0;  // Disable stop in idle mode
    T1CONbits.TWDIS = 1; // Async timer Write
    T1CONbits.TCKPS = 3; // Input clock prescale select (1:1)
    T1CONbits.TCS = 0;   // Source is internal periph bus clock

    PR1 = 13000; // Timer 1 period
    // PR1 = 13000; // Timer 1 period
    TMR1 = 0; // Clear timer 1 counter

    IPC1bits.T1IP = 6; // Interrupt priority 6
    IPC1bits.T1IS = 3; // Sub-priority 3
    IFS0bits.T1IF = 0; // Clear interrupt flag
    IEC0bits.T1IE = 1; // Interrupt enable

    PRISS = 0x76543210; // Shadow set

    T1CONbits.ON = 1; // Enable timer 1

    asm volatile("ei"); // Enable interrupts
}

//
// Timer 2 - Tape Delay
//

void t2Init() {
    asm volatile("DI"); // Disable interrupts

    T2CON = 0;           // Clear Timer 2 config
    T2CONbits.ON = 0;    // Disable timer 2
    T2CONbits.TCKPS = 2; // Input clock prescale select (1:256 (T2CLKIN = 31250 Hz))
    T2CONbits.TCS = 0;   // Source is internal peripheral clock
    T2CONbits.T32 = 0;   // 32 bit

    PR2 = 500; // Timer 2 period
    TMR2 = 0;  // Clear timer 2 counter

    IPC2bits.T2IP = 7; // Interrupt priority
    IPC2bits.T2IS = 1; // Sub-priority 2
    IFS0bits.T2IF = 0; // Clear interrupt flag
    IEC0bits.T2IE = 1; // Enable timer 2 interrupt

    T2CONbits.ON = 1; // Enable timer 2

    asm volatile("EI"); // Enable interrupts
}

//
// Timer 3 - Tap Light
//

void t3Init() {
    asm volatile("DI"); // Disable interrupts

    T3CON = 0;           // Clear Timer 3 config
    T3CONbits.ON = 0;    // Disable timer 3
    T3CONbits.TCKPS = 2; // Input clock prescale select (1:256 (T3CLKIN = 31250 Hz))
    T3CONbits.TCS = 0;   // Source is internal peripheral clock

    PR3 = 64000; // Timer 3 period
    TMR3 = 0;    // Clear timer 3 counter

    IPC3bits.T3IP = 5; // Interrupt priority
    IPC3bits.T3IS = 1; // Sub-priority 3
    IFS0bits.T3IF = 0; // Clear interrupt flag
    IEC0bits.T3IE = 1; // Enable timer 3 interrupt

    T3CONbits.ON = 1; // Enable timer 3

    asm volatile("EI"); // Enable interrupts
}
