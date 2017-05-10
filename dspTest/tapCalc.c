#include <stdio.h>

int main() {
    // Clock divider
    unsigned int timerClockDiv = 1;

    // Peripheral clock frequency
    unsigned int periphClock = 94500000;

    unsigned int length = 48000;
    unsigned int samplerate = 48000;

    unsigned int avg = 48000;

    // Calculates loop period in seconds
    double period = (double) avg / (double) samplerate;
    period = 0.1;
    printf("Loop period (s): %lf\n", period);

    // Tap period in seconds based on sample rate

    // Desired timer frequency 1/(period/length) = length/period
    unsigned int timerFrequency = length / period;
    printf("Timer frequency: %u\n", timerFrequency);

    unsigned int PR2 = (periphClock / timerClockDiv) / timerFrequency;
    printf("PR2: %u\n", PR2);
}
