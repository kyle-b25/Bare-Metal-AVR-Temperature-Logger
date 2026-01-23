#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

typedef struct {
	uint8_t stable_state;  // Debounced button state.
	uint8_t last_raw_state;  // Last measured instanteous state.
	uint32_t last_change_ms;  // Timestamp of last state change.
} button_t;

void Button_Init(void);
void Button_Update(button_t *btn, uint32_t curr_ms);
uint8_t Button_Read_Raw(void);

#endif