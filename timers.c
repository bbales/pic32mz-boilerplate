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
#include "adc.h"

// Timer1 handler
int adc1 = 0;
int flip = 0;
int res = 0;
char str[20];
void t1Handler(){
    flip = !flip;
    LATDbits.LATD4 = flip;
    IFS0bits.T1IF = 0; // Clear interrupt

    uartClearScreen();

    res = readFilteredADC(0);
    adc1 = res;
    sprintf(str, "Pot 0: %d", res);
    uartSendString(str);
    uartNewline();

    res = readADC(0);
    sprintf(str, "Pot 0: %d", res);
    uartSendString(str);
    uartNewline();

    res = readFilteredADC(1);
    sprintf(str, "Pot 1: %d", res);
    uartSendString(str);
    uartNewline();

    res = readADC(1);
    sprintf(str, "Pot 1: %d", res);
    uartSendString(str);
    uartNewline();

    res = readFilteredADC(2);
    sprintf(str, "Pot 2: %d", res);
    uartSendString(str);
    uartNewline();

    res = readADC(2);
    sprintf(str, "Pot 2: %d", res);
    uartSendString(str);
    uartNewline();

    res = readFilteredADC(3);
    sprintf(str, "Pot 3: %d", res);
    uartSendString(str);
    uartNewline();

    res = readADC(3);
    sprintf(str, "Pot 3: %d", res);
    uartSendString(str);
    uartNewline();

    res = readFilteredADC(4);
    sprintf(str, "Pot 4: %d", res);
    uartSendString(str);
    uartNewline();

    res = readADC(4);
    sprintf(str, "Pot 4: %d", res);
    uartSendString(str);
    uartNewline();
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
    // PR1 = 2000;

    // Clear timer 1 counter
    TMR1 = 0;

    IPC1bits.T1IP = 6; // Interrupt priority 5
    IPC1bits.T1IS = 3; // Sub-priority 3
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;

    // Shadow set 6
    PRISSbits.PRI6SS = 6;

    // Enable timer 1
    T1CONbits.ON = 1;

    asm volatile("ei"); // Enable interrupts
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
