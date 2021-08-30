# Tips and Tricks
Here are some useful tips or knowledge you may find useful.

## Documentation types
You may be overwhelmed by the amount of documentation available by the board manufacturer that 
we need to read for this course in order to complete the lab. There are two type of documents that
I found useful for most of the time:

- User Manual for the MCU board: This document describe the general features for the TI-RSLK Board 
that we need to program with, including information on peripherals binding (e.g. LEDs on the board
is binding to which GPIO peripherals, which port, etc).
- Reference Manual for the Microprocessor (ARM Cortex M4 by TI) that is located in the middle/core of the
TI-RSLK board. This reference manual documents in details how should the peripherals be set up, the layout 
of register blocks, etc.

As such, a good flow in programming a feature such as turning an LED light on and off might be as following:
- Find the label of the LED light on your MCU board (let say it is L3).
- Read the user manual for the MCU board and find out which peripheral (GPIO) the L3 LED is bounded to. Normally, LED
is bounded to a pin with format such as PE9 (this depends on which manufacture (TI, STM32, etc of the board)). This means,
we have to deal with GPIO peripherals of port E, Pin 9.
- Read the reference manual and find out the base address of GPIO E, the registers we need to interact with, and its offset
from the base address.
- Read/Write the required action to the base + offset address to interact with the register as necessary.

## Other good resources