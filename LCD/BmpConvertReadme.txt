To create a bitmap for Nokia 5110
Jonathan Valvano, 1/24/2018
1) Create a bitmap file.  Since the Nokia 5110 is black and white,
   it will look best to have a white image on a black background.
   Also consider framing the image with a black border of at least
   one pixel on all four sides so the image can be slid without
   smearing.
   4-bit color, 16-color bmp
   width less than or equal to 84 pixels
   height less than or equal 48 pixels
   width must be even
   save the image as a 16-color bmp file
   store in same directory as BmpConvert.exe
2) Execute BmpConvert.exe
   Type the image name 
   E.g., if the file is ti.bmp, then type ti
3) Open the corresponding txt file, select all, copy
4) Open Code Composer Studio compiler
   Paste new image as a data constant
   (Optional) Change "unsigned char" to "uint8_t"
5) If the image is 84x48 pixels, print it in the top left corner with the following:
a) Clear the RAM image buffer
     Nokia5110_ClearBuffer();
b) Put the bitmap in the RAM image buffer
     Nokia5110_PrintBMP(0, 0, ti, 0);
   (Optional) The fourth parameter 0 makes any non-zero pixel 'on'.
   Depending on how colors are used, this might look bad in black
   and white.  Increase this parameter up to 14 to reject more
   colors from turning their corresponding pixels 'on'.
c) Display the RAM image buffer to the entire screen
     Nokia5110_DisplayBuffer();


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