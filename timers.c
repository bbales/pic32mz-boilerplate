/*
* @Author: bbales
* @Date:   2016-03-14 17:20:57
* @Last Modified by:   bbales
* @Last Modified time: 2016-03-15 22:57:23
*/

#include <xc.h>
#include <sys/attribs.h>
#include <stdio.h>
#include "timers.h"
#include "uart.h"

// Timer 1 - Sampling
void t1Init(){
    asm volatile("di"); // Disable interrupts

    T1CONbits.ON = 0; // Disable timer 1
    T1CONbits.TCKPS = 0b00; // Input clock prescale select (1:1)
    T1CONbits.TCS = 0; // Source is internal peripheral clock

    PR1 = 521; // Timer 1 period

    TMR1 = 0; // Clear timer 1 counter

    IPC1bits.T1IP = 0b101; // Interrupt priority 5
    IPC1bits.T1IS = 0b11; // Sub-priority 3
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;

    PRISSbits.PRI6SS = 0b0110; // Shadow set 6
    INTCONbits.MVEC = 1; // Enable multi-vectored interrupts

    asm volatile("ei"); // Enable interrupts

    T1CONbits.ON = 1; // Enable timer 1
}

// Timer 2 - Controls
void t2Init(){
    asm volatile("di"); // Disable interrupts

    // Peripheral Clock appears to be 25Mhz (50Mhz/2))
    T2CONbits.ON = 0; // Disable timer 2
    T2CONbits.TCKPS = 7; // Input clock prescale select (1:256 (T2CLKIN = 31250 Hz))
    T2CONbits.TCS = 0; // Source is internal peripheral clock

    PR2 = 976; // Timer 2 period

    TMR2 = 0; // Clear timer 2 counter

    IPC2bits.T2IP = 0b101; // Interrupt priority
    IPC2bits.T2IS = 0b10; // Sub-priority 2
    IFS0bits.T2IF = 0; // Clear interrupt flag
    IEC0bits.T2IE = 1; // Enable timer 2 interrupt

    PRISSbits.PRI6SS = 0b0110; // Shadow set 6
    INTCONbits.MVEC = 1; // Enable multi-vectored interrupts

    asm volatile("ei"); // Enable interrupts

    T2CONbits.ON = 1; // Enable timer 2
}

// int a1 = 0;
// int a2 = 0;

// void __ISR_AT_VECTOR(_TIMER_2_VECTOR, IPL5SRS) T2Interrupt(void) {
   // a1 = !a1;
   // LATEbits.LATE7 = a1;
   // uartSendString("aur ");
   // uartSendChar('b');
   // IFS0bits.T2IF = 0; // Clear interrupt
// }

// void __ISR_AT_VECTOR(_TIMER_1_VECTOR, IPL5SRS) T1Interrupt(void){
   // a2= !a2;
   // LATEbits.LATE5 = !a2;
   // IFS0bits.T1IF = 0; // Clear interrupt
// }
