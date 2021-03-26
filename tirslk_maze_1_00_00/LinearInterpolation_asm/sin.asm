; Sin.asm
; Runs on any Cortex M
; Use linear interpolation to implement a sine table lookup using a table
; of only 21 elements.  For input values between table entries, it is
; assumed that the sine function is linear between the entries.  The input
; is given in 8-bit unsigned fixed point with a resolution of 2*pi/256,
; and the output is given in 8-bit signed fixed point with a resolution of
; 1/127.
; In other words:
; sin(90 deg) = sin(pi/2 rad) = Sin(64 inputUnits) = 127 outputUnits = 1
; Daniel Valvano
; September 11, 2013

; This example accompanies the books
;   "Embedded Systems: Introduction to the MSP432 Microcontroller",
;       ISBN: 978-1512185676, Jonathan Valvano, copyright (c) 2017
;   "Embedded Systems: Real-Time Interfacing to the MSP432 Microcontroller",
;       ISBN: 978-1514676585, Jonathan Valvano, copyright (c) 2017
;   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
;       ISBN: 978-1466468863, , Jonathan Valvano, copyright (c) 2017
; For more information about my classes, my research, and my books, see
; http://users.ece.utexas.edu/~valvano/
;
;Simplified BSD License (FreeBSD License)
;Copyright (c) 2017, Jonathan Valvano, All rights reserved.
;
;Redistribution and use in source and binary forms, with or without modification,
;are permitted provided that the following conditions are met:
;
;1. Redistributions of source code must retain the above copyright notice,
;   this list of conditions and the following disclaimer.
;2. Redistributions in binary form must reproduce the above copyright notice,
;   this list of conditions and the following disclaimer in the documentation
;   and/or other materials provided with the distribution.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
;LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
;DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
;AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
;OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
;USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
;The views and conclusions contained in the software and documentation are
;those of the authors and should not be interpreted as representing official
;policies, either expressed or implied, of the FreeBSD Project.


       .thumb
       .text
       .align 2
       .global Sin
IxTab .long 0, 13, 26, 38, 51, 64, 77, 90
      .long 102, 115, 128, 141, 154, 166, 179
      .long 192, 205, 218, 230, 243, 255, 256

IyTab .long 0, 39, 75, 103, 121, 127, 121, 103
      .long 75, 39, 0, -39, -75, -103, -121
      .long -127, -121, -103, -75, -39, 0, 0

;------------Sin------------
; Calculate the sine value of a given angle using table lookup and
; linear interpolation.
; Input: R0 is Ix, 8-bit unsigned angle 0 to 255 (units of pi/128)
; Output: R0 is Iy, 8-bit signed result -127 to +127 (units of 1/127)
; Modifies: R1, R2
Sin:   .asmfunc
       PUSH {R4-R6,LR}
       LDR  R1,IxTabAddr  ;find x1<=Ix<x2
       LDR  R2,IyTabAddr
lookx1 LDR  R6,[R1,#4] ;x2
       CMP  R0,R6      ;check Ix<x2
       BLO  found      ;R1 => x1
       ADD  R1,#4
       ADD  R2,#4
       B    lookx1
found  LDR  R4,[R1]    ;x1
       SUB  R4,R0,R4   ;Ix-x1
       LDR  R5,[R2,#4] ;y2
       LDR  R2,[R2]    ;y1
       SUB  R5,R2      ;y2-y1
       LDR  R1,[R1]    ;x1
       SUB  R6,R1      ;x2-x1
       MUL  R0,R4,R5   ;(y2-y1)*(Ix-x1)
       SDIV R0,R0,R6
       ADD  R0,R2      ;Iy
       POP {R4-R6,PC}
IxTabAddr .field IxTab,32
IyTabAddr .field IyTab,32
    .endasmfunc
    .end
