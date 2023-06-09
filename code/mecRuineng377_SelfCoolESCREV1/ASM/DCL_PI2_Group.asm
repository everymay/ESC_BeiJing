; DCL_PI.asm - PI controller
; version 1.0, August 2015

; C prototype:
; float DCL_runPI(PI *p, float rk, float yk)
; argument 1 = *p : 32-bit PID structure address [XAR4]
; argument 2 = rk : 32-bit floating-point reference [R0H]
; argument 3 = yk : 32-bit floating-point feedback [R1H]
; return = uk : 32-bit floating-point [R0H]

; float DCL_runPI_Group(PI *p,float *in,float *out,int N)
; argument 1 = *p : 32-bit PID structure address [XAR4]
; argument 2 = *in  : 32-bit input address  [XAR5]
; argument 3 = *out : 32-bit output *-SP[4]
; argument 4 = loop : 32-bit loop number ACC
   		.def 	_DCL_runPI_Group				; define C callable label
;
; C type definition:
; typedef volatile struct {
; 	float Kp;		// [0] proportional gain
; 	float Ki;		// [2] integral gain
; 	float i10;		// [4] I storage
;	float Umax;		// [6] upper saturation limit
;	float Umin;		// [8] lower saturation limit
;	float i6;		// [A] saturation storage
; } PI;

; example of section allocation in linker command file:
;	dclfuncs		: > RAML4,		PAGE = 0

		.sect	"dclfuncs"
_DCL_runPI_Group:
; context save
        PUSH	AR1H:AR0H
		PUSH 	XAR6
		MOVL    XAR6, *-SP[6]    			; XAR6 = *out
		SUBB	ACC,#1
		MOVL   	XAR3, ACC					; XAR3 = LOOP NUM -1

		MOV32   *SP++, R3H
		MOV32   *SP++, R4H
		MOV32   *SP++, R5H
		MOV32   *SP++, R6H

;*** servo error ***
LOOP:	MOV32	R4H, *XAR5++				; R4H =v1= *in

;*** proportional path ***
		MOV32	R5H, *+XAR4[0]				; R5H = Kp
		MPYF32	R6H, R4H, R5H				; R6H = v2

;*** integral path ***
		MOV32	R5H, *+XAR4[2]				; R5H = Ki
		MPYF32	R4H, R5H, R6H				; R4H = v3
		MOV		AR1, #0xA					; AR1 = 10	MOV		AR1, #0xA
		MOV32	R5H, *+XAR4[AR1]			; R5H = i6	MOV32	R7H, *+XAR4[AR1]
		MPYF32	R3H, R4H, R5H				; R3H = v8
		MOV32	R4H, *+XAR4[4]				; R4H = i10
		ADDF32	R5H, R4H, R3H				; R5H = v4
		ZERO	R1H							; R1H = 0.0f
		MOV32	*+XAR4[4], R5H				; save i10

;*** control ***
		ADDF32	R0H, R5H, R6H				; R0H = v5
		ADDF32	R5H, R1H, #1.0				; R5H = 1.0f
		MOV32	R3H, *+XAR4[6]				; R3H = Umax
		MINF32	R0H, R3H					; if (v5 > Umax) R0H = Umax else R0H = v5
||		MOV32	R5H, R1H					; R5H = 0.0f
		MOV		AR0, #8						; AR0 = 8
		MOV32	R3H, *+XAR4[AR0]			; R3H = Umin
		MAXF32	R0H, R3H					; if (v5 < Umin) R0H = Umin else R0H = v5
||		MOV32	R5H, R1H					; R5H = 0.0f

;*** anti-windup ***
		MOV32   R6H, *--SP, UNCF			; delay slot
		MOV32	*+XAR4[AR1], R5H			; save i6
		MOV32	*XAR6++, R0H				; *out=R0H
		BANZ 	LOOP, AR3--

; context restore
	    MOV32   R5H, *--SP, UNCF
	    MOV32   R4H, *--SP, UNCF
	    MOV32   R3H, *--SP, UNCF
	    POP 	XAR6
        POP     AR1H:AR0H
		LRETR

		.end
		
; end of file
