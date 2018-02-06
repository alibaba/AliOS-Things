; Copyright (C) 2014 Renesas Electronics Corporation
; RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
; This program must be used solely for the purpose for which
; it was furnished by Renesas Electronics Corporation. No part of this
; program may be reproduced or disclosed to others, in any
; form, without the prior written permission of Renesas Electronics
; Corporation.
;---------------------------------------------------------------------
;	_stkinit
;
;	void _stkinit(void __near * stackbss);
;
;	input:
;		stackbss = AX (#LOWW(_stackend))
;	output:
;		NONE
;---------------------------------------------------------------------

;   NOTE       : THIS IS A TYPICAL EXAMPLE.

	.PUBLIC	_stkinit

.textf	.CSEG	TEXTF
_stkinit:
	MOVW	HL,AX		; stack_end_addr
	MOV	[SP+3],#0x00	; [SP+0]-[SP+2] for return address
	MOVW	AX,SP
	SUBW	AX,HL		; SUBW  AX,#LOWW _@STEND
	BNH	$LSTINIT3	; goto end
	SHRW	AX,5		; loop count for 32 byte transfer
	MOVW	BC,AX
	CLRW	AX
LSTINIT1:
	CMPW	AX,BC
	BZ	$LSTINIT2
	MOVW	[HL],AX
	MOVW	[HL+2],AX
	MOVW	[HL+4],AX
	MOVW	[HL+6],AX
	MOVW	[HL+8],AX
	MOVW	[HL+10],AX
	MOVW	[HL+12],AX
	MOVW	[HL+14],AX
	MOVW	[HL+16],AX
	MOVW	[HL+18],AX
	MOVW	[HL+20],AX
	MOVW	[HL+22],AX
	MOVW	[HL+24],AX
	MOVW	[HL+26],AX
	MOVW	[HL+28],AX
	MOVW	[HL+30],AX
	XCHW	AX,HL
	ADDW	AX,#0x20
	XCHW	AX,HL
	DECW	BC
	BR	$LSTINIT1
LSTINIT2:
	MOVW	AX,SP
	CMPW	AX,HL
	BZ	$LSTINIT3	; goto end
	CLRW	AX
	MOVW	[HL],AX
	INCW	HL
	INCW	HL
	BR	$LSTINIT2
LSTINIT3:
	RET
