/***************************************************************************//**
* \file CyBootAsmGnu.s
* \version 5.70
*
* \brief Assembly routines for GNU as.
*
********************************************************************************
* \copyright
* Copyright 2010-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

.syntax unified
.text
.thumb
.include "cyfittergnu.inc"


/*******************************************************************************
* Function Name: CyDelayCycles
****************************************************************************//**
*
*  Delays for the specified number of cycles.
*
*  \param uint32 cycles: number of cycles to delay.
*
* \return
*  None
*
*******************************************************************************/
/* void CyDelayCycles(uint32 cycles) */
.align 3                    /* Align to 8 byte boundary (2^n) */
.global CyDelayCycles
.func CyDelayCycles, CyDelayCycles
.type CyDelayCycles, %function
.thumb_func
CyDelayCycles:              /* cycles bytes */
    ADDS r0, r0, #2         /*    1    2    Round to nearest multiple of 4 */
    LSRS r0, r0, #2         /*    1    2    Divide by 4 and set flags */
    BEQ CyDelayCycles_done  /*    2    2    Skip if 0 */
.IFDEF CYIPBLOCK_m0s8cpuss_VERSION
    NOP                     /*    1    2    Loop alignment padding */
.ELSE
    .IFDEF CYIPBLOCK_s8srsslt_VERSION
        .IFDEF CYIPBLOCK_m0s8cpussv2_VERSION
            .IFDEF CYIPBLOCK_mxusbpd_VERSION
                /* Do nothing */
            .ELSE
                .IFDEF CYIPBLOCK_m0s8usbpd_VERSION
                    /* Do nothing */
                .ELSE
                    NOP         /*    1    2    Loop alignment padding */
                .ENDIF
            .ENDIF
        .ENDIF
    .ENDIF
    /* Leave loop unaligned */
.ENDIF
CyDelayCycles_loop:
/* For CM0+ branch instruction takes 2 CPU cycles, for CM0 it takes 3 cycles */
.IFDEF CYDEV_CM0P_BASE
    ADDS r0, r0, #1         /*    1    2    Increment counter */
    SUBS r0, r0, #2         /*    1    2    Decrement counter by 2 */
    BNE CyDelayCycles_loop  /*    2    2    2 CPU cycles (if branche is taken)*/
    NOP                     /*    1    2    Loop alignment padding */	
.ELSE
    SUBS r0, r0, #1         /*    1    2    Decrement counter */
    BNE CyDelayCycles_loop  /*    3    2    3 CPU cycles (if branche is taken)*/
    NOP                     /*    1    2    Loop alignment padding */
    NOP                     /*    1    2    Loop alignment padding */
.ENDIF
CyDelayCycles_done:
    NOP                     /*    1    2    Loop alignment padding */
    BX lr                   /*    3    2 */
.endfunc


/*******************************************************************************
* Function Name: CyEnterCriticalSection
****************************************************************************//**
*
*  CyEnterCriticalSection disables interrupts and returns a value indicating
*  whether interrupts were previously enabled (the actual value depends on
*  whether the device is PSoC 3 or PSoC 5).
*
*  Note Implementation of CyEnterCriticalSection manipulates the IRQ enable bit
*  with interrupts still enabled. The test and set of the interrupt bits is not
*  atomic; this is true for both PSoC 3 and PSoC 5. Therefore, to avoid a
*  corrupting processor state, it must be the policy that all interrupt routines
*  restore the interrupt enable bits as they were found on entry.
*
* \return
*  uint8
*   Returns 0 if interrupts were previously enabled or 1 if interrupts
*   were previously disabled.
*
*******************************************************************************/
/* uint8 CyEnterCriticalSection(void) */
.global CyEnterCriticalSection
.func CyEnterCriticalSection, CyEnterCriticalSection
.type CyEnterCriticalSection, %function
.thumb_func
CyEnterCriticalSection:
    MRS r0, PRIMASK         /* Save and return interrupt state */
    CPSID I                 /* Disable interrupts */
    BX lr
.endfunc


/*******************************************************************************
* Function Name: CyExitCriticalSection
****************************************************************************//**
*
*  CyExitCriticalSection re-enables interrupts if they were enabled before
*  CyEnterCriticalSection was called. The argument should be the value returned
*  from CyEnterCriticalSection.
*
*  \param uint8 savedIntrStatus:
*   Saved interrupt status returned by the CyEnterCriticalSection function.
*
* \return
*  None
*
*******************************************************************************/
/* void CyExitCriticalSection(uint8 savedIntrStatus) */
.global CyExitCriticalSection
.func CyExitCriticalSection, CyExitCriticalSection
.type CyExitCriticalSection, %function
.thumb_func
CyExitCriticalSection:
    MSR PRIMASK, r0         /* Restore interrupt state */
    BX lr
.endfunc

.end


/* [] END OF FILE */
