// UltrasoundLCDmain.c
// Runs on MSP432
// Test the operation of the HC-SR04 ultrasonic sensor by
// repeatedly taking measurements.  For stand-alone
// operation, the measurements are printed to the
// Nokia 5110 LCD.
// Daniel Valvano
// May 4, 2017

/* This example accompanies the books
   "Embedded Systems: Introduction to the MSP432 Microcontroller",
   ISBN: 978-1512185676, Jonathan Valvano, copyright (c) 2017

   "Embedded Systems: Real-Time Interfacing to the MSP432 Microcontroller",
   ISBN: 978-1514676585, Jonathan Valvano, copyright (c) 2017

   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2017

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

// Pololu #3543 Vreg (5V regulator output) connected to HC-SR04 Vcc (+5V) and MSP432 +5V (J3.21)
// 22k top connected to HC-SR04 Echo (digital output from sensor)
// 22k bottom connected to 33k top and MSP432 P6.6 (J4.36) (digital input to MSP432)
// 33k bottom connected to ground
// Pololu ground connected to HC-SR04 ground and MSP432 ground (J3.22)
// MSP432 P5.6 (J4.37) (digital output from MSP432) connected to HC-SR04 trigger

// Blue Nokia 5110
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST, pin 1) connected to P9.3
// UCA3STE       (CE,  pin 2) connected to P9.4
// Data/Command  (DC,  pin 3) connected to P9.6
// UCA3SIMO      (Din, pin 4) connected to P9.7
// UCA3CLK       (Clk, pin 5) connected to P9.5
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 3.3 V white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground

// Red SparkFun Nokia 5110 (LCD-10168)
// -----------------------------------
// Signal        (Nokia 5110) LaunchPad pin
// 3.3V          (VCC, pin 1) power
// Ground        (GND, pin 2) ground
// UCA3STE       (SCE, pin 3) connected to P9.4
// Reset         (RST, pin 4) connected to P9.3
// Data/Command  (D/C, pin 5) connected to P9.6
// UCA3SIMO      (DN,  pin 6) connected to P9.7
// UCA3CLK       (SCLK, pin 7) connected to P9.5
// back light    (LED, pin 8) not connected, consists of 4 3.3 V white LEDs which draw ~80mA total

#include <stdint.h>
#include "msp.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/Nokia5110.h"
#include "../inc/Ultrasound.h"

#define ARRAYLEN 10                      // number of elements in the test array
uint16_t CmBuff[ARRAYLEN];
uint16_t InBuff[ARRAYLEN];
uint16_t Current;
uint16_t Minimum;
uint16_t Maximum;
uint16_t Average;

// ------------min------------
// Simple math function that returns the minimum
// value in an array.
// Input: array is an array of 16-bit unsigned numbers
//        length is the number of elements in 'array'
// Output: the smallest value in the array
uint16_t min(uint16_t *array, int length){
  int i;
  uint16_t minimum = 65535;
  for(i=0; i<length; i=i+1){
    if(array[i] < minimum){
      minimum = array[i];
    }
  }
  return minimum;
}

// ------------max------------
// Simple math function that returns the maximum
// value in an array.
// Input: array is an array of 16-bit unsigned numbers
//        length is the number of elements in 'array'
// Output: the largest value in the array
uint16_t max(uint16_t *array, int length){
  int i;
  uint16_t maximum = 0;
  for(i=0; i<length; i=i+1){
    if(array[i] > maximum){
      maximum = array[i];
    }
  }
  return maximum;
}

// ------------avg------------
// Simple math function that returns the average
// value of an array.
// Input: array is an array of 16-bit unsigned numbers
//        length is the number of elements in 'array'
// Output: the average value of the array
// Note: overflow is not considered
uint16_t avg(uint16_t *array, int length){
  int i;
  uint32_t sum = 0;
  for(i=0; i<length; i=i+1){
      sum = sum + array[i];
  }
  return (sum/length);
}



void main(void){
  int i = 0;
  uint16_t distCm, distIn;
  Clock_Init48MHz();                     // set system clock to 48 MHz
  Nokia5110_Init();
  Nokia5110_Clear();
  Nokia5110_OutString("Distance    cur:      cmmin:      cmmax:      cmavg:      cmnum:");
  Ultrasound_Init();
  for(i=0; i<ARRAYLEN; i=i+1){
    Ultrasound_Start();
    while(Ultrasound_End(&distCm, &distIn) == 0){};
    CmBuff[i] = distCm;
    InBuff[i] = distIn;
    Nokia5110_SetCursor(5, 1);           // five leading spaces, second row
    Nokia5110_OutUFix1(distCm);
    Nokia5110_SetCursor(5, 5);           // five leading spaces, sixth row
    Nokia5110_OutUDec((uint16_t)(i+1));
    Clock_Delay1ms(250);                 // delay ~0.25 sec at 48 MHz
  }
  while(1){
    Ultrasound_Start();
    while(Ultrasound_End(&distCm, &distIn) == 0){};
    if(i >= ARRAYLEN){
      i = 0;
    }
    CmBuff[i] = distCm;
    InBuff[i] = distIn;
    Current = distCm;
    Minimum = min(CmBuff, ARRAYLEN);
    Maximum = max(CmBuff, ARRAYLEN);
    Average = avg(CmBuff, ARRAYLEN);
    i = i + 1;
    Nokia5110_SetCursor(5, 1);           // five leading spaces, second row
    Nokia5110_OutUFix1(Current);
    Nokia5110_SetCursor(5, 2);           // five leading spaces, third row
    Nokia5110_OutUFix1(Minimum);
    Nokia5110_SetCursor(5, 3);           // five leading spaces, fourth row
    Nokia5110_OutUFix1(Maximum);
    Nokia5110_SetCursor(5, 4);           // five leading spaces, fifth row
    Nokia5110_OutUFix1(Average);
    Clock_Delay1ms(250);                 // delay ~0.25 sec at 48 MHz
  }
}
