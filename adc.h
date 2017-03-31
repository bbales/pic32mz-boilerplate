/*
 * File:   adc.h
 * Author: bbales
 *
 * Created on October 1, 2016, 7:48 PM
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif

void adcInit();
int readADC(unsigned int achannel);
int readADCAlt(unsigned int achannel);
int readFilteredADC(unsigned int achannel);

#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */
