/*
* @Author: bbales
* @Date:   2016-03-08 17:24:54
* @Last Modified by:   bbales
* @Last Modified time: 2016-04-15 20:32:33
*/

#include <xc.h>
#include <sys/attribs.h>
#include "audio.h"

void initCodec() {
    // Initialize channel samples
    left_input = 0;
    right_input = 0;
    left_output = 0;
    right_output = 0;

    // CODEC SDO (PIC SDI @ RD11)
    TRISDbits.TRISD11 = 1;
    SDI4R = 0b0011;

    // CODEC SDI (PIC SDO @ RC13)
    TRISCbits.TRISC13 = 0;
    RPC13R = 0b1000; // 0b1000 = SDO4

    // CODEC WS (PIC SS @ RD9)
    PMCONbits.ON = 0; // Disable PMCS2
    PMAENbits.PTEN = 0; // DIsable PMA15
    TRISDbits.TRISD9 = 0;
    RPD9R = 0b1000; // 0b1000 = SS4

    // CODEC SCK (PIC SCK @ RD10)
    ETHCON1bits.ON = 0; // Disable ECOL
    I2C1CONbits.ON = 0; // Disable SCL1
    TRISDbits.TRISD10 = 0; // SCK4

    // SPI4 Setup
    SPI4CONbits.ON = 0; // Turn off

    // Enable codec support
    SPI4CON2bits.AUDEN = 1;

    // Audio Protocol Mode
    //   11 = PCM/DSP
    //   10 = Right Justified
    //   01 = Left Justified
    //   00 = I2S
    SPI4CON2bits.AUDMOD = 0b00;

    // PBCLK / SPIBRG
    SPI4BRG = 0b000000001; // 64 * Fs (384x Mode)  === 6,139,089 Hz
    SPI4BUF = 0; // Clear Buffer

    SPI4STATbits.SPIROV = 0;

    SPI4CONbits.FRMEN = 0; // Framed mode disabled
    SPI4CONbits.MSSEN = 0; // Master mode SS enable
    SPI4CONbits.ENHBUF = 0; // Enhanced buffer mode 0*
    SPI4CONbits.MCLKSEL = 1; // Master Clk Select, 1 = REFCLKO1, 0 = PBCLK2
    SPI4CONbits.DISSDO = 0; // Disable SDO, 1 = SDO not used, 0 = SDO is used
    SPI4CONbits.DISSDI = 0; // Disable SDI, 1 = SDI not used, 0 = SDI is used

    // 23/16 Bit communication select bits
    //  When AUDEN = 1
    //   32 16 communication
    //   1  1  24 bit data, 32 bit fifo, 32 bit channel/64 bit frame
    //   1  0  32 bit data, 32 bit fifo, 32 bit channel/64 bit frame
    //   0  1  16 bit data, 16 bit fifo, 32 bit channel/64 bit frame
    //   0  0  16 bit data, 16 bit fifo, 16 bit channel/32 bit frame
    SPI4CONbits.MODE16 = 1; // 24 bit data
    SPI4CONbits.MODE32 = 1; // 24 bit data

    SPI4CONbits.SMP = 0; // Input sample phase, 1* = end, 0 = middle
    SPI4CONbits.CKE = 1; // Clock edge select, 1 = data changes on active->idle, 0 = data changes on idle->active
    SPI4CONbits.CKP = 0; // Clock polarity select, 1 = active is low, 0 = active is high
    SPI4CONbits.MSTEN = 1; // Master Mode Enable, 1 = master, 0 = slave
    SPI4CONbits.SRXISEL = 0b11; // Recieve buffer interrupt is generated when buffer is full (pg 310)

    SPI4CON2bits.AUDMONO = 0; // Audio is stereo

    // Sign Extend Read data from RX FIFO bit 0*
    SPI4CON2bits.SPISGNEXT = 0;

    // Ignore overflow and dont generate error events
    SPI4CON2bits.IGNROV = 1; // Ignore recieve overflow
    SPI4CON2bits.IGNTUR = 1; // Ignore transmit underrun bit
    SPI4CON2bits.SPIROVEN = 0; // Enable overglow error interrupts
    SPI4CON2bits.SPITUREN = 0; // Enable transmit underrun error interrupts
    SPI4CON2bits.FRMERREN = 0; // Enable frame error interrupts

    // Disable SPI4 Interrupts
    asm volatile("di");

    // IFS = Interrupt flags
    IFS5bits.SPI4EIF = 0; // Fault?
    IFS5bits.SPI4RXIF = 0; // Recieve
    IFS5bits.SPI4TXIF = 0; // Transmit

    // IPC - Interrupt priority and shadow register
    IPC41bits.SPI4RXIP = 7; // This line causes trouble for some reason
    IPC41bits.SPI4RXIS = 3;

    // IEC - Interrupt enable
    IEC5bits.SPI4EIE = 0;
    IEC5bits.SPI4RXIE = 1; // Enable recieve interrupts on SPI4
    IEC5bits.SPI4TXIE = 0;

    // Turn on SPI4
    SPI4CONbits.ON = 1;

    // PRISS - Priority shadow select
    PRISSbits.PRI7SS = 0b0111; // Shadow set 7
    INTCONbits.MVEC = 1; // Enable multi-vectored mode

    // Enable interrupts
    asm volatile("ei");
}

// void rwCodec(){
//     // Write to SPI4BUF
//     SPI4BUF = left_output;
//
//     while(!IFS5bits.SPI4RXIF);
//
//     // Read SPI4BUF
//     right_input = SPI4BUF;
//
//     // Clear Interrupt Flag
//     IFS5bits.SPI4RXIF = 0;
//     // Write to SPI4BUF
//     SPI4BUF = right_output;
//
//     while(!IFS5bits.SPI4RXIF);
//
//     // Read SPI4BUF
//     left_input = SPI4BUF;
//
//     // Clear Interrupt Flag
//     IFS5bits.SPI4RXIF = 0;
// }

int channel = 0;
unsigned int sinTable[48] = {100000, 113052, 125881, 138268, 149999, 160876, 170710, 179335, 186602, 192387, 196592, 199144, 200000, 199144, 196592, 192387, 186602, 179335, 170710, 160876, 149999, 138268, 125881, 113052, 100000, 86947, 74118, 61731, 49999, 39123, 29289, 20664, 13397, 7612, 3407, 855, 0, 855, 3407, 7612, 13397, 20664, 29289, 39123, 49999, 61731, 74118, 86947};
int sinCount = 0;
void __ISR_AT_VECTOR(_SPI4_RX_VECTOR,IPL7SRS) rwCodec2(void){
    if(channel){
        channel = 0;

        // Passthrough
        left_output = left_input;
        // sinCount+=1;
        // if(sinCount == 48) sinCount = 0;
        // left_output = sinTable[sinCount];
        // left_output = 0b11111111111111111111111111111111;

        // Read SPI4BUF
        left_input = SPI4BUF;

        // Convert to 32 Bit
        // if(0b100000000000000000000000 & left_input){
        //     left_input = 0b11111111000000000000000000000000 + left_input;
        // }

        // Convert back to 24 bit
        // if(0b10000000000000000000000000000000 & left_output){
        //     left_output = 0b00000000111111111111111111111111 & left_output;
        // }

        // Write to SPI4BUF
        SPI4BUF = left_output;
    }else{
        channel = 1;

        // Passthrough
        right_output = right_input;
        // right_output = sinTable[sinCount];
        // right_output = 0b01111111111111111111111111111111;

        // Read SPI4BUF
        right_input = SPI4BUF;

        // Convert to 32 Bit
        // if(0b100000000000000000000000 & right_input){
        //     right_input = 0b11111111000000000000000000000000 + right_input;
        // }

        // Convert back to 24 bit
        // if(0b10000000000000000000000000000000 & right_output){
        //     right_output = 0b00000000111111111111111111111111 & right_output;
        // }

        // Write to SPI4BUF
        SPI4BUF = right_output;
    }
    IFS5bits.SPI4RXIF = 0;
    // while(!IFS5bits.SPI4RXIF);
}

void initReferenceClocks() {
    // OSC4
    I2C5CONbits.ON = 0; // Disable SDA5
    PMCONbits.ON = 0;   // Disable PMA9

    REFO4CONbits.ON = 0;
    REFO4CONbits.ACTIVE = 0;
    REFO4CONbits.RODIV = 0b000000000000100; // Divide by 2
    REFO4CONbits.SIDL = 0;
    REFO4CONbits.OE = 1;
    REFO4CONbits.RSLP = 1;
    REFO4CONbits.ROSEL = 0b0111; // PLL
    REFO4TRIMbits.ROTRIM = 0b000100011;
    REFO4CONbits.ON = 1;

    // REFCLK FREQ = PLL / ( 2 * (RODIV + ROTRIM/512))
    TRISFbits.TRISF4 = 0;
    LATFbits.LATF4 = 0;
    RPF4R = 0b1101;

    // REFCLKO1 for ADC
    REFO1CONbits.ON = 0;
    REFO1CONbits.ACTIVE = 0;
    REFO1CONbits.OE = 1;
    REFO1CONbits.ROSEL = 0b0111; // PLL
    REFO1CONbits.RODIV = 0b000000000000100; // 8
    REFO1TRIMbits.ROTRIM = 0b000100011;
    REFO1CONbits.ON = 1;

    TRISFbits.TRISF5 = 0;
    LATFbits.LATF5 = 0;
    RPF5R = 0b1111;
}
