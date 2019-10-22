/**
 * @file      Scoreboard.h
 * @brief     Simple scoreboard in flash
 * @details   Test the functions provided by FlashProgram.c by implementing
 * a simple scoreboard for a video game to save and recall high
 * scores using flash memory.

 * @version   V1.0
 * @author    Valvano
 * @copyright Copyright 2017 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      April 7, 2016
 ******************************************************************************/

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

/**
 * \brief Number of initials and scores returned by Scoreboard_Get
 */
#define SCOREBOARDSIZE          5

/**
 * \brief structure for the scoreboard containing initials and numerical score
 */
struct ScoreboardElement{
  char first;      /**< Player's first initial */
  char middle;     /**< Player's middle initial */
  char last;       /**< Player's last initial */
  uint32_t score;  /**<  Player's numerical score*/
};
/**
 * \brief data type of a scoreboard entry containing initials and numerical score
 */
typedef struct ScoreboardElement SBEType;


/**
 * @details   Initialize the scoreboard in a given block of flash memory.
 * @note   This flash ROM block must be 4-KB aligned and must not contain any
 * data other than scores.
 * @param  addr 4-KB aligned flash memory address to hold scores
 * @return pointer to an array of Scoreboard elements of length SCOREBOARDSIZE
 * @brief  Initialize the scoreboard
 */
SBEType* Scoreboard_Init(uint32_t addr);


/**
 * @details   Record a score in the scoreboard, regardless of whether or
 * not the numerical score is high enough to be shown at the top.
 * @param  first  first initial
 * @param  middle middle initial
 * @param  last   last initial
 * @param  score  numerical score earned in the game
 * @return none
 * @brief  Record a score
 */
void Scoreboard_Record(char first, char middle, char last, uint32_t score);


/**
 * @details   Get the top 'SCOREBOARDSIZE' scores and return them in
 * descending order.
 * @param  none
 * @return pointer to an array of Scoreboard elements of length SCOREBOARDSIZE
 * @brief  Get the top scores from the scoreboard
 */
SBEType* Scoreboard_Get(void);
