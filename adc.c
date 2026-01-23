#include "adc.h"
#include <avr/io.h>

void ADC_Init(void) {
	ADMUX = (1 << REFS0);
	// Sets bit 6 of ADMUX to 1. REFS1:0 = 01, which selects 5V as the reference voltage.
	// Not including (X << MUX?), MUX4:0 = 0000, A0 is the input selected.
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	// ADEN turns ADC on, ADPS2:0 set the prescaler bits. 111 = 128 prescaler.

}
uint16_t ADC_Read(void) {
	ADCSRA |= (1 << ADSC);           // |= sets bits, = overwrites bits. Changes ADSC without overwriting others.
	while (ADCSRA & (1 << ADSC)) {}  // Wait for the conversion to finish.
	return ADCL | (ADCH << 8);       // Combining high and low after the conversion is confirmed to be complete.
}
