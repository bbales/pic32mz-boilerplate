#ifndef AUDIO_H
#define AUDIO_H

#include <dsplib_def.h> // Required for int32 type
#include <sys/attribs.h> // Required for Interrups constants

#define OUTPUT 0
#define INPUT 1

#define CHANNEL_B 1
#define CHANNEL_A 0

// Twos complement 23/32 bit conversions
#define C24TO32(num) ((0b100000000000000000000000 & num) ? 0b11111111000000000000000000000000 + num : num)
#define C32TO24(num) ((0b10000000000000000000000000000000 & num) ? 0b00000000111111111111111111111111 & num : num)

// CS4272 Codec struct
typedef struct CS4272{
    int32 leftIn;
    int32 rightIn;
    int32 leftOut;
    int32 rightOut;
    char channel;
    void (*enable)(char);
    void (*rw)();
} CS4272;
CS4272 codec;

// CODEC protos
void codecRW();
void codecEnable(char enable);
void codecInitMCLK();
void codecInit();

// Main CODEC interrupt subroutine
void __ISR_AT_VECTOR(_SPI4_RX_VECTOR,IPL7SRS) codecRW(void);

#endif
