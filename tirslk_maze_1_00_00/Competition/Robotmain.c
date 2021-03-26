// Robotmain.c
// Runs on MSP432
// Starter project for robot competition
// Daniel and Jonathan Valvano
// October 28, 2017

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

// Pololu #3543 Vreg (5V regulator output) connected to all three Pololu #136 GP2Y0A21YK0F Vcc's (+5V) and MSP432 +5V (J3.21)
// Pololu #3543 Vreg (5V regulator output) connected to positive side of three 10 uF capacitors physically near the sensors
// Pololu ground connected to all three Pololu #136 GP2Y0A21YK0F grounds and MSP432 ground (J3.22)
// Pololu ground connected to negative side of all three 10 uF capacitors
// MSP432 P9.0 (J5) (analog input to MSP432) connected to right GP2Y0A21YK0F Vout
// MSP432 P4.1 (J1.5) (analog input to MSP432) connected to center GP2Y0A21YK0F Vout
// MSP432 P9.1 (J5) (analog input to MSP432) connected to left GP2Y0A21YK0F Vout

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

// Blue Adafruit Nokia 5110/3310 (Adafruit #338 Newark #53W5865)
// -------------------------------------------------------------
// Signal        (Nokia 5110) LaunchPad pin
// Ground        (GND, pin 1) ground
// 3.3V          (VCC, pin 2) power
// UCA3CLK       (CLK, pin 3) connected to P9.5
// UCA3SIMO      (DIN, pin 4) connected to P9.7
// Data/Command  (D/C, pin 5) connected to P9.6
// UCA3STE       (CS,  pin 6) connected to P9.4
// Reset         (RST, pin 7) connected to P9.3
// back light    (LED, pin 8) not connected, consists of 4 3.3 V white LEDs which draw ~80mA total

// reflectance LED illuminate connected to P5.3
// reflectance sensor 1 connected to P7.0 (robot's right, robot off road to left)
// reflectance sensor 2 connected to P7.1
// reflectance sensor 3 connected to P7.2
// reflectance sensor 4 connected to P7.3 center
// reflectance sensor 5 connected to P7.4 center
// reflectance sensor 6 connected to P7.5
// reflectance sensor 7 connected to P7.6
// reflectance sensor 8 connected to P7.7 (robot's left, robot off road to right)

// Pololu #3542 Romi Encoder connected to Pololu #3543 Motor Driver and Power Distribution Board
//   This connects motor, power, encoders, and grounds.  Kit includes this hardware.  See images.
// Sever VPU = VREG jumper on Motor Driver and Power Distribution Board and connect VPU to 3.3V.
//   This is necessary because MSP432 inputs are not 5V tolerant.
// Left Encoder A connected to P8.2 (J5)
// Left Encoder B connected to P9.2 (J5)
// Right Encoder A connected to P10.4 (J5)
// Right Encoder B connected to P10.5 (J5)

// UCA0RXD connected to P1.2, use UART0_InChar UART0_InUDec UART0_InUHex or UART0_InString to receive from virtual serial port (VCP)
// scanf does not work
// UCA0TXD connected to P1.3, printf sent to PC via virtual serial port (VCP)

#include "msp.h"
#include <stdint.h>
#include <stdio.h>
#include "..\inc\ADC14.h"
#include "..\inc\Bump.h"
#include "..\inc\Clock.h"
#include "..\inc\CortexM.h"
#include "..\inc\IRDistance.h"
#include "..\inc\LaunchPad.h"
#include "..\inc\LPF.h"
#include "..\inc\Motor.h"
#include "..\inc\Nokia5110.h"
#include "..\inc\Reflectance.h"
#include "..\inc\SysTickInts.h"
#include "..\inc\Tachometer.h"
#include "..\inc\TimerA1.h"
#include "..\inc\UART0.h"

//**************distance sampling*******************
volatile uint32_t nr,nc,nl; // filtered ADC samples
uint32_t Right,Center,Left; // distance in mm
void IRsampling(void){  // runs at 2000 Hz
  uint32_t raw17,raw12,raw16;
  ADC_In17_12_16(&raw17,&raw12,&raw16); // sample
  nr = LPF_Calc(raw17);   // right is channel 17 P9.0
  nc = LPF_Calc2(raw12);  // center is channel 12, P4.1
  nl = LPF_Calc3(raw16);  // left is channel 16, P9.1
}


uint16_t LeftSpeed;            //  rotations per minute
uint16_t RightSpeed;           // rotations per minute
uint16_t LeftTach;             // tachometer period of left wheel (number of 0.0833 usec cycles to rotate 1/360 of a wheel rotation)
enum TachDirection LeftDir;    // direction of left rotation (FORWARD, STOPPED, REVERSE)
int32_t LeftSteps;             // number of tachometer steps of left wheel (units of 220/360 = 0.61 mm traveled)
uint16_t RightTach;            // tachometer period of right wheel (number of 0.0833 usec cycles to rotate 1/360 of a wheel rotation)
enum TachDirection RightDir;   // direction of right rotation (FORWARD, STOPPED, REVERSE)
int32_t RightSteps;            // number of tachometer steps of right wheel (units of 220/360 = 0.61 mm traveled)

volatile uint32_t Time;    // ms
volatile uint8_t Data;     // QTR-8RC
volatile int32_t Position; // -332 to +332 mm from line
void SysTick_Handler(void){ // every 1ms
  Time = Time + 1;
  if(Time%10==1){
    Reflectance_Start(); // start every 10ms
  }
  if(Time%10==2){
    Data = Reflectance_End(); // finish 1ms later
    Position = Reflectance_Position(Data);
  }
}
// Nokia5110 and printf (to PC) used for debugging
void main(void){
  uint32_t raw17,raw12,raw16;
  int i;
  DisableInterrupts();
  Clock_Init48MHz();                     // set system clock to 48 MHz
  ADC0_InitSWTriggerCh17_12_16();        // initialize channels 17,12,16
  ADC_In17_12_16(&raw17, &raw12, &raw16);// sample
  LPF_Init(raw17, 64);  // P9.0/channel 17, 64-wide FIR filter
  LPF_Init2(raw12, 64); // P4.1/channel 12, 64-wide FIR filter
  LPF_Init3(raw16, 64); // P9.1/channel 16, 64-wide FIR filter
  UART0_Initprintf();   // serial port channel to PC, with printf
  Nokia5110_Init();
  Nokia5110_Clear();
  LaunchPad_Init();
  TimerA1_Init(&IRsampling, 250);   // 1/(2us*250)=2000 Hz sampling
  Bump_Init();
  Tachometer_Init();
  Motor_Init();
  Time = 0;
  Reflectance_Init();    // line sensor
  SysTick_Init(48000,3); // set up SysTick for 1kHz interrupts
  EnableInterrupts();    // TimerA1 is priority 2, SysTick is priority 3
  while(1){
    Motor_Stop();
    Nokia5110_Clear();
    Nokia5110_SetCursor(0, 0);           // zero leading spaces, first row
    Nokia5110_OutString("ti_rslk_maze"); printf("\n\rti_rslk_maze\n\r");
    Nokia5110_SetCursor(0, 1);
    Nokia5110_OutString("Bump to go"); printf("Bump to go\n\r");
    while(Bump_Read() == 0){   // flash the blue LED while not touched
      LaunchPad_Output(BLUE);
      Clock_Delay1ms(100);     // delay ~0.1 sec at 48 MHz
      LaunchPad_Output(0x00);
      Clock_Delay1ms(100);     // delay ~0.1 sec at 48 MHz
    }
    Nokia5110_SetCursor(0, 1);
    Nokia5110_OutString("Release   "); printf("Release\n\r");

    while(Bump_Read() != 0){   // flash the red LED while touched
       LaunchPad_Output(RED);
       Clock_Delay1ms(100);    // delay ~0.1 sec at 48 MHz
       LaunchPad_Output(0x00);
       Clock_Delay1ms(100);    // delay ~0.1 sec at 48 MHz
    }
    Nokia5110_SetCursor(0, 1);
    Nokia5110_OutString("Ready ");printf("Ready\n\r");
    for(i=5; i; i=i-1){    // flash the yellow LED
      LaunchPad_Output(RED+GREEN);       // red LED is in bit 0, green LED is in bit 1
      Clock_Delay1ms(100);               // delay ~0.1 sec at 48 MHz
      Nokia5110_OutChar(i+'0');
      LaunchPad_Output(0x00);
      Clock_Delay1ms(100);               // delay ~0.1 sec at 48 MHz
    }
    while(Bump_Read() == 0){// repeat the control code until the robot hits the wall
      LaunchPad_Output(GREEN);           // green LED is in bit 1
      Motor_Forward(3000, 3000);
      Nokia5110_SetCursor(0, 1);
      Nokia5110_OutString("Forward   "); printf("Forward\n\r");
      Tachometer_Get(&LeftTach, &LeftDir, &LeftSteps, &RightTach, &RightDir, &RightSteps);
      // (1/tach step/cycles) * (12,000,000 cycles/sec) * (60 sec/min) * (1/360 rotation/step)
      LeftSpeed = 2000000/LeftTach;
      RightSpeed = 2000000/RightTach;
      Right = RightConvert(nr);
      Center = CenterConvert(nc);
      Left = LeftConvert(nl);
      // control logic: avoid forward wall by turning due to no room on right or left
      if(Center < 150){
        LaunchPad_Output(RED);
        Motor_Backward(2500, 2500);
        Nokia5110_SetCursor(0, 1);
        Nokia5110_OutString("Back      ");printf("Back\n\r");
        Clock_Delay1ms(2000);            // delay 2 sec at 48 MHz
      }else if(Left < 150){
        LaunchPad_Output(RED+GREEN);    // yellow
        Motor_Right(2500, 2500);
        Nokia5110_OutString("Right     ");printf("Right\n\r");
        Clock_Delay1ms(2000);          // delay 1 sec at 48 MHz
      }else if(Right < 150){
        LaunchPad_Output(RED+BLUE);
        Motor_Left(2500, 2500);
        Nokia5110_OutString("Left      ");printf("Left\n\r");
        Clock_Delay1ms(2000);          // delay 1 sec at 48 MHz
      }
      Clock_Delay1ms(62);              // delay ~0.062 sec at 48 MHz
    }
  }
}
