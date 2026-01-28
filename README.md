# Bare-Metal-AVR-Temperature-Logger
- Just for some important background, the files have been moved to Microchip Studio. The .hex file from Microchip Studio, AVRDude.exe, and AVRDude.conf,
- were launched in command prompt because Arduino IDE makes the use of header files complicated. Organized code is a must have.

C:\Users\BLANK>
"C:\Users\-\Desktop\avrdude.exe" -C"C:\Users\-\Desktop\avrdude.conf" -c arduino -P COM4 -p m328p -b 115200 -D -U flash:w:"C:\Users\-\Desktop\Arduino\MicrochipStudioHex\Temp_Logger.hex":i

Features:
- Hardware timer with a software millisecond counter implementation.
- Direct ADC initialization and reading for 10-bit measurements.
- Continuous temperature calculation from the TMP36 voltage reading.
- UART communication to display real-time temperature on a PC.
- Displays the current temperature mode and instructions on an LCD screen.
- A push-button that switches the current selected units for temperature. C, F, or K.
- No Arduino handicaps such as serial objects or libraries.
- Non-blocking button debounce algorithm, enabling accurate input detection.
- Fixed point mathematics.
- A noise filter.

Learning Accomplishments:
- Hands-on experience with interrupts, safe concurrency, and time-based logic.
- Greater understanding of AVR registers.
- Designed and implented UART transmission routines from scratch.
- Created a temperature unit (C, F, K) switching system through button input.
- Designed initialization and helper functions for LCD.
- Gained efficiency in navigating various datasheets.
- Built a real-time sensor-reading loop in embedded C.
- Stronger comprehension of fixed-point mathematics.
- Developed skills in breadboard problem solving.
- Learned how to handle time-based events in microcontrollers without blocking main program flow.

Future Goals:
- Store the last selected unit in EEPROM, so it remembers user-preferance after the power is shut off.
- Take the average of a specifc number of readings before displaying for increased accuracy.
- Synchronize LCD functions with time-logic based system and remove reliance on delays. Not sure if a good decision or possible yet.

Hardware List:
- TMP36 Sensor
- Arduino Uno R3 (ATmega328P)
- 21 Wires
- 100uF Capacitor
- 10 kOhm resistor
- LCD Screen
- Pushbutton
- Breadboard
