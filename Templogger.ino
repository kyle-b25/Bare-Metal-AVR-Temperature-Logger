#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

// Global Variables
volatile uint32_t ms_counter = 0;

ISR(TIMER1_COMPA_vect) {  // When the Timer1 Compare Match A interrupt occurs (every 250 ticks)
  ms_counter++;           // Jump to this code and increment the ms_counter correctly.
}

// Function Prototypes
void ADC_init(void);
uint16_t ADC_read(void);

void UART_init(void);
void UART_sendchar(char c);
void UART_print(const char* str);  // I put const to make sure the function does not modify the string.
void UART_printint(int x);

void Timer1_init(void);

int main(void) {

  // Variable Space Reservation
  uint16_t adc_result;
  int16_t tempC_x10;
  uint32_t now;
  uint32_t last_time = 0;

  Timer1_init();  // Initialize Timer1.
  ADC_init();     // Initialize ADC.
  UART_init();    // Initalize UART.
  sei();          // Enables interrupts.

  while (1) {

    cli();  // Allows the ISR to interrupt mid-read.
    now = ms_counter;
    sei();

    if ((now - last_time) >= 1000) {         // Checks if a second has passed by.
      adc_result = ADC_read();               // Gets the value from ADC_read().
      tempC_x10 = (int16_t)adc_result + 50;  // Convert voltage to temperature in Celsius, x10 represents the SCALE = 10. Offset is 5 degrees.

      UART_print("Temperature is ");
      UART_printint(tempC_x10 / 10);
      UART_print(".");
      UART_printint(abs(tempC_x10 % 10));
      UART_print(" C\r\n");

      last_time = now;
    }
  }
  return 0;
}

// Function Definitions
void ADC_init(void) {
  ADMUX = (1 << REFS0);  // Sets bit 6 of ADMUX to 1. REFS1:0 = 01, which selects 5V as the reference voltage.
  // Not including (X << MUX?), MUX4:0 = 0000, A0 is the input selected.
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // ADEN turns ADC on, ADPS2:0 set the prescaler bits. 111 = 128 prescaler.
}

uint16_t ADC_read(void) {
  ADCSRA |= (1 << ADSC);           // |= sets bits, = overwrites bits. Changes ADSC without overwriting others.
  while (ADCSRA & (1 << ADSC)) {}  // Wait for the conversion to finish.
  return ADCL | (ADCH << 8);       // Combining high and low after the conversion is confirmed to be complete.
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

void Timer1_init(void) {
  TCCR1A = 0;
  TCCR1B = 0;                           // Fully resets the timer for initialziation.
  TCCR1B |= (1 << WGM12);               // Set Timer1 to CTC mode.
  OCR1A = 249;                          // 0 -> 249 = 250 ticks per interrupt.
  TIMSK1 |= (1 << OCIE1A);              // Enable Timer1 compare match A interrupt.
  TCCR1B |= (1 << CS11) | (1 << CS10);  // Prescaler = 64.
}
