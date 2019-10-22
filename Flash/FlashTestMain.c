// FlashTestMain.c
// Runs on MSP432
// Test the functions provided by FlashProgram.c by writing
// constant values to the flash memory and then finding
// them in the memory viewer in the debugger.
// Daniel Valvano
// April 13, 2016

/* This example accompanies the book
   "Embedded Systems: Real-Time Interfacing to the MSP432 Microcontroller",
   ISBN: 978-1514676585, Jonathan Valvano, copyright (c) 2016
   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2016

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

#include <stdint.h>
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/FlashProgram.h"
#include "Scoreboard.h"
#include "../inc/UART0.h"

// Flash ROM addresses must be 4k byte aligned, e.g., 0x00020000, 0x00021000, 0x00022000...
#define FLASH                   0x00020000  // location in flash to write; make sure no program code is in this block
#define SCOREBOARD              0x00022000  // location in flash for scoreboard

// Print the top scores from the scoreboard to the UART.
// Assumes that the UART is initialized.
void printscores(void){
  SBEType* Scoreboard;                              // pointer to array of ScoreboardElements holding top scores
  int i;
  UART0_OutString("High Scores:\r\n");
  Scoreboard = Scoreboard_Get();
  for(i=0; i<SCOREBOARDSIZE; i=i+1){
    UART0_OutChar(Scoreboard[i].first);              // print first initial
    UART0_OutChar(Scoreboard[i].middle);             // print middle initial
    UART0_OutChar(Scoreboard[i].last);               // print last initial
    UART0_OutChar(' ');
    UART0_OutChar(' ');
    UART0_OutUDec(Scoreboard[i].score);              // print the score
    UART0_OutChar('\r');
    UART0_OutChar('\n');
  }
}

// Return nonzero if character is an upper or lower case
// letter.
int isletter(char c){
  if((('A' <= c) && (c <= 'Z')) || (('a' <= c) && (c <= 'z'))){
    return 1;
  }
  return 0;
}

// Convert a lower case character to upper case and return it.
// Input that is not a lower case letter is unchanged.
char touppercase(char c){
  if(('a' <= c) && (c <= 'z')){
    c = c - ' ';                                    // convert to upper case
  }
  return c;
}

// Get new initials and score and record them in the
// scoreboard.  Input characters from the UART until three
// letters (a through z) are entered, which are the
// player's initials.  Convert them to upper case if
// needed.  Ignore non-letter characters, including
// backspace.  Then, input a 32-bit number, which is the
// score that the player would have earned in a game.
// Assumes that the UART is initialized.
void getnewscore(void){
  char first, middle, last;
  uint32_t score;
  UART0_OutString("Enter three letters: ");
  // get the first initial
  first = UART0_InChar();
  while(!isletter(first)){
    first = UART0_InChar();                          // repeat until a letter is pressed
  }
  first = touppercase(first);                       // convert to upper case
  UART0_OutChar(first);                              // echo the character
  // get the middle initial
  middle = UART0_InChar();
  while(!isletter(middle)){
    middle = UART0_InChar();                         // repeat until a letter is pressed
  }
  middle = touppercase(middle);                     // convert to upper case
  UART0_OutChar(middle);                             // echo the character
  // get the last initial
  last = UART0_InChar();
  while(!isletter(last)){
    last = UART0_InChar();                           // repeat until a letter is pressed
  }
  last = touppercase(last);                         // convert to upper case
  UART0_OutChar(last);                               // echo the character
  // get the score
  UART0_OutString("\r\nEnter your score as a number and hit ENTER: ");
  score = UART0_InUDec();
  UART0_OutChar('\r');
  UART0_OutChar('\n');
  Scoreboard_Record(first, middle, last, score);
}

// Perform a long test by writing stuff to each word of
// memory.  Once each word has been written, go back and
// check that each memory location has the expected
// contents.  After that, clear the flash.
//
// According to the discussion found in the forum thread:
// http://e2e.ti.com/support/microcontrollers/tiva_arm/f/908/t/367605.aspx
// The TM4C123 microcontroller may sometimes skip
// instructions following a flash memory write or erase.
// However, I have not been able to repeat the error using
// the programs and test functions in this file.  This test
// has functioned correctly ('errors' == 0 and no crashes)
// for nearly 24 hours of continuous writing and erasing
// operations and over 16,000 writes and erases to each
// memory location ('runs' > 16000).  Both a TM4C123 and an
// LM4F120 microcontroller were tested with the same
// results.  Nevertheless, it may still be better to move
// the contents of Flash_Write(), Flash_FastWrite(), and
// Flash_Erase() to SRAM, where code execution can continue
// during flash writes and erases.
// Assumes that the UART is initialized.
#define LTTSTART  0x00020000
#define LTTEND    0x0003FFFC
#define LTTWRITES ((LTTEND - LTTSTART + 4)/4)       // number of 32-bit words written during the Long Term Test
void longtermtest(void){
  uint32_t volatile *flashPtr;
  uint32_t static addr = LTTSTART;
  uint32_t static data = 0xFFFFFFFF;
  uint32_t static errors = 0;
  uint32_t static runs = 0;
  int i;
  Flash_Write(addr, data);
  addr = addr + 4;
  data = data - 1;
  if(addr > LTTEND){
    runs = runs + 1;
    UART0_OutString("Run #");
    UART0_OutUDec(runs);
    UART0_OutString(" complete.  ");
    flashPtr = (uint32_t volatile*)LTTSTART;
    i = 0;
    while(flashPtr <= (uint32_t volatile*)LTTEND){
      if(*flashPtr != (data + LTTWRITES - i)){
        errors = errors + 1;
      }
      flashPtr = flashPtr + 1;
      i = i + 1;
    }
    UART0_OutUDec(errors);
    UART0_OutString(" errors.\r\n");
    addr = LTTSTART;
    UART0_OutString("Erasing 0x");
    UART0_OutUHex(LTTSTART);
    UART0_OutString(" through 0x");
    UART0_OutUHex(LTTEND);
    UART0_OutChar('\r');
    UART0_OutChar('\n');
    for(i=LTTSTART; i<LTTEND; i=i+4096){
      Flash_Erase(i);
    }
  }
}

uint32_t DataArray[10];
uint32_t volatile *FlashPtr = (uint32_t volatile*)FLASH;
int SuccessfulWrites;
int main(void){
  uint32_t errors;
  int i;
  Clock_Init48MHz();                          // set system clock to 48 MHz
  Flash_Init(Clock_GetFreq()/1000000);        // initialize flash
  UART0_Init();                               // initialize UART
  LaunchPad_Init();                    // initialize LED digital interface
  DataArray[0] = 0x00001111;
  DataArray[1] = 0x00022220;
  DataArray[2] = 0x00333300;
  DataArray[3] = 0x04444000;
  DataArray[4] = 0x55550000;
  DataArray[5] = 0x66600007;
  DataArray[6] = 0x77000088;
  DataArray[7] = 0x80000999;
  DataArray[8] = 0x0000AAAA;
  DataArray[9] = 0x000BBBB0;
  Flash_Erase(FLASH);                               // erase 0x00020000 through 0x00020FFF
  Flash_Write(FLASH + 0, 0x10101010);               // write to location 0x00020000
  Flash_Write(FLASH + 4, 0x0BADBEEF);               // write to location 0x00020004
  Flash_Write(FLASH + 8, 0xBEEFF00D);               // write to location 0x00020008
  Flash_Write(FLASH + 0x1000, 0x45464153);          // write to location 0x00021000; this value should persist after this program is run (see note at bottom of file)
  Flash_Write(FLASH + 0x0FFC, 0x454E4F47);          // write to location 0x00020FFC
  Flash_Write(FLASH + 0x0FF8, 0x0FEDCBA0);          // write to location 0x00020FF8
  LaunchPad_Output(0);                           // red LED off; green LED off; blue LED off
  SuccessfulWrites = Flash_WriteArray(DataArray, FLASH + 9, 10);  // invalid address
  LaunchPad_Output(1);                           // red LED off; green LED off; blue LED on
  SuccessfulWrites = Flash_WriteArray(DataArray, FLASH + 12, 10); // use scope to measure J4.37 (TM4C123 PC4, MSP432 P5.6) high time (610 usec)
  LaunchPad_Output(0);                           // red LED off; green LED off; blue LED off
  SuccessfulWrites = Flash_FastWrite(DataArray, FLASH + 124, 10); // invalid address
  LaunchPad_Output(4);                           // red LED on; green LED off; blue LED off
  SuccessfulWrites = Flash_FastWrite(DataArray, FLASH + 128, 10); // use scope to measure J4.39 (TM4C123 PF3, MSP432 P2.6) high time (97 usec)
  LaunchPad_Output(0);                           // red LED off; green LED off; blue LED off
  // memory test
  errors = 0;
  UART0_OutString("\r\n");
  if(FlashPtr[0] != 0x10101010){
    UART0_OutString("Error at 0x");
    UART0_OutUHex(FLASH + 0);
    UART0_OutString(": expected 0x10101010 but actually 0x");
    UART0_OutUHex(FlashPtr[0]);
    UART0_OutString(".\r\n");
    errors = errors + 1;
  }
  if(FlashPtr[1] != 0x0BADBEEF){
    UART0_OutString("Error at 0x");
    UART0_OutUHex(FLASH + 4);
    UART0_OutString(": expected 0x0BADBEEF but actually 0x");
    UART0_OutUHex(FlashPtr[1]);
    UART0_OutString(".\r\n");
    errors = errors + 1;
  }
  if(FlashPtr[2] != 0xBEEFF00D){
    UART0_OutString("Error at 0x");
    UART0_OutUHex(FLASH + 8);
    UART0_OutString(": expected 0xBEEFF00D but actually 0x");
    UART0_OutUHex(FlashPtr[2]);
    UART0_OutString(".\r\n");
    errors = errors + 1;
  }
  for(i=0; i<10; i=i+1){
    if(FlashPtr[i+3] != DataArray[i]){
      UART0_OutString("Error at 0x");
      UART0_OutUHex(FLASH + 12 + 4*i);
      UART0_OutString(": expected 0x");
      UART0_OutUHex(DataArray[i]);
      UART0_OutString(" but actually 0x");
      UART0_OutUHex(FlashPtr[i+3]);
      UART0_OutString(".\r\n");
      errors = errors + 1;
    }
  }
  for(i=0; i<10; i=i+1){
    if(FlashPtr[i+32] != DataArray[i]){
      UART0_OutString("Error at 0x");
      UART0_OutUHex(FLASH + 128 + 4*i);
      UART0_OutString(": expected 0x");
      UART0_OutUHex(DataArray[i]);
      UART0_OutString(" but actually 0x");
      UART0_OutUHex(FlashPtr[i+32]);
      UART0_OutString(".\r\n");
      errors = errors + 1;
    }
  }
  if(FlashPtr[1022] != 0x0FEDCBA0){
    UART0_OutString("Error at 0x");
    UART0_OutUHex(FLASH + 0x0FF8);
    UART0_OutString(": expected 0x0FEDCBA0 but actually 0x");
    UART0_OutUHex(FlashPtr[1022]);
    UART0_OutString(".\r\n");
    errors = errors + 1;
  }
  if(FlashPtr[1023] != 0x454E4F47){
    UART0_OutString("Error at 0x");
    UART0_OutUHex(FLASH + 0xFFC);
    UART0_OutString(": expected 0x454E4F47 but actually 0x");
    UART0_OutUHex(FlashPtr[1023]);
    UART0_OutString(".\r\n");
    errors = errors + 1;
  }
  UART0_OutString("Memory test complete.  ");
  UART0_OutUDec(errors);
  UART0_OutString(" errors.\r\n\r\n");
  // scoreboard test
  Scoreboard_Init(SCOREBOARD);
  LaunchPad_Output(1);                           // red LED off; green LED off; blue LED on
/*  for(i=LTTSTART; i<LTTEND; i=i+4096){
    Flash_Erase(i);                                 // clear the flash so long term tests start at 0 errors; otherwise the first run will report errors since flash already contains data
  }*/
  while(1){ uint8_t led=3;
    printscores();
    getnewscore();
//    longtermtest();
    LaunchPad_Output(led);                      // toggle LEDs between blue and yellow
    led ^= 0x07;
  }
}

// Note on writing to location 0x00021000:
// The way flash memory works is that an erase sets all bits
// in a block to 1, and writing clears desired bits to 0.
// You cannot "write" individual bits back to 1's.  When
// FlashTestMain is compiled and run, there may already be
// data stored at 0x00021000.  After the line
// Flash_Write(FLASH + 0x1000, 0x45464153);
// the contents of flash location 0x00021000 will be the
// bitwise AND of the previous data contents and 0x45464153.
//
// To deal with this issue, you should erase the block once. E.g.,
// Flash_Erase(0x00021000);   // erase 0x00021000 through 0x00021FFF

// Another way to erase Flash is to use the Keil 5.14 debugger
// 1) Open the "Flash" menu and select "Configure Flash Tools...".
// 2) Select the "Utilities" sub-tab.
// 3) Click the "Settings" button near the line that says
//  "--- Use Debug Driver ---".
// 4) Click the radio button that says "Erase Full Chip".
// 5) Click both "OK" buttons.
// 6) Clear the flash and download your program with the regular
//  "Download" button.  In my experience, the debugger failed
//  after the full erase finished.  If this happens, repeat
//  Steps 1, 2, and 3.  Then in Step 4, click the radio button
//  that says "Erase Sectors".  You can also try the ultra-
//  scientific process of repeatedly disconnecting and re-
//  connecting the USB cable or restarting.
//
// This process guarantees that the flash memory at 0x0002.1000 is clear
//  (set to all 1's) in case a previously loaded program left data there.
