// Reflectance.c
// Provide functions to take measurements using a QTR-8RC reflectance
// sensor array (Pololu part number 961).  This works by outputting to
// the sensor, waiting, then reading the digital value of each of the
// eight phototransistors.  The more reflective the target surface is,
// the faster the voltage decays.
// Student version of GPIO lab
// Daniel and Jonathan Valvano
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


// reflectance LED illuminate connected to P5.3
// reflectance sensor 1 connected to P7.0 (robot's right, robot off road to left)
// reflectance sensor 2 connected to P7.1
// reflectance sensor 3 connected to P7.2
// reflectance sensor 4 connected to P7.3 center
// reflectance sensor 5 connected to P7.4 center
// reflectance sensor 6 connected to P7.5
// reflectance sensor 7 connected to P7.6
// reflectance sensor 8 connected to P7.7 (robot's left, robot off road to right)

#include <stdint.h>
#include "msp432.h"
#include "..\inc\Clock.h"

// ------------Reflectance_Init------------
// Initialize the GPIO pins associated with the QTR-8RC
// reflectance sensor.  Infrared illumination LEDs are
// initially off.
// Input: none
// Output: none
void Reflectance_Init(void){
    Clock_Init48MHz();

    P5->SEL0 &= ~0x08;
    P5->SEL1 &= ~0x08;
    P5->DIR |= 0x08;
    P5->OUT &= ~0x08;

    P7->SEL0 &=  ~0x00;
    P7->SEL1 &=  ~0x00;
    P7->DIR &=  ~0x00;

    P4->SEL0 &= ~0x01;
    P4->SEL1 &= ~0x01;    //  P4.0 as GPIO
    P4->DIR |= 0x01;      //  make P4.0 out
}

// ------------Reflectance_Read------------
// Read the eight sensors
// Turn on the 8 IR LEDs
// Pulse the 8 sensors high for 10 us
// Make the sensor pins input
// wait t us
// Read sensors
// Turn off the 8 IR LEDs
// Input: time to wait in usec
// Output: sensor readings
// Assumes: Reflectance_Init() has been called
uint8_t Reflectance_Read(uint32_t time){
uint8_t measured;
P5->OUT |= 0x08;//1) Set P5.3 high (turn on IR LED)
P7->DIR = 0xFF;       // 2) Make P7.7 ? P7.0 outputs, and set them high (charging 8 capacitors)
P7->OUT = 0xFF;       // prime for measurement
Clock_Delay1us(10);//3) 3) Wait 10 us, Clock_Delay1us(10);
P7->DIR = 0x00;       // make P7.7-P7.0 in //4) Make P7.7 ? P7.0 input
Clock_Delay1us(time); //5) Wait time us, Clock_Delay1us(time);
    measured=P7->IN ;// convert input to digital
 //6) Read P7.7 ? P7.0 inputs (converts voltages into binary)
            P5->OUT &= ~0x08;//6) Set P5.3 low (turn off IR LED, saving power)
  return measured;
}

// ------------Reflectance_Center------------
// Read the two center sensors
// Turn on the 8 IR LEDs
// Pulse the 8 sensors high for 10 us
// Make the sensor pins input
// wait t us
// Read sensors
// Turn off the 8 IR LEDs
// Input: time to wait in usec
// Output: 0 (off road), 1 off to left, 2 off to right, 3 on road
// (Left,Right) Sensors
// 1,1          both sensors   on line
// 0,1          just right     off to left
// 1,0          left left      off to right
// 0,0          neither        lost
// Assumes: Reflectance_Init() has been called

// ------------Reflectance_Center------------
// Read the two center sensors
// Turn on the 8 IR LEDs
// Pulse the 8 sensors high for 10 us
// Make the sensor pins input
// wait t us
// Read sensors
// Turn off the 8 IR LEDs
// Input: time to wait in usec
// Output: 0 (off road), 1 off to left, 2 off to right, 3 on road
// (Left,Right) Sensors
// 1,1          both sensors   on line
// 0,1          just right     off to left
// 1,0          left left      off to right
// 0,0          neither        lost
// Assumes: Reflectance_Init() has been called
uint8_t Reflectance_Center(uint32_t time){
    uint8_t result;
        P5->OUT |=0x08;
        P7->DIR=0xFF;
        P7->OUT =0xFF;
        Clock_Delay1us(10000);
        P7->DIR=0x00;
        Clock_Delay1us(time);
        for(int i=0;i<10;i++){
            result=((P7->IN&0x18)>>3); // convert input to digital
            }
        P5->OUT &=~0x08;
    // write this as part of Lab 6
  return result; // replace this line
    }
   // uint8_t measured;
          // P5->OUT |=0x08;
         //  P7->DIR=0xFF;
         //  P7->OUT =0xFF;
        //   Clock_Delay1us(10);
         //  P7->DIR=0x00;
         //  Clock_Delay1us(time);
            //  measured= P7->IN&0x01; // convert input to digital
           //    }

        //   P5->OUT &=~0x08;

       // write this as part of Lab 6
    // return measured; // replace this line
//}

// Perform sensor integration
// Input: data is 8-bit result from line sensor
// Output: position in 0.1mm relative to center of line


int32_t Reflectance_Position(uint8_t data){
    int32_t measured;
        if (data==0){
        measured=350;
    }else
    {
    int bolum=0;
    int toplam=0;
    int W[8]={332, 237, 142, 47, -47, -142, -237, -332};
    uint8_t P[8]={0x01, 0x02, 0x04, 0x08,0x010,0x020,0x040,0x080};
    for (int i=0; i<8;i++){
        if (data&P[i]) {
            bolum= bolum+1;
            toplam = toplam +W[i];
        }
    }
    measured=toplam/bolum;
    }
    // write this as part of Lab 6
 return measured; // replace this line
}

// ------------Reflectance_Start------------
// Begin the process of reading the eight sensors
// Turn on the 8 IR LEDs
// Pulse the 8 sensors high for 10 us
// Make the sensor pins input
// Input: none
// Output: none
// Assumes: Reflectance_Init() has been called
void Reflectance_Start(void){
    P5->OUT |= 0x08;//1) Set P5.3 high (turn on IR LED)
    P7->DIR = 0xFF;
    P7->OUT = 0xFF;
    Clock_Delay1us(10);
    P7->DIR = 0x00;
    // write this as part of Lab 10
}

// ------------Reflectance_End------------
// Finish reading the eight sensors
// Read sensors
// Turn off the 8 IR LEDs
// Input: none
// Output: sensor readings
// Assumes: Reflectance_Init() has been called
// Assumes: Reflectance_Start() was called 1 ms ago
uint8_t Reflectance_End(void)
{
    uint8_t data = P5->OUT;
    P5->OUT &= ~0x08;
// write this as part of Lab 10
 return data; // replace this line
}

