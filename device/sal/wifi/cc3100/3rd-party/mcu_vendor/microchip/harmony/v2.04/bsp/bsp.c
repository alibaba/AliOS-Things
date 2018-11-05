/*******************************************************************************
  Board Support Package Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    bsp.c

  Summary:
    Board Support Package implementation for PIC32MZ EF Curiosity Development
    Board.

  Description:
    This file contains routines that implement the board support package for
    PIC32MZ EF Curiosity Development Board.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2017 Microchip Technology Incorporated and its subsidiaries.

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE
*******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "bsp.h"


// *****************************************************************************
/* Data Structure: 
    switch_port_channel_map[]

  Summary:
    Maps each switch to its port channel
  
  Description:
    The switch_port_channel_map array, indexed by BSP_SWITCH, maps each switch 
    to its port channel.

  Remarks:
    Refer to bsp.h for usage information.
*/
static const PORTS_CHANNEL switch_port_channel_map[] =
{
    PORT_CHANNEL_G
};

// *****************************************************************************
/* Data Structure: 
    switch_port_bit_pos_map[]

  Summary:
    Maps each switch to its port bit position
  
  Description:
    The switch_port_bit_pos_map array, indexed by BSP_SWITCH, maps each switch to its port bit position

  Remarks:
    Refer to bsp.h for usage information.
*/
static const PORTS_BIT_POS switch_port_bit_pos_map[] =
{
    PORTS_BIT_POS_12
};


// *****************************************************************************
/* Function: 
    void BSP_SwitchStateGet(BSP_SWITCH switch);

  Summary:
    Returns the present state (pressed or not pressed) of the specified switch.
  
  Description:
    This function returns the present state (pressed or not pressed) of the
    specified switch.

  Remarks:
    Refer to bsp.h for usage information.
*/

BSP_SWITCH_STATE BSP_SwitchStateGet( BSP_SWITCH bspSwitch )
{
    return ( PLIB_PORTS_PinGet(PORTS_ID_0, switch_port_channel_map[bspSwitch], switch_port_bit_pos_map[bspSwitch]) );
}

// *****************************************************************************
/* Data Structure: 
    led_port_channel_map[]

  Summary:
    Maps each led to its port channel
  
  Description:
    The led_port_channel_map array, indexed by BSP_LED, maps each led to its 
    port channel.

  Remarks:
    Refer to bsp.h for usage information.
*/
static const PORTS_CHANNEL led_port_channel_map[] =
{
    PORT_CHANNEL_E,
    PORT_CHANNEL_B,
    PORT_CHANNEL_B,
    PORT_CHANNEL_B,
    PORT_CHANNEL_E,
    PORT_CHANNEL_E
};

// *****************************************************************************
/* Data Structure: 
    led_port_bit_pos_map[]

  Summary:
    Maps each led to its port bit position
  
  Description:
    The led_port_bit_pos_map array, indexed by BSP_LED, maps each led to its port 
    bit position.

  Remarks:
    Refer to bsp.h for usage information.
*/
static const PORTS_BIT_POS led_port_bit_pos_map[] =
{
    PORTS_BIT_POS_6,
    PORTS_BIT_POS_5,
    PORTS_BIT_POS_1,
    PORTS_BIT_POS_0,
    PORTS_BIT_POS_3,
    PORTS_BIT_POS_4
};

// *****************************************************************************
/* Data Structure: 
    led_active_level_map[]

  Summary:
    Maps each led to its active level
  
  Description:
    The led_active_level_map array, indexed by BSP_LED, maps each led to its active 
    level.

  Remarks:
    Refer to bsp.h for usage information.
*/
static const BSP_LED_ACTIVE_LEVEL led_active_level_map[] =
{
   BSP_LED_ACTIVE_HIGH,
   BSP_LED_ACTIVE_LOW,
   BSP_LED_ACTIVE_LOW,
   BSP_LED_ACTIVE_LOW,
   BSP_LED_ACTIVE_HIGH,
   BSP_LED_ACTIVE_HIGH
};

// *****************************************************************************
/* Function: 
    void BSP_LEDStateSet(BSP_LED led, BSP_LED_STATE state);

  Summary:
    Controls the state of the LED.
  
  Description:
    This function allows the application to specify the state of the LED.

  Remarks:
    Refer to bsp_config.h for usage information.
*/

void BSP_LEDStateSet(BSP_LED led, BSP_LED_STATE state)
{
    /* Set the state of the LED */
    if(led_active_level_map[led] == BSP_LED_ACTIVE_HIGH)
    {
        PLIB_PORTS_PinWrite (PORTS_ID_0 , led_port_channel_map[led], led_port_bit_pos_map[led], 
							 (BSP_LED_STATE_ON == state ? true : false));
    }
    else
    {
        PLIB_PORTS_PinWrite (PORTS_ID_0 , led_port_channel_map[led], led_port_bit_pos_map[led], 
							 (BSP_LED_STATE_ON == state ? false : true));
    }
}

// *****************************************************************************
/* Function: 
    void BSP_LEDToggle(BSP_LED led);

  Summary:
    Toggles the state of the LED between BSP_LED_STATE_ON and BSP_LED_STATE_OFF.
  
  Description:
    This function toggles the state of the LED between BSP_LED_STATE_ON and
    BSP_LED_STATE_OFF.

  Remarks:
    Refer to bsp.h for usage information.
*/    

void BSP_LEDToggle(BSP_LED led)
{
    PLIB_PORTS_PinToggle(PORTS_ID_0, led_port_channel_map[led], led_port_bit_pos_map[led] );
}

// *****************************************************************************
/* Function: 
    BSP_LED_STATE BSP_LEDStateGet(BSP_LED led);

  Summary:
    Returns the present state of the LED.
  
  Description:
    This function returns the present state of the LED.

  Remarks:
    Refer to bsp.h for usage information.
*/    

BSP_LED_STATE BSP_LEDStateGet (BSP_LED led)
{
    bool value;

    /* Get LED Status */
    value = PLIB_PORTS_PinGetLatched (PORTS_ID_0, led_port_channel_map[led], led_port_bit_pos_map[led]);

    if(led_active_level_map[led] == BSP_LED_ACTIVE_LOW)
    {
        value = !value;
    }

    return (value ? BSP_LED_STATE_ON : BSP_LED_STATE_OFF);
}

// *****************************************************************************
/* Function: 
    void BSP_LEDOn(BSP_LED led);

  Summary:
    Switches ON the specified LED.
  
  Description:
    This function switches ON the specified LED.

  Remarks:
    Refer to bsp.h for usage information.
*/

void BSP_LEDOn(BSP_LED led)
{
    if(led_active_level_map[led] == BSP_LED_ACTIVE_HIGH)
    {
        PLIB_PORTS_PinSet( PORTS_ID_0, led_port_channel_map[led], led_port_bit_pos_map[led] );
    }
    else
    {
        PLIB_PORTS_PinClear( PORTS_ID_0, led_port_channel_map[led], led_port_bit_pos_map[led] );
    }
}

// *****************************************************************************
/* Function: 
    void BSP_LEDOff(BSP_LED led);

  Summary:
    Switches Off the specified LED.
  
  Description:
    This function switches Off the specified LED.

  Remarks:
    Refer to bsp.h for usage information.
*/

void BSP_LEDOff(BSP_LED led)
{
    if(led_active_level_map[led] == BSP_LED_ACTIVE_HIGH)
    {
        PLIB_PORTS_PinClear( PORTS_ID_0, led_port_channel_map[led], led_port_bit_pos_map[led] );
    }
    else
    {
        PLIB_PORTS_PinSet( PORTS_ID_0, led_port_channel_map[led], led_port_bit_pos_map[led] );
    }
}
// *****************************************************************************
/* Function: 
    void BSP_USBVBUSSwitchStateSet(BSP_USB_VBUS_SWITCH_STATE state);

  Summary:
    This function enables or disables the USB VBUS switch on the board.
  
  Description:
    This function enables or disables the VBUS switch on the board.

  Remarks:
    Refer to bsp_config.h for usage information.
*/

void BSP_USBVBUSSwitchStateSet(BSP_USB_VBUS_SWITCH_STATE state)
{
    /* Enable the VBUS switch */

    PLIB_PORTS_PinWrite( PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_13, state );
}

// *****************************************************************************
/* Function: 
    bool BSP_USBVBUSSwitchOverCurrentDetect(uint8_t port)

  Summary:
    Returns true if the over current is detected on the VBUS supply.
  
  Description:
    This function returns true if over current is detected on the VBUS supply.

  Remarks:
    None.
*/

bool BSP_USBVBUSSwitchOverCurrentDetect(uint8_t port)
{
    return(false);
}

// *****************************************************************************
/* Function: 
    bool BSP_USBVBUSPowerEnable(uint8_t port, bool enable)

  Summary:
    This function controls the USB VBUS supply.
  
  Description:
    This function controls the USB VBUS supply.

  Remarks:
    None.
*/

void BSP_USBVBUSPowerEnable(uint8_t port, bool enable)
{
    /* Enable the VBUS switch */

    PLIB_PORTS_PinWrite( PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_13, enable );
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void BSP_Initialize(void)

  Summary:
    Performs the necessary actions to initialize a board

  Description:
    This function initializes the LED, Switch and other ports on the board.
    This function must be called by the user before using any APIs present in
    this BSP.

  Remarks:
    Refer to bsp.h for usage information.
*/

void BSP_Initialize(void )
{
    /* Setup the USB VBUS Switch Control Pin */
    BSP_USBVBUSSwitchStateSet(BSP_USB_VBUS_SWITCH_STATE_DISABLE);

    /* Switch off LEDs */
    BSP_LEDOff(BSP_LED_3);
    BSP_LEDOff(BSP_RGB_LED_RED);
    BSP_LEDOff(BSP_RGB_LED_GREEN);
    BSP_LEDOff(BSP_RGB_LED_BLUE);
    BSP_LEDOff(BSP_LED_1);
    BSP_LEDOff(BSP_LED_2);
}

/*******************************************************************************
 End of File
*/
