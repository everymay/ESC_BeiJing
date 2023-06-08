; DCL_DF22.asm - Direct Form 2 implementation in second order
; version 1.0, August 2015

; define C callable labels
   		.def 	_DCL_runDF22Group			; full DF2 controller

; C type definition:
; typedef volatile struct {
;	float b0;	// [0]
;	float b1;	// [2]
;	float b2;	// [4]
;	float a1;	// [6]
;	float a2;	// [8]
;	float x1;	// [A]
;	float x2;	// [C]
; } DF22;


;*** full DF22 controller ***

; C prototype:
; void DCL_runDF22Group(DF22 *p, float *in,float *out,int N)
; argument 1 = *p   : 32-bit ARMA structure address [XAR4]
; argument 2 = *in  : 32-bit input address  [XAR5]
; argument 3 = *out : 32-bit output *-SP[4]
; argument 4 = loop : 32-bit loop number ACC

; example of section allocation in linker command file:
;	dclfuncs		: > RAML4,		PAGE = 0

		.sect	"dclfuncs"
_DCL_runDF22Group:
		MOVL    XAR6, *-SP[4]    		; XAR6 = *out
		ADDB 	AL, #-1            		; Subtract 1 from N since RPTB is 'n-1'
		MOVL   	XAR7, ACC				; XAR7 = LOOP NUM -1

		MOV32   *SP++, R4H
		MOV32   *SP++, R5H
		MOV32   *SP++, R6H
		MOV32	*SP++, R7H

LOOP:	MOV32	R2H, *XAR4++			; R2H = points to b0 of the first filter block
		MOV32	R0H, *XAR5++			; R0H = *in

		MPYF32	R3H, R2H, R0H			; R3H = v0 = b0 * ek
||		MOV32	R4H, *XAR4++			; R4H = b1
		MPYF32	R5H, R4H, R0H			; R5H = v1 = b1 * ek
||		MOV32	R6H, *XAR4++			; R6H = b2
		MPYF32	R4H, R6H, R0H			; R4H = v2 = b2 * ek
||		MOV32	R7H, *XAR4++			; R7H = a1
		MOV32	R0H, *XAR4++			; R0H = a2
		MOV32	R6H, *XAR4++			; R6H = x1
		ADDF32	R1H, R3H, R6H			; R1H = uk = v0 + x1
||		MOV32	R2H, *XAR4				; R2H = x2
		ADDF32	R5H, R5H, R2H			; R5H = v1 + x2
		MPYF32	R6H, R7H, R1H			; R6H = v3 = a1 * uk
		MPYF32	R7H, R0H, R1H			; R7H = v4 = a2 * uk
		SUBF32	R5H, R5H, R6H			; R5H = x1d = v1 + x2 - v3
		SUBF32	R6H, R4H, R7H			; R6H = x2d = v2 - v4
		MOV32	*XAR6++, R1H			; *out=R1H
		MOV32	*XAR4, R6H				; save x2
		MOV32	*--XAR4, R5H			; save x1

		ADDB	XAR4,#4					; points to b0 of the second filter block

		BANZ 	LOOP, AR7--

	    MOV32   R7H, *--SP, UNCF
	    MOV32   R6H, *--SP, UNCF
		MOV32   R5H, *--SP, UNCF
	    MOV32   R4H, *--SP, UNCF
		LRETR
		.end

; end of file
