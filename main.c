#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "adc.h"
#include "uart.h"
#include "button.h"
#include "lcd.h"
#include "temp.h"

volatile uint32_t ms_counter = 0;
temp_unit_t current_unit = UNIT_C;

ISR(TIMER1_COMPA_vect) {
	ms_counter++;
	
}

int main(void) {

	// Variable Space Reservation
	uint16_t adc_result;
	int16_t tempC_x10;
	uint16_t tempDisplay_x10;
	uint32_t last_time = 0;
	uint32_t now;
	uint8_t last_stable_btn = 0;
	button_t unit_btn = {
		.stable_state = 0,
		.last_raw_state = 0,
		.last_change_ms = 0
	};

	LCD_Pre_Init();
	LCD_Init();
	LCD_Update_Mode(current_unit);
	Timer1_Init();
	ADC_Init();
	UART_Init();
	Button_Init();
	sei();  // Enable global interrupts

	while (1) {

		cli();  // Prevents the ISR from interrupting mid-read.
		now = ms_counter;
		sei();

		Button_Update(&unit_btn, now);

		if (unit_btn.stable_state && !last_stable_btn) {  // Detect button press.
			current_unit = (current_unit + 1) % 3;            // Select the next temperature mode.
			LCD_Update_Mode(current_unit);                   // Refresh display immediately
		}

		last_stable_btn = unit_btn.stable_state;

		if ((now - last_time) >= 1000) {         // Checks if a second has passed by.
			adc_result = ADC_Read();               // Gets the value from ADC_read().
			tempC_x10 = (int16_t)adc_result + 48;  // Convert voltage to temperature in Celsius, x10 represents the SCALE = 10. Offset is 4.88 mV.

			switch (current_unit) {
				case UNIT_C:
				tempDisplay_x10 = tempC_x10;
				UART_Print("Temperature is ");
				UART_Print_Int(tempDisplay_x10 / 10);
				UART_Print(".");
				UART_Print_Int(abs(tempDisplay_x10 % 10));
				UART_Print(" C\r\n");
				break;

				case UNIT_F:
				tempDisplay_x10 = (tempC_x10 * 9) / 5 + 320;
				UART_Print("Temperature is ");
				UART_Print_Int(tempDisplay_x10 / 10);
				UART_Print(".");
				UART_Print_Int(abs(tempDisplay_x10 % 10));
				UART_Print(" F\r\n");
				break;

				case UNIT_K:
				tempDisplay_x10 = tempC_x10 + 2731;
				UART_Print("Temperature is ");
				UART_Print_Int(tempDisplay_x10 / 10);
				UART_Print(".");
				UART_Print_Int(abs(tempDisplay_x10 % 10));
				UART_Print(" K\r\n");
				break;
			}

			last_time = now;
		}
	}
	return 0;
}

void Timer1_Init(void) {
	TCCR1A = 0;
	TCCR1B = 0;                           // Fully resets the timer for initialziation.
	TCCR1B |= (1 << WGM12);               // Set Timer1 to CTC mode.
	OCR1A = 249;                          // 0 -> 249 = 250 ticks per interrupt.
	TIMSK1 |= (1 << OCIE1A);              // Enable Timer1 compare match A interrupt.
	TCCR1B |= (1 << CS11) | (1 << CS10);  // Prescaler = 64.
}