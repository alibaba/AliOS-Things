;-------------------------------------------------------------------------------
; \file CyBootAsmRv.s
; \version 5.70
;
;  \brief Assembly routines for RealView.
;
;-------------------------------------------------------------------------------
; Copyright 2010-2018, Cypress Semiconductor Corporation.  All rights reserved.
; You may use this file only in accordance with the license, terms, conditions,
; disclaimers, and limitations in the end user license agreement accompanying
; the software package with which this file was provided.
;-------------------------------------------------------------------------------

    AREA |.text|,CODE,ALIGN=3
    THUMB
    EXTERN Reset
    INCLUDE cyfitterrv.inc

;-------------------------------------------------------------------------------
; Function Name: CyDelayCycles
;-------------------------------------------------------------------------------
;
; Summary:
;  Delays for the specified number of cycles.
;
; Parameters:
;  uint32 cycles: number of cycles to delay.
;
; Return:
;  None
;
;-------------------------------------------------------------------------------
; void CyDelayCycles(uint32 cycles)
    ALIGN 8
CyDelayCycles FUNCTION
    EXPORT CyDelayCycles
                            ; cycles bytes
    ADDS r0, r0, #2         ;    1    2    Round to nearest multiple of 4
    LSRS r0, r0, #2         ;    1    2    Divide by 4 and set flags
    BEQ CyDelayCycles_done  ;    2    2    Skip if 0
    IF :DEF: CYIPBLOCK_m0s8cpuss_VERSION
        NOP                     ;    1    2    Loop alignment padding
    ELSE
        IF :DEF:  CYIPBLOCK_s8srsslt_VERSION
            IF :DEF:  CYIPBLOCK_m0s8cpussv2_VERSION
                IF :DEF: CYIPBLOCK_mxusbpd_VERSION
                    ; Do nothing
                ELSE
                    IF :DEF: CYIPBLOCK_m0s8usbpd_VERSION
                        ; Do nothing
                    ELSE
                        NOP             ;    1    2    Loop alignment padding
                    ENDIF
                ENDIF
            ENDIF
        ENDIF
        ;Leave loop unaligned
    ENDIF
CyDelayCycles_loop
    ; For CM0+ branch instruction takes 2 CPU cycles, for CM0 it takes 3 cycles 
    IF :DEF: CYDEV_CM0P_BASE
        ADDS r0, r0, #1         ;    1    2    Increment counter
        SUBS r0, r0, #2         ;    1    2    Decrement counter by 2
        BNE CyDelayCycles_loop  ;    2    2    2 CPU cycles (if branche is taken)
        NOP                     ;    1    2    Loop alignment padding
    ELSE
        SUBS r0, r0, #1         ;    1    2    Decrement counter 
        BNE CyDelayCycles_loop  ;    3    2    3 CPU cycles (if branche is taken)
        NOP                     ;    1    2    Loop alignment padding
        NOP                     ;    1    2    Loop alignment padding
    ENDIF
CyDelayCycles_done
    BX lr                   ;    3    2
    ENDFUNC


;-------------------------------------------------------------------------------
; Function Name: CyEnterCriticalSection
;-------------------------------------------------------------------------------
;
; Summary:
;  CyEnterCriticalSection disables interrupts and returns a value indicating
;  whether interrupts were previously enabled (the actual value depends on
;  whether the device is PSoC 3 or PSoC 5).
;
;  Note Implementation of CyEnterCriticalSection manipulates the IRQ enable bit
;  with interrupts still enabled. The test and set of the interrupt bits is not
;  atomic; this is true for both PSoC 3 and PSoC 5. Therefore, to avoid a
;  corrupting processor state, it must be the policy that all interrupt routines
;  restore the interrupt enable bits as they were found on entry.
;
; Parameters:
;  None
;
; Return:
;  uint8
;   Returns 0 if interrupts were previously enabled or 1 if interrupts
;   were previously disabled.
;
;-------------------------------------------------------------------------------
; uint8 CyEnterCriticalSection(void)
CyEnterCriticalSection FUNCTION
    EXPORT CyEnterCriticalSection
    MRS r0, PRIMASK         ; Save and return interrupt state
    CPSID I                 ; Disable interrupts
    BX lr
    ENDFUNC


;-------------------------------------------------------------------------------
; Function Name: CyExitCriticalSection
;-------------------------------------------------------------------------------
;
; Summary:
;  CyExitCriticalSection re-enables interrupts if they were enabled before
;  CyEnterCriticalSection was called. The argument should be the value returned
;  from CyEnterCriticalSection.
;
; Parameters:
;  uint8 savedIntrStatus:
;   Saved interrupt status returned by the CyEnterCriticalSection function.
;
; Return:
;  None
;
;-------------------------------------------------------------------------------
; void CyExitCriticalSection(uint8 savedIntrStatus)
CyExitCriticalSection FUNCTION
    EXPORT CyExitCriticalSection
    MSR PRIMASK, r0         ; Restore interrupt state
    BX lr
    ENDFUNC

    END

; [] END OF FILE
