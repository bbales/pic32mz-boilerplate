#ifndef AUDIO_H
#define AUDIO_H

#include <dsplib_def.h>

#define OUTPUT 0
#define INPUT 1

#define LEFT_CHANNEL 1
#define RIGHT_CHANNEL 0

// Twos complement 23/32 bit conversions
#define C24TO32(num) ((0b100000000000000000000000 & num) ? 0b11111111000000000000000000000000 + num : num)
#define C32TO24(num) ((0b10000000000000000000000000000000 & num) ? 0b00000000111111111111111111111111 & num : num)

// Audio and sample variables
char channel;
int32 left_input;
int32 right_input;
int32 left_output;
int32 right_output;

// CODEC protos

void codecRW();
void codecEnable(int enable);
void codecInitMCLK();
void codecInit();

// Main CODEC interrupt subroutine
void __ISR_AT_VECTOR(_SPI4_RX_VECTOR,IPL7SRS) codecRW(void);

#endif
