// PeriodMeasure.c
// Runs on MSP432
// Use Timer A0 in 16-bit capture mode to request interrupts on the
// rising edge of P7.3 (TA0CCP0), and measure period between pulses.
// Daniel Valvano
// July 24, 2017

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


#include <stdint.h> // C99 variable types
#include "msp.h"
#include "../inc/Clock.h"
#include "../inc/CortexM.h"
#include "../inc/TA0InputCapture.h"
#include "../inc/UART0.h"
#include "../inc/TimerA1.h"

#define P1_0 (*((volatile uint8_t *)(0x42098040)))
#define P2_4 (*((volatile uint8_t *)(0x42098070)))
#define P2_3 (*((volatile uint8_t *)(0x4209806C)))
#define P2_2 (*((volatile uint8_t *)(0x42098068)))
#define P2_1 (*((volatile uint8_t *)(0x42098064)))
#define P2_0 (*((volatile uint8_t *)(0x42098060)))

uint16_t Period;              // (1/SMCLK) units = 83.3 ns units
uint16_t First;               // Timer A0 first edge
int Done;                     // set each rising
// max period is (2^16-1)*83.3 ns = 5.4612 ms
// min period determined by time to run ISR, which is about 1 us
void PeriodMeasure(uint16_t time){
  P2_0 = P2_0^0x01;           // thread profile, P2.0
  Period = (time - First)&0xFFFF; // 16 bits, 83.3 ns resolution
  First = time;                   // setup for next
  Done = 1;
  //Period =0;
}

void toggle_GPIO(void){
    P2_4 ^= 0x01;     // create output
}

#define PERIOD 1000  // must be even
// connect P2.4 output to P7.3
// creates a PERIOD (us) wave out P2.4
uint32_t main_count=0;

int main(void){
  Clock_Init48MHz();  // 48 MHz clock; 12 MHz Timer A clock
  First = 0;          // first will be wrong
  Done = 0;           // set on subsequent
  UART0_Init();          // initialize UART0 115,200 baud rate
  TimerA1_Init(&toggle_GPIO,10);    // 50Khz sampling
  TimerA0Capture_Init(&PeriodMeasure);// initialize Timer A0 in capture mode
  P2->SEL0 &= ~0x11;
  P2->SEL1 &= ~0x11;  // configure P2.0 and P2.4 as GPIO
  P2->DIR |= 0x11;    // P2.0 and P2.4 outputs
  EnableInterrupts();
  while(1){
      WaitForInterrupt();
      main_count++;
      if(main_count%1000){
          UART0_OutString("Period = ");UART0_OutUDec5(Period);UART0_OutString(" \r\n");
      }
    //P2_4 ^= 0x01;     // create output
    //Clock_Delay1us(PERIOD/2);
  }
}
