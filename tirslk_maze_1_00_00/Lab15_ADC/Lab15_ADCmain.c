// Lab15_ADCmain.c
// Runs on MSP432
// Test the operation of the GP2Y0A21YK0F infrared distance
// sensors by repeatedly taking measurements.  Either
// perform statistical analysis of the data, stream data
// directly from all three channels, or stream calibrated
// measurements from all three channels.  In this case, the
// results are sent through the UART to a computer running
// TExaSdisplay or another terminal program.
// Daniel Valvano
// May 25, 2017

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

// Pololu #3543 Vreg (5V regulator output) connected to all three Pololu #136 GP2Y0A21YK0F Vcc's (+5V) and MSP432 +5V (J3.21)
// Pololu #3543 Vreg (5V regulator output) connected to positive side of three 10 uF capacitors physically near the sensors
// Pololu ground connected to all three Pololu #136 GP2Y0A21YK0F grounds and MSP432 ground (J3.22)
// Pololu ground connected to negative side of all three 10 uF capacitors
// MSP432 P9.0 (J5) (analog input to MSP432) connected to right GP2Y0A21YK0F Vout
// MSP432 P4.1 (J1.5) (analog input to MSP432) connected to center GP2Y0A21YK0F Vout
// MSP432 P9.1 (J5) (analog input to MSP432) connected to left GP2Y0A21YK0F Vout

#include <stdint.h>
#include "msp.h"
#include "../inc/Clock.h"
#include "../inc/CortexM.h"
#include "../inc/IRDistance.h"
#include "../inc/TimerA1.h"
#include "../inc/UART0.h"
#include "../inc/LaunchPad.h"
#include "../inc/ADC14.h"
#include "../inc/LPF.h"


//**************Program 15.1*******************
volatile uint32_t ADCvalue;
volatile uint32_t ADCflag;

void Program15_1_ISR(void){  // runs at 2000 Hz
  uint32_t RawADC;
  P1OUT ^= 0x01;                   // profile
  P1OUT ^= 0x01;                   // profile
  RawADC = ADC_In12();  // sample P4.1/channel 12
  ADCvalue = LPF_Calc(RawADC);
  ADCflag = 1;                     // semaphore
  P1OUT ^= 0x01;                   // profile
}
#define N 256
uint32_t Data[N];
#define M 1024
uint16_t Histogram[M];
uint32_t Sum;      // sum of data
uint32_t Sum2;     // sum of (data-average)^2
uint32_t Average;  // average of data = sum/N
uint32_t Variance; // =sum2/(N-1)
uint32_t Sigma;    // standard deviation = sqrt(Variance)
// Newton's method
// s is an integer
// sqrt(s) is an integer
uint32_t sqrt(uint32_t s){
uint32_t t;   // t*t will become s
int n;             // loop counter
  t = s/16+1;      // initial guess
  for(n = 16; n; --n){ // will finish
    t = ((t*t+s)/t)/2;
  }
  return t;
}

int Program15_1(void){ // example program 15.1
  int32_t n; uint32_t min,max,s=256;
  Clock_Init48MHz();
  ADCflag = 0;
  ADC0_InitSWTriggerCh12();   // initialize ADC P4.1/channel12
  LPF_Init(ADC_In12(),s);     // P4.1/channel12
  UART0_Init();               // initialize UART0 115,200 baud rate
  LaunchPad_Init();
  TimerA1_Init(&Program15_1_ISR,250);    // 2000 Hz sampling
  UART0_OutString("Program 15.1 FIR filter test\nValvano July 2017\n2000Hz 14-bit sampling\nConnect analog signal to P4.1\n");
  EnableInterrupts();
  while(1){
    UART0_OutString("\nADC resolution test\n ");
    LPF_Init(ADC_In12(),s);
    for(n=0; n<2*s; n++){
      while(ADCflag == 0){};
      ADCflag = 0; // skip the first 2*s
    }
    UART0_OutString("Collect "); UART0_OutUDec(N); UART0_OutString(" samples\n");
    Sum = Sum2 = 0;
    for(n=0; n<N; n++){
      while(ADCflag == 0){};
      Sum = Sum+ADCvalue;            // 14bits*100 = 17 bits
      Data[n] = ADCvalue;
      ADCflag = 0;
    }
    Average = Sum/N;
    for(n=0; n<N; n++){
      Sum2 = Sum2+(Data[n]-Average)*(Data[n]-Average); // 28bits*100 = 31 bits
    }
    Variance = (100*Sum2)/(N-1);
    Sigma = sqrt(Variance);

    min = 16384; max = 0;
    for(n=0; n<N; n++){
      if(Data[n] < min)
        min = Data[n]; // smallest
      if(Data[n] > max)
        max = Data[n]; // largest
      }
    for(n=0;n<M;n++){
      Histogram[n] = 0;
    }
    for(n=0; n<N; n++){
      if((Data[n]>=min)&&(Data[n]<min+M)){
        Histogram[Data[n]-min]++;
      }
    }
    UART0_OutString("Probability Mass Function (PMF)\n Data  Count\n");
    for(n=0;n<M;n++){
      if(Histogram[n]){
        UART0_OutUDec5(n+min); UART0_OutString("  \t");UART0_OutUDec(Histogram[n]);UART0_OutChar('\n');
      }
    }

    UART0_OutString("FIR filter "); UART0_OutUDec(s); UART0_OutString("-point average\n");
    UART0_OutString("Average      ="); UART0_OutUDec(Average); UART0_OutChar('\n');
    UART0_OutString("Range        ="); UART0_OutUDec(max-min); UART0_OutChar('\n');
    UART0_OutString("Variance     ="); UART0_OutUFix2(Variance); UART0_OutChar('\n');
    UART0_OutString("Sigma        ="); UART0_OutUFix1(Sigma); UART0_OutChar('\n');
    UART0_OutString("Average/Sigma="); UART0_OutUFix1((100*Average)/Sigma); UART0_OutChar('\n');

    while(LaunchPad_Input()==0x00){}; // wait for touch
    while(LaunchPad_Input()!=0x00){}; // wait for release
    if(s<512) s = s*2; // 32,64,128,256,512...
    else s = 32;
  }
}

//**************Program 15.2*******************
volatile uint32_t nr,nc,nl;
void Program15_2_ISR(void){  // runs at 2000 Hz
  uint32_t raw17,raw12,raw16;
  P1OUT ^= 0x01;         // profile
  P1OUT ^= 0x01;         // profile
  ADC_In17_12_16(&raw17,&raw12,&raw16);  // sample
  nr = LPF_Calc(raw17);  // right is channel 17 P9.0
  nc = LPF_Calc2(raw12); // center is channel 12, P4.1
  nl = LPF_Calc3(raw16); // left is channel 16, P9.1
  ADCflag = 1;           // semaphore
  P1OUT ^= 0x01;         // profile
}
// Calibration and testing
int Program15_2(void){ // example program 15.2
  uint32_t raw17,raw12,raw16;
  int32_t n; uint32_t s;
  Clock_Init48MHz();
  ADCflag = 0;
  s = 256; // replace with your choice
  ADC0_InitSWTriggerCh17_12_16();   // initialize channels 17,12,16
  ADC_In17_12_16(&raw17,&raw12,&raw16);  // sample
  LPF_Init(raw17,s);     // P9.0/channel 17
  LPF_Init2(raw12,s);    // P4.1/channel 12
  LPF_Init3(raw16,s);    // P9.1/channel 16
  UART0_Init();          // initialize UART0 115,200 baud rate
  LaunchPad_Init();
  TimerA1_Init(&Program15_2_ISR,250);    // 2000 Hz sampling
  UART0_OutString("Program 15.2 Calibration test\nValvano July 2017\nConnect analog signals to P9.0,P4.1,P9.1\n");
  EnableInterrupts();
  while(1){
    for(n=0; n<2000; n++){
      while(ADCflag == 0){};
      ADCflag = 0; // show every 2000th point
    }
    UART0_OutUDec5(nl); UART0_OutUDec5(LeftConvert(nl));UART0_OutChar(' ');
    UART0_OutUDec5(nc); UART0_OutUDec5(CenterConvert(nc));UART0_OutChar(' ');
    UART0_OutUDec5(nr); UART0_OutUDec5(RightConvert(nr));
    UART0_OutChar('\n'); // once a second
  }
}

// Test
int main(void){
  uint32_t raw17,raw12,raw16;
  int32_t n; uint32_t s;
  Clock_Init48MHz();
  ADCflag = 0;
  s = 256; // replace with your choice
  ADC0_InitSWTriggerCh17_12_16();   // initialize channels 17,12,16
  ADC_In17_12_16(&raw17,&raw12,&raw16);  // sample
  LPF_Init(raw17,s);     // P9.0/channel 17
  LPF_Init2(raw12,s);    // P4.1/channel 12
  LPF_Init3(raw16,s);    // P9.1/channel 16
  UART0_Init();          // initialize UART0 115,200 baud rate
  LaunchPad_Init();
  TimerA1_Init(&Program15_2_ISR,250);    // 2000 Hz sampling
  UART0_OutString("GP2Y0A21YK0F test\nValvano Oct 2017\nConnect analog signals to P9.0,P4.1,P9.1\n");
  EnableInterrupts();
  while(1){
    for(n=0; n<2000; n++){
      while(ADCflag == 0){};
      ADCflag = 0; // show every 2000th point
    }
    UART0_OutUDec5(LeftConvert(nl));UART0_OutString(" mm,");
    UART0_OutUDec5(CenterConvert(nc));UART0_OutString(" mm,");
    UART0_OutUDec5(RightConvert(nr));UART0_OutString(" mm\n");
  }
}

