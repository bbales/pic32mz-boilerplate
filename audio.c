/*
* @Author: bbales
* @Date:   2016-03-08 17:24:54
* @Last Modified by:   bbales
* @Last Modified time: 2016-04-15 20:32:33
*/
#include <xc.h>

#include "audio.h"
#include "delay.h"
#include "dsp.h"
#include "controls.h"

void codecRW() {
    if (codec.channel == CHANNEL_B) {
        // Processing
        codec.leftOut = dry * codec.leftIn + wet * leaky.func(tapeDelay.func(codec.leftIn));

        // Read SPI4BUF
        codec.leftIn = SPI4BUF;

        // Write to SPI4BUF
        SPI4BUF = codec.leftOut;

        // Switch channels
        codec.channel = 0;
    } else {
        // Processing
        codec.rightOut = 0;

        // Read SPI4BUF
        codec.rightIn = SPI4BUF;

        // Write to SPI4BUF
        SPI4BUF = codec.rightOut;

        // Switch channels
        codec.channel = 1;
    }

    audioCycles++;

    // Clear interrupt flag
    IFS5bits.SPI4RXIF = 0;
}

void codecEnable(char enable) {
    // RST pin on CS4272
    TRISCbits.TRISC14 = OUTPUT;
    delay_ms(3);
    LATCbits.LATC14 = enable;
}

void codecInit() {
    // Initialize struct
    codec = (CS4272){.leftIn = 0,
                     .leftOut = 0,
                     .rightIn = 0,
                     .rightOut = 0,
                     .channel = 0,
                     .enable = codecEnable,
                     .rw = codecRW};

    // Disable codec
    codec.enable(0);

    // Setup reference clocks for MCLK
    codecInitMCLK();

    // Initialize channel samples
    codec.leftIn = codec.rightIn = codec.leftOut = codec.rightOut = 0;

    // Overlapping modules
    PMCONbits.ON = 0;   // Disable PMCS2
    PMAENbits.PTEN = 0; // DIsable PMA15
    ETHCON1bits.ON = 0; // Disable ECOL
    I2C1CONbits.ON = 0; // Disable SCL1
    I2C5CONbits.ON = 0; // Disable SDA5
    PMCONbits.ON = 0;   // Disable PMA9

    //
    // Set up pin IO
    //

    // CODEC SDO (PIC SDI @ RD11)
    TRISDbits.TRISD11 = INPUT;
    SDI4R = 0b0011;

    // CODEC SDI (PIC SDO @ RD0) <-- Move to RD0 on next board
    // TRISDbits.TRISD0 = OUTPUT;
    // RPD0R = 0b1000;
    TRISDbits.TRISD5 = OUTPUT;
    RPD5R = 0b1000;
    // TRISDbits.TRISD0 = OUTPUT;
    // RPD0R = 0b1000;

    // CODEC WS (PIC SS @ RD9)
    TRISDbits.TRISD9 = OUTPUT;
    RPD9R = 0b1000;

    // CODEC SCK (PIC SCK @ RD10)
    TRISDbits.TRISD10 = OUTPUT;

    // CODEC MCLK Output
    TRISFbits.TRISF5 = OUTPUT;
    RPF5R = 0b1111;

    //
    // Set up SPI4 for I2S
    //

    // Baud Rate - PBCLK / SPIBRG (64 * Fs (384x Mode)  === 6,139,089 Hz)
    SPI4BRG = 0b000000001;

    SPI4BUF = 0;                // Clear Buffer
    SPI4CONbits.ON = 0;         // Turn off SPI4 for config
    SPI4CON2bits.AUDEN = 1;     // Enable codec support
    SPI4CON2bits.AUDMOD = 0b00; // Audio Protocol Mode (I2S)

    // Important config
    SPI4CONbits.FRMEN = 0;   // Framed mode disabled
    SPI4CONbits.MSSEN = 0;   // Master mode SS enable
    SPI4CONbits.ENHBUF = 0;  // Enhanced buffer mode 0*
    SPI4CONbits.MCLKSEL = 1; // Master Clk Select, 1 = REFCLKO1, 0 = PBCLK2
    SPI4CONbits.DISSDO = 0;  // Disable SDO, 1 = SDO not used, 0 = SDO is used
    SPI4CONbits.DISSDI = 0;  // Disable SDI, 1 = SDI not used, 0 = SDI is used
    SPI4CONbits.SMP = 0;     // Input sample phase, 1* = end, 0 = middle
    SPI4CONbits.CKE = 1;     // Clock edge select
                             // 1 = data changes on active->idle, 0 = data changes on idle->active
    SPI4CONbits.CKP = 1;     // Clock polarity select, 1 = active is low, 0 = active is high
    SPI4CONbits.MSTEN = 1;   // Master Mode Enable, 1 = master, 0 = slave
    SPI4CONbits.SRXISEL = 0b11; // Recieve buffer interrupt is generated when
                                // buffer is full (pg 310)

    // Configure for 24-bit data in 32-bit frames
    SPI4CONbits.MODE16 = 1;
    SPI4CONbits.MODE32 = 1;

    // Sign Extend Read data from RX FIFO bit 0 (This may be useful)
    SPI4CON2bits.SPISGNEXT = 1;

    // Ignore overflow and dont generate error events
    SPI4CON2bits.IGNROV = 1;   // Ignore recieve overflow
    SPI4CON2bits.SPIROVEN = 0; // Enable overglow error interruptsN2bits.IGNTUR = 1;
                               // Ignore transmit underrun bit
    SPI4CON2bits.SPITUREN = 0; // Enable transmit underrun error interrupts
    SPI4CON2bits.FRMERREN = 0; // Enable frame error interrupts
    SPI4STATbits.SPIROV = 0;   // Reset overflow stat

    //
    // Configure SPI4 interrupts
    //

    // Disable SPI4 Interrupts
    asm volatile("di");

    // IFS = Interrupt flags
    IFS5bits.SPI4EIF = 0;  // Fault?
    IFS5bits.SPI4RXIF = 0; // Recieve
    IFS5bits.SPI4TXIF = 0; // Transmit

    // IPC - Interrupt priority and shadow register
    IPC41bits.SPI4RXIP = 7; // This line causes trouble for some reason
    IPC41bits.SPI4RXIS = 2;

    // IEC - Interrupt enable
    IEC5bits.SPI4EIE = 0;
    IEC5bits.SPI4RXIE = 1; // Enable recieve interrupts on SPI4
    IEC5bits.SPI4TXIE = 0;

    // Turn on SPI4
    SPI4CONbits.ON = 1;

    // PRISS - Priority shadow select
    PRISSbits.PRI7SS = 0b0111; // Shadow set 7

    // Enable interrupts
    asm volatile("ei");

    // Enable codec
    codec.enable(1);
}

void codecInitMCLK() {
    // REFCLKO1 for ADC
    REFO1CONbits.ON = 0;
    REFO1CONbits.ACTIVE = 0;
    REFO1CONbits.OE = 1;
    REFO1CONbits.ROSEL = 0b0111;            // PLL
    REFO1CONbits.RODIV = 0b000000000001000; // 8
    REFO1TRIMbits.ROTRIM = 0b000100011;
    REFO1CONbits.ON = 1;
}
