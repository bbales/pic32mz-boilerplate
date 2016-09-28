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
    asm volatile("di");

    T1CONbits.ON = 0;
    T1CONbits.TCKPS = 0b00;
    T1CONbits.TCS = 0;
    PR1 = 521;
    TMR1 = 0;

    IPC1bits.T1IP = 5; // Priority 6
    IPC1bits.T1IS = 0b11; // Sub-priority 3
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
        
    PRISSbits.PRI6SS = 0b0110;
    INTCONbits.MVEC = 1;
    asm volatile("ei");
    T1CONbits.ON = 1;
}

// Timer 2 - Controls
void t2Init(){
    asm volatile("di");
    // Peripheral Clock appears to be 25Mhz (50Mhz/2))
    // This timer is currently running at 100 Hz
    T2CONbits.ON = 0;
    T2CONbits.TCKPS = 7; /* pre-scale = 1:256 (T2CLKIN = 31250 Hz) */
    T2CONbits.TCS = 0;
    // PR2 = 976;
    PR2 = 0xFFFF;
    // PR2 = 2500;
    TMR2 = 0;

    IPC2bits.T2IP = 5;
    IPC2bits.T2IS = 0b10; // Sub-priority 2
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;

    PRISSbits.PRI6SS = 0b0110;
    INTCONbits.MVEC = 1;
    asm volatile("ei");

    T2CONbits.ON = 1; 
}

int a1 = 0;
int a2 = 0;
//void __ISR_AT_VECTOR(_TIMER_2_VECTOR, IPL5SRS) T2Interrupt(void) {
//    // a1 = !a1;
//    // LATEbits.LATE7 = a1;
////    uartSendString("aur ");
//    // uartSendChar('b');
//    IFS0bits.T2IF = 0; // Clear
//}

//void __ISR_AT_VECTOR(_TIMER_1_VECTOR, IPL5SRS) T1Interrupt(void){
//    a2= !a2;
//    LATEbits.LATE5 = !a2;
//    IFS0bits.T1IF = 0; // Clear
//}