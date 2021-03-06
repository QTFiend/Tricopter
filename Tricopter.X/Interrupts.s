/**********************************************************
;Asterik Research Electonics
;Interrupt Header Templet

;**********************************************************/

.include "p33FJ32MC202.inc"
.include "Init_ASM_h.inc"
.include "PWM.s"
.include "I2C_Data.s"

      .global __T1Interrupt    ;Declare Timer 1 ISR name global

;..............................................................................
;Constants stored in Program space
;..............................................................................
 .text


__T1Interrupt: 
      	push.d	w0	
      	push.d	w2
      	push.d	w4

        bclr	IFS0,#T1IF 
		mov		_c_Tmr1_Tim,w0
		mov		w0,TMR1

		inc 	_uc_100uS_Timer

        btg     PORTB,#cLEDBLINK

;get accelerometer values
;get gyro values
Call PWMSET

Call GETACCTEMPGYRODATA


/* NOW IN I2C Data

;make accelerometer average
mov AccX, WREG
add AccXOLD
RRNC AccXOLD

mov AccY, WREG
add AccYOLD
RRNC AccYOLD

mov AccZ, WREG
add AccZOLD
RRNC AccZOLD

mov Temperature, WREG
add TemperatureOLD
RRNC TemperatureOLD

mov AccX, WREG
add AccXOLD
RRNC AccXOLD

mov AccX, WREG
add AccXOLD
RRNC AccXOLD

mov AccX, WREG
add AccXOLD
RRNC AccXOLD

mov AccX, WREG
add AccXOLD
RRNC AccXOLD

mov AccX, WREG
add AccXOLD
RRNC AccXOLD

mov AccX, WREG
add AccXOLD
RRNC AccXOLD

*/

;make gyro average

;read if battery low
;RB13
;Read if battery is less than 2.9
;RB14
        
T1_Out:
		pop.d	w4
		pop.d	w2
		pop.d	w0
        RETFIE 
	  
.end
