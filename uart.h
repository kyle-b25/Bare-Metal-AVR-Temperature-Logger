#ifndef UART_H
#define UART_H

void UART_Init(void);
void UART_Send_Char(char c);
void UART_Print(const char* str);
void UART_Print_Int(int x);

#endif