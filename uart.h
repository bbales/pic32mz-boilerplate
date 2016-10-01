#ifndef UART_H
#define UART_H

#define CR 0x0D
#define LF 0x0A
#define uartNewline() uartSendChar(CR); uartSendChar(LF);  

// void adcInit();
void uartInit();
void uartSend(char a);
void uartSendString(char * s);

#endif
