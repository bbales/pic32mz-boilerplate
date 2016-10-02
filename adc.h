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
int readADC(int achannel);
int readFilteredADC(int achannel);

#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */
