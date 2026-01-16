# Bare-Metal-AVR-Temperature-Logger

Features:
- Hardware timer with a software millisecond counter implementation.
- Direct ADC initialization and reading for 10-bit measurements.
- Continuous temperature calculation from the TMP36 voltage reading.
- UART communication to display real-time temperature on a PC.
- Displays the current temperature mode and instructions on an LCD screen.
- A push-button that switches the current selected units for temperature. C, F, or K.
- No Arduino handicaps such as serial objects or libraries.
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

Future Goals:
- Debouncing the button properly.
- Store the last selected unit in EEPROM, so it remembers user-preferance after the power is shut off.
- Take the average of a specifc number of readings before displaying for increased accuracy.
- Synchronize LCD functions with time-logic based system and remove reliance on delays. Not sure if a good decision or possible yet.
