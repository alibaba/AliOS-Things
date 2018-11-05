/*******************************************************************************
  SPI Peripheral Library structure implementation

  File Name:
    spi_registers.h

  Summary:
    SPI PLIB base structure implementation

  Description:
    This header file contains instance structure for spi plib module.
    SPI2 is used as reference instance instead of SPI1 because 
    some devices doesn't support SPI1

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

#ifndef _SPI_STRUCTURE_H
#define _SPI_STRUCTURE_H

/* This is the register set structure of USART module */
typedef struct __attribute__((packed , aligned(4)))
{
    __SPI2CONbits_t           SPIxCON;
    volatile unsigned int     SPIxCONCLR;
    volatile unsigned int     SPIxCONSET;
    volatile unsigned int     SPIxCONINV;
    __SPI2STATbits_t          SPIxSTAT;
    volatile unsigned int     SPIxSTATCLR;
    volatile unsigned int     SPIxSTATSET;
    volatile unsigned int     SPIxSTATINV;
    volatile unsigned int     SPIxBUF;
    volatile unsigned int     offset1[3];
    volatile unsigned int     SPIxBRG;
#ifdef _SPI2CON2_w_MASK
    volatile unsigned int     offset2[3];
    __SPI2CON2bits_t          SPIxCON2;
    volatile unsigned int     SPIxCON2CLR;
    volatile unsigned int     SPIxCON2SET;
    volatile unsigned int     SPIxCON2INV;
#endif
 
} spi_registers_t;


/* These are the mask definitions*/
#define    SPIxCON_MODE32_MASK        _SPI2CON_MODE32_MASK
#define    SPIxCON_MODE16_MASK        _SPI2CON_MODE16_MASK
#define    SPIxCON_MODE16_POSITION    _SPI2CON_MODE16_POSITION
#define    SPIxCON2_IGNROV_MASK       _SPI2CON2_IGNROV_MASK
#define    SPIxCON2_IGNTUR_MASK       _SPI2CON2_IGNTUR_MASK
#define    SPIxCON2_AUDEN_MASK        _SPI2CON2_AUDEN_MASK
#define    SPIxCON2_AUDMOD_MASK       _SPI2CON2_AUDMOD_MASK
#define    SPIxCON_ON_MASK            _SPI2CON_ON_MASK
#define    SPIxCON2_FRMERREN_MASK     _SPI2CON2_FRMERREN_MASK
#define    SPIxCON2_SPIROVEN_MASK     _SPI2CON2_SPIROVEN_MASK
#define    SPIxCON2_SPITUREN_MASK     _SPI2CON2_SPITUREN_MASK
#define    SPIxCON_ENHBUF_MASK        _SPI2CON_ENHBUF_MASK
#define    SPIxCON_FRMEN_MASK         _SPI2CON_FRMEN_MASK
#define    SPIxSTAT_FRMERR_MASK       _SPI2STAT_FRMERR_MASK
#define    SPIxCON_MSTEN_MASK         _SPI2CON_MSTEN_MASK
#define    SPIxCON_SSEN_MASK          _SPI2CON_SSEN_MASK
#define    SPIxCON_DISSDI_MASK        _SPI2CON_DISSDI_MASK
#define    SPIxCON_DISSDO_MASK        _SPI2CON_DISSDO_MASK
#define    SPIxCON_MSSEN_MASK         _SPI2CON_MSSEN_MASK
#define    SPIxCON_SIDL_MASK          _SPI2CON_SIDL_MASK
#define    SPIxSTAT_SPITUR_MASK       _SPI2STAT_SPITUR_MASK
#define    SPIxSTAT_SPIROV_MASK       _SPI2STAT_SPIROV_MASK


#endif /*_SPI_STRUCTURE_H*/

/******************************************************************************
 End of File
*/

