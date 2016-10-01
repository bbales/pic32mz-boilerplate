#ifndef AUDIO_H
#define AUDIO_H

#define OUTPUT 0
#define INPUT 1

// CODEC protos
void codecInitMCLK();
void codecInit();
void codecEnable(int enable);
void codecRW();

// Main CODEC interrupt subroutine
void __ISR_AT_VECTOR(_SPI4_RX_VECTOR,IPL7SRS) codecRW(void);

// Audio and sample variables
int channel;
signed long left_input;
signed long right_input;
signed long left_output;
signed long right_output;

#endif
