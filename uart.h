#ifndef UART_H
#define UART_H

#define CR 0x0D
#define LF 0x0A

#define uartNewline() uartSendChar(CR); uartSendChar(LF);
#define uartClearScreen() uartSendString("\033[2J");

void uartInit();
void uartSend(char a);
void uartSendString(char * s);

#endif
