// PointerTrafficLightmain.c
// Runs on MSP432
// Use a pointer implementation of a Moore finite state machine to operate
// a traffic light.
// Daniel Valvano
// May 10, 2017

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

// east facing red light connected to P4.5
// east facing yellow light connected to P4.4
// east facing green light connected to P4.3
// north facing red light connected to P4.2
// north facing yellow light connected to P4.1
// north facing green light connected to P4.0
// north facing car detector connected to P5.1 (1=car present)
// east facing car detector connected to P5.0 (1=car present)

#include <stdint.h>
#include "msp.h"
#include "..\inc\Clock.h"
#include "..\inc\TExaS.h"

struct State {
  uint8_t Out;             // 6-bit output
  uint32_t Time;           // 10 ms
  const struct State *Next[4];};// depends on 2-bit input
typedef const struct State State_t;
#define goN   &FSM[0]
#define waitN &FSM[1]
#define goE   &FSM[2]
#define waitE &FSM[3]
State_t FSM[4]={
 {0x21,3000,{goN,waitN,goN,waitN}},
 {0x22, 500,{goE,goE,goE,goE}},
 {0x0C,3000,{goE,goE,waitE,waitE}},
 {0x14, 500,{goN,goN,goN,goN}}};
// SENSOR is set to P5IN and LIGHT is set to P4OUT, making the code more readable.
#define SENSOR  (*((volatile uint8_t *)0x40004C40))
#define LIGHT   (*((volatile uint8_t *)0x40004C23))

int main(void){
  State_t *Pt;  // state pointer
  uint8_t Input;
  Clock_Init48MHz();              // initialize clock to 48MHz
  TExaS_Init(LOGICANALYZER_P4);
  // activate Port 4 and Port 5
  // initialize P4.5-P4.0 and make them outputs
  P4SEL0 &= ~0x3F;
  P4SEL1 &= ~0x3F;             // configure light pins as GPIO
  P4DIR |= 0x3F;               // make light pins out
  // initialize P5.1-P5.0 and make them inputs
  P5SEL0 &= ~0x03;
  P5SEL1 &= ~0x03;             // configure car detector pins as GPIO
  P5DIR &= ~0x03;              // make car detector pins in
  Pt = goN;                    // initial state: Green north; Red east
  while(1){
                               // set lights to current state's Out value
    LIGHT = (LIGHT&~0x3F)|(Pt->Out);
    Clock_Delay1ms(Pt->Time);  // wait 1 ms * current state's Time value
    Input = (SENSOR&0x03);     // get new input from car detectors
    Pt = Pt->Next[Input];      // transition to next state
  }
}

// Volume 2, program 3.3
#define BSP_InPort           P5IN
#define BSP_InPort_DIR       P5DIR
#define BSP_OutPort          P4OUT
#define BSP_OutPort_DIR      P4DIR
#define BSP_In_M             0x00000003  // bit mask for pins 1,0
#define BSP_In_Shift         0x00000000  // shift value for Input pins
#define BSP_Out_M            0x0000003F  // bit mask for pins 5-0
#define BSP_Out_Shift        0x00000000  // shift value for Output pins

int main2(void){ State_t *pt;      // state pointer
  uint32_t input;
  Clock_Init48MHz();            // initialize clock to 48MHz
  TExaS_Init(LOGICANALYZER);
  BSP_InPort_DIR &= ~BSP_In_M;  // make InPort pins inputs
  BSP_OutPort_DIR |= BSP_Out_M; // make OutPort pins out
  pt = goN;
  while(1){
    BSP_OutPort = (BSP_OutPort&(~BSP_Out_M))|((pt->Out)>>BSP_Out_Shift);
    TExaS_Set(BSP_OutPort);
    Clock_Delay1ms(pt->Time);
    input = (BSP_InPort&BSP_In_M)>>BSP_In_Shift; //00,01,10,11
    pt = pt->Next[input];
  }
}
