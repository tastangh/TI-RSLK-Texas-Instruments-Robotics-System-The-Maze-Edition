// Lab16_Tachmain.c
// Runs on MSP432
// Test the operation of the tachometer by implementing
// a simple DC motor speed controller.
// Daniel Valvano
// September 5, 2017

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

// Pololu #3542 Romi Encoder connected to Pololu #3543 Motor Driver and Power Distribution Board
//   This connects motor, power, encoders, and grounds.  Kit includes this hardware.  See images.
// Sever VPU = VREG jumper on Motor Driver and Power Distribution Board and connect VPU to 3.3V.
//   This is necessary because MSP432 inputs are not 5V tolerant.
// Left Encoder A connected to P8.2 (J5)
// Left Encoder B connected to P9.2 (J5)
// Right Encoder A connected to P10.4 (J5)
// Right Encoder B connected to P10.5 (J5)

// Sever VCCMD=VREG jumper on Motor Driver and Power Distribution Board and connect VCCMD to 3.3V.
//   This makes P3.7 and P3.6 low power disables for motor drivers.  0 to sleep/stop.
// Sever nSLPL=nSLPR jumper.
//   This separates P3.7 and P3.6 allowing for independent control
// Left motor direction connected to P1.7 (J2.14)
// Left motor PWM connected to P2.7/TA0CCP4 (J4.40)
// Left motor enable connected to P3.7 (J4.31)
// Right motor direction connected to P1.6 (J2.15)
// Right motor PWM connected to P2.6/TA0CCP3 (J4.39)
// Right motor enable connected to P3.6 (J2.11)

// See Bump.c for bumper connections (Port 8 or Port 4)


#include <stdint.h>
#include "msp.h"
#include "../inc/Clock.h"
#include "../inc/CortexM.h"
#include "../inc/LaunchPad.h"
#include "../inc/Motor.h"
#include "../inc/Nokia5110.h"
#include "../inc/Tachometer.h"
#include "../inc/TimerA1.h"
#include "../inc/TA3InputCapture.h"
#include "../inc/TExaS.h"
#include "../inc/FlashProgram.h"
#include "../inc/Bump.h"

#define P2_4 (*((volatile uint8_t *)(0x42098070)))
#define P2_3 (*((volatile uint8_t *)(0x4209806C)))
#define P2_2 (*((volatile uint8_t *)(0x42098068)))
#define P2_1 (*((volatile uint8_t *)(0x42098064)))
#define P2_0 (*((volatile uint8_t *)(0x42098060)))

uint16_t Period0;              // (1/SMCLK) units = 83.3 ns unitsqqqq
uint16_t Rpm0;
uint16_t deg0;

uint16_t First0;               // Timer A3 first edge, P10.4
int Done0;                     // set each rising
// max period is (2^16-1)*83.3 ns = 5.4612 ms
// min period determined by time to run ISR, which is about 1 us
void PeriodMeasure0(uint16_t time){
  P2_0 = P2_0^0x01;           // thread profile, P2.0
  Period0 = (time - First0)&0xFFFF; // 16 bits, 83.3 ns resolution
  First0 = time;                   // setup for next
  Done0 = 1;
  Rpm0=2000000/Period0;
  deg0=(Rpm0*360)/60;
}
uint16_t deg2;
uint16_t Rpm2;
uint16_t Period2;              // (1/SMCLK) units = 83.3 ns units
uint16_t First2;               // Timer A3 first edge, P8.2
int Done2;                     // set each rising
// max period is (2^16-1)*83.3 ns = 5.4612 ms
// min period determined by time to run ISR, which is about 1 us
void PeriodMeasure2(uint16_t time){

  P2_4 = P2_4^0x01;           // thread profile, P2.4
  Period2 = (time - First2)&0xFFFF; // 16 bits, 83.3 ns resolution
  First2 = time;                   // setup for next
  Done2 = 1;
  Rpm2=2000000/Period2;
  deg2=(Rpm2*360)/60;
}
int main2(void){
  PWM_Init34(14998,0,0);
  Clock_Init48MHz();   // 48 MHz clock; 12 MHz Timer A clock
  P2->SEL0 &= ~0x11;
   P2->SEL1 &= ~0x11;   // configure P2.0 and P2.4 as GPIO
   P2->DIR |= 0x11;     // P2.0 and P2.4 outputs
  First0 = First2 = 0; // first will be wrong
  Done0 = Done2 = 0;   // set on subsequent
  Motor_Init();        // activate Lab 13 software
  TimerA3Capture_Init(&PeriodMeasure0,&PeriodMeasure2);
  Motor_Forward(7500,7500); // 50%
  EnableInterrupts();
  TExaS_Init(LOGICANALYZER_P10);

  while(1){
    WaitForInterrupt();
  }
}
uint16_t SpeedBuffer[1000];      // RPM---->1000
uint32_t PeriodBuffer[1000];     // 1/12MHz = 0.083 usec
  Duty,DutyBuffer[1000];  // 0 to 15000
uint32_t Time; // in 0.01 sec
void Collect(void){
  P2_1 = P2_1^0x01;    // thread profile, P2.1
  if(Done0==0) Period0 = 65534; // stopped
  if(Done2==0) Period2 = 65534; // stopped
  Done0 = Done2 = 0;   // set on subsequent
  if(Time==200){       // 4 sec---->8sec
     Duty = 3750;
     Motor_Forward(3750,3750);    // 25%
   }
  if(Time==400){       // 1 sec--->2sec
    Duty = 7500;
    Motor_Forward(7500,7500);    // 50%
  }
  if(Time==600){       // 2 sec----->4sec
    Duty = 11250;
    Motor_Forward(11250,11250);  // 75%
  }
  if(Time==800){       // 3 sec------>6sec
      Duty = 14998;
      Motor_Forward(14998,14998);    // 100%
  }
  if(Time==1000){       // 4 sec---->8sec
    Duty = 3750;
    Motor_Forward(3750,3750);    // 25%
  }
  if(Time<1000){       // 5 sec---->10 sec
    SpeedBuffer[Time] = 2000000/Period0;
    PeriodBuffer[Time] = Period0;
    DutyBuffer[Time] = Duty;
    Time = Time+1;
  }
  if(Time==1000){
    Duty = 0;
    Motor_Stop();     // 0%
    TimerA1_Stop();
  }
}
int main1(void){//main1
  DisableInterrupts();
  PWM_Init34(8000,0,0);

  Clock_Init48MHz();   // 48 MHz clock; 12 MHz Timer A clock
  P2->SEL0 &= ~0x11;
  P2->SEL1 &= ~0x11;   // configure P2.0 and P2.4 as GPIO
  P2->DIR |= 0x11;     // P2.0 and P2.4 outputs
  First0 = First2 = 0; // first will be wrong
  Done0 = Done2 = 0;   // set on subsequent
  Time = 0; Duty = 3750;
  Bump_Init();
  Motor_Init();        // activate Lab 13 software
  TimerA3Capture_Init(&PeriodMeasure0,&PeriodMeasure2);
  TimerA1_Init(&Collect,5000); // 100 Hz
  Motor_Forward(3750,3750); // 25%
  TExaS_Init(LOGICANALYZER_P10);
  EnableInterrupts();
  while(1){
    WaitForInterrupt();
  }
}
#define FLASH_BANK1_MIN     0x00020000  // Flash Bank1 minimum address
#define FLASH_BANK1_MAX     0x0003FFFF  // Flash Bank1 maximum address
void Debug_FlashInit(void){ uint32_t addr;
  Flash_Init(48);
  for(addr=FLASH_BANK1_MIN;addr<0x0003FFFF;addr=addr+4096){
    if(Flash_Erase(addr)==ERROR){
      while(1){
        LaunchPad_Output(BLUE);  Clock_Delay1ms(200);
        LaunchPad_Output(RED);  Clock_Delay1ms(500);
        LaunchPad_Output(GREEN);  Clock_Delay1ms(300);
      }
    }
  }
}
// record 32 halfwords
void Debug_FlashRecord(uint16_t *pt){uint32_t addr;
  addr=FLASH_BANK1_MIN;
  while(*(uint32_t*)addr != 0xFFFFFFFF){ // find first free block
    addr=addr+64;
    if(addr>FLASH_BANK1_MAX) return; // full
  }
  Flash_FastWrite((uint32_t *)pt, addr, 16); // 16 words is 32 halfwords, 64 bytes
}
int main254(void){//main2(void){
int i;
PWM_Init34(14998,0,0);

  DisableInterrupts();
  Clock_Init48MHz();   // 48 MHz clock; 12 MHz Timer A clock
  LaunchPad_Init();
  if(LaunchPad_Input()){
    LaunchPad_Output(RED);
    Debug_FlashInit(); // erase flash if either switch pressed
    while(LaunchPad_Input()){}; // wait for release
  }
  P2->SEL0 &= ~0x11;
  P2->SEL1 &= ~0x11;   // configure P2.0 and P2.4 as GPIO
  P2->DIR |= 0x11;     // P2.0 and P2.4 outputs
  First0 = First2 = 0; // first will be wrong
  Done0 = Done2 = 0;   // set on subsequent
  Time = 0; Duty = 3750;
  Bump_Init();
  Motor_Init();        // activate Lab 13 software
  TimerA3Capture_Init(&PeriodMeasure0,&PeriodMeasure2);
  TimerA1_Init(&Collect,5000); // 100 Hz
  Motor_Forward(3750,3750); // 25%
  TExaS_Init(LOGICANALYZER_P10);
  EnableInterrupts();
  while(1){
    WaitForInterrupt();
    if(Time>=1000){
      LaunchPad_Output(GREEN);
      for(i=0;i<16;i++){
        Debug_FlashRecord(&SpeedBuffer[32*i]);
      }
      while(1){
        LaunchPad_Output(BLUE);  Clock_Delay1ms(200);
     }
    }
  }
}

uint16_t Rpm;

void main45(void){
    int i;
    DisableInterrupts();
    Clock_Init48MHz();
    LaunchPad_Init();
    First0 = First2 = 0; // first will be wrong
     Done0 = Done2 = 0;   // set on subsequent
     Time = 0; Duty = 3750;
     Bump_Init();
     Motor_Init();        // activate Lab 13 software
     TimerA3Capture_Init(&PeriodMeasure0,&PeriodMeasure2);
     TimerA1_Init(&Collect,5000); // 100 Hz
     Motor_Forward(Rpm0,Rpm2);
     TExaS_Init(LOGICANALYZER_P10);
     EnableInterrupts();
     Motor_Stop();     // 0%

  // write a main program that uses the tachometer
  while(1){
      WaitForInterrupt();
         if(Time>=1000){
           LaunchPad_Output(GREEN);
           for(i=0;i<16;i++){
             Debug_FlashRecord(&SpeedBuffer[32*i]);
           }
           while(1){
             LaunchPad_Output(BLUE);  Clock_Delay1ms(200);

 }
}
  }}

void main4555(void){
    Clock_Init48MHz();
    PWM_Init34(14998,0,0);
    First0 = First2 = 0; // first will be wrong
    Done0 = Done2 = 0;   // set on subsequent
    Time = 0; Duty = 3750;
      Motor_Init();
      TimerA3Capture_Init(&PeriodMeasure0,&PeriodMeasure2);
      TimerA1_Init(&Collect,5000); // 100 Hz
      Motor_Forward(3750,3750); // 25%
      EnableInterrupts();
      TExaS_Init(LOGICANALYZER_P10);

  // write a main program that uses the tachometer
  while(1){

 }
}
/*
int kp;
int kd;
int ki;
int XstarL;int XstarR;int rpmright;int rpmleft;int degleft;int degright;int ErrorL;int ErrorR;int UL;int UR;int A;int PR;int PL;
int INTR;int INTL;int IR;int IL;int DR0;int DL0;int T;int DR1;int DL1;int i;int errorR;int errorL;
uint32_t AL;uint32_t AR;
void SysTick_Wait1us(uint32_t delay){
     uint32_t i;
    for(i=0; i<delay; i++){
       SysTick_Wait(5);
      }
    } // write this code

void main(void){{uint32_t H,L;
Clock_Init48MHz();
PWM_Init34(14998,0,0);
First0 = First2 = 0; // first will be wrong
Done0 = Done2 = 0;   // set on subsequent
Time = 0; Duty = 3750;
  Motor_Init();
  TimerA3Capture_Init(&PeriodMeasure0,&PeriodMeasure2);
  TimerA1_Init(&Collect,5000); // 100 Hz
  Motor_Forward(3750,3750); // 25%
  EnableInterrupts();
  TExaS_Init(LOGICANALYZER_P10);DisableInterrupts();
    Motor_Init();        // activate Lab 13 software
    Tachometer_Init();
    EnableInterrupts();
    //if ((ErrorL=>3) && (UL=<14998)) {UL=UL+100;}
    //else if ((ErrorL<-3) && (UL>=100)) {UL=UL-100;}
    //else if ((ErrorR=>3) && (UR=<14998)) { UR=UR+100;}
    //else if ((ErrorR<-3) && (UR>=100)) {UR=UR-100;}
    P2->SEL0 &= ~0x11;
    P2->SEL1 &= ~0x11;   // configure P2.0 and P2.4 as GPIO
    P2->DIR |= 0x11;     // P2.0 and P2.4 outputs
    H = 1000;
    L = 10000-H;
    TimerA3Capture_Init(&PeriodMeasure0,&PeriodMeasure2);
    TimerA1_Init(&Collect,5000); // 100 Hz
    rpmright=Rpm2;
            rpmleft=Rpm0;
  while(1){

        P2->OUT |= 0x11;   // on
        SysTick_Wait1us(H);
        rpmright=Rpm2;
        rpmleft=Rpm0;
        AL=1024*1500/rpmleft;
        AR=1024*1500/rpmright;
        degleft=rpmleft*360/60;
        degright=rpmright*360/60;
        XstarL=rpmleft;
        XstarR=rpmright;
        int XprimeL=100;
        int XprimeR=200;
        for (i=0;i>Time;i++){
        errorR=ErrorR;
        errorL=ErrorL;
        ErrorR = XstarR- XprimeR;
        ErrorL = XstarL- XprimeL;
        }
        UL = UL + (AL*ErrorL)/1024;
        UR = UR + (AR*ErrorR)/1024 ;
        Motor_Forward(UL,UR);
        PR=kp*ErrorR;
        PL=kp*ErrorL;
        INTR=INTR+(ErrorR+errorR)*T;
        INTL=INTL+(ErrorL+errorL)*T;
        IR=ki*INTR;
        IL=ki*INTL;
        DR0=(ErrorR-errorR)/T;
        DL0=(errorL-errorL)/T;
        DR1=kd*DR0;
        DL1=kd*DL0;
        UL=PL+IL+DL1;
        UR=PR+IR+DR1;
        Motor_Forward(UL,UR);
        P2->OUT &= ~0x11;  // off
        SysTick_Wait1us(L);

      TimerA3Capture_Init(&PeriodMeasure0,&PeriodMeasure2);
 ///     TimerA1_Init(&Control,5000); // 100 Hz

// write this as part of Lab 17


  }
}}*/

// Lab17_Control.c
// Runs on MSP432
// Implementation of the control system.
// Daniel and Jonathan Valvano
// September 12, 2017

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
#include "../inc/UART0.h"
#include "../inc/Motor.h"
#include "../inc/Bump.h"
#include "../inc/ADC14.h"
#include "../inc/TimerA1.h"
#include "../inc/IRDistance.h"
#include "../inc/Nokia5110.h"
#include "../inc/LPF.h"
#include "../inc/SysTickInts.h"
#include "../inc/Tachometer.h"
#include "../inc/Reflectance.h"
//******************************************************
// Distance to wall proportional control
// Incremental speed control
// Integral control, Line follower
uint16_t leftTach;
uint16_t rightTach;
enum TachDirection leftDir;
enum TachDirection rightDir;
int32_t leftSteps;
int32_t rightSteps;
int LeftSpeed;
int RightSpeed;
int XstarR=120;
int XstarL=150;
int errorR;
int errorL;
int errorPR;
int errorPL;
int InTR;
int InTL;
int oranR;
int IntegralR;
int DerivativePartR;
int oranL;
int IntegralL;
int UR1;
int UL1;
int UR;
int UL;
uint16_t leftTach;
uint16_t rightTach;
enum TachDirection leftDir;
enum TachDirection rightDir;
int32_t leftSteps;
int32_t rightSteps;
int LeftSpeed;
int RightSpeed;
void readTach(){
    Tachometer_Get(&leftTach,&leftDir,&leftSteps,&rightTach,&rightDir,&rightSteps);
    LeftSpeed=2000000/leftTach;
    RightSpeed=200000/rightTach;
}

void controller(int T,int kp,int ki,int kd){
    LeftSpeed=Rpm0;
    RightSpeed=Rpm2;
    errorR=XstarR-RightSpeed;
    errorL=XstarL-LeftSpeed;

    InTR=InTR+T*(errorR+errorPR)/2;
    InTL=InTL+T*(errorR+errorPR)/2;
    if(InTR>=14998){InTR=14998;}
    if(InTR<=-14998){InTR=-14998;}
    if(InTL>=14998){InTL=14998;}
    if(InTL<=-14998){InTL=-14998;}
    oranR=kp*errorR;
    oranL=kp*errorL;

    IntegralR=ki*InTR;
    IntegralL=ki*InTL;

    UR1=oranR+IntegralR;
    UL1=oranL+IntegralL;
    if(UR1>=14998){UR1=14998;}
    else if(UR1<=3500){UR1=3500;}
    else{UR=UR1;}if(UL1>=14998){UL=14998;}
    else if(UL1<=1500){UL=1500;}
    else{UL=UL1;}
    errorPR=errorR;
    errorPL=errorL;
    Motor_Forward(UL,UR);
}

void Controller(void){
// write this as part of Lab 17

}

void main(void){

    DisableInterrupts();
    Clock_Init48MHz();
    Motor_Init();
    PWM_Init34(15000, 0,0);
    Tachometer_Init();
    TimerA1_Init(&controller, 5000); // 100 Hz
    EnableInterrupts();


  while(1){
// write this as part of Lab 17


  }
}

