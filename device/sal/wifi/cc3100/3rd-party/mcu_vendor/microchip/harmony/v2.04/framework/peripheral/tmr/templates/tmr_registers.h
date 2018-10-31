/*******************************************************************************
  TMR Peripheral Library structure implementation

  File Name:
    tmr_structure.h

  Summary:
    TMR PLIB base structure implementation

  Description:
    This header file contains instance structure for timer plib module.

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/

//DOM-IGNORE-END
#include <xc.h>

#ifndef _TMR_STRUCTURE_H
#define _TMR_STRUCTURE_H

/* This is the register set structure of TIMER 1 */
typedef struct __attribute__((packed , aligned(4)))
{
    __T1CONbits_t             TxCON;
    volatile unsigned int   TxCONCLR;
    volatile unsigned int   TxCONSET;
    volatile unsigned int   TxCONINV;
    volatile unsigned int   TMRx;
    volatile unsigned int   TMRxCLR;
    volatile unsigned int   TMRxSET;
    volatile unsigned int   TMRxINV;
    volatile unsigned int   PRx;
    volatile unsigned int   PRxCLR;
    volatile unsigned int   PRxSET;
    volatile unsigned int   PRxINV;
 
} tmr1_registers_t;

/* This is the register set structure of TIMER PAIR X/Y (2/3, 4/5 etc) */
typedef struct __attribute__((packed , aligned(4)))
{
    __T2CONbits_t             TxCON;
    volatile unsigned int   TxCONCLR;
    volatile unsigned int   TxCONSET;
    volatile unsigned int   TxCONINV;
    volatile unsigned int   TMRx;
    volatile unsigned int   TMRxCLR;
    volatile unsigned int   TMRxSET;
    volatile unsigned int   TMRxINV;
    volatile unsigned int   PRx;
    volatile unsigned int   PRxCLR;
    volatile unsigned int   PRxSET;
    volatile unsigned int   PRxINV;
 
} tmr_registers_t;

/* This macro decides if the selected timer is even */
#define _TMR_MODULE_ID_IS_EVEN(index)  (((index - _TMR1_BASE_ADDRESS) & 0x200) ? true : false )

/* These are the mask definitions*/
#define    TxCON_TSYNC_MASK        _T1CON_TSYNC_MASK
#define    TxCON_TWDIS_MASK        _T1CON_TWDIS_MASK
#define    TxCON_ON_MASK           _T1CON_ON_MASK
#define    TxCON_TGATE_MASK        _T1CON_TGATE_MASK
#define    TxCON_SIDL_MASK         _T1CON_SIDL_MASK
#define    TxCON_T32_MASK          _T2CON_T32_MASK
#define    TxCON_TCS_MASK          _T1CON_TCS_MASK


#endif /*_TMR_STRUCTURE_H*/

/******************************************************************************
 End of File
*/

