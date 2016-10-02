/*
* @Author: bbales
* @Date:   2016-03-14 17:20:57
* @Last Modified by:   bbales
* @Last Modified time: 2016-03-15 22:57:23
*/

#include <xc.h>
#include <sys/attribs.h>
#include <stdio.h>
#include "adc.h"

void adcInit(){
    // ADC1 does channels 0-31...

    // Clear control registers
    ADCCON1 = 0;
    ADCCON2 = 0;
    ADCCON3 = 0;
    ADCANCON = 0;
    ADCTRGSNS = 0;

    // Configure all channels as single-ended, unipolar inputs
    ADCIMCON1 = 0;
    ADCIMCON2 = 0;
    ADCIMCON3 = 0;

    // Disable all global interrupts
    ADCGIRQEN1 = 0;
    ADCGIRQEN2 = 0;

    // Disable all early interrupts
    ADCEIEN1 = 0;
    ADCEIEN2 = 0;

    // Disable digital comparators
    ADCCMPCON1 = 0;
    ADCCMPCON2 = 0;
    ADCCMPCON3 = 0;

    // Disable oversampling filters
    ADCFLTR1 = 0;
    ADCFLTR2 = 0;
    ADCFLTR3 = 0;

    // ADCCON1
    ADCCON1bits.FRACT = 0; // Fractional Data Output Format (Integer)
    ADCCON1bits.SELRES = 2; // ADC7 Resolution (10 bits)
    ADCCON1bits.STRGSRC = 1; // Scan Trigger Source Select (Global Software Trigger (GSWTRG))

    // ADCCON2
    ADCCON2bits.SAMC = 68; // ADC7 Sample Time (70 * Tad7)
    ADCCON2bits.ADCDIV = 1; // ADC7 Clock Divider (Divide by 2)

    // ADCCON3
    ADCCON3bits.ADCSEL = 0; // ADC Clock Source SYSCLK
    ADCCON3bits.CONCLKDIV = 1; // ADC Control Clock <Tq> Divider (Divide by 20)
    ADCCON3bits.VREFSEL = 0; // Voltage Reference Input Selection (AVdd, AVss)

    // Select ADC sample time and conversion clock, 10 bit res, sampling time of 5 * TADx
    ADC0TIMEbits.ADCDIV = 1;
    ADC0TIMEbits.SAMC = 5;
    ADC0TIMEbits.SELRES = 2;
    ADC1TIMEbits.ADCDIV = 1;
    ADC1TIMEbits.SAMC = 5;
    ADC1TIMEbits.SELRES = 2;
    ADC2TIMEbits.ADCDIV = 1;
    ADC2TIMEbits.SAMC = 5;
    ADC2TIMEbits.SELRES = 2;

    // Enable filter 1
    ADCFLTR1bits.DFMODE = 1; // Averaging mode
    ADCFLTR1bits.OVRSAM = 5; // 4 samples to be averaged, can try higher
    ADCFLTR1bits.AFGIEN = 1; // Interrupt enabled
    ADCFLTR1bits.AFEN = 1; // Enable the filter

    // Select analog input for ADC modules, no presync trigger, not sync sampling
    ADCTRGMODE = 0;

    ADCCSS1 = 0;
    ADCCSS2 = 0;

    // Wake-Up Clock Count (2^7 Clocks)
    ADCANCONbits.WKUPCLKCNT = 7;

    // Set up trigger sources
    ADCTRGSNS = 0;

    // Set up software triggering
    ADCTRG1bits.TRGSRC0 = 1;
    ADCTRG1bits.TRGSRC1 = 1;
    ADCTRG1bits.TRGSRC2 = 1;

    // Turn on ADC module
    ADCCON1bits.ON = 1;

    // Let Vref stabilize
    while(!ADCCON2bits.BGVRRDY);
    while(ADCCON2bits.REFFLT);

    // Enable clk to analog
    ADCANCONbits.ANEN0 = 1;
    ADCANCONbits.ANEN1 = 1;
    ADCANCONbits.ANEN2 = 1;

    // Wait for ADC ready
    while(!ADCANCONbits.WKRDY0); // Wait until ADC0 is ready
    while(!ADCANCONbits.WKRDY1); // Wait until ADC1 is ready
    while(!ADCANCONbits.WKRDY2); // Wait until ADC2 is ready

    // Enable ADC
    ADCCON3bits.DIGEN0 = 1; // Enable ADC0
    ADCCON3bits.DIGEN1 = 1; // Enable ADC1
    ADCCON3bits.DIGEN2 = 1; // Enable ADC2
}

int getDataADC(int achannel){
    switch(achannel){
        case 0: return ADCDATA0;
        case 1: return ADCDATA1;
        case 2: return ADCDATA2;
    }
}

int readADC(int achannel){
    ADCCON3bits.GSWTRG = 1;
    while(((ADCDSTAT1 >> achannel) | 1) == 0);
    return getDataADC(achannel);
}

int readFilteredADC(int achannel){
    ADCFLTR1bits.CHNLID = achannel; // Select channel 1
    ADCFLTR1bits.AFEN = 1; // Enable the filter
    ADCCON3bits.ADINSEL = achannel; // Channel to manually trigger
    ADCCON3bits.RQCNVRT = 1; // Manually trigger selected channel

    while(ADCFLTR1bits.AFRDY == 0); // Wait until oversampling is done
    ADCFLTR1bits.AFEN = 0; // Enable the digital filtering

    return ADCFLTR1bits.FLTRDATA;
}
