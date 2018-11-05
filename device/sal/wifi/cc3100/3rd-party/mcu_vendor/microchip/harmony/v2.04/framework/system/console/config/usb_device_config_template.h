/*******************************************************************************
  Console Device Compile Time Options

  Company:
    Microchip Technology Inc.

  File Name:
    console_device_config_template.h

  Summary:
    Console device configuration template header file.

  Description:
    This file contains console device compile time options (macros) that are to
    be configured by the user. This file is a template file and must be used as
    an example only. This file must not be directly included in the project.

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

#ifndef _CONSOLE_DEVICE_CONFIG_TEMPLATE_H_
#define _CONSOLE_DEVICE_CONFIG_TEMPLATE_H_

#error (" This is a template file and must not be included directly in the project" );

// *****************************************************************************
// *****************************************************************************
// Section: Console configuration Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* System console objects configuration 
  Summary:
    Sets the maximum possible number of instances of the console device that
    can be instantiated by using SYS_CONSOLE_Initialize() routine. 

  Description:
    In case of applications with more than one console, the value
    of this constant can be increased to support more than one instance of  
    of the console service.
    
  Remarks:
    The default implementation supports only one instance.
    
    Increasing the instance count consumes RAM and can lead to performance
    degradation.
*/

#define SYS_CONSOLE_DEVICE_MAX_INSTANCES 1


// *****************************************************************************
/* System Console source peripheral
  Summary:
    Sets the peripheral used for the console I/O.

  Description:
    Sets the peripheral used for the console I/O.
    
  Remarks:
    None.
*/

#define SYS_CONSOLE_SOURCE  UART_CONSOLE


// *****************************************************************************
/* System Console UART read queue depth
  Summary:
    Sets the size of the read queue for the UART console device.

  Description:
    Sets the size of the read queue for the UART console device.
    
  Remarks:
    None.
*/

#define SYS_CONSOLE_UART_RD_QUEUE_DEPTH 64


// *****************************************************************************
/* System Console UART write queue depth
  Summary:
    Sets the size of the write queue for the UART console device.

  Description:
    Sets the size of the write queue for the UART console device.
    
  Remarks:
    None.
*/

#define SYS_CONSOLE_UART_WR_QUEUE_DEPTH 64


// *****************************************************************************
/* System Console USB-CDC read queue depth
  Summary:
    Sets the size of the read queue for the USB-CDC console device.

  Description:
    Sets the size of the read queue for the USB-CDC console device.
    
  Remarks:
    None.
*/

#define SYS_CONSOLE_USB_CDC_RD_QUEUE_DEPTH 64


// *****************************************************************************
/* System Console USB-CDC write queue depth
  Summary:
    Sets the size of the write queue for the USB-CDC console device.

  Description:
    Sets the size of the write queue for the USB-CDC console device.
    
  Remarks:
    None.
*/

#define SYS_CONSOLE_USB_CDC_WR_QUEUE_DEPTH 64


#endif



