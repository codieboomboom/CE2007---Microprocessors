# Tips and Tricks
Here are some useful tips or knowledge you may find useful.

## But why Embedded C and not Arduino?
Arduino and IDE includes a series of high level library that abstract away the underlying details
of hardware platform we are interacting with. Technically, it is a Hardware Abstraction Layer and
is meant to make coding portable across different hardware platform (AVR chip, STM32, TI chips that support
Arduino HAL)

Although simple to work with (some of you taking MDP will see how fast you can move a motor with little 
effort of finding a library on the Arduino Library List). How the library is implemented is hidden to us
and sometimes, the settings those libraries have selected in their implementations is not optimal to our task.
For example, a timer done using the Arduino HAL might be 2Hz while if we choose the hard-core Embedded C path, 
we can do it at 10MHz.

Yes, Embedded C is hard, and you will probably (like me), pull your hair off to get to the point that you can
blink an LED, but doing so give you an intricated understand about embedded systems, and I guess that was the point of
this course to begin with.

If you just want to play around with the hardware from the lab, consider trying Energia SDK, an Arduino compatible 
library for the TI-RSLK kit. But the fun is always in working in lower level code :)

Good luck

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
