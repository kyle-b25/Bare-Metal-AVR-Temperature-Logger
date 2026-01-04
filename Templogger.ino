#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

// Function Prototypes
void ADC_init(void);
void UART_init(void);
uint16_t ADC_read(void);
void UART_sendchar(char c);
void UART_print(const char* str);  // I put const to make sure the function does not modify the string.
void UART_printint(int x);

int main(void) {

  // Variable Space Reservation
  uint16_t adc_result;
  float voltage;
  float tempC;

  ADC_init();   // Initialize ADC.
  UART_init();  // Initalize UART.

  while (1) {
    adc_result = ADC_read();                // Gets the value from ADC_read().
    voltage = adc_result * (5.0 / 1023.0);  // Convert adc_result to voltage.
    tempC = (voltage - 0.5) / 0.01;         // Convert voltage to temperature in Celsius.

    UART_print("Temperature is ");
    UART_printint((int)tempC);
    UART_print(" C\r\n");

    _delay_ms(500);  // 500ms delay between readings.
  }

  return 0;
}

// Function Definitions
void ADC_init(void) {
  ADMUX = (1 << REFS0);  // Sets bit 6 of ADMUX to 1. REFS1:0 = 01, which selects 5V as the reference voltage.
  // Not including (X << MUX?), MUX4:0 = 0000, A0 is the input selected.
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // ADEN turns ADC on, ADPS2:0 set the prescaler bits. 111 = 128 prescaler.
}

void UART_init(void) {
  unsigned int UBRR_val = 103;  // UBRR = Fcpu / (16 * baud) = 103.
  UBRR0H = (unsigned char)(UBRR_val >> 8);
  UBRR0L = (unsigned char)(UBRR_val);
  UCSR0B = (1 << TXEN0);                   // Bit 3: (TXEB0), of UCSR0B enables the transmitter
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // Bits 2, 1, and 0 need to = 0, 1, and 1 respectively for 8-bit character size.
}

void UART_sendchar(char c) {
  while (!(UCSR0A & (1 << UDRE0))) {}  // Wait while UDRE0 is not empty.
  UDR0 = c;                            // I/O data register recieves the singular character.
}

void UART_print(const char* str) {
  for (int i = 0; str[i]; i++)  // Automatically waits until str[i] is the null terminator at the end of the string.
    UART_sendchar(str[i]);      // Each iteration sends one character which is in line with how UART works.
}

void UART_printint(int x) {
  char buffer[10];  // Buffer to store digits in limbo.
  int i = 0;

  if (x == 0) {  // Special case of 0.
    UART_sendchar('0');
    return;
  }

  while (x > 0) {
    buffer[i] = (x % 10) + '0';  // Separates the last digit and converts it into a character, then stores it into the buffer.
    x = x / 10;                  // Removes last digit from the original number so the next digits can be processed into characters.
    i++;
  }

  for (int j = i - 1; j >= 0; j--) {  // Digits are stored in reverse order, so we need to print them in reverse order.
    UART_sendchar(buffer[j]);
  }
}

uint16_t ADC_read(void) {          // uint16_t will return the needed 16 bit result.
  ADCSRA |= (1 << ADSC);           // |= sets bits, = overwrites bits. Changes ADSC without overwriting others.
  while (ADCSRA & (1 << ADSC)) {}  // Wait for the conversion to finish.
  return ADCL | (ADCH << 8);     // Combining high and low after the conversion is confirmed to be complete.
}