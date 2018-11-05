/*******************************************************************************
  SPI Peripheral Library Template Implementation

  File Name:
    spi_AudioCommunicationWidth_Unsupported.h

  Summary:
    SPI PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : AudioCommunicationWidth
    and its Variant : Unsupported
    For following APIs :
        PLIB_SPI_AudioCommunicationWidthSelect
        PLIB_SPI_ExistsAudioCommunicationWidth

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

#ifndef _SPI_AUDIOCOMMUNICATIONWIDTH_UNSUPPORTED_H
#define _SPI_AUDIOCOMMUNICATIONWIDTH_UNSUPPORTED_H

//******************************************************************************
/* Function :  SPI_AudioCommunicationWidthSelect_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_SPI_AudioCommunicationWidthSelect 

  Description:
    This template implements the Unsupported variant of the PLIB_SPI_AudioCommunicationWidthSelect function.
*/

PLIB_TEMPLATE void SPI_AudioCommunicationWidthSelect_Unsupported( SPI_MODULE_ID index , SPI_AUDIO_COMMUNICATION_WIDTH mode )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_SPI_AudioCommunicationWidthSelect");
}


//******************************************************************************
/* Function :  SPI_ExistsAudioCommunicationWidth_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_SPI_ExistsAudioCommunicationWidth

  Description:
    This template implements the Unsupported variant of the PLIB_SPI_ExistsAudioCommunicationWidth function.
*/

PLIB_TEMPLATE bool SPI_ExistsAudioCommunicationWidth_Unsupported( SPI_MODULE_ID index )
{
    return false;
}


#endif /*_SPI_AUDIOCOMMUNICATIONWIDTH_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

