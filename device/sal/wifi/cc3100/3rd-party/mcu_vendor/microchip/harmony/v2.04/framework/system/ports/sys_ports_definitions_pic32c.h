/*******************************************************************************
  PORTS System Service Library Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    sys_ports_definitions_pic32c.h

  Summary:
    PORTS System Service data type definitions header.

  Description:
    This file contains the data type definitions header.
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
//DOM-IGNORE-END

#ifndef _SYS_PORTS_DEFINITIONS_PIC32C_H
#define _SYS_PORTS_DEFINITIONS_PIC32C_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "system_config.h"
#include "system/common/sys_common.h"
#include "system/common/sys_module.h"
#include "system/int/sys_int.h"
#include "arch/arm/devices_pic32c.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

/* PORTS channel set */
typedef enum {

    PORTS_CHANNEL_A = _PORTA_BASE_ADDRESS,
    PORTS_CHANNEL_B = _PORTB_BASE_ADDRESS,
    PORTS_CHANNEL_C = _PORTC_BASE_ADDRESS,
    PORTS_CHANNEL_D = _PORTD_BASE_ADDRESS,
    PORTS_CHANNEL_E = _PORTE_BASE_ADDRESS

} PORTS_CHANNEL;

/* PORTS bit position */
typedef enum {

    PORTS_BIT_POS_0 = 0,
    PORTS_BIT_POS_1 = 1,
    PORTS_BIT_POS_2 = 2,
    PORTS_BIT_POS_3 = 3,
    PORTS_BIT_POS_4 = 4,
    PORTS_BIT_POS_5 = 5,
    PORTS_BIT_POS_6 = 6,
    PORTS_BIT_POS_7 = 7,
    PORTS_BIT_POS_8 = 8,
    PORTS_BIT_POS_9 = 9,
    PORTS_BIT_POS_10 = 10,
    PORTS_BIT_POS_11 = 11,
    PORTS_BIT_POS_12 = 12,
    PORTS_BIT_POS_13 = 13,
    PORTS_BIT_POS_14 = 14,
    PORTS_BIT_POS_15 = 15,
    PORTS_BIT_POS_16 = 16,
    PORTS_BIT_POS_17 = 17,
    PORTS_BIT_POS_18 = 18,
    PORTS_BIT_POS_19 = 19,
    PORTS_BIT_POS_20 = 20,
    PORTS_BIT_POS_21 = 21,
    PORTS_BIT_POS_22 = 22,
    PORTS_BIT_POS_23 = 23,
    PORTS_BIT_POS_24 = 24,
    PORTS_BIT_POS_25 = 25,
    PORTS_BIT_POS_26 = 26,
    PORTS_BIT_POS_27 = 27,
    PORTS_BIT_POS_28 = 28,
    PORTS_BIT_POS_29 = 29,
    PORTS_BIT_POS_30 = 30,
    PORTS_BIT_POS_31 = 31

} PORTS_BIT_POS;


/* PORTS data types */
typedef uint32_t       PORTS_DATA_TYPE;
typedef uint32_t       PORTS_DATA_MASK;
typedef uint32_t       PORTS_BIT_POS_MASK;

// *****************************************************************************
/* SYS PORTS PULLUP status

  Summary:
    Provides the pull-up and pull-down status.

  Description:
    These constants provide the pull-up or pull-down status definitions.

  Remarks:
    None.

*/

typedef enum
{
    /* PULLUP Disable */
    SYS_PORTS_PULLUP_DISABLE
        /*DOM-IGNORE-BEGIN*/  = 0 /*DOM-IGNORE-END*/,
    /* PULLUP Enable */
    SYS_PORTS_PULLUP_ENABLE
        /*DOM-IGNORE-BEGIN*/  = 1 /*DOM-IGNORE-END*/,

} SYS_PORTS_PULLUP_PULLDOWN_STATUS;

// *****************************************************************************
/* SYS PORTS PIN DIRECTION

  Summary:
    Defines the direction of the port pins.

  Description:
    These constants provide the port pin direction definitions.

  Remarks:
    None.

*/

typedef enum
{
    /* Direction as output */
    SYS_PORTS_DIRECTION_OUTPUT
        /*DOM-IGNORE-BEGIN*/  = 0 /*DOM-IGNORE-END*/,
    /* Direction as input */
    SYS_PORTS_DIRECTION_INPUT
        /*DOM-IGNORE-BEGIN*/  = 1 /*DOM-IGNORE-END*/,

} SYS_PORTS_PIN_DIRECTION;

// *****************************************************************************
/* For plib data type compatibility */
typedef enum {

    PORTS_PIN_MODE_NONE

} PORTS_PIN_MODE;

typedef enum {

    PORTS_CHANGE_NOTICE_PIN_NONE

} PORTS_CHANGE_NOTICE_PIN;

typedef enum {

    PORTS_CN_PIN_NONE

} PORTS_CN_PIN;

typedef enum {

    PORTS_ANALOG_PIN_NONE

} PORTS_ANALOG_PIN;

typedef enum {

    PORTS_AN_PIN_NONE

} PORTS_AN_PIN;

typedef enum {

    PORTS_REMAP_INPUT_FUNCTION_NONE

} PORTS_REMAP_INPUT_FUNCTION;

typedef enum {

    PORTS_REMAP_INPUT_PIN_NONE

} PORTS_REMAP_INPUT_PIN;

typedef enum {

    PORTS_REMAP_OUTPUT_FUNCTION_NONE

} PORTS_REMAP_OUTPUT_FUNCTION;

typedef enum {

    PORTS_REMAP_OUTPUT_PIN_NONE

} PORTS_REMAP_OUTPUT_PIN;

typedef enum {

    PORTS_CHANGE_NOTICE_EDGE_NONE

} PORTS_CHANGE_NOTICE_EDGE;

typedef enum {

    PORTS_PIN_SLEW_RATE_NONE

} PORTS_PIN_SLEW_RATE;

typedef enum {

    PORTS_CHANGE_NOTICE_METHOD_NONE

} PORTS_CHANGE_NOTICE_METHOD;

typedef enum {

    PORTS_PIN_INTERRUPT_NONE,
    PORTS_PIN_INTERRUPT_BOTH_EDGE,
    PORTS_PIN_INTERRUPT_RISING_EDGE,
    PORTS_PIN_INTERRUPT_FALLING_EDGE,
    PORTS_PIN_INTERRUPT_HIGH_LEVEL,
    PORTS_PIN_INTERRUPT_LOW_LEVEL

} PORTS_PIN_INTERRUPT_TYPE;

// *****************************************************************************
/* SYS PORTS PIN CALLBACK

  Summary:
    Defines the pointer to the pin event callback function.

  Description:
    This datatype defines the function signature for the pin event callback 
    function.

  Remarks:
    None.

*/

typedef void (*SYS_PORTS_PIN_CALLBACK)(uintptr_t context);

//******************************************************************************
/* Function:
    void SYS_PORTS_PinCallbackRegister
    (
        PORTS_CHANNEL channel,
        PORTS_BIT_POS bitPos,
        SYS_PORTS_PIN_CALLBACK callback,
        uintptr_t context
    )

  Summary:
    Registers the event callback function for the selected port pin.

  Description:
    This function registers the event callback function for the selected port pin.

  Preconditions:
    Interrupt for the pin to be enabled from the PIO Interrupt tab of Pin Settings in MHC.

  Parameters:
    channel  - Identifier for the PORT channel: A, B, C, etc.
    bitPos   - Possible values of PORTS_BIT_POS
    callback - Callback function to be called on event on the Pin.
    context  - Pointer to identify any user specific data object(optional).

  Returns:
    None.

  Example:
    <code>
    SYS_PORTS_PinCallbackRegister( MY_PORTS_CHANNEL, MY_PORT_BIT_POS, MY_APP_Callback, myAppDataObject );
	</code>
    
  Remarks:
    This API is available only for the PIC32C Device family.
*/

void SYS_PORTS_PinCallbackRegister( PORTS_CHANNEL channel,
                                    PORTS_BIT_POS bitPos,
                                    SYS_PORTS_PIN_CALLBACK callback,
                                    uintptr_t context );

//******************************************************************************
/* Function:
    void SYS_PORTS_Tasks ( PORTS_CHANNEL channel )

  Summary:
    Implements the ISR for the given channel.

  Description:
    This function implements the ISR for the given channel.

  Preconditions:
    Interrupt for the pin to be enabled from the PIO Interrupt tab of Pin Settings in MHC.

  Parameters:
    channel - Identifier for the PORT channel: A, B, C, etc.

  Returns:
    None.

  Example:
    <code>
    SYS_PORTS_Tasks(MY_PORTS_CHANNEL);
	</code>
    
  Remarks:
    This API is available only for the PIC32C Device family.
*/

void SYS_PORTS_Tasks ( PORTS_CHANNEL channel );

#endif // #ifndef _SYS_PORTS_DEFINITIONS_PIC32C_H
/*******************************************************************************
 End of File
*/
