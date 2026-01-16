#define F_CPU 16000000UL
#define LCD_RS PB4  // D12
#define LCD_E PB3   // D11
#define LCD_D4 PD5  // D5 → DB4
#define LCD_D5 PD4  // D4 → DB5
#define LCD_D6 PD3  // D3 → DB6
#define LCD_D7 PD2  // D2 → DB7


#include <avr/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

// Enums
typedef enum {
  UNIT_C,  // 0
  UNIT_F,  // 1
  UNIT_K   // 2
} temp_unit_t;


// Global Variables
volatile uint32_t ms_counter = 0;
temp_unit_t currentUnit = UNIT_C;  // Start in Celsius mode.

// Timer

ISR(TIMER1_COMPA_vect) {  // When the Timer1 Compare Match A interrupt occurs (every 250 ticks)
  ms_counter++;           // Jump to this code and increment the ms_counter correctly.
}

// ADC Function Prototypes
void ADC_init(void);
uint16_t ADC_read(void);

// UART Function Prototypes
void UART_init(void);
void UART_sendchar(char c);
void UART_print(const char* str);  // I put const to make sure the function does not modify the string.
void UART_printint(int x);

// Timer1 Function Prototypes
void Timer1_init(void);

// BTN Function Prototypes
void BTN_init(void);

// LCD Function Prototypes
void LCD_init(void);
void LCD_preinit(void);
void LCD_delay_us(uint16_t us);  // Temporary until time-logic synchronization.
void LCD_write_nibble(uint8_t nibble);
void LCD_write_cmd(uint8_t cmd);
void LCD_write_data(char c);
void LCD_write_string(const char* str);
void LCD_cursor(uint8_t row, uint8_t col);
void LCD_update_mode(temp_unit_t unit);

int main(void) {

  // Variable Space Reservation
  uint16_t adc_result;
  int16_t tempC_x10;
  uint16_t tempDisplay_x10;
  uint32_t last_time = 0;
  uint32_t now;
  uint8_t last_btn_state = 1;  // Pull-Up means release = high. So it starts "off."
  uint8_t curr_btn_state;

  LCD_preinit();                 // Initialize MCU with LCD.
  LCD_init();                    // Initialize LCD.
  LCD_update_mode(currentUnit);  // Turn LCD on.
  Timer1_init();                 // Initialize Timer1.
  ADC_init();                    // Initialize ADC.
  UART_init();                   // Initalize UART.
  BTN_init();                    // Initalize BTN.
  sei();                         // Enables interrupts.

  while (1) {

    cli();  // Prevents the ISR from interrupting mid-read.
    now = ms_counter;
    sei();

    curr_btn_state = (PIND & (1 << PIND7));  // = 0, low. != 0, high.

    if (last_btn_state && !curr_btn_state) {  // Detects if button was pressed.
      currentUnit = (currentUnit + 1) % 3;    // Confines currentUnit to 0, 1, or 2 representing C, F, K respectively.
      LCD_update_mode(currentUnit);           // Refresh display immediately
    }

    last_btn_state = curr_btn_state;

    if ((now - last_time) >= 1000) {         // Checks if a second has passed by.
      adc_result = ADC_read();               // Gets the value from ADC_read().
      tempC_x10 = (int16_t)adc_result + 48;  // Convert voltage to temperature in Celsius, x10 represents the SCALE = 10. Offset is 4.88 mV.

      switch (currentUnit) {
        case UNIT_C:
          tempDisplay_x10 = tempC_x10;
          UART_print("Temperature is ");
          UART_printint(tempDisplay_x10 / 10);
          UART_print(".");
          UART_printint(abs(tempDisplay_x10 % 10));
          UART_print(" C\r\n");
          break;

        case UNIT_F:
          tempDisplay_x10 = (tempC_x10 * 9) / 5 + 320;
          UART_print("Temperature is ");
          UART_printint(tempDisplay_x10 / 10);
          UART_print(".");
          UART_printint(abs(tempDisplay_x10 % 10));
          UART_print(" F\r\n");
          break;

        case UNIT_K:
          tempDisplay_x10 = tempC_x10 + 2731;
          UART_print("Temperature is ");
          UART_printint(tempDisplay_x10 / 10);
          UART_print(".");
          UART_printint(abs(tempDisplay_x10 % 10));
          UART_print(" K\r\n");
          break;
      }

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

void BTN_init(void) {
  DDRD &= ~(1 << DDD7);    // Selects Digital Pin 7
  PORTD |= (1 << PORTD7);  // Enables Pull-Up
}

void LCD_init(void) {
  _delay_ms(20);

  LCD_write_nibble(0x03);
  _delay_ms(5);

  LCD_write_nibble(0x03);
  _delay_us(150);

  LCD_write_nibble(0x03);
  _delay_us(150);

  LCD_write_nibble(0x02);  // Switches to 4-bit mode.

  LCD_write_cmd(0x28);  // 4-bit, 2-line, 5x8.
  LCD_write_cmd(0x0C);  // Display ON, cursor OFF.
  LCD_write_cmd(0x06);  // Entry mode set to increment with no shift.
  LCD_write_cmd(0x01);  // Clear display.
  _delay_ms(2);
}

void LCD_preinit(void) {                                      // Sets LCD pins as output for the MCU.
  DDRB |= (1 << LCD_RS) | (1 << LCD_E);                       // RS and E are PORTB
  DDRD |= (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5);  // D4–D7
}

void LCD_delay_us(uint16_t us) {
  while (us--) {
    _delay_us(1);
  }
}

void LCD_write_nibble(uint8_t nibble) {
  // Clear data pins first
  PORTD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5));

  if (nibble & 0x01) PORTD |= (1 << PD5);  // D4
  if (nibble & 0x02) PORTD |= (1 << PD4);  // D5
  if (nibble & 0x04) PORTD |= (1 << PD3);  // D6
  if (nibble & 0x08) PORTD |= (1 << PD2);  // D7

  // Enable pulse
  PORTB |= (1 << LCD_E);
  LCD_delay_us(1);
  PORTB &= ~(1 << LCD_E);
}

void LCD_write_cmd(uint8_t cmd) {
  PORTB &= ~(1 << LCD_RS);  // Command mode

  LCD_write_nibble(cmd >> 4);
  LCD_write_nibble(cmd & 0x0F);

  LCD_delay_us(40);  // Command execution time
}

void LCD_write_data(char c) {
  PORTB |= (1 << LCD_RS);      // RS = 1 → data mode
  LCD_write_nibble(c >> 4);    // High nibble
  LCD_write_nibble(c & 0x0F);  // Low nibble
  LCD_delay_us(40);            // Wait for LCD to process
}

void LCD_write_string(const char* str) {
  while (*str) {
    LCD_write_data(*str++);
  }
}

void LCD_cursor(uint8_t row, uint8_t col) {
  uint8_t addr = 0;
  if (row == 0) addr = 0x80 + col;       // Sets cursor on top row if the output belongs on the top row, then goes to the correct column.
  else if (row == 1) addr = 0xC0 + col;  // Sets cursor on bottom row if the output belongs on the bottom row, then goes to the correct column.

  LCD_write_cmd(addr);
}

void LCD_update_mode(temp_unit_t unit) {
  LCD_write_cmd(0x01);  // Clear display
  _delay_ms(2);         // Clearing takes >1.5ms

  LCD_cursor(0, 0);  // Top line
  LCD_write_string("BTN = Next Mode");

  LCD_cursor(1, 0);  // Bottom line
  LCD_write_string("Current Mode = ");

  char mode_char = 'C';
  if (unit == UNIT_C) mode_char = 'C';
  else if (unit == UNIT_F) mode_char = 'F';
  else if (unit == UNIT_K) mode_char = 'K';

  LCD_write_data(mode_char);
}
