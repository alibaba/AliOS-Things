/*******************************************************************************
  Ports Peripheral Library Compatibility Header

  Company:
    Microchip Technology Inc.

  File Name:
    plib_ports_compatibility.h

  Summary:
    Ports Peripheral Library Interface header for backward compatibility.

  Description:
    This header file contains the definitions of the functions which are
    supported for backward compatibility only. These will be deprecated later. 
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2013-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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
// DOM-IGNORE-END

#ifndef _PLIB_PORTS_COMPATIBILITY_H
#define _PLIB_PORTS_COMPATIBILITY_H

#include <stdint.h>
#include <stddef.h>


// *****************************************************************************
/* Function:
   bool PLIB_PORTS_ChangeNoticePerPortHasOccured ( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel, PORTS_BIT_POS  bitPos );

  Summary:
    checks the status of change on the pin

  Description:
    This function checks if the change has occurred on the given pin or not.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    bitPos          - Position in the PORT pins

  Returns:
    None.

  Example:
    <code>
    
    if(PLIB_PORTS_ChangeNoticePerPortHasOccured( PORTS_ID_0, 
                                    PORT_CHANNEL_C, PORTS_BIT_POS_4 ) == True)
    {
     //do something
    }
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePerPortStatus in your application to determine
    whether this feature is available.
    
    This function will be deprecated later.
*/

#define PLIB_PORTS_ChangeNoticePerPortHasOccured    PLIB_PORTS_ChangeNoticePerPortHasOccurred


#endif // #ifndef _PLIB_PORTS_COMPATIBILITY_H
/*******************************************************************************
 End of File
*/