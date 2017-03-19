### PIC32MZ Boilerplate

This repository contains the boilerplate code for a DSP platform I am developing for select PIC32MZ microprocessors

#### Current Features

- *delay.c* Blocking delay routines
- *audio.c* Audio CODEC support through I2S protocol
- *adc.c* Basic on-board ADC routines with software filtering leveraged
- *timers.c* Timer configuration for peripherals and other misc tasks
- *uart.c* Basic UART routines, mainly for debugging
- *dsp.c* DSP structures and routines

#### Building and Programming

In order to avoid using MPLAB IDE, I have written a basic script in `scripts/prog`, this script takes advantage of the `mdb` tool to allow programming of the board using the command line. To build and program, run the command : `make && make program`

#### Monitoring UART

: `sudo minicom -b 115200 -o -D /dev/ttyUSB0`
