#ifndef LCD_H
#define LCD_H

#include "temp.h"
#include <stdint.h>

void LCD_Init(void);
void LCD_Pre_Init(void);
void LCD_Delay_us(uint16_t us);  // Temporary until time-logic synchronization.
void LCD_Write_Nibble(uint8_t nibble);
void LCD_Write_Cmd(uint8_t cmd);
void LCD_Write_Data(char c);
void LCD_Write_String(const char* str);
void LCD_Cursor(uint8_t row, uint8_t col);
void LCD_Update_Mode(temp_unit_t unit);

#endif