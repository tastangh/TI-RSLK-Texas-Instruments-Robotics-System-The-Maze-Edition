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
#include "../inc/TimerA2.h"
#include "../inc/IRDistance.h"
#include "../inc/Nokia5110.h"
#include "../inc/LPF.h"
#include "../inc/SysTickInts.h"
#include "Tachometer.h"
#include "../inc/Reflectance.h"
//******************************************************
// Distance to wall proportional control
// Incremental speed control 
// Integral control, Line follower
// write this as part of Lab 17

#define P2_4 (*((volatile uint8_t *)(0x42098070)))
#define P2_3 (*((volatile uint8_t *)(0x4209806C)))
#define P2_2 (*((volatile uint8_t *)(0x42098068)))
#define P2_1 (*((volatile uint8_t *)(0x42098064)))
#define P2_0 (*((volatile uint8_t *)(0x42098060)))

uint32_t Values[255];
uint32_t centerSensorAdcValue=0;
uint32_t distanceRight,distanceLeft,distanceCenter;
float LeftSpeed;            //  rotations per minute
float RightSpeed;           // rotations per minute
int32_t LeftTach;             // tachometer period of left wheel (number of 0.0833 usec cycles to rotate 1/360 of a wheel rotation)
int32_t RightTach;            // tachometer period of right wheel (number of 0.0833 usec cycles to rotate 1/360 of a wheel rotation)

uint32_t prevLeftTach = 0;
uint32_t leftTachCounter = 0;
uint32_t prevRightTach = 0;
uint32_t RightTachCounter = 0;
uint32_t prevTachometer_LeftDir = 0;
uint32_t prevTachometer_RightDir = 0;

int16_t prevduty4 = 0;
int16_t prevduty3 = 0;
uint16_t changeDirection4 = 0;
uint16_t changeDirection3 = 0;
int16_t direction_rightmotor;

void UpdateMotorRightDutyCycle(int16_t duty3)
{
    if(duty3>0)
    {
        direction_rightmotor = FORWARD;
        P1->OUT &= ~0x40;
    }
    else if(duty3<0)
    {
        direction_rightmotor = REVERSE;
        P1->OUT |= 0x40;
    }

    TIMER_A0->CCR[3] = fabs(duty3);        // CCR2 duty cycle is duty2/period
}

int16_t direction_leftmotor = STOPPED;
void UpdateMotorLeftDutyCycle(int16_t duty4)
{
    if(duty4>0)
    {
        direction_leftmotor = FORWARD;
        P1->OUT &= ~0x80;
    }
    else if(duty4<0)
    {
        direction_leftmotor = REVERSE;
        P1->OUT |= 0x80;
    }
    TIMER_A0->CCR[4] = fabs(duty4);        // CCR2 duty cycle is duty2/period
}

float prevUi_right = 0;
float prevUi_left = 0;
float prevUd_right = 0;
float prevUd_left = 0;
float prev_Er_right = 0;
float prev_Er_left = 0;

void OkuTakometre(void)
{
    LeftTach =  fabs(Tachometer_SecondLeftTime - Tachometer_FirstLeftTime);
    RightTach = fabs(Tachometer_SecondRightTime - Tachometer_FirstRightTime);

    if ((Tachometer_LeftDir == REVERSE) && (prevTachometer_LeftDir != REVERSE))
    {
        LeftTach = LeftTach * -1.0;
    }
    if ((Tachometer_RightDir == REVERSE) && (prevTachometer_RightDir != REVERSE))
    {
        RightTach = RightTach * -1.0;
    }

    // (1/tach step/cycles) * (12,000,000 cycles/sec) * (60 sec/min) * (1/360 rotation/step)
    if ((LeftTach - prevLeftTach) < 0.0001f)
    {
        Tachometer_LeftDir = STOPPED;
        LeftSpeed = 0;
    }
    else
    {
        leftTachCounter = 0;
        LeftSpeed = 2000000.0f/(float)LeftTach;
    }

    if ((RightTach - prevRightTach) < 0.0001f)
    {
        Tachometer_RightDir = STOPPED;
        RightSpeed = 0;
    }
    else
    {
        RightTachCounter = 0;
        RightSpeed = 2000000.0f/(float)RightTach;
    }

    prevLeftTach = LeftTach;
    prevRightTach = RightTach;

}
float SpeedReferenceRight = 50;
float SpeedReferenceLeft =50;
float Kp = 0.0008;
float Ki = 2.5;
float Kd = 0.0003;
float prevKp = 0;
float prevKi = 0;
float prevKd = 0;
float PwmRight = 0;
float PwmLeft = 0;
float ProportionalPartRight = 0;
float IntegralPartRight = 0;
float DerivativePartRight = 0;
float ProportionalPartLeft = 0;
float IntegralPartLeft = 0;
float DerivativePartLeft = 0;

void KosturKontrolor()
{
    // PID Kontrolor
    // ilklendir
    const float T = 0.1f;
    float e_k_Right = 0;
    float e_k_Left = 0;
    // takometre hatasi
    e_k_Right = (SpeedReferenceRight - RightSpeed);
    e_k_Left  = (SpeedReferenceLeft  - LeftSpeed); //SpeedReferenceLeft

    // kontrolor parametreleri degisim kontrolu
    if ((Kp == prevKp) && (Ki == prevKi) && (Ki == prevKi))
    {
        // kontrolor degismedi
    }
    else
    {
        // kontrolor reset
        prevUi_right = 0;
        prevUi_left = 0;
        prevUd_right=0;
        prevUd_left=0;
        prev_Er_right = e_k_Right;
        prev_Er_left  = e_k_Left;
    }

    prevKp = Kp;
    prevKi = Ki;
    prevKd = Kd;

    // Kontrolor kosuyor
    ProportionalPartRight = Kp * e_k_Right;
    ProportionalPartLeft  = Kp * e_k_Left;

    IntegralPartRight = (prevUi_right) +(Ki*(T * 0.5f)*(e_k_Right+prev_Er_right));
    IntegralPartLeft = (prevUi_left) + (Ki*(T * 0.5f)*(e_k_Left+prev_Er_left)) ;

    DerivativePartRight = (Kd*(1.0f/T))*(e_k_Right-prev_Er_right);
    DerivativePartLeft = (Kd*(1.0f/T))*(e_k_Left-prev_Er_left);

    // Pwm uygulama
    PwmRight = (ProportionalPartRight + IntegralPartRight + DerivativePartRight);
    PwmLeft  = (ProportionalPartLeft + IntegralPartLeft + DerivativePartLeft);

    if(IntegralPartRight>=90)
    {
        IntegralPartRight=90;
    }
    else if (IntegralPartRight <= -90)
    {
        IntegralPartRight = -90;
    }
    if(IntegralPartLeft>=90)
    {
        IntegralPartLeft=90;
    }
    else if (IntegralPartLeft <= -90)
    {
        IntegralPartLeft = -90;
    }


    prevUi_right=IntegralPartRight;
    prevUi_left=IntegralPartLeft;
    prevUd_right=DerivativePartRight;
    prevUd_left=DerivativePartLeft;
    prev_Er_right = e_k_Right;
    prev_Er_left  = e_k_Left;

    if (PwmRight >= 90)
    {
        PwmRight = 90;
    }
    else if (PwmRight <= -90)
    {
        PwmRight = -90;
    }

    if (PwmLeft >= 90)
    {
        PwmLeft = 90;
    }
    else if (PwmLeft <= -90)
    {
        PwmLeft = -90;
    }

    UpdateMotorRightDutyCycle(PwmRight);
    UpdateMotorLeftDutyCycle(PwmLeft);

}

void TimerA2Function(void)
{
        //  P1OUT ^= 0x01;                   // profile

        OkuTakometre();
        //controller
        KosturKontrolor();

        UART0_OutString("RPM Value of Left: "); UART0_OutUDec(LeftSpeed); UART0_OutString("    \t");
        UART0_OutString("RPM Value of Right: "); UART0_OutUDec(RightSpeed); UART0_OutString("\n");

}
float PWM_RIGHT = 0;
float PWM_LEFT = 0;

void main(void){
    DisableInterrupts();
    Clock_Init48MHz();
    // initialization
    uint32_t s = 0;
    uint32_t rightSensorAdcValue  = 0;
    uint32_t centerSensorAdcValue = 0;
    uint32_t leftSensorAdcValue   = 0;
    ADC0_InitSWTriggerCh17_12_16();   // initialize channels 17,12,16
    ADC_In17_12_16(&rightSensorAdcValue,&centerSensorAdcValue,&leftSensorAdcValue);  // sample
    LPF_Init(rightSensorAdcValue,s);     // P9.0/channel 17
    LPF_Init2(centerSensorAdcValue,s);    // P4.1/channel 12
    LPF_Init3(leftSensorAdcValue,s);    // P9.1/channel 1

    Motor_Init();
    PWM_Init34(100, 0, 0);
    TimerA2_Init(&TimerA2Function, 50000); // 100 Hz
    Tachometer_Init();
    UART0_Init();          // initialize UART0 115,200 baud rate
    LaunchPad_Init();
    EnableInterrupts();

    while(1){
        ADC_In17_12_16(&rightSensorAdcValue,&centerSensorAdcValue,&leftSensorAdcValue);  // sample
        distanceCenter = CenterConvert(centerSensorAdcValue);
        distanceLeft   = LeftConvert(leftSensorAdcValue);
        distanceRight  = RightConvert(rightSensorAdcValue);

     //  UpdateMotorRightDutyCycle(PwmRight);
     //  UpdateMotorLeftDutyCycle(PwmLeft);

        //TimerA1Function();
    }
}
