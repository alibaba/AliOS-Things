/*******************************************************************************
  SPI Peripheral Library Template Implementation

  File Name:
    spi_Buffer_Unsupported.h

  Summary:
    SPI PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : Buffer
    and its Variant : Unsupported
    For following APIs :
        PLIB_SPI_BufferClear
        PLIB_SPI_BufferRead
        PLIB_SPI_BufferWrite
        PLIB_SPI_ExistsBuffer
        PLIB_SPI_BufferAddressGet

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

#ifndef _SPI_BUFFER_UNSUPPORTED_H
#define _SPI_BUFFER_UNSUPPORTED_H

//******************************************************************************
/* Function :  SPI_BufferClear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_SPI_BufferClear 

  Description:
    This template implements the Unsupported variant of the PLIB_SPI_BufferClear function.
*/

PLIB_TEMPLATE void SPI_BufferClear_Unsupported( SPI_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_SPI_BufferClear");
}


//******************************************************************************
/* Function :  SPI_BufferRead_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_SPI_BufferRead 

  Description:
    This template implements the Unsupported variant of the PLIB_SPI_BufferRead function.
*/

PLIB_TEMPLATE uint8_t SPI_BufferRead_Unsupported( SPI_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_SPI_BufferRead");

    return 0;
}


//******************************************************************************
/* Function :  SPI_BufferWrite_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_SPI_BufferWrite 

  Description:
    This template implements the Unsupported variant of the PLIB_SPI_BufferWrite function.
*/

PLIB_TEMPLATE void SPI_BufferWrite_Unsupported( SPI_MODULE_ID index , uint8_t data )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_SPI_BufferWrite");
}


//******************************************************************************
/* Function :  SPI_ExistsBuffer_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_SPI_ExistsBuffer

  Description:
    This template implements the Unsupported variant of the PLIB_SPI_ExistsBuffer function.
*/

PLIB_TEMPLATE bool SPI_ExistsBuffer_Unsupported( SPI_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  SPI_BufferAddressGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_SPI_BufferAddressGet 

  Description:
    This template implements the Unsupported variant of the PLIB_SPI_BufferAddressGet function.
*/

PLIB_TEMPLATE void* SPI_BufferAddressGet_Unsupported( SPI_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_SPI_BufferAddressGet");
    return (uint32_t*)0;
}


#endif /*_SPI_BUFFER_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

