/*
* @Author: bbales
* @Date:   2016-03-15 15:07:20
* @Last Modified by:   bbales
* @Last Modified time: 2016-03-15 23:05:26
*/

#include <xc.h>

#include "uart.h"

void uartInit() {
    // Baud = 115200 8N1

    // Disable all interrupts
    asm volatile("di");

    // Disable UART for config
    U1MODEbits.ON = 0;

    // U1TX
    TRISFbits.TRISF0 = 0;
    RPF0Rbits.RPF0R = 0b0001;

    // U1RX
    TRISFbits.TRISF1 = 1;
    U1RXR = 0b0100;

    // Configure UART interrupts
    IPC28bits.U1TXIP = 0b000; //! Interrupt priority of 0
    IPC28bits.U1TXIS = 0b00;  //! Interrupt sub-priority of 0
    IFS3bits.U1TXIF = 0;      //! Clear Tx flag
    IFS3bits.U1RXIF = 0;      //! Clear Rx flag

    // Baud rate generator
    U1BRG = 52;
    U1STA = 0;

    // Enable UART for 8-bit data
    U1MODE = 0x8000;

    // No Parity, 1 Stop bit
    U1STASET = 0x9400; // Enable Transmit and Receive

    // Enable all interrupts
    asm volatile("ei");
}

void uartSendChar(char a) {
    IFS3bits.U1TXIF = 0;
    U1TXREG = a;
    while (!IFS3bits.U1TXIF) continue;
}

void uartSendString(char *s) {
    int i = 0;
    while (s[i] != '\0') uartSendChar(s[i++]);
}
