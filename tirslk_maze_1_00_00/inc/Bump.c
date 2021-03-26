// Bump.c
// Runs on MSP432
// Provide low-level functions that interface bump switches the robot.
// Daniel Valvano and Jonathan Valvano
// July 2, 2017

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
#include "bump.h"
//#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
// Initialize Bump sensors
// Make six Port 4 pins inputs
// Activate interface pullup
// pins 7,6,5,3,2,0
void Bump_Init(void){

        /*P5->SEL0 &= ~0x08;
        P5->SEL1 &= ~0x08;
        P5->DIR |= 0x08;
        P5->OUT &= ~0x08;

        P7->SEL0 &=  ~0x00;
        P7->SEL1 &=  ~0x00;
        P7->DIR &=  ~0x00;*/

        P4->SEL0 &= ~0x00;
        P4->SEL1 &= ~0x00;    //  P4.0 as GPIO
        P4->DIR &= 0x00;      //  make P4.0 out
        P4->REN |= 0xFF;
    //P4->SEL0 &= ~0x00;
   // P4->SEL1 &= ~0x00;
    //P4->DIR &= ~0x00;
    //P4->REN |= 0xED;
    //P4->OUT |= 0xED;

}
// Read current state of 6 switches
// Returns a 6-bit positive logic result (0 to 63)
// bit 5 Bump5
// bit 4 Bump4
// bit 3 Bump3
// bit 2 Bump2
// bit 1 Bump1
// bit 0 Bump0
uint8_t Bump_Read(void){
    //uint8_t data;
    //data = P4->IN;
    //return data;
    uint8_t data;
    /*P4->OUT |= 0x08;//1) Set P5.3 high (turn on IR LED)
    P4->DIR = 0xFF;       // 2) Make P7.7 – P7.0 outputs, and set them high (charging 8 capacitors)
    P4->OUT = 0xFF;       // prime for measurement
    Clock_Delay1us(10);//3) 3) Wait 10 us, Clock_Delay1us(10);
    P4->DIR = 0x00;*/       // make P7.7-P7.0 in //4) Make P7.7 – P7.0 input
        data=P4->IN ;// convert input to digital
     //6) Read P7.7 – P7.0 inputs (converts voltages into binary)
                P4->OUT &= ~0x08;//6) Set P5.3 low (turn off IR LED, saving power)
      return data;

    }


