// PWMSine.c
// Runs on MSP432
// Use Timer A1 in periodic mode to request interrupts at a
// particular period.
// In those interrupts, change the PWM duty cycle to produce
// a sine wave.
// Daniel Valvano
// July 6, 2017

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

// oscilloscope connected to P2.4/TA0.1 for PWM output
// P2.1 toggles on each timer interrupt (flash at frequency of sine wave * 32 / 2)

#include <stdint.h>
#include "msp.h"
#include "..\inc\Clock.h"
#include "..\inc\PWM.h"
#include "..\inc\TimerA2.h"
#include "..\inc\CortexM.h"
#include "..\inc\LaunchPad.h"

const uint16_t Wave[32] = {
  30,35,40,44,48,51,53,55,55,55,53,
  51,48,44,40,35,30,25,20,16,12,9,
  7,5,5,5,7,9,12,16,20,25
};
void OutputSineWave(void){
  static uint8_t index = 0;        // counting index of output sequence
  PWM_Duty1(Wave[index]);          // output next value in sequence
  index = (index + 1)&0x1F;        // increment counter
  P2OUT ^= 0x02;
}
int main(void){
  Clock_Init48MHz();               // 48 MHz
  // initialize P2.2-P2.0 and make them outputs (P2.2-P2.0 built-in RGB LEDs)
  LaunchPad_Init();
  PWM_Init1(60, 30);                // initialize PWM, 100kHz, 50% duty
//  TimerA2_Init(&OutputSineWave, 852);// initialize 440 Hz sine wave output
  TimerA2_Init(&OutputSineWave, 375);// initialize 1000 Hz sine wave output
  while(1){
    WaitForInterrupt();
  }
}
