// Lab08_LED_Switchesmain.c
// Runs on MSP432
// Student version of Switches_LED lab
// Daniel and Jonathan Valvano
// February 28, 2017

/* This example accompanies the books
   "Embedded Systems: Introduction to the MSP432 Microcontroller",
       ISBN: 978-1512185676, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Interfacing to the MSP432 Microcontroller",
       ISBN: 978-1514676585, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
       ISBN: 978-1466468863, , Jonathan Valvano, copyright (c) 2017
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2017, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/

// P5.0 is positive logic Activate, Toggle switch, internal pull-down
// P5.1 is positive logic Window1, momentary switch, internal pull-down
// P5.2 is positive logic Window2, momentary switch, internal pull-down
// P5.4 is positive logic alarm, LED
// Activate    Window                Alarm
//   off       doesn't matter        LED should be off
//   on    either window not pressed LED flashes 5Hz
//   on,   both sensors are pressed  LED should be off
#include <stdint.h>
#include "msp.h"
#include "..\inc\TExaS.h"
#include "..\inc\Clock.h"


// *********Debug software************
uint8_t sensor;
int Program8_1(void){
  Clock_Init48MHz();  // makes bus clock 48 MHz
  P5->SEL0 &= ~0x01;  // configure P5.0 GPIO
  P5->SEL1 &= ~0x01;
  P5->DIR &= ~0x01;   // make P5.0 in
  P5->REN |= 0x01;    // enable pull resistors on P5.0
  P5->OUT &= ~0x01;   // P5.0 pull-down
  while(1){
    sensor = P5->IN&0x01; // read switch
  }
}

int Program8_2(void){
  Clock_Init48MHz();  // makes bus clock 48 MHz
  TExaS_Init(LOGICANALYZER);
  P5->SEL0 &= ~0x01;  // configure P5.0 GPIO
  P5->SEL1 &= ~0x01;
  P5->DIR &= ~0x01;   // make P5.0 in
  P5->REN |= 0x01;    // enable pull resistors on P5.0
  P5->OUT &= ~0x01;   // P5.0 pull-down
  while(1){
    sensor = P5->IN&0x01; // read switch
    TExaS_Set(sensor);
  }
}
void LED_Init(void){
  P5->SEL0 &= ~0x01;  // configure P5.0 GPIO
  P5->SEL1 &= ~0x01;
  P5->DIR |= 0x01;    // make P5.0 output
}
void LED_On(void){
  P5->OUT |= 0x01;  // turn on
}
void LED_Off(void){
  P5->OUT &= ~0x01; // turn off
}
void LED_Toggle(void){
  P5->OUT ^= 0x01;  // change
}
int Program8_3(void){
  Clock_Init48MHz();  // makes bus clock 48 MHz
  LED_Init();         // activate output for LED
  while(1){
    LED_On();
    LED_Off();
  }
}
int Program8_4(void){
  Clock_Init48MHz();  // makes bus clock 48 MHz
  TExaS_Init(LOGICANALYZER_P5);
  LED_Init();         // activate output for LED
  while(1){
    LED_Toggle();
    Clock_Delay1ms(100);  // approximately 100 ms
  }
}
// *********Lab software************

void Security_Init(void){
    // write this code
}

// *********Security_InputActivate******
// read arm/disarm input
// inputs: none
// output: true if armed, false if disarmed
uint8_t Security_InputActivate(void){
  return 0; // replace this line
}
// *********Security_InputSensors******
// read window sensors input
// inputs: none
// output: 0 both not pressed
//         0x01 one pressed
//         0x02 the other pressed
//         0x03 both pressed
uint8_t Security_InputSensors(void){
  return 0; // replace this line
}
// *********Security_OutputAlarm******
// write to alarm
// inputs: 0 off
//         1 on
// output: none
void Security_OutputAlarm(uint8_t data){
    // write this code
}
// *********Security_ToggleAlarm******
// toggle alarm output
// inputs: none
// output: none
void Security_ToggleAlarm(void){
    // write this code
}

int main(void){
  Clock_Init48MHz(); // makes it 48 MHz
  TExaS_Init(LOGICANALYZER_P5);
  Security_Init();   // sensors and alarm

  while(1){
      // write this code
  }
}
