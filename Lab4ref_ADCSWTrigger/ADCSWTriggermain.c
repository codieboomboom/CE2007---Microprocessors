// ADCSWTriggermain.c
// Runs on MSP432
// 1000Hz ADC statistical data printed to UART as a measure of resolution
// UART runs at 115,200 baud rate.
// Daniel Valvano
// July 14, 2017
// input on P4.7 = A6
// ADC input, software trigger, 14-bit conversion,
// 3.3V reference

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

#include <stdio.h>
#include <stdint.h> // C99 variable types
#include "msp.h"
#include "../inc/Clock.h"
#include "../inc/CortexM.h"
#include "../inc/ADC14.h"
#include "../inc/UART0.h"
#include "../inc/SysTickInts.h"
#include "../inc/LaunchPad.h"

volatile uint32_t ADCvalue;
volatile uint32_t ADCflag;
uint32_t Size;      // Size-point average, Size=1 to 512
uint32_t x[1024];   // two copies of MACQ
uint32_t *Pt;       // pointer to current
uint32_t LPFSum;    // sum of the last Size samples
void LPF_Init(uint32_t initial, uint32_t size){ int i;
  if(size>512) size=512; // max
  Size = size;
  Pt = &x[0];
  LPFSum = Size*initial; // prime MACQ with initial data
  for(i=0; i<2*Size; i++){
    x[i] = initial;
  }
}

// calculate one filter output, called at sampling rate
// Input: new ADC data   Output: filter output, DAC data
// y(n) = (x(n)+x(n-1)+…+x(n-Size-1)/Size
uint32_t LPF_Calc(uint32_t newdata){
  if(Pt == &x[0]){
    Pt = &x[Size-1];             // wrap
  } else{
    Pt--;                         // make room for data
  }
  LPFSum = LPFSum+newdata -*Pt; // subtract oldest, add newest
  *Pt = *(Pt+Size) = newdata;     // two copies of the new data
  return LPFSum/Size;
}

void SysTick_Handler(void){
  uint32_t RawADC;
  P1OUT ^= 0x01;                   // profile
  P1OUT ^= 0x01;                   // profile
  RawADC = ADC_In6();
  ADCvalue = LPF_Calc(RawADC);
  ADCflag = 1;                   // semaphore
  P1OUT ^= 0x01;                   // profile
}
#define N 1024
uint32_t Data[N];
#define M 256
uint16_t Histogram[M];
uint32_t Sum;      // sum of data
uint32_t Sum2;     // sum of (data-average)^2
uint32_t Average;  // average of data = sum/N
uint32_t Variance; // =sum2/(N-1)
uint32_t Sigma;    // standard deviation = sqrt(Variance)
// Newton's method
// s is an integer
// sqrt(s) is an integer
uint32_t sqrt(uint32_t s){
uint32_t t;   // t*t will become s
int n;             // loop counter
  t = s/16+1;      // initial guess
  for(n = 16; n; --n){ // will finish
    t = ((t*t+s)/t)/2;
  }
  return t;
}

int main(void){

  int32_t n; uint32_t min,max,s=32;

  Clock_Init48MHz();
  ADCflag = 0;
  ADC0_InitSWTriggerCh6();   // initialize ADC sample P4.7/A6
  LPF_Init(ADC_In6(),s);
  UART0_Init();              // initialize UART0 115,200 baud rate
  LaunchPad_Init();
  SysTick_Init(48000,2);     // 1000 Hz sampling
  UART0_OutString("ADCSWTrigger project\r\nValvano July 2017\r\n1000 Hz 14-bit sampling\r\nConnect analog signal to P4.7\r\n");
  EnableInterrupts();
  while(1){
     UART0_OutString("\r\nADC resolution test\r\n");
     LPF_Init(ADC_In6(),s);
     for(n=0; n<2*s; n++){
      while(ADCflag == 0){};
      ADCflag = 0; // skip the first 2*s
    }
    UART0_OutString("Collect "); UART0_OutUDec(N); UART0_OutString(" samples\r\n");
    Sum = Sum2 = 0;
    for(n=0; n<N; n++){
      while(ADCflag == 0){};
      Sum = Sum+ADCvalue;            // 14bits*100 = 17 bits
      Data[n] = ADCvalue;
      ADCflag = 0;
    }
    Average = Sum/N;
    for(n=0; n<N; n++){
      Sum2 = Sum2+(Data[n]-Average)*(Data[n]-Average); // 28bits*100 = 31 bits
    }
    Variance = (100*Sum2)/(N-1);
    Sigma = sqrt(Variance);

    min = 16384; max = 0;
    for(n=0; n<N; n++){
      if(Data[n] < min)
        min = Data[n]; // smallest
      if(Data[n] > max)
        max = Data[n]; // largest
      }
    for(n=0;n<M;n++){
      Histogram[n] = 0;
    }
    for(n=0; n<N; n++){
      if((Data[n]>=min)&&(Data[n]<min+M)){
        Histogram[Data[n]-min]++;
      }
    }
    UART0_OutString("Probability Mass Function (PMF)\r\n Data  Count\r\n");
    for(n=0;n<M;n++){
      if(Histogram[n]){
       // UART0_OutUDec5(n+min); UART0_OutString("  \t");UART0_OutUDec(Histogram[n]);UART0_OutChar('\r\n');
          UART0_OutUDec5(n+min); UART0_OutString("  \t");UART0_OutUDec(Histogram[n]);UART0_OutString("\r\n");
      }
    }

    UART0_OutString("FIR filter "); UART0_OutUDec(s); UART0_OutString("-point average\r\n");
    UART0_OutString("Average      ="); UART0_OutUDec(Average); UART0_OutString("\r\n");
    UART0_OutString("Range        ="); UART0_OutUDec(max-min); UART0_OutString("\r\n");
    UART0_OutString("Variance     ="); UART0_OutUFix2(Variance); UART0_OutString("\r\n");
    UART0_OutString("Sigma        ="); UART0_OutUFix1(Sigma); UART0_OutString("\r\n");
    UART0_OutString("Average/Sigma="); UART0_OutUFix1((100*Average)/Sigma); UART0_OutString("\r\n");

    /*
    UART0_OutString("Average      ="); UART0_OutUDec(Average); UART0_OutChar('\r\n');
    UART0_OutString("Range        ="); UART0_OutUDec(max-min); UART0_OutChar('\r\n');
    UART0_OutString("Variance     ="); UART0_OutUFix2(Variance); UART0_OutChar('\r\n');
    UART0_OutString("Sigma        ="); UART0_OutUFix1(Sigma); UART0_OutChar('\r\n');
    UART0_OutString("Average/Sigma="); UART0_OutUFix1((100*Average)/Sigma); UART0_OutChar('\r\n');
    */

    while(LaunchPad_Input()==0x00){}; // wait for touch
    while(LaunchPad_Input()!=0x00){}; // wait for release
    if(s<512) s = s*2; // 1,2,4,8,16,32,64,...,512
    else s = 1;
  }
}
