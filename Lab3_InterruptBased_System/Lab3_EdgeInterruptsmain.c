// Lab3_EdgeInterruptsmain.c


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

// Negative logic bump sensors
// P4.7 Bump5, left side of robot
// P4.6 Bump4
// P4.5 Bump3
// P4.3 Bump2
// P4.2 Bump1
// P4.0 Bump0, right side of robot

#include <stdint.h>
#include "msp.h"
#include "../inc/Clock.h"
#include "../inc/CortexM.h"
#include "../inc/LaunchPad.h"
#include "../inc/Motor.h"
#include "../inc/BumpInt.h"
#include "../inc/TExaS.h"
#include "../inc/TimerA1.h"
#include "../inc/FlashProgram.h"

void TimedPause(uint32_t time){
  Clock_Delay1ms(time);          // run for a while and stop
  Motor_Stop();
  while(LaunchPad_Input()==0);  // wait for touch
  while(LaunchPad_Input());     // wait for release
}

uint8_t CollisionData, CollisionFlag;  // mailbox
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


int main(void){
  DisableInterrupts();
  Clock_Init48MHz();   // 48 MHz clock; 12 MHz Timer A clock
  LaunchPad_Init(); // built-in switches and LEDs
  // write this as part of Lab 14, section 14.4.4 Integrated Robotic System
  CollisionFlag = 0;
  Motor_Init();        // activate Lab 14 software
  BumpInt_Init(&HandleCollision);
  TimerA1_Init(&MotorMovt,50000);  // 10 Hz
  TExaS_Init(LOGICANALYZER_P4_765320);

  TimedPause(500);

  EnableInterrupts();
  while(1){
    WaitForInterrupt();
  }
}

