#include "lcd.h"
#include <avr/delay.h>
#include <avr/io.h>

#define LCD_RS PB4  // D12
#define LCD_E PB3   // D11
#define LCD_D4 PD5  // D5 ? DB4
#define LCD_D5 PD4  // D4 ? DB5
#define LCD_D6 PD3  // D3 ? DB6
#define LCD_D7 PD2  // D2 ? DB7

void LCD_Init(void) {
	_delay_ms(20);

	LCD_Write_Nibble(0x03);
	_delay_ms(5);

	LCD_Write_Nibble(0x03);
	_delay_us(150);

	LCD_Write_Nibble(0x03);
	_delay_us(150);

	LCD_Write_Nibble(0x02);  // Switches to 4-bit mode.

	LCD_Write_Cmd(0x28);  // 4-bit, 2-line, 5x8.
	LCD_Write_Cmd(0x0C);  // Display ON, cursor OFF.
	LCD_Write_Cmd(0x06);  // Entry mode set to increment with no shift.
	LCD_Write_Cmd(0x01);  // Clear display.
	_delay_ms(2);
}

void LCD_Pre_Init(void) {                                      // Sets LCD pins as output for the MCU.
	DDRB |= (1 << LCD_RS) | (1 << LCD_E);                       // RS and E are PORTB
	DDRD |= (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5);  // D4–D7
}

void LCD_Delay_us(uint16_t us) {
	while (us--) {
		_delay_us(1);
	}
}

void LCD_Write_Nibble(uint8_t nibble) {
	// Clear data pins first
	PORTD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5));

	if (nibble & 0x01) PORTD |= (1 << PD5);  // D4
	if (nibble & 0x02) PORTD |= (1 << PD4);  // D5
	if (nibble & 0x04) PORTD |= (1 << PD3);  // D6
	if (nibble & 0x08) PORTD |= (1 << PD2);  // D7

	// Enable pulse
	PORTB |= (1 << LCD_E);
	LCD_Delay_us(1);
	PORTB &= ~(1 << LCD_E);
}

void LCD_Write_Cmd(uint8_t cmd) {
	PORTB &= ~(1 << LCD_RS);  // Command mode

	LCD_Write_Nibble(cmd >> 4);
	LCD_Write_Nibble(cmd & 0x0F);

	LCD_Delay_us(40);  // Command execution time
}

void LCD_Write_Data(char c) {
	PORTB |= (1 << LCD_RS);      // RS = 1 ? data mode
	LCD_Write_Nibble(c >> 4);    // High nibble
	LCD_Write_Nibble(c & 0x0F);  // Low nibble
	LCD_Delay_us(40);            // Wait for LCD to process
}

void LCD_Write_String(const char* str) {
	while (*str) {
		LCD_Write_Data(*str++);
	}
}

void LCD_Cursor(uint8_t row, uint8_t col) {
	uint8_t addr = 0;
	if (row == 0) addr = 0x80 + col;       // Sets cursor on top row if the output belongs on the top row, then goes to the correct column.
	else if (row == 1) addr = 0xC0 + col;  // Sets cursor on bottom row if the output belongs on the bottom row, then goes to the correct column.

	LCD_Write_Cmd(addr);
}

void LCD_Update_Mode(temp_unit_t unit) {
	LCD_Write_Cmd(0x01);  // Clear display
	_delay_ms(2);         // Clearing takes >1.5ms

	LCD_Cursor(0, 0);  // Top line
	LCD_Write_String("BTN = Next Mode");

	LCD_Cursor(1, 0);  // Bottom line
	LCD_Write_String("Current Mode = ");

	char mode_char = 'C';
	if (unit == UNIT_C) mode_char = 'C';
	else if (unit == UNIT_F) mode_char = 'F';
	else if (unit == UNIT_K) mode_char = 'K';

	LCD_Write_Data(mode_char);
}