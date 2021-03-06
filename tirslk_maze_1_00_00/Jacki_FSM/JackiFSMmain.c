//*****************************************************************************
//
// Jacki FSM explorer challenge, follow line with FSM
// MSP432 with Jacki
// Daniel and Jonathan Valvano
// September 23, 2017
//****************************************************************************
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

#include <stdint.h>
#include "msp.h"
#include "../inc/Clock.h"
#include "../inc/CortexM.h"
#include "../inc/PWM.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Bump.h"
#include "../inc/Reflectance.h"
#include "../inc/SysTickInts.c"

struct State {
  uint32_t out;                // 2-bit output
  uint32_t delay;              // time to delay in 1ms
  const struct State *next[4]; // Next if 2-bit input is 0-3
};
typedef const struct State State_t;

#define Center    &fsm[0]
#define Right1     &fsm[1]
#define Right2     &fsm[2]
#define Left1     &fsm[3]
#define Left2     &fsm[4]

State_t fsm[5]={
  {0x03, 1, { Right1, Left1,   Right1,  Center }},  // Center
  {0x01, 1, { Right1,  Left1, Right2 ,  Center }},  // Right1
  {0x03, 1, { Right1, Left1,   Right1 , Center }},   // Right2
  {0x02, 1, { Left1, Left2,   Right1, Center}},   // Left1
  {0x03, 1, { Left1, Left1,   Right1 , Center }}   // Left2

};
//FSM DENEME 1
//State_t fsm[5]={
  //              {0x03, 500, { Right1, Right1,   Left1,  Center }},  // Center
    //            {0x01, 500, { Right1,  Right2, Left1 ,  Center }},  // Right1
      //          {0x03, 500, { Right2, Right2,   Right1 , Right1 }},   // Right2
        //        {0x02, 500, { Left1, Right1,   Left2, Center }},   // Left1
          //      {0x03, 500, { Left2, Left1,   Left2 , Left1 }}   // Left2
//};

 /* State_t fsm[5]={//DENEME 2
                {0x03, 50, { Right1, Left1,   Right1,  Center }},  // Center
                {0x01, 50, { Right1,  Left1, Right2,  Center }},  // Right1
                {0x03, 50, { Right1, Left1,   Right1, Center }},   // Right2
                {0x02, 50, { Left1, Left2,   Right2, Center }},   // Left1
                {0x03, 50, { Left2, Left1,   Right1, Center }},   // Left2
                };*/
State_t *Spt;
uint32_t Input;
uint32_t Output;


int main(void){uint32_t heart=0;
  Clock_Init48MHz();
  TExaS_Init(LOGICANALYZER);  // optional
  Motor_Init();
  PWM_Init34(8000,0,0);
  Reflectance_Init();
  Spt = Center;
  while(1){
    Output = Spt->out; // set output from FSM
    Motor_Way(Output);
    Clock_Delay1ms(Spt->delay);   // wait
    Input=Reflectance_Center(1000);
    Spt = Spt->next[Input];// next depends on input and state

  }
}
