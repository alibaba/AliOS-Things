/*******************************************************************************
  USART Peripheral Library Template Implementation

  File Name:
    usart_WakeOnStart_Unsupported.h

  Summary:
    USART PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : WakeOnStart
    and its Variant : Unsupported
    For following APIs :
        PLIB_USART_ExistsWakeOnStart
        PLIB_USART_WakeOnStartEnable
        PLIB_USART_WakeOnStartDisable
        PLIB_USART_WakeOnStartIsEnabled
        PLIB_USART_InitializeModeGeneral
        PLIB_USART_ErrorsGet

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _USART_WAKEONSTART_UNSUPPORTED_H
#define _USART_WAKEONSTART_UNSUPPORTED_H


//******************************************************************************
/* Function :  USART_ExistsWakeOnStart_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USART_ExistsWakeOnStart

  Description:
    This template implements the Unsupported variant of the PLIB_USART_ExistsWakeOnStart function.
*/

PLIB_TEMPLATE bool USART_ExistsWakeOnStart_Unsupported( USART_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  USART_WakeOnStartEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USART_WakeOnStartEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_USART_WakeOnStartEnable function.
*/

PLIB_TEMPLATE void USART_WakeOnStartEnable_Unsupported( USART_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USART_WakeOnStartEnable");
}


//******************************************************************************
/* Function :  USART_WakeOnStartDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USART_WakeOnStartDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_USART_WakeOnStartDisable function.
*/

PLIB_TEMPLATE void USART_WakeOnStartDisable_Unsupported( USART_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USART_WakeOnStartDisable");
}


//******************************************************************************
/* Function :  USART_WakeOnStartIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USART_WakeOnStartIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_USART_WakeOnStartIsEnabled function.
*/

PLIB_TEMPLATE bool USART_WakeOnStartIsEnabled_Unsupported( USART_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USART_WakeOnStartIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  USART_InitializeModeGeneral_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USART_InitializeModeGeneral 

  Description:
    This template implements the Unsupported variant of the PLIB_USART_InitializeModeGeneral function.
*/

PLIB_TEMPLATE void USART_InitializeModeGeneral_Unsupported( USART_MODULE_ID index , bool autobaud , bool loopBackMode , bool wakeFromSleep , bool irdaMode , bool stopInIdle )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USART_InitializeModeGeneral");
}


//******************************************************************************
/* Function :  USART_ErrorsGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USART_ErrorsGet 

  Description:
    This template implements the Unsupported variant of the PLIB_USART_ErrorsGet function.
*/

PLIB_TEMPLATE USART_ERROR USART_ErrorsGet_Unsupported( USART_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USART_ErrorsGet");

    return (USART_ERROR) 0;
}


#endif /*_USART_WAKEONSTART_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

