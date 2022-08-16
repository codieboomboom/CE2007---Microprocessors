# CE2007 - Microprocessors

## Course Overview
This module has been revamped by SCSE NTU in AY19/20 with special details toward the Lab sessions to prepare its students better for the Embedded industry. Unlike previous syllabus where labs are surrounded the ARM Cortex M0 processor and emulation of this processor using FPGA, the new lab series are about Cortex M4F processor. Student get to learn about the basics of ARM microprocessors such as instruction set, effecient Embedded C programming,...

The main goals of this project, as I have learnt are:
- Developing a "Board Support Package" for the complete robot (which include the Hardware Abstraction Layer of the MCU, various device drivers such as motors, tachometer, etc)
- Developing simple embedded application for this robot based on the BSP.
- Embedded C fundamentals
- Bench equipments funadmentals (Oscilloscope, etc).

## Logistics
The course logistics are as follow:
+ TI-RSLK Maze Edition Robot kit (provided by school)
More info: https://www.element14.com/community/docs/DOC-88585/l/introducing-the-ti-robotics-system-learning-kit-ti-rslk
+ Lab Src Template (provided on NTULearn)
+ TI Code Composer Studio IDE (provided by TI website)
+ TI-RSLK full syllabus, documentation and datasheets
More info: https://university.ti.com/en/faculty/ti-robotics-system-learning-kit/ti-robotics-system-learning-kit/curriculum-design-launch

*Do note that this course dwells with the bare-metal implementation of programs on the target architecture, in contrast to CE3003, which deals with RTOS implementation on the same microcontroller platform.*

## Lab & Assessment
The coursework program was adapted from the TI-RSLK University Program curriculum, however this should include enough concepts for students to work with any bare-metal system. The breakdown for labs and courseworks are as follow:

Lab 1: Introduction about Cortex M4F, ARM assembly, Embedded C programming, TI RSLK robot kit.

Lab 2: Introduction about GPIO and Interrupts in Cortex M4F.

Lab 3: Introduction about Timer Compare interrupt.

Lab 4: Introduction about Timer Capture interrup and ADC.

Lab 5: Assessment with Self-test program of the robot.

Assessment:
- All Lab handouts
- Lab 5 Self test program: basic 4 features + any add on. Highly advice to implement the most crazy things you can.

## A note on BSP, HAL and what not
*Abstraction layers* are the most fundamental thing about software development that students in SCSE should learn about. On one extreme, you have established through Digital Logic course that logic gates like AND, OR, XOR, NOT are the building blocks of computer hardware. On the other extreme, you have modules like Software Engineering where you learn how to develop web-apps that are written in high-level languages like js, go, etc and completely abstracted away from details like how your application translate a HTTPs request down to the port level and send the signal across the network line.

This course seek to build a BSP of the robot kit. BSP composes of external device drivers and BSP of the MCU eval board (which composes of HAL of the MCU unit in the middle of the board). For easier understanding, here is roughly a few definition that I think might be useful for you when dealing more with embedded systems (although, the definitions are pretty murky in the fields). An illustrated image is avail below (although it is about Rust libraries implementation of embedded systems)

![image](https://user-images.githubusercontent.com/32509869/184795340-1974442d-3446-480f-a504-d0f957429b71.png)

### Micro-processor libraries
- Library that support functionalities that is common to all processor core of the same family.
- For this course and for most part of embedded programming, you are dealing with Cortex-M core processor, but there are many more processor family like MIPS, RISC-V, etc.
- Example on functionalities or routines that common to all Cortex-M cores and should be implemented here are: Systick Timer, Reset Handler, etc
- Complexity of use: very high as setting things up demand you to do setting, resetting, masking of bits of registers.
- Most of the time, some for of memory-mapped registers are available (#define REGISTER_NAME 0xXXXX_XXXX)

### Peripherals Access Libraries
- MCU = Microprocessor (Cortex-M) plus peripherals such as ADC, GPIO, PWM, Timer lines. This kind of lib establish the memory maps of those lines and help us set them up
- Complexity of use: very high as setting things up demand you to do setting, resetting, masking of bits of registers
- Most of the time, some for of memory-mapped registers are available (#define REGISTER_NAME 0xXXXX_XXXX)

### Hardware Abstraction Layer
- HAL seek to abstract the low-level setup (by bit operations on memory mapped device) from user. 
- Instead of checking which register's bit to set to enable a GPIO pin to be output or input or the mode they need to be operating in, user can just call GPIO_init()
- This is pretty much how working with Arduino looks like if you have to use it in MDP.
- HAL tends to contain Micro-processor libraries and HAL underneath it and expose high level API to user.
- Complexity of use: medium.

### Board Support Package
- For each evaluation board or PCB board that we are using, the BSP is a complete support of all devices on it (LEDs, Accelerometer, Bluetooth, etc).
- Instead of writing drivers for each of this devices, which again include us to know which pin mode they need to be set (LED needs GPIO to be pin out, etc), we simply call LED_init()
- Implementation on how these drivers works are abstracted away.
- Complexity of use: medium to low.

With BSP, an application programmer can develop very fast (refer to MDP course where calling functions for motors or IR sensors just work) but their control on the system is very limited (can't control the nitty gritty details of the hardware). Conversely for PAL or Micro-processor libraries, we can control each register and their bits content but this will waste a lot of time. In developing a HAL, BSP through this course, you will learn more how things work on the low-level and make adjustment to your BSP/HAL should there be a need in the future!

## This Repository
The repo can be navigated as following:
- Lab Manual and Handouts store the lab handouts that students are expected to submit after every lab session, based on what they learn in the lab session.
- Labxref_xxxx stores the reference code for every lab session. These modules serve as a guide of student on how to prototype certain things and are meant to be used for studying before lab.
- Labx-xxxx stores the source code for the lab sessions.
- For the aspiring student, a look into inc/ folder will show you the gist of how HAL/BSP should contains: a bunch of header files defining the functionalitiy that application can call and a bunch of .c files implementing those APIs at lower level (flipping bit of registers for example)

## Other tips
Some [tips and tricks](./TIPS.md) to get yourself going with embedded programming

## Donate
If I saved your ass during CE2007, please consider buying me a coffee:
- BTC: bc1qd57am435yy2wamsy8h7pjfwgw9l8fvpts49snt
- ETH/MATIC/AMP: 0x5824Ca736409ABb3dE87B6ab7328E7Ec3B14C086


