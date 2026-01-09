# Bare-Metal-AVR-Temperature-Logger

Features:
- Hardware timer with a software millisecond counter implementation.
- Direct ADC initialization and reading for 10-bit measurements.
- Continuous temperature calculation from the TMP36 voltage reading.
- UART communication to display real-time temperature on a PC.
- No Arduino handicaps such as serial objects or libraries.
- A noise filter.

Learning Goals:
- Hands-on experience with interrupts, safe concurrency, and time-based logic.
- Understanding AVR registers.
- Implementing UART transmission routines from scratch.
- Efficiency in navigating various datasheets.
- Building a real-time sensor-reading loop in embedded C.

Future Goals:
- Implement the option to switch between Celsius, Farenheit, and Kelvin.
- LCD screen implementation.
- Removing reliance on float types by adding fixed-point mathematics.
