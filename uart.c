#include "uart.h"
#include <avr/io.h>

void UART_Init(void) {
	unsigned int UBRR_val = 103;
	// UBRR = Fcpu / (16 * baud) = 103.
	UBRR0H = (unsigned char)(UBRR_val >> 8);
	UBRR0L = (unsigned char)(UBRR_val);
	UCSR0B = (1 << TXEN0);                   // Bit 3: (TXEN0), of UCSR0B enables the transmitter
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // Bits 2, 1, and 0 need to = 0, 1, and 1 respectively for 8-bit character size.
}

void UART_Send_Char(char c) {
	while (!(UCSR0A & (1 << UDRE0))) {}  // Wait while UDRE0 is not empty.
	UDR0 = c;                            // I/O data register recieves the singular character.
}

void UART_Print(const char* str) {
	for (int i = 0; str[i]; i++)  // Automatically waits until str[i] is the null terminator at the end of the string.
	UART_Send_Char(str[i]);     // Each iteration sends one character which is in line with how UART works.
}

void UART_Print_Int(int x) {
	char buffer[10];  // Buffer to store digits in limbo.
	int i = 0;

	if (x == 0) {  // Special case of 0.
		UART_Send_Char('0');
		return;
	}

	while (x > 0) {
		buffer[i] = (x % 10) + '0';  // Separates the last digit and converts it into a character, then stores it into the buffer.
		x = x / 10;                  // Removes last digit from the original number so the next digits can be processed into characters.
		i++;
	}

	for (int j = i - 1; j >= 0; j--) {  // Digits are stored in reverse order, so we need to print them in reverse order.
		UART_Send_Char(buffer[j]);
	}
}