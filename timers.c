/*
* @Author: bbales
* @Date:   2016-03-14 17:20:57
* @Last Modified by:   bbales
* @Last Modified time: 2016-03-15 22:57:23
*/

#include <xc.h>
#include <sys/attribs.h>
#include <dsplib_def.h>
#include <stdio.h>
#include "timers.h"
#include "uart.h"
#include "adc.h"

// Timer1 handler
int res = 0;
char str[20];
int turn = 0;

void t1Handler(){
    // Clear interrupt
    IFS0bits.T1IF = 0;

    switch(turn){
        case 0:
            // uartClearScreen();
            adc1 = readFilteredADC(0)/4096.0;
            // sprintf(str, "Pot 1: %f", adc1);
            turn = 1;
            break;
        case 1:
            adc2 = readFilteredADC(1)/4096.0;
            // sprintf(str, "Pot 2: %f", adc2);
            turn = 0;
            break;
    }

    // uartSendString(str);
    // uartNewline();


    return;
}

// Timer 1 - Sampling
void t1Init(){
    asm volatile("di"); // Disable interrupts

    // Disable timer 1
    T1CONbits.ON = 0;

    // Disable stop in idle mode
    T1CONbits.SIDL = 0;

    // Async timer Write
    T1CONbits.TWDIS = 1;

    // Input clock prescale select (1:1)
    T1CONbits.TCKPS = 3;

    // Source is internal peripheral bus clock
    T1CONbits.TCS = 0;

    // Timer 1 period
    PR1 = 20000;

    // Clear timer 1 counter
    TMR1 = 0;

    IPC1bits.T1IP = 6; // Interrupt priority 6
    IPC1bits.T1IS = 3; // Sub-priority 3
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;

    // Shadow set 6
    // PRISSbits.PRI6SS = 6;
    PRISS=0x76543210;

    // Enable timer 1
    T1CONbits.ON = 1;

    asm volatile("ei"); // Enable interrupts
}

void t2Handler(void){
    IFS0bits.T2IF = 0; // Clear interrupt flag
    // LATBbits.LATB9 = 0;
    LATBINV = 1 << 9;
}

// Timer 2 - Controls
void t2Init(){
    asm volatile("DI"); // Disable interrupts

    // Peripheral Clock appears to be 25Mhz (50Mhz/2))
    T2CON = 0;
    T2CONbits.ON = 0; // Disable timer 2
    T2CONbits.TCKPS = 3; // Input clock prescale select (1:256 (T2CLKIN = 31250 Hz))
    T2CONbits.TCS = 0; // Source is internal peripheral clock
    T2CONbits.T32 = 0; // Source is internal peripheral clock

    TMR2 = 0; // Clear timer 2 counter
    PR2 = 1000; // Timer 2 period

    IPC2bits.T2IP = 1; // Interrupt priority
    IPC2bits.T2IS = 1; // Sub-priority 2
    IFS0bits.T2IF = 0; // Clear interrupt flag
    IEC0bits.T2IE = 1; // Enable timer 2 interrupt

    asm volatile("EI"); // Enable interrupts
    T2CONbits.ON = 1; // Enable timer 2
    INTCONbits.MVEC = 1;
}
