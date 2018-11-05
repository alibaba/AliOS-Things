/*******************************************************************************
  Board Support Package Header File.

  Company:      
    Microchip Technology Inc.

  File Name:    
    bsp.h

  Summary:      
    Board Support Package Header File for PIC32MZ EF Curiosity Development Board.

  Description:
    This file contains constants, macros, type definitions and function
    declarations required by the PIC32MZ EF Curiosity Development Board.
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

#ifndef _BSP_H
#define _BSP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "peripheral/ports/plib_ports.h"


// *****************************************************************************
/* BSP Switch.

  Summary:
    Defines the switches available on this board.

  Description:
    This enumeration defines the switches available on this board.

  Remarks:
    None.
*/

typedef enum
{
    BSP_SWITCH_1 = 0
} BSP_SWITCH;


// *****************************************************************************
/* BSP Switch state.

  Summary:
    Defines possible states of the switches on this board.

  Description:
    This enumeration defines the possible states of the switches on this board.

  Remarks:
    None.
*/

typedef enum
{
    /* Switch pressed */
    BSP_SWITCH_STATE_PRESSED =/*DOM-IGNORE-BEGIN*/ 0/*DOM-IGNORE-END*/,

   /* Switch not pressed */
    BSP_SWITCH_STATE_RELEASED =/*DOM-IGNORE-BEGIN*/ 1/*DOM-IGNORE-END*/

} BSP_SWITCH_STATE;


// *****************************************************************************
/* Function: 
    BSP_SWITCH_STATE BSP_SwitchStateGet(BSP_SWITCH switch);

  Summary:
    Returns the present state (pressed or not pressed) of the specified switch.
  
  Description:
    This function returns the present state (pressed or not pressed) of the
    specified switch.

  Precondition:
    BSP_Initialize() should have been called.

  Parameters:
    switch  - The switch whose state needs to be obtained.
  
  Returns:
    The pressed released state of the switch.

  Example:
    <code>
    
    // Initialize the BSP
    BSP_Initialize();

    // Check the state of the switch.
    if(BSP_SWITCH_STATE_PRESSED == BSP_SwitchStateGet(BSP_SWITCH_1))
    {
        // This means that Switch 1 on the board is pressed.
    }

    </code>

  Remarks:
    None                                                                   
*/

BSP_SWITCH_STATE BSP_SwitchStateGet(BSP_SWITCH bspSwitch);

// *****************************************************************************
/* BSP_LED.

  Summary:
    Defines the LEDs available on this board.

  Description:
    This enumeration defines the LEDs available on this board.

  Remarks:
    None.
*/
typedef enum
{
    BSP_LED_3 = 0,
    BSP_RGB_LED_RED = 1,
    BSP_RGB_LED_GREEN = 2,
    BSP_RGB_LED_BLUE = 3,
    BSP_LED_1 = 4,
    BSP_LED_2 = 5
} BSP_LED;


// *****************************************************************************
/* LED State

  Summary:
    Enumerates the supported LED states.

  Description:
    This enumeration defines the supported LED states.

  Remarks:
    None.
*/

typedef enum
{
    /* LED State is on */
    BSP_LED_STATE_OFF = /*DOM-IGNORE-BEGIN*/0/*DOM-IGNORE-END*/,

   /* LED State is off */
    BSP_LED_STATE_ON = /*DOM-IGNORE-BEGIN*/1/*DOM-IGNORE-END*/

} BSP_LED_STATE;

// *****************************************************************************
/* LED Active Level

  Summary:
    Enumerates the supported LED active level.

  Description:
    This enumeration defines the supported LED sactive levels.

  Remarks:
    None.
*/

typedef enum
{
    /* LED active level is low */
    BSP_LED_ACTIVE_LOW = /*DOM-IGNORE-BEGIN*/0/*DOM-IGNORE-END*/,

    /* LED active level is high */
    BSP_LED_ACTIVE_HIGH = /*DOM-IGNORE-BEGIN*/1/*DOM-IGNORE-END*/

} BSP_LED_ACTIVE_LEVEL;
// *****************************************************************************
/* Function: 
    void BSP_LEDStateSet(BSP_LED led, BSP_LED_STATE state);

  Summary:
    Controls the state of the LED.
  
  Description:
    This function allows the application to specify the state of the LED.

  Precondition:
    BSP_Initialize() should have been called.

  Parameters:
    led     - The LED to operate on.
    state   - The state to be set.
  
  Returns:
    None.

  Example:
    <code>
    
    // Initialize the BSP
    BSP_Initialize();

    // Switch on LED1 on the board 
    BSP_LEDStateSet(BSP_LED_1, BSP_LED_STATE_ON);
    
    // Switch off LED2 on the board 
    BSP_LEDStateSet(BSP_LED_2, BSP_LED_STATE_OFF);

    </code>

  Remarks:
    None                                                                   
*/

void BSP_LEDStateSet(BSP_LED led, BSP_LED_STATE state);


// *****************************************************************************
/* Function: 
    BSP_LED_STATE BSP_LEDStateGet(BSP_LED led);

  Summary:
    Returns the present state of the LED.
  
  Description:
    This function returns the present state of the LED.

  Precondition:
    BSP_Initialize() should have been called.

  Parameters:
    led - The LED to whose status needs to be obtained.
  
  Returns:
    The ON/OFF state of the LED.

  Example:
    <code>
    
    // Initialize the BSP
    BSP_Initialize();

    // Check if LED2 is off
    if(BSP_LED_STATE_OFF == BSP_LEDStateGet(BSP_LED_2)
    {
        // Switch on the LED.
        BSP_LEDStateSet(BSP_LED_2, BSP_LED_STATE_ON);
    }

    </code>

  Remarks:
    None                                                                   
*/

BSP_LED_STATE BSP_LEDStateGet(BSP_LED led);


// *****************************************************************************
/* Function: 
    void BSP_LEDToggle(BSP_LED led);

  Summary:
    Toggles the state of the LED between BSP_LED_STATE_ON and BSP_LED_STATE_OFF.
  
  Description:
    This function toggles the state of the LED between BSP_LED_STATE_ON and
    BSP_LED_STATE_OFF.

  Precondition:
    BSP_Initialize() should have been called.

  Parameters:
    led - The LED to toggle.
  
  Returns:
    None.

  Example:
    <code>
    
    // Initialize the BSP
    BSP_Initialize();

    // Switch on LED1 on the board 
    BSP_LEDStateSet(BSP_LED_1, BSP_LED_STATE_ON);
    
    // Switch off LED2 on the board 
    BSP_LEDStateSet(BSP_LED_2, BSP_LED_STATE_OFF);

    // Toggle state of LED3
    BSP_LEDToggle(BSP_LED_3);
    </code>

  Remarks:
    None                                                                   
*/

void BSP_LEDToggle(BSP_LED led);


// *****************************************************************************
/* Function: 
    void BSP_LEDOn(BSP_LED led);

  Summary:
    Switches ON the specified LED.
  
  Description:
    This function switches ON the specified LED.

  Precondition:
    BSP_Initialize() should have been called.

  Parameters:
    led - The LED to switch on.
  
  Returns:
    None.

  Example:
    <code>
    
    // Initialize the BSP
    BSP_Initialize();

    // Switch on LED1 on the board 
    BSP_LEDOn(BSP_LED_1);
    
    </code>

  Remarks:
    None                                                                   
*/

void BSP_LEDOn(BSP_LED led);


// *****************************************************************************
/* Function: 
    void BSP_LEDOff(BSP_LED led);

  Summary:
    Switches OFF the specified LED.
  
  Description:
    This function switches OFF the specified LED.

  Precondition:
    BSP_Initialize() should have been called.

  Parameters:
    led - The LED to switch off.
  
  Returns:
    None.

  Example:
    <code>
    
    // Initialize the BSP
    BSP_Initialize();

    // Switch off LED1 on the board 
    BSP_LEDOff(BSP_LED_1);
    
    </code>

  Remarks:
    None                                                                   
*/

void BSP_LEDOff(BSP_LED led);
// *****************************************************************************
/* USB VBUS Switch State

  Summary:
    Defines the possible states of the USB VBUS Switch on this board

  Description:
    This enumeration defines the possible states of the USB VBUS Switch on this
    board.

  Remarks:
    None.
*/

typedef enum
{
    /* USB VBUS Switch disable */
    BSP_USB_VBUS_SWITCH_STATE_DISABLE = /*DOM-IGNORE-BEGIN*/0/*DOM-IGNORE-END*/,

    /* USB VBUS Switch enable */
    BSP_USB_VBUS_SWITCH_STATE_ENABLE = /*DOM-IGNORE-BEGIN*/1/*DOM-IGNORE-END*/

} BSP_USB_VBUS_SWITCH_STATE;


// *****************************************************************************
/* Function: 
    void BSP_USBVBUSSwitchStateSet(BSP_USB_VBUS_SWITCH_STATE state);

  Summary:
    This function enables or disables the USB VBUS switch on the board.
  
  Description:
    This function enables or disables the VBUS switch on the board.

  Precondition:
    BSP_Initialize() should have been called.

  Parameters:
    state - If BSP_USB_VBUS_SWITCH_STATE_ENABLE, then the USB VBUS switch is
            enabled and VBUS is supplied on the USB. 
            If BSP_USB_VBUS_SWITCH_STATE_DISABLE, then the USB VBUS
            switch is disabled and VBUS is not supplied on the USB.
  
  Returns:
    None.

  Example:
    <code>
    
    // Initialize the BSP
    BSP_Initialize();

    // Enable the VBUS switch.
    BSP_USBVBUSSwitchStateSet(BSP_USB_VBUS_SWITCH_STATE_ENABLE);
    
    </code>

  Remarks:
    None                                                                   
*/

void BSP_USBVBUSSwitchStateSet(BSP_USB_VBUS_SWITCH_STATE state);

// *****************************************************************************
/* Function: 
    bool BSP_USBVBUSPowerEnable(uint8_t port, bool enable)

  Summary:
    This function controls the USB VBUS supply.
  
  Description:
    This function controls the USB VBUS supply.

  Precondition:
    BSP_Initialize() should have been called.

  Parameters:
    port - This parameter is ignored.

    enable  - if true VBUS supply is enabled. If false VBUS supply is disabled.
  
  Returns:
    None.

  Example:
    <code>
    
    // Initialize the BSP
    BSP_Initialize();

    // Enable the power.
    BSP_USBVBUSPowerEnable(0, true);

    // Disable the power.
    BSP_USBVBUSPowerEnable(0, false);

    </code>

  Remarks:
    None.
*/

void BSP_USBVBUSPowerEnable(uint8_t port, bool enable);

// *****************************************************************************
/* Function: 
    bool BSP_USBVBUSSwitchOverCurrentDetect(uint8_t port)

  Summary:
    Returns true if the over current is detected on the VBUS supply.
  
  Description:
    This function returns true if over current is detected on the VBUS supply.

  Precondition:
    BSP_Initialize() should have been called.

  Parameters:
    port - This parameter is ignored.
  
  Returns:
    true - VBUS supply over current is detected.
    false - VBUS supply over current is not detected.

  Example:
    <code>
    
    // Initialize the BSP
    BSP_Initialize();

    // Enable the power.
    BSP_USBVBUSPowerEnable(0, true);

    if(BSP_USBVBUSSwitchOverCurrentDetect(0))
    {
        // Disable the power.
        BSP_USBVBUSPowerEnable(0, false);
    }

    </code>

  Remarks:
    None.
*/

bool BSP_USBVBUSSwitchOverCurrentDetect(uint8_t port);

// *****************************************************************************
// *****************************************************************************
// Section: Constants and Type Definitions.
// *****************************************************************************
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
    This function initializes the LED and Switch ports on the board.  This
    function must be called by the user before using any APIs present on this
    BSP.  

  Precondition:
    None.

  Parameters:
    None
  
  Returns:
    None.

  Example:
    <code>
    //Initialize the BSP
    BSP_Initialize();
    </code>

  Remarks:
    None                                                                   
*/

void BSP_Initialize(void);

#endif // _BSP_H

/*******************************************************************************
 End of File
*/
