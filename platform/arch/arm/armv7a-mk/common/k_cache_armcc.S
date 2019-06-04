;/*
;*
;* ;file k_cache_gcc.S
;*
;*/

;******************************************************************************
;                            EXPORT FUNCTIONS
;******************************************************************************
	export k_dcache_clean_all
	export k_dcache_invalidate_all
	export k_dcache_clean_invalidate_all

;******************************************************************************
;                        CODE GENERATION DIRECTIVES
;******************************************************************************
	AREA ||.text||, CODE, READONLY, ALIGN = 2

;******************************************************************************
; Functions:
;     void k_dcache_clean_all(void);
; reference:
;   "ARM ®  Architecture Reference Manual ARMv7-A and ARMv7-R edition"
;   --Example code for cache maintenance operations
;******************************************************************************
k_dcache_clean_all
    PUSH {R4-R11}
    MRC p15, #1, R0, c0, c0, #1   ; Read CLIDR into R0
    ANDS R3, R0, #0x07000000
    MOV R3, R3, LSR #23           ; Cache level value (naturally aligned)
    BEQ A_Finished
    MOV R10, #0
A_Loop1
    ADD R2, R10, R10, LSR #1      ; Work out 3 x cachelevel
    MOV R1, R0, LSR R2            ; bottom 3 bits are the Cache type for this level
    AND R1, R1, #7                ; get those 3 bits alone
    CMP R1, #2
    BLT A_Skip                    ; no cache or only instruction cache at this level
    MCR p15, #2, R10, c0, c0, #0  ; write CSSELR from R10
    ISB                           ; ISB to sync the change to the CCSIDR
    MRC p15, #1, R1, c0, c0, #0   ; read current CCSIDR to R1
    AND R2, R1, #7                ; extract the line length field
    ADD R2, R2, #4                ; add 4 for the line length offset (log2 16 bytes)
    LDR R4, =0x3FF
    ANDS R4, R4, R1, LSR #3       ; R4 is the max number on the way size (right aligned)
    CLZ R5, R4                    ; R5 is the bit position of the way size increment
    MOV R9, R4                    ; R9 working copy of the max way size (right aligned)
A_Loop2
    LDR R7, =0x00007FFF
    ANDS R7, R7, R1, LSR #13      ; R7 is the max number of the index size (right aligned)
A_Loop3
    ORR R11, R10, R9, LSL R5      ; factor in the way number and cache number into R11
    ORR R11, R11, R7, LSL R2      ; factor in the index number
    MCR p15, #0, R11, c7, c10, #2 ; DCCSW, clean by set/way
    SUBS R7, R7, #1               ; decrement the index
    BGE A_Loop3
    SUBS R9, R9, #1               ; decrement the way number
    BGE A_Loop2
A_Skip
    ADD R10, R10, #2              ; increment the cache number
    CMP R3, R10
    BGT A_Loop1
    DSB
A_Finished
    POP {R4-R11}
    BX  lr

;******************************************************************************
; Functions:
;     void k_dcache_invalidate_all(void);
; reference:
;   "ARM ®  Architecture Reference Manual ARMv7-A and ARMv7-R edition"
;   --Example code for cache maintenance operations
;******************************************************************************
k_dcache_invalidate_all
    PUSH {R4-R11}
    MRC p15, #1, R0, c0, c0, #1   ; Read CLIDR into R0
    ANDS R3, R0, #0x07000000
    MOV R3, R3, LSR #23           ; Cache level value (naturally aligned)
    BEQ B_Finished
    MOV R10, #0
B_Loop1
    ADD R2, R10, R10, LSR #1      ; Work out 3 x cachelevel
    MOV R1, R0, LSR R2            ; bottom 3 bits are the Cache type for this level
    AND R1, R1, #7                ; get those 3 bits alone
    CMP R1, #2
    BLT B_Skip                    ; no cache or only instruction cache at this level
    MCR p15, #2, R10, c0, c0, #0  ; write CSSELR from R10
    ISB                           ; ISB to sync the change to the CCSIDR
    MRC p15, #1, R1, c0, c0, #0   ; read current CCSIDR to R1
    AND R2, R1, #7                ; extract the line length field
    ADD R2, R2, #4                ; add 4 for the line length offset (log2 16 bytes)
    LDR R4, =0x3FF
    ANDS R4, R4, R1, LSR #3       ; R4 is the max number on the way size (right aligned)
    CLZ R5, R4                    ; R5 is the bit position of the way size increment
    MOV R9, R4                    ; R9 working copy of the max way size (right aligned)
B_Loop2
    LDR R7, =0x00007FFF
    ANDS R7, R7, R1, LSR #13      ; R7 is the max number of the index size (right aligned)
B_Loop3
    ORR R11, R10, R9, LSL R5      ; factor in the way number and cache number into R11
    ORR R11, R11, R7, LSL R2      ; factor in the index number
    MCR p15, #0, R11, c7, c6, #2  ; DCISW, clean by set/way
    SUBS R7, R7, #1               ; decrement the index
    BGE B_Loop3
    SUBS R9, R9, #1               ; decrement the way number
    BGE B_Loop2
B_Skip
    ADD R10, R10, #2              ; increment the cache number
    CMP R3, R10
    BGT B_Loop1
    DSB
B_Finished
    POP {R4-R11}
    BX  lr

;******************************************************************************
; Functions:
;     void k_dcache_clean_invalidate_all(void);
; reference:
;   "ARM ®  Architecture Reference Manual ARMv7-A and ARMv7-R edition"
;   --Example code for cache maintenance operations
;******************************************************************************
k_dcache_clean_invalidate_all
    PUSH {R4-R11}
    MRC p15, #1, R0, c0, c0, #1   ; Read CLIDR into R0
    ANDS R3, R0, #0x07000000
    MOV R3, R3, LSR #23           ; Cache level value (naturally aligned)
    BEQ C_Finished
    MOV R10, #0
C_Loop1
    ADD R2, R10, R10, LSR #1      ; Work out 3 x cachelevel
    MOV R1, R0, LSR R2            ; bottom 3 bits are the Cache type for this level
    AND R1, R1, #7                ; get those 3 bits alone
    CMP R1, #2
    BLT C_Skip                    ; no cache or only instruction cache at this level
    MCR p15, #2, R10, c0, c0, #0  ; write CSSELR from R10
    ISB                           ; ISB to sync the change to the CCSIDR
    MRC p15, #1, R1, c0, c0, #0   ; read current CCSIDR to R1
    AND R2, R1, #7                ; extract the line length field
    ADD R2, R2, #4                ; add 4 for the line length offset (log2 16 bytes)
    LDR R4, =0x3FF
    ANDS R4, R4, R1, LSR #3       ; R4 is the max number on the way size (right aligned)
    CLZ R5, R4                    ; R5 is the bit position of the way size increment
    MOV R9, R4                    ; R9 working copy of the max way size (right aligned)
C_Loop2
    LDR R7, =0x00007FFF
    ANDS R7, R7, R1, LSR #13      ; R7 is the max number of the index size (right aligned)
C_Loop3
    ORR R11, R10, R9, LSL R5      ; factor in the way number and cache number into R11
    ORR R11, R11, R7, LSL R2      ; factor in the index number
    MCR p15, #0, R11, c7, c14, #2 ; DCCISW, clean by set/way
    SUBS R7, R7, #1               ; decrement the index
    BGE C_Loop3
    SUBS R9, R9, #1               ; decrement the way number
    BGE C_Loop2
C_Skip
    ADD R10, R10, #2              ; increment the cache number
    CMP R3, R10
    BGT C_Loop1
    DSB
C_Finished
    POP {R4-R11}
    BX  lr

    END

