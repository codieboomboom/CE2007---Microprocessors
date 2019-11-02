// RSLK Self Test via UART

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

#include "msp.h"
#include <stdint.h>
#include <string.h>
#include "..\inc\UART0.h"
#include "..\inc\EUSCIA0.h"
#include "..\inc\FIFO0.h"
#include "..\inc\Clock.h"
//#include "..\inc\SysTick.h"
#include "..\inc\SysTickInts.h"
#include "..\inc\CortexM.h"
#include "..\inc\TimerA1.h"
//#include "..\inc\Bump.h"
#include "..\inc\BumpInt.h"
#include "..\inc\LaunchPad.h"
#include "..\inc\Motor.h"
#include "../inc/IRDistance.h"
#include "../inc/ADC14.h"
#include "../inc/LPF.h"
#include "..\inc\Reflectance.h"
#include "../inc/TA3InputCapture.h"
#include "../inc/Tachometer.h"

#define P2_4 (*((volatile uint8_t *)(0x42098070)))
#define P2_3 (*((volatile uint8_t *)(0x4209806C)))
#define P2_2 (*((volatile uint8_t *)(0x42098068)))
#define P2_1 (*((volatile uint8_t *)(0x42098064)))
#define P2_0 (*((volatile uint8_t *)(0x42098060)))

// At 115200, the bandwidth = 11,520 characters/sec
// 86.8 us/character
// normally one would expect it to take 31*86.8us = 2.6ms to output 31 characters
// Random number generator
// from Numerical Recipes
// by Press et al.
// number from 0 to 31
uint32_t Random(void){
static uint32_t M=1;
  M = 1664525*M+1013904223;
  return(M>>27);
}
char WriteData,ReadData;
uint32_t NumSuccess,NumErrors;
void TestFifo(void){char data;
  while(TxFifo0_Get(&data)==FIFOSUCCESS){
    if(ReadData==data){
      ReadData = (ReadData+1)&0x7F; // 0 to 127 in sequence
      NumSuccess++;
    }else{
      ReadData = data; // restart
      NumErrors++;
    }
  }
}
uint32_t Size;
int Program5_1(void){
//int main(void){
    // test of TxFifo0, NumErrors should be zero
  uint32_t i;
  Clock_Init48MHz();
  WriteData = ReadData = 0;
  NumSuccess = NumErrors = 0;
  TxFifo0_Init();
  TimerA1_Init(&TestFifo,43);  // 83us, = 12kHz
  EnableInterrupts();
  while(1){
    Size = Random(); // 0 to 31
    for(i=0;i<Size;i++){
      TxFifo0_Put(WriteData);
      WriteData = (WriteData+1)&0x7F; // 0 to 127 in sequence
    }
    Clock_Delay1ms(10);
  }
}

char String[64];
uint32_t MaxTime,First,Elapsed;
int Program5_2(void){
//int main(void){
    // measurement of busy-wait version of OutString
  uint32_t i;
  DisableInterrupts();
  Clock_Init48MHz();
  UART0_Init();
  WriteData = 'a';
  SysTick_Init(0x1000000,2); //OHL - using systick INT api
  MaxTime = 0;
  while(1){
    Size = Random(); // 0 to 31
    for(i=0;i<Size;i++){
      String[i] = WriteData;
      WriteData++;
      if(WriteData == 'z') WriteData = 'a';
    }
    String[i] = 0; // null termination
    First = SysTick->VAL;
    UART0_OutString(String);
    Elapsed = ((First - SysTick->VAL)&0xFFFFFF)/48; // usec

    if(Elapsed > MaxTime){
        MaxTime = Elapsed;
    }
    UART0_OutChar(CR);UART0_OutChar(LF);
    Clock_Delay1ms(100);
  }
}

int Program5_3(void){
//int main(void){
    // measurement of interrupt-driven version of OutString
  uint32_t i;
  DisableInterrupts();
  Clock_Init48MHz();
  EUSCIA0_Init();
  WriteData = 'a';
  SysTick_Init(0x1000000,2); //OHL - using systick INT api
  MaxTime = 0;
  EnableInterrupts();
  while(1){
    Size = Random(); // 0 to 31
    for(i=0;i<Size;i++){
      String[i] = WriteData;
      WriteData++;
      if(WriteData == 'z') WriteData = 'a';
    }
    String[i] = 0; // null termination
    First = SysTick->VAL;
    EUSCIA0_OutString(String);
    Elapsed = ((First - SysTick->VAL)&0xFFFFFF)/48; // usec
    if(Elapsed > MaxTime){
        MaxTime = Elapsed;
    }
    EUSCIA0_OutChar(CR);EUSCIA0_OutChar(LF);
    Clock_Delay1ms(100);
  }
}
int Program5_4(void){
//int main(void){
    // demonstrates features of the EUSCIA0 driver
  char ch;
  char string[20];
  uint32_t n;
  DisableInterrupts();
  Clock_Init48MHz();  // makes SMCLK=12 MHz
  EUSCIA0_Init();     // initialize UART
  EnableInterrupts();
  EUSCIA0_OutString("\nLab 5 Test program for EUSCIA0 driver\n\rEUSCIA0_OutChar examples\n");
  for(ch='A'; ch<='Z'; ch=ch+1){// print the uppercase alphabet
     EUSCIA0_OutChar(ch);
  }
  EUSCIA0_OutChar(LF);
  for(ch='a'; ch<='z'; ch=ch+1){// print the lowercase alphabet
    EUSCIA0_OutChar(ch);
  }
  while(1){
    EUSCIA0_OutString("\n\rInString: ");
    EUSCIA0_InString(string,19); // user enters a string
    EUSCIA0_OutString(" OutString="); EUSCIA0_OutString(string); EUSCIA0_OutChar(LF);

    EUSCIA0_OutString("InUDec: ");   n=EUSCIA0_InUDec();
    EUSCIA0_OutString(" OutUDec=");  EUSCIA0_OutUDec(n); EUSCIA0_OutChar(LF);
    EUSCIA0_OutString(" OutUFix1="); EUSCIA0_OutUFix1(n); EUSCIA0_OutChar(LF);
    EUSCIA0_OutString(" OutUFix2="); EUSCIA0_OutUFix2(n); EUSCIA0_OutChar(LF);

    EUSCIA0_OutString("InUHex: ");   n=EUSCIA0_InUHex();
    EUSCIA0_OutString(" OutUHex=");  EUSCIA0_OutUHex(n); EUSCIA0_OutChar(LF);
  }
}





/////////////////////////////////FOR IR SENSOR/////////////////////

//COPIED FROM LAB 4
volatile uint32_t ADCflag = 0;
volatile uint32_t nr,nc,nl;

void SensorRead_ISR(void){  // runs at 2000 Hz
  uint32_t raw17,raw12,raw16;
  P1OUT ^= 0x01;         // profile
  P1OUT ^= 0x01;         // profile
  ADC_In17_12_16(&raw17,&raw12,&raw16);  // sample
  nr = LPF_Calc(raw17);  // right is channel 17 P9.0
  nc = LPF_Calc2(raw12); // center is channel 12, P4.1
  nl = LPF_Calc3(raw16); // left is channel 16, P9.1
  ADCflag = 1;           // semaphore
  P1OUT ^= 0x01;         // profile
}

void IRSensor_Init(void){
    uint32_t raw17,raw12,raw16;
    uint32_t s;
    s = 256; // replace with your choice
    ADC0_InitSWTriggerCh17_12_16();   // initialize channels 17,12,16
    ADC_In17_12_16(&raw17,&raw12,&raw16);  // sample
    LPF_Init(raw17,s);     // P9.0/channel 17 right
    LPF_Init2(raw12,s);    // P4.1/channel 12 center
    LPF_Init3(raw16,s);    // P9.1/channel 16 left
    TimerA1_Init(&SensorRead_ISR,250);    // 2000 Hz sampling
    ADCflag = 0;
}

///////////////FOR BUMPER SWITCH//////////////////////////////////////

volatile uint8_t CollisionData, CollisionFlag;  // mailbox
void HandleCollision(uint8_t bumpSensor){
   Motor_Stop();
   CollisionData = bumpSensor;
   CollisionFlag = 1;
   P4->IFG &= ~0xED;                  // clear interrupt flags (reduce possibility of extra interrupt)
}

void MotorMovt(void){
    static uint32_t count=0;
    static uint8_t motor_state=0;

    //Write this as part of lab3 Bonus Challenge
    if(CollisionFlag){
            if(!(CollisionData&0x08) && !(CollisionData&0x04)){
                // if both bump sensors 2 & 3 are activated, motor state = 1
                // obstacle directly in front of robot
                motor_state = 1;
            }
            else if(!(CollisionData&0x20) || !(CollisionData&0x10) || !(CollisionData&0x08)){
                // if any sensors 3,4,5 are activated, motor state = 2
                // obstacle on the left side
                motor_state = 2;
            }
            else if(!(CollisionData&0x04) || !(CollisionData&0x02) || !(CollisionData&0x01)){
                // if any sensors 2,1,0 are activated, motor state = 3
                // obstacle on the right side
                motor_state = 3;
            }
            count = 0;
            CollisionFlag = 0;
            Clock_Delay1ms(500);
        }

     while(count<30){
        switch(motor_state){
        case 0:
            //No obstacle, continue moving forward
            Motor_Forward(3000, 3000);
            break;
        case 1:
            if(count<10){
                //direct obstacle in front, need move backward
                Motor_Backward(3000, 3000);
            }
            else if(count> 10 && count < 20){
                //rotate to the right
                Motor_Right(2000, 2000);
            }
            else if(count> 20 && count < 30){
                //safe to move forward a bit again, test water lah
                Motor_Forward(3000, 3000);
                motor_state = 0;
            }
            break;
        case 2:
            // obstacle on the left side
            if(count<10){
                Motor_Backward(3000, 3000);
            }
            else if(count> 10 && count < 20){
                Motor_Right(2000, 2000);
            }
            else if(count> 20 && count < 30){
                Motor_Forward(3000, 3000);
                motor_state = 0;
            }
            break;
        case 3:
            // obstacle on the right side
            if(count<10){
                Motor_Backward(3000, 3000);
            }
            else if(count> 10 && count < 20){
                //only can move left
                Motor_Left(2000, 2000);
            }
            else if(count> 20 && count < 30){
                Motor_Forward(3000, 3000);
                motor_state = 0;
            }
            break;
        default:
            break;
        }

     count++;
    }
}
///////////////////////////////////TACHOMETER//////////////////////////////////////
uint16_t Period0;              // (1/SMCLK) units = 83.3 ns units
uint16_t First0;               // Timer A3 first edge, P10.4
int Done0;                     // set each rising
// max period is (2^16-1)*83.3 ns = 5.4612 ms
// min period determined by time to run ISR, which is about 1 us
void PeriodMeasure0(uint16_t time){
  P2_0 = P2_0^0x01;           // thread profile, P2.0
  Period0 = (time - First0)&0xFFFF; // 16 bits, 83.3 ns resolution
  First0 = time;                   // setup for next
  Done0 = 1;
}

uint16_t Period2;              // (1/SMCLK) units = 83.3 ns units
uint16_t First2;               // Timer A3 first edge, P8.2
int Done2;                     // set each rising
// max period is (2^16-1)*83.3 ns = 5.4612 ms
// min period determined by time to run ISR, which is about 1 us
void PeriodMeasure2(uint16_t time){
  P2_2 = P2_2^0x01;           // thread profile, P2.4
  Period2 = (time - First2)&0xFFFF; // 16 bits, 83.3 ns resolution
  First2 = time;                   // setup for next
  Done2 = 1;
}

void TimedPause(uint32_t time){
  Clock_Delay1ms(time);          // run for a while and stop
  Motor_Stop();
  while(LaunchPad_Input()==0);  // wait for touch
  while(LaunchPad_Input());     // wait for release
}

#define PERIOD 1000  // must be even

void toggle_GPIO(void){
    P2_4 ^= 0x01;     // create output
}

uint32_t main_count=0;
//////////////////////////////////RESET////////////////////////////////////////
void RSLK_Reset(void){
    DisableInterrupts();

    LaunchPad_Init();
    //Initialise modules used e.g. Reflectance Sensor, Bump Switch, Motor, Tachometer etc
    // ... ...
    CollisionFlag = 0;
    CollisionData = 0x63;

    EnableInterrupts();
}
////////////////////////////////////////// RSLK Self-Test/////////////////////
int main(void) {
  uint32_t cmd=0xDEAD, menu=0;
  uint8_t RefData;
  CollisionFlag = 0;
  CollisionData = 0x3F;


  DisableInterrupts();
  Clock_Init48MHz();  // makes SMCLK=12 MHz
  //SysTick_Init(48000,2);  // set up SysTick for 1000 Hz interrupts
  Motor_Init();
  Motor_Stop();
  LaunchPad_Init();
  Reflectance_Init();
  IRSensor_Init();
  BumpInt_Init(&HandleCollision);
    //Bump_Init();
  //  Bumper_Init();
  Tachometer_Init(); //this prevents words from appearing
  EUSCIA0_Init();     // initialize UART
  EnableInterrupts();

  while(1){                     // Loop forever
      // write this as part of Lab 5
      EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
      EUSCIA0_OutString("RSLK Testing"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
      EUSCIA0_OutString("[0] RSLK Reset"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
      EUSCIA0_OutString("[1] Motor Test"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
      EUSCIA0_OutString("[2] Bumper Test"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
      EUSCIA0_OutString("[3] Reflectance Sensors Test"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
      EUSCIA0_OutString("[4] IR Sensors Test"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
      EUSCIA0_OutString("[5] Tachometer Test"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);

      EUSCIA0_OutString("CMD: ");
      cmd=EUSCIA0_InUDec();
      EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);

      switch(cmd){
////////////////////////////////////////////////////////////////////////////////////////////////////
          case 0:
              EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("RSLK Resetting... Please Wait and Don't Smash Robot"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              RSLK_Reset();
              menu =1;
              cmd=0xDEAD;
              break;
////////////////////////////////////////////////////////////////////////////////////////////////////
          case 1:
              //MOTOR TEST WITH PWM FROM TIMER
              //UI
              EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("RSLK Motor Testing Loading..."); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("Please Select Test Mode (0-3)"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("[0] Motor Forward "); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("[1] Motor Backward"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("[2] Motor Left"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("[3] Motor Right"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("[4] Go Back To Menu"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("Choice: ");
              uint32_t choice =EUSCIA0_InUDec();
              EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              uint32_t left_pwm;
              uint32_t right_pwm;
              switch (choice){
                  case 0:
                      //move motor forward with PWM, need ask user what PWM they want
                      EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                      EUSCIA0_OutString("Motor Forward Test. Please input PWM (0 - 14998) for motor left, right!"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                      EUSCIA0_OutString("LEFT: ");
                      left_pwm = EUSCIA0_InUDec();
                      //users are stupid...
                      if (left_pwm > 14998)
                        left_pwm = 14998;

                      EUSCIA0_OutString("RIGHT: ");
                      right_pwm = EUSCIA0_InUDec();
                      //users are stupid again...
                      if (left_pwm > 14998)
                        right_pwm = 14998;


                      //Time to buckle up
                      Motor_Forward(left_pwm, right_pwm);
                      Clock_Delay1ms(3000); //delay 3s
                      Motor_Stop();
                      break;
                  case 1:
                        //move motor backward with PWM, need ask user what PWM they want
                        EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                        EUSCIA0_OutString("Motor Backward Test. Please input PWM (0 - 14998) for motor left, right!"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                        EUSCIA0_OutString("LEFT: ");
                        left_pwm = EUSCIA0_InUDec();
                        //users are stupid...
                        if (left_pwm > 14998)
                          left_pwm = 14998;

                        EUSCIA0_OutString("RIGHT: ");
                        right_pwm = EUSCIA0_InUDec();
                        //users are stupid again...
                        if (left_pwm > 14998)
                          right_pwm = 14998;


                        //Time to buckle up
                        Motor_Backward(left_pwm, right_pwm);
                        Clock_Delay1ms(3000); //delay 3s
                        Motor_Stop();
                        break;

                  case 2:
                        //move motor left with PWM, need ask user what PWM they want
                        EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                        EUSCIA0_OutString("Motor Left Test. Please input PWM (0 - 14998) for motor left!"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                        EUSCIA0_OutString("LEFT: ");
                        left_pwm = EUSCIA0_InUDec();
                        //users are stupid...
                        if (left_pwm > 14998)
                          left_pwm = 14998;
                        else if (left_pwm < 0)
                          left_pwm = 0;
                        right_pwm = 0;

                        //Time to buckle up
                        Motor_Left(left_pwm, right_pwm);
                        Clock_Delay1ms(3000); //delay 3s
                        Motor_Stop();
                        break;

                  case 3:
                        //move motor right with PWM, need ask user what PWM they want
                        EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                        EUSCIA0_OutString("Motor Right Test. Please input PWM (0 - 14998) for motor right!"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                        EUSCIA0_OutString("RIGHT: ");
                        right_pwm = EUSCIA0_InUDec();
                        //users are stupid again...
                        if (left_pwm > 14998)
                          right_pwm = 14998;

                        left_pwm = 0;

                        //Time to buckle up
                        Motor_Right(left_pwm, right_pwm);
                        Clock_Delay1ms(3000); //delay 3s
                        Motor_Stop();
                        break;
                  default:
                      break;
              }
              menu = 1;
              cmd=0xDEAD;
              break; //END OF MOTOR TEST
/////////////////////////////////////////////////////////////////////////////////////////////////////
          case 2:
                //BUMPER SWITCH TEST
                EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                EUSCIA0_OutString("RSLK Bumper Switch Loading..."); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                EUSCIA0_OutString("Please Select Test [0-1]"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                EUSCIA0_OutString("[0] Edge Interrupt Bump Switches Contact"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                EUSCIA0_OutString("[1] Individual Bumper Test"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                EUSCIA0_OutString("[2] Go Back To Main Menu"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                EUSCIA0_OutString("Choice: ");
                choice =EUSCIA0_InUDec();
                EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);

                switch(choice){
                    case 0:
                        //TEST BUMP SWITCH MECHANISM (multiple bumps)
                        EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                        EUSCIA0_OutString("Waiting for Interrupt To Happen....."); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                        while (1){
                            WaitForInterrupt(); //wait for bumper to be switched
                            if (CollisionFlag == 1)
                                break;
                        }
                        EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                        EUSCIA0_OutString("Bumper Switches triggered interrupt, investigating..."); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                        EUSCIA0_OutString("Bumper Switches Readings: "); EUSCIA0_OutUDec(CollisionData);
                        EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                        for(uint8_t x=1; x<7;x++ ){
                            if(CollisionData %2 == 0){
                                EUSCIA0_OutString("Bumper Switch ");
                                EUSCIA0_OutUDec(x);
                                EUSCIA0_OutString(" Held. ");
                                EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                            }
                            CollisionData = CollisionData >> 1;
                        }
                        CollisionData = 0x3F;
                        EUSCIA0_OutString("Bump Switches are studied... Returning to Menu"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                        EUSCIA0_OutString("Select Individual Bumper for continously checking next time!"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                        menu=1;
                        cmd=0xDEAD;
                        break;

                    case 1:
                        //Nothing contacted 63 = 0x3F
                        EUSCIA0_OutString("Individual Bumper Test. Please press any bumper!"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                        EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                        while (1){
                            Clock_Delay1ms(3000); //delay cuz humans are slow poke
                            for(uint8_t x=1; x<7;x++ ){
                                if(CollisionData %2 == 0){
                                    EUSCIA0_OutString("Bumper Switch ");
                                    EUSCIA0_OutUDec(x);
                                    EUSCIA0_OutString(" Contacted. ");
                                    EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                                    break; //only 1 bumper tested.
                                }
                                CollisionData = CollisionData >> 1;
                            }
                            CollisionData = 0x3F; //Reset
                            EUSCIA0_OutString("Continue?"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                            EUSCIA0_OutString("[0] Yes"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                            EUSCIA0_OutString("[1] No, go back to Menu"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                            EUSCIA0_OutString("Choice: ");
                            choice =EUSCIA0_InUDec();
                            if (choice == 1)
                                break; //stop if input from user is 1
                        }
                        break;

                    default:
                        break;
                }
                menu = 1;
                cmd=0xDEAD;
                break;
///////////////////////////////////////////////////////////////////////////////////////////////////////
         case 3:
             EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
             EUSCIA0_OutString("RSLK Reflectance Sensors Test Loading..."); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
             EUSCIA0_OutString("Testing 10 samples at interval of 1s"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              for(int x=0; x<10;x++){
                  RefData = Reflectance_Read(1000);
                  EUSCIA0_OutString("Reflectance Sensor Data: ");
                  EUSCIA0_OutUHex(RefData);EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
                  Clock_Delay1ms(1000);
              }
              menu = 1;
              cmd=0xDEAD;
              break;
//////////////////////////////////////////////////////////////////////////////////////////////////
          case 4:
              EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("RSLK IR Sensors Test Loading..."); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("Testing 10 samples"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              for(int i = 0; i<10; i++){
                  for(int n=0; n<2000; n++){
                      while(ADCflag == 0){};
                      ADCflag = 0; // show every 2000th point
                  }
                  UART0_OutUDec5(LeftConvert(nl));UART0_OutString(" cm,");
                  UART0_OutUDec5(CenterConvert(nc));UART0_OutString(" cm,");
                  UART0_OutUDec5(RightConvert(nr));UART0_OutString(" cm\r\n");
              }
              menu = 1;
              cmd=0xDEAD;
              break;
//////////////////////////////////////////////////////////////////////////////////////////////////////
          case 5:
              EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              EUSCIA0_OutString("Tachometer Testing"); EUSCIA0_OutChar(CR); EUSCIA0_OutChar(LF);
              TimerA1_Init(&toggle_GPIO,10);    // 50Khz sampling
              TimerA3Capture_Init(&PeriodMeasure0,&PeriodMeasure2);
              Clock_Delay1ms(500);
              Motor_Forward(3000,3000); // 50%
              for(int i = 0; i<500; i++){
                  main_count++;
                  if(main_count%10000){
                      UART0_OutString("Period0 = ");
                      UART0_OutUDec5(Period0);
                      UART0_OutString(" Period2 = ");
                      UART0_OutUDec5(Period2);
                      UART0_OutString(" \r\n");
                  }
              }
              Motor_Stop();
              menu=1;
              cmd=0xDEAD;
                break;
          // ....
          // ....

        default:
          menu=1;
          break;
      }

      if(!menu)Clock_Delay1ms(3000);
      else{
          menu=0;
      }

      // ....
      // ....
  }
}
