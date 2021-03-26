// ApplicationProcessor.c
// Runs on any computer
// Test of the CC2650 simple network processor.
// To port this program to a different processor, 
//     re-write GPIO.c, UART0.c, and UART1.c for that processor.
// This file should contain nothing hardware specific and require no significant changes.
// Daniel Valvano and Jonathan Valvano
// February 23, 2017
// This version automatically dumps all SNP communication to the UART0 for debugging

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

//------------See AP.c for details of hardware connections to CC2650--------------------
//------------See LaunchPad.c for details of switches and LEDs--------------------------

#include <stdint.h>
#include "../inc/Clock.h"
#include "../inc/CortexM.h"
#include "../inc/AP.h"
#include "../inc/UART0.h"
#include "../inc/LaunchPad.h"
#include "msp.h"

//--------------------------------------------------
// 1. Initialize GATT (add services, characteristics, CCCD’s, etc.). See Section 9.5.
// 2. Initialize GAP (advertisement data, connection parameters, etc.). See Section 9.3.
// 3. Advertise and optionally wait for a connection. See Section 9.4.
// 4. Respond to GATT requests and send notifications / indications as desired. See Section 9.6

uint8_t  ByteData;      // 8-bit user read/write data
uint16_t HalfWordData;  // 16-bit user read only data from switches 
uint32_t WordData;      // 32-bit user write only data to LED
uint16_t Switch1;       // 16-bit notify data from Button 1
uint32_t Switch2;       // 32-bit notify data from Button 2

// ********OutValue**********
// Debugging dump of a data value to virtual serial port to PC
// data shown as 1 to 8 hexadecimal characters
// Inputs:  response (number returned by last AP call)
// Outputs: none
void OutValue(char *label,uint32_t value){ 
  UART0_OutString(label);
  UART0_OutUHex(value);
}
void ReadByteData(void){ // called on a SNP Characteristic Read Indication for characteristic ByteData
  ByteData++;
  LaunchPad_Output(ByteData);    // toggle LED 
  OutValue("\n\rRead ByteData=",ByteData);
}
void ReadHalfWordData(void){ // called on a SNP Characteristic Read Indication for characteristic HalfWordData
  LaunchPad_Output(ByteData^GREEN);   // toggle green color
  HalfWordData = LaunchPad_Input();
  OutValue("\n\rRead HalfWord=",HalfWordData);
}
void WriteByteData(void){ // called on a SNP Characteristic Write Indication on characteristic ByteData
  LaunchPad_Output(ByteData^BLUE);   // toggle blue color
  OutValue("\n\rWrite ByteData=",ByteData);
}
void WriteWordData(void){  // called on a SNP Characteristic Write Indication on characteristic WordData
  LaunchPad_Output(WordData);  // set LEDs with bottom bits
  OutValue("\n\rWordData=",WordData);
}
void Button1(void){ // called on SNP CCCD Updated Indication
  OutValue("\n\rButton 1 CCCD=",AP_GetNotifyCCCD(0));
}
void Button2(void){ 
  OutValue("\n\rButton 2 CCCD=",AP_GetNotifyCCCD(1));
}  


int main(void){ volatile int r; 
  int time=0;  
  DisableInterrupts();
  Clock_Init48MHz();
  UART0_Init();
  LaunchPad_Init();  // input from switches, output to LEDs on LaunchPad
  EnableInterrupts();
  UART0_OutString("\n\rApplication Processor - MSP432-CC2650\n\r");
  r = AP_Init(); 
  AP_GetStatus();  // optional
  AP_GetVersion(); // optional
  AP_AddService(0xFFF0); 
  //------------------------
  ByteData = 1;  // read/write parameter
  AP_AddCharacteristic(0xFFF1,1,&ByteData,0x03,0x0A,"ByteData",&ReadByteData,&WriteByteData);
  //------------------------
  HalfWordData = LaunchPad_Input(); // read only parameter (get from switches)
  AP_AddCharacteristic(0xFFF2,2,&HalfWordData,0x01,0x02,"HalfWordData",&ReadHalfWordData,0);
  //------------------------
  WordData = 0;   // write only parameter (sent to LED)
  AP_AddCharacteristic(0xFFF3,4,&WordData,0x02,0x08,"WordData",0,&WriteWordData);
  //------------------------
  Switch1 = 0; 
  AP_AddNotifyCharacteristic(0xFFF4,2,&Switch1,"Button 1",&Button1);
  //------------------------
  Switch2 = 0x00000000; 
  AP_AddNotifyCharacteristic(0xFFF5,4,&Switch2,"Button 2",&Button2);
  //------------------------
  AP_RegisterService();
  AP_StartAdvertisement();
  AP_GetStatus(); // optional
  while(1){
    time++;
    AP_BackgroundProcess();  // handle incoming SNP frames
    if(time>2000000){
      time = 0;
      Switch1 = LaunchPad_Input()&0x01;   // Button 1 
      if(AP_GetNotifyCCCD(0)){  
        OutValue("\n\rNotify Button 1=",Switch1);
        AP_SendNotification(0); 
      }
      Switch2 = (LaunchPad_Input()>>1)&0x01;   // Button 2 
      if(AP_GetNotifyCCCD(1)){
        OutValue("\n\rNotify Button 2=",Switch2);
        AP_SendNotification(1);
      }
    }
  }
}
