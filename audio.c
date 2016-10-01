/*
* @Author: bbales
* @Date:   2016-03-08 17:24:54
* @Last Modified by:   bbales
* @Last Modified time: 2016-04-15 20:32:33
*/

#include <xc.h>
#include <sys/attribs.h>
#include "audio.h"
#include "delay.h"

void codecEnable(int enable){
    TRISCbits.TRISC14 = 0;
    if(enable){
        delay_ms(3);
        LATCbits.LATC14 = 1;
    }else{
        LATCbits.LATC14 = 0;
    }
}

void codecInit() {
    // Disable codec
    codecEnable(0);

    // Setup reference clocks for MCLK
    initReferenceClocks();

    // Initialize channel samples
    left_input = 0;
    right_input = 0;
    left_output = 0;
    right_output = 0;

    // CODEC SDO (PIC SDI @ RD11)
    TRISDbits.TRISD11 = 1; // Input
    SDI4R = 0b0011;

    // CODEC SDI (PIC SDO @ RC13)
    TRISBbits.TRISB3 = 0; // Output
    RPB3R = 0b1000; // 0b1000 = SDO4

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
    SPI4CONbits.CKP = 1; // Clock polarity select, 1 = active is low, 0 = active is high
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

    // Enable codec
    codecEnable(1);
}

// var sinTable = []; for(var i = 0; i < 48; i++) sinTable[i] = Math.floor(Math.sin((i/48)*2*Math.PI)*2147483647)
unsigned int sinTable[48] = {0, 280302863, 555809666, 821806412, 1073741823, 1307305213, 1518500249, 1703713324, 1859775392, 1984016187, 2074309916, 2129111626, 2147483647, 2129111626, 2074309916, 1984016187, 1859775392, 1703713324, 1518500249, 1307305213, 1073741823, 821806412, 555809666, 280302863, 0, -280302864, -555809667, -821806413, -1073741824, -1307305214, -1518500250, -1703713325, -1859775393, -1984016188, -2074309917, -2129111627, -2147483647, -2129111627, -2074309917, -1984016188, -1859775393, -1703713325, -1518500250, -1307305214, -1073741824, -821806413, -555809667, -280302864};
int sinCount = 0;
void __ISR_AT_VECTOR(_SPI4_RX_VECTOR,IPL7SRS) rwCodec2(void){
    if(channel){
        // Passthrough
        // left_output = left_input;
        sinCount+=1;
        if(sinCount == 48) sinCount = 0;
        left_output = sinTable[sinCount] >> 8;

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
        // Passthrough
        // right_output = right_input;
        right_output = sinTable[sinCount] >> 8;

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

    // Switch channels
    channel = !channel;

    // Clear interrupt flag
    IFS5bits.SPI4RXIF = 0;
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
