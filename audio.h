#ifndef AUDIO_H
#define AUDIO_H

void rwCodec();
void initReferenceClocks();
void codecInit();
void codecEnable(int enable);

int channel;
signed long left_input;
signed long right_input;
signed long left_output;
signed long right_output;

#endif
