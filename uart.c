/*
* @Author: bbales
* @Date:   2016-03-15 15:07:20
* @Last Modified by:   bbales
* @Last Modified time: 2016-03-15 23:05:26
*/

#include <xc.h>
#include <sys/attribs.h>  
#include <proc/p32mz2048efg064.h>
#include "uart.h"


void adcInit(){

}

void uartInit() {
    asm volatile("di"); // Disable all interrupts
    // asm volatile("ehb"); // Disable all interrupts
    U1MODEbits.ON = 0; //! disable UART
    TRISFbits.TRISF0 = 0;
    RPF0Rbits.RPF0R = 0b0001; // U1TX

    IPC28bits.U1TXIP = 0b000; //! Interrupt priority of 7
    IPC28bits.U1TXIS = 0b00; //! Interrupt sub-priority of 0
    IFS3bits.U1TXIF = 0; //! Clear Tx flag
    IFS3bits.U1RXIF = 0; //! Clear Rx flag
    U1BRG = 26; // 38.4k
    U1STA = 0;
    U1MODE = 0x8000; // Enable UART for 8-bit data
    // No Parity, 1 Stop bit
    U1STASET = 0x9400; // Enable Transmit and Receive
    
    asm volatile("ei");
}

void uartSendChar(char a){
    IFS3bits.U1TXIF = 0;
    // U1TXREG = 0b000000001;
    U1TXREG = a;
    while(!IFS3bits.U1TXIF);
}

void uartSendString(char * s){
    int i = 0;
    while(s[i] != '\0'){
        uartSendChar(s[i]);
        i++;
    }
}