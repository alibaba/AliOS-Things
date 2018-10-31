/*******************************************************************************
  USART Peripheral Library structure implementation

  File Name:
    usart_registers.h

  Summary:
    USART PLIB base structure implementation

  Description:
    This header file contains instance structure for usart plib module.

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _USART_STRUCTURE_H
#define _USART_STRUCTURE_H

/* This is the register set structure of USART module */
typedef struct __attribute__((packed , aligned(4)))
{
    __U1MODEbits_t            UxMODE;
    volatile unsigned int     UxMODECLR;
    volatile unsigned int     UxMODESET;
    volatile unsigned int     UxMODEINV;
    __U1STAbits_t             UxSTA;
    volatile unsigned int     UxSTACLR;
    volatile unsigned int     UxSTASET;
    volatile unsigned int     UxSTAINV;
    volatile unsigned int     UxTXREG;
    volatile unsigned int     offset1[3];
    volatile unsigned int     UxRXREG;
    volatile unsigned int     offset2[3];
    volatile unsigned int     UxBRG;
 
} usart_registers_t;


/* These are the mask definitions*/
#define    UxMODE_ABAUD_MASK        _U1MODE_ABAUD_MASK
#define    UxMODE_BRGH_MASK         _U1MODE_BRGH_MASK
#define    UxMODE_ON_MASK           _U1MODE_ON_MASK
#define    UxSTA_UTXEN_MASK         _U1STA_UTXEN_MASK
#define    UxSTA_URXEN_MASK         _U1STA_URXEN_MASK
#define    UxMODE_IREN_MASK         _U1MODE_IREN_MASK
#define    UxMODE_STSEL_MASK        _U1MODE_STSEL_MASK
#define    UxMODE_LPBACK_MASK       _U1MODE_LPBACK_MASK
#define    UxSTA_ADDR_MASK          _U1STA_ADDR_MASK
#define    UxSTA_ADDR_POSITION      _U1STA_ADDR_POSITION
#define    UxSTA_ADDEN_MASK         _U1STA_ADDEN_MASK
#define    UxSTA_MASK_MASK          _U1STA_MASK_MASK
#define    UxSTA_MASK_POSITION      _U1STA_MASK_POSITION
#define    UxSTA_URXISEL_MASK       _U1STA_URXISEL_MASK
#define    UxSTA_URXISEL_POSITION   _U1STA_URXISEL_POSITION
#define    UxSTA_UTXISEL_MASK       _U1STA_UTXISEL_MASK
#define    UxSTA_UTXISEL_POSITION   _U1STA_UTXISEL_POSITION
#define    UxSTA_OERR_MASK          _U1STA_OERR_MASK
#define    UxMODE_RXINV_MASK        _U1MODE_RXINV_MASK
#define    UxMODE_SIDL_MASK         _U1MODE_SIDL_MASK
#define    UxSTA_UTXBRK_MASK        _U1STA_UTXBRK_MASK
#define    UxSTA_UTXINV_MASK        _U1STA_UTXINV_MASK
#define    UxMODE_WAKE_MASK         _U1MODE_WAKE_MASK
#define    UxSTA_ADM_EN_MASK        _U1STA_ADM_EN_MASK
#define    UxMODE_RUNOVF_MASK       _U1MODE_RUNOVF_MASK
#define    UxMODE_SLPEN_MASK        _U1MODE_SLPEN_MASK


#endif /*_USART_STRUCTURE_H*/

/******************************************************************************
 End of File
*/

