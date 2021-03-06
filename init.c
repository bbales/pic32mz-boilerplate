/*
* @Author: bbales
* @Date:   2016-03-08 17:42:00
* @Last Modified by:   bbales
* @Last Modified time: 2016-03-08 17:44:28
*/
#include <xc.h>

#include "init.h"

void oscInit() {
    // Removed in favor of boot config
}

void memInit() {
    // SDO3 on RB9
    TRISBbits.TRISB9 = 0;

    // SDI3 on RB10
    // Disable AN5
    // Disable TMS (JTAG))
    CVRCONbits.CVROE = 0; // Disable CVREFOUT
    SDI3R = 0b0110;
    TRISBbits.TRISB10 = 1;

    // SS3 RB8
    TRISBbits.TRISB9 = 0;
    SS3R = 0b0010;

    // SCK3 on 29

    // Enable SPI3
    SPI3CONbits.ON = 0;
    SPI3CONbits.FRMEN = 0;
    SPI3CONbits.MSSEN = 0;
    SPI3CONbits.MCLKSEL = 1;
    SPI3CONbits.SIDL = 0;
    SPI3CONbits.DISSDO = 0;
    SPI3CONbits.MODE16 = 1;
    SPI3CONbits.MODE32 = 1;
}
