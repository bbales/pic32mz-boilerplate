#include <xc.h>

// DEVCFG3
#pragma config FMIIEN = OFF  // Ethernet RMII/MII Enable (RMII Enabled)
#pragma config FETHIO = OFF  // Ethernet I/O Pin Select (Alternate Ethernet I/O)
#pragma config PGL1WAY = OFF // Permission Group Lock One Way Configuration
#pragma config PMDL1WAY = OFF // Peripheral Module Disable Configuration (Allow
#pragma config IOL1WAY = OFF  // Peripheral Pin Select Configuration (Allow only
#pragma config FUSBIDIO = OFF

// DEVCFG2
#pragma config FPLLIDIV = 0b001   // System PLL Input Divider (2x Divider)
#pragma config FPLLRNG = 0b001    // System PLL Input Range (5-10 MHz Input)
#pragma config FPLLICLK = PLL_FRC // System PLL Input Clock Selection (FRC is
#pragma config FPLLMULT = MUL_50  // System PLL Multiplier (PLL Multiply by 25)
#pragma config FPLLODIV = 0b000 // System PLL Output Clock Divider (2x Divider)
#pragma config UPLLFSEL = FREQ_24MHZ // USB PLL Input Frequency Selection (USB PLL input is 24 MHz)

// DEVCFG1
#pragma config FNOSC = SPLL // Oscillator Selection Bits (SPLL)
#pragma config DMTINTV = WIN_127_128 // DMT Count Window Interval
#pragma config FSOSCEN = OFF // Secondary Oscillator Enable (Enable SOSC)
#pragma config IESO = OFF    // Internal/External Switch Over (Enabled)
#pragma config POSCMOD = OFF // Primary Oscillator Configuration (Primary osc disabled)
#pragma config OSCIOFNC = OFF // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FCKSM = CSDCMD    // Clock Switching and Monitor Selection (Clock
#pragma config WDTPS = PS1048576 // Watchdog Timer Postscaler (1:1048576)
#pragma config WDTSPGM = STOP    // Watchdog Timer Stop During Flash Programming
#pragma config WINDIS = NORMAL // Watchdog Timer Window Mode (Watchdog Timer is
#pragma config FWDTEN = OFF    // Watchdog Timer Enable (WDT Disabled)
#pragma config FWDTWINSZ = WINSZ_25 // Watchdog Timer Window Size (Window size is 25%)
#pragma config DMTCNT = DMT31 // Deadman Timer Count Selection (2^31 (2147483648))
#pragma config FDMTEN = OFF // Deadman Timer Enable (Deadman Timer is enabled)

// DEVCFG0
#pragma config DEBUG = OFF  // Background Debugger Enable (Debugger is disabled)
#pragma config JTAGEN = OFF // JTAG Enable (JTAG Port Enabled)
#pragma config ICESEL = ICS_PGx2 // ICE/ICD Comm Channel Select (Communicate on PGEC2/PGED2)
#pragma config TRCEN = OFF // Trace Enable (Trace features in the CPU are enabled)
#pragma config BOOTISA = MIPS32 // Boot ISA Selection (Boot code and Exception code is MIPS32)
#pragma config FECCCON = OFF_UNLOCKED
#pragma config FSLEEP = OFF
#pragma config DBGPER = ALLOW_PG2
#pragma config SMCLR = MCLR_NORM
// #pragma config SOSCGAIN = GAIN_1X // Secondary Oscillator Gain Control bits (2x gain setting)
// #pragma config SOSCBOOST = OFF // Secondary Oscillator Boost Kick Start Enable
#pragma config POSCGAIN = GAIN_2X // Primary Oscillator Gain Control bits (2x gain setting)
#pragma config POSCBOOST = ON // Primary Oscillator Boost Kick Start Enable bit
#pragma config EJTAGBEN = NORMAL // EJTAG Boot (Normal EJTAG functionality)

// DEVCP0
#pragma config CP = OFF // Code Protect (Protection Disabled)
