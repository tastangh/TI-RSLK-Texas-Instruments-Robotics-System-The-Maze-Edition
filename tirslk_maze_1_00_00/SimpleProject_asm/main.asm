; main.s
; Runs on any Cortex M processor
; A very simple first project implementing a random number generator
; Daniel Valvano
; May 12, 2015

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



; we align 32 bit variables to 32-bits
; we align op codes to 16 bits
       .thumb
       .data
       .align 2
M      .space 4
n      .space 4
       .text
       .align 2
       .global  main
main:  .asmfunc
       MOV R0,#1       ; Initial seed
       BL  Seed        ; M=1
loop   BL  Rand
       LDR R1,nAddr
       STR R0,[R1]
       B   loop
       .endasmfunc

;------------Random------------
; Return R0= random number
; Linear congruential generator
; from Numerical Recipes by Press et al.
Seed: .asmfunc
      LDR R1,MAddr ; R1=&M
      STR R0,[R1]  ; set M
      BX  LR
      .endasmfunc
Rand: .asmfunc
      LDR R2,MAddr ; R2=&M, address of M
      LDR R0,[R2]  ; R0=M, value of M
      LDR R1,Slope
      MUL R0,R0,R1 ; R0 = 1664525*M
      LDR R1,Offst
      ADD R0,R0,R1 ; 1664525*M+1013904223
      STR R0,[R2]  ; store M
      LSR R0,#24   ; 0 to 255
      BX  LR
      .endasmfunc
MAddr .field M,32
Slope .field 1664525,32
Offst .field 1013904223,32
nAddr .field n,32

       .end
