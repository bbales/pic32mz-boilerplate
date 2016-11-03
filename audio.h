#ifndef AUDIO_H
#define AUDIO_H

#define OUTPUT 0
#define INPUT 1

#define LEFT_CHANNEL 1
#define RIGHT_CHANNEL 0

// CODEC protos
void codecRW();
void codecEnable(int enable);
void codecInitMCLK();
void codecInit();

// Main CODEC interrupt subroutine
void __ISR_AT_VECTOR(_SPI4_RX_VECTOR,IPL7SRS) codecRW(void);

// Audio and sample variables
char channel;
long left_input;
long right_input;
long left_output;
long right_output;

#endif
