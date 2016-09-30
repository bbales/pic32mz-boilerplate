/*
 * File:   delay.h
 * Author: bbales
 *
 * Created on September 29, 2016, 9:53 PM
 */

#ifndef DELAY_H
#define	DELAY_H

#ifdef	__cplusplus
extern "C" {
#endif

void delay_us(unsigned long us);
void delay_ms(unsigned long ms);

#ifdef	__cplusplus
}
#endif

#endif	/* DELAY_H */
