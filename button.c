#include "button.h"
#include <avr/io.h>
#define DEBOUNCE_MS 20

void Button_Init(void) {
	DDRD &= ~(1 << DDD7);    // Selects Digital Pin 7
	PORTD |= (1 << PORTD7);  // Enables Pull-Up
}

uint8_t Button_Read_Raw(void) {
	return !(PIND & (1 << PD7));  // Inverted with "!" because pull-up means LOW when pressed.
}

void Button_Update(button_t* btn, uint32_t curr_ms) {
	uint8_t raw = Button_Read_Raw();

	if (raw != btn->last_raw_state) {
		btn->last_raw_state = raw;
		btn->last_change_ms = curr_ms;
	}

	if ((curr_ms - btn->last_change_ms) >= DEBOUNCE_MS) {
		btn->stable_state = raw;
	}
}
