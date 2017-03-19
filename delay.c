/*
* @Author: bbales
* @Date:   2016-03-08 17:24:54
* @Last Modified by:   bbales
* @Last Modified time: 2016-04-15 20:32:33
*/
#include <xc.h>

#include "delay.h"

void delay_us(unsigned long us) {
    unsigned int i = 0;
    asm volatile("nop");
    while (i++ < us) {
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
    }
}

void delay_ms(unsigned long ms) {
    unsigned int i = 0;
    while (i++ < ms) delay_us(1000);
}
