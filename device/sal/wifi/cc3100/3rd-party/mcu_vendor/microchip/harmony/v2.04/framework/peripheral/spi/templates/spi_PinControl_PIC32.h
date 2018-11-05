/*******************************************************************************
  SPI Peripheral Library Template Implementation

  File Name:
    spi_PinControl_PIC32.h

  Summary:
    SPI PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : PinControl
    and its Variant : PIC32
    For following APIs :
        PLIB_SPI_PinEnable
        PLIB_SPI_PinDisable
        PLIB_SPI_ExistsPinControl

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _SPI_PINCONTROL_PIC32_H
#define _SPI_PINCONTROL_PIC32_H

#include "spi_registers.h"

//******************************************************************************
/* Function :  SPI_PinEnable_PIC32

  Summary:
    Implements PIC32 variant of PLIB_SPI_PinEnable 

  Description:
    This template implements the PIC32 variant of the PLIB_SPI_PinEnable function.
*/

PLIB_TEMPLATE void SPI_PinEnable_PIC32( SPI_MODULE_ID index , SPI_PIN pin )
{
    spi_registers_t volatile * spi = ((spi_registers_t *)(index));

    if(SPI_PIN_SLAVE_SELECT == pin)
    {
        spi->SPIxCONSET = SPIxCON_SSEN_MASK;
    }
    if(SPI_PIN_DATA_OUT == pin)
    {
        spi->SPIxCONCLR = SPIxCON_DISSDO_MASK;
    }
    if(SPI_PIN_DATA_IN == pin)
    {
        spi->SPIxCONCLR = SPIxCON_DISSDI_MASK;
    }
}


//******************************************************************************
/* Function :  SPI_PinDisable_PIC32

  Summary:
    Implements PIC32 variant of PLIB_SPI_PinDisable 

  Description:
    This template implements the PIC32 variant of the PLIB_SPI_PinDisable function.
*/

PLIB_TEMPLATE void SPI_PinDisable_PIC32( SPI_MODULE_ID index , SPI_PIN pin )
{
    spi_registers_t volatile * spi = ((spi_registers_t *)(index));

    if(SPI_PIN_SLAVE_SELECT == pin)
    {
        spi->SPIxCONCLR = SPIxCON_SSEN_MASK;
    }
    if(SPI_PIN_DATA_OUT == pin)
    {
        spi->SPIxCONSET = SPIxCON_DISSDO_MASK;
    }
    if(SPI_PIN_DATA_IN == pin)
    {
        spi->SPIxCONSET = SPIxCON_DISSDI_MASK;
    }
}


//******************************************************************************
/* Function :  SPI_ExistsPinControl_PIC32

  Summary:
    Implements PIC32 variant of PLIB_SPI_ExistsPinControl

  Description:
    This template implements the PIC32 variant of the PLIB_SPI_ExistsPinControl function.
*/

#define PLIB_SPI_ExistsPinControl PLIB_SPI_ExistsPinControl
PLIB_TEMPLATE bool SPI_ExistsPinControl_PIC32( SPI_MODULE_ID index )
{
    return true;
}


#endif /*_SPI_PINCONTROL_PIC32_H*/

/******************************************************************************
 End of File
*/

