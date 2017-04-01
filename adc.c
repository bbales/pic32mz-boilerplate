/*
* @Author: bbales
* @Date:   2016-03-14 17:20:57
* @Last Modified by:   bbales
* @Last Modified time: 2016-03-15 22:57:23
*/

#include <xc.h>

#include "adc.h"

void adcInit() {
    // ADC1 controls channel 0-31 on EFG064 devices...

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
    ADCCON1bits.FRACT = 0;   // Fractional Data Output Format (Integer)
    ADCCON1bits.SELRES = 2;  // ADC7 Resolution (10 bits)
    ADCCON1bits.STRGSRC = 1; // Scan Trig Source Select (Global Software Trigger (GSWTRG))

    // ADCCON2
    // ADCCON2bits.SAMC = 68;  // ADC7 Sample Time (70 * Tad7)
    ADCCON2bits.SAMC = 1;   // ADC7 Sample Time (70 * Tad7)
    ADCCON2bits.ADCDIV = 1; // ADC7 Clock Divider (Divide by 2)

    // ADCCON3
    ADCCON3bits.ADCSEL = 0;    // ADC Clock Source (0 : Tclk, 1: SYSCLK)
    ADCCON3bits.CONCLKDIV = 1; // ADC Control Clock <Tq> Divider (Divide by 20)
    ADCCON3bits.VREFSEL = 0;   // Voltage Reference Input Selection (AVdd, AVss)

    // Select ADC sample time and conversion clock, 10 bit res, sampling time of
    // 5 * TADx
    ADC0TIMEbits.ADCDIV = 1;
    ADC1TIMEbits.ADCDIV = 1;
    ADC2TIMEbits.ADCDIV = 1;
    ADC3TIMEbits.ADCDIV = 1;
    ADC4TIMEbits.ADCDIV = 1;

    ADC0TIMEbits.SAMC = 1;
    ADC1TIMEbits.SAMC = 1;
    ADC2TIMEbits.SAMC = 1;
    ADC3TIMEbits.SAMC = 1;
    ADC4TIMEbits.SAMC = 1;

    ADC0TIMEbits.SELRES = 2;
    ADC1TIMEbits.SELRES = 2;
    ADC2TIMEbits.SELRES = 2;
    ADC3TIMEbits.SELRES = 2;
    ADC4TIMEbits.SELRES = 2;

    // Enable filter 1
    ADCFLTR1bits.DFMODE = 1; // Averaging mode
    ADCFLTR1bits.OVRSAM = 5; // 4 samples to be averaged, can try higher
    ADCFLTR1bits.AFGIEN = 1; // Interrupt enabled
    ADCFLTR1bits.AFEN = 1;   // Enable the filter

    // Select analog input for ADC modules, no presync trigger, not sync
    // sampling
    ADCTRGMODE = 0;

    // Common scan select skip for input scan
    ADCCSS1 = 0;
    ADCCSS2 = 0;

    // Wake-Up Clock Count (2^7 Clocks)
    ADCANCONbits.WKUPCLKCNT = 7;

    // Set up trigger sources
    ADCTRGSNS = 0;

    // Set up software triggering - need this for each desired channel
    ADCTRG1bits.TRGSRC0 = 1;
    ADCTRG1bits.TRGSRC1 = 1;
    ADCTRG1bits.TRGSRC2 = 1;
    ADCTRG1bits.TRGSRC3 = 1;
    ADCTRG2bits.TRGSRC4 = 1;
    ADCTRG2bits.TRGSRC5 = 1;
    ADCTRG2bits.TRGSRC6 = 1;
    ADCTRG2bits.TRGSRC7 = 1;
    ADCTRG3bits.TRGSRC8 = 1;
    ADCTRG3bits.TRGSRC9 = 1;
    ADCTRG3bits.TRGSRC10 = 1;
    ADCTRG3bits.TRGSRC11 = 1;

    // Turn on ADC module
    ADCCON1bits.ON = 1;

    // Let Vref stabilize
    while (!ADCCON2bits.BGVRRDY) continue;
    while (ADCCON2bits.REFFLT) continue;

    // Enable clk to analog - need this for each desired channel
    ADCANCONbits.ANEN0 = 1;
    ADCANCONbits.ANEN1 = 1;
    ADCANCONbits.ANEN2 = 1;
    ADCANCONbits.ANEN3 = 1;
    ADCANCONbits.ANEN4 = 1;
    ADCANCONbits.ANEN7 = 1;

    // Wait for ADC ready
    while (!ADCANCONbits.WKRDY0) continue;
    while (!ADCANCONbits.WKRDY1) continue;

    // Enable ADC - need this for each desired channel
    ADCCON3bits.DIGEN0 = 1;
    ADCCON3bits.DIGEN1 = 1;
    ADCCON3bits.DIGEN2 = 1;
    ADCCON3bits.DIGEN3 = 1;
    ADCCON3bits.DIGEN4 = 1;
    ADCCON3bits.DIGEN7 = 1;
}

int readADC(unsigned int achannel) {
    ADCCON3bits.GSWTRG = 1;
    while (((ADCDSTAT1 >> achannel) | 1) == 0) continue;
    return *(&ADCDATA0 + achannel);
}

int readADCAlt(unsigned int achannel) {
    ADCCON3bits.ADINSEL = achannel; // Channel to manually trigger
    ADCCON3bits.RQCNVRT = 1;        // Manually trigger selected channel
    while (((ADCDSTAT1 >> achannel) | 1) == 0) continue;
    return *(&ADCDATA0 + achannel);
}

int readFilteredADC(unsigned int achannel) {
    ADCFLTR1bits.CHNLID = achannel;           // Select channel 1
    ADCFLTR1bits.AFEN = 1;                    // Enable the filter
    ADCCON3bits.ADINSEL = achannel;           // Channel to manually trigger
    ADCCON3bits.RQCNVRT = 1;                  // Manually trigger selected channel
    while (ADCFLTR1bits.AFRDY == 0) continue; // Wait until oversampling is done
    ADCFLTR1bits.AFEN = 0;                    // Enable the digital filtering
    return ADCFLTR1bits.FLTRDATA;
}
