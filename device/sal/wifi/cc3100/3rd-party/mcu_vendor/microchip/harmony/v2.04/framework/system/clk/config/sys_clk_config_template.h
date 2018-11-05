/*******************************************************************************
  Clock System Service Configuration Templates

  Company:
    Microchip Technology Inc.

  File Name:
    sys_clk_config_template.h

  Summary:
    Clock System Service configuration templates.

  Description:
    This file contains constants to configure the Clock System Service.
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

#ifndef _SYS_OSC_CONFIG_TEMPLATE_H
#define _SYS_OSC_CONFIG_TEMPLATE_H


// *****************************************************************************
/* Primary Oscillator clock input frequency

  Summary:
    Gives the Primary oscillator input frequency. This is from the hardware and
	not the output required frequency.

  Description:
    This macro gives the Primary Oscillator clock input frequency.

  Remarks:
    Find this out by checking the hardware. examine the crystal connected to the
	Primary Oscillator input pin.
*/

#define SYS_CLK_CONFIG_PRIMARY_XTAL                                  8000000L


// *****************************************************************************
/* System PLL input divisor configured through configuration bits

  Summary:
    Gives the System PLL input divisor configured through configuration bits.
	For some of the devices, the system PLL input divisor is not accessible
	through software. So use this macro to inform the System service about the
	input divisor.

  Description:
    Gives the System PLL input divisor configured through configuration bits.
	For some of the devices, the system PLL input divisor is not accessible
	through software. So use this macro to inform the System service about the
	input divisor. In case of some devices PLL expects its input frequency to
	be in a specific range. So it is important to set the input divisor get the
	right input frequency.

  Remarks:
    This must be changed every time you change the System PLL input divisor through
	the configuration bits.
*/

#define SYS_CLK_CONFIG_SYSPLL_INP_DIVISOR                               2


// *****************************************************************************
/* Secondary Oscillator clock input frequency

  Summary:
    Gives the Secondary oscillator input frequency. This is from the hardware and
	not the output required frequency.

  Description:
    This macro gives the Secondary Oscillator clock input frequency.

  Remarks:
    Find this out by checking the hardware. examine the crystal connected to the
	Secondary Oscillator input pin.
*/

#define SYS_CLK_CONFIG_SECONDARY_XTAL                                   8000000


// *****************************************************************************
/* System clock PLL enable configuration

  Summary:
    Defines the status of the USB PLL.

  Description:
    Setting this macro to 'true' states that USB PLL is enabled through
    the Configuration bits.

  Remarks:
    This must be changed every time you enable/disable the USB PLL through the
	configuration bits.
*/

#define SYS_CLK_CONFIGBIT_USBPLL_ENABLE                                    true


// *****************************************************************************
/* Oscillator USB clock divisor configured through configuration bits

  Summary:
    Gives the Oscillator USB clock divisor.

  Description:
    This macro sets up the Oscillator USB clock divisor. Use OSC_CONFIGBITS_PLLIDIV
    to select the configuration.

  Remarks:
    This must be changed every time you change the USB PLL divisor through the
	configuration bits.
*/

#define SYS_CLK_CONFIGBIT_USBPLL_DIVISOR								2


// *****************************************************************************
/* Frequency Configuration error

  Summary:
    Sets the allowed frequency configuration error Hertz.

  Description:
    This macro sets the allowed frequency error in Hertz.

  Remarks:
    None.
*/

#define SYS_CLK_CONFIG_FREQ_ERROR_LIMIT                                  10


// *****************************************************************************
/* External clock frequency

  Summary:
    Gives the clock that is provided on the Reference clock output pin.

  Description:
    This macro gives the clock that is provided on the Reference clock output pin.

  Remarks:
    None.
*/

#define SYS_CLK_CONFIG_EXTERNAL_CLOCK				   				4000000


// *****************************************************************************
/* USB clock frequency

  Summary:
    Defines the value of the USB clock for the devices which does not have a
    software controllable USB clock.

  Description:
    Defines the value of the USB clock for the devices which does not have a
    software controllable USB clock(Fully controlled through configuration
    bits).

  Remarks:
    None.
*/

#define SYS_CLK_UPLL_BEFORE_DIV2_FREQ								48000000


#endif //_SYS_OSC_CONFIG_TEMPLATE_H

/*******************************************************************************
 End of File
*/

