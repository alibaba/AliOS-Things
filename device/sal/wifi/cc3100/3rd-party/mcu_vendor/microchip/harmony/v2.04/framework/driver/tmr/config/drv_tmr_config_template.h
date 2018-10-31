/*******************************************************************************
  Timer Driver Configuration Definitions for the Template Version

  Company:
    Microchip Technology Inc.

  File Name:
    drv_tmr_config_template.h

  Summary:
    Timer driver configuration definitions for the template version.

  Description:
    These definitions set up the driver for the default mode of operation of
    the driver.

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

#ifndef _DRV_TMR_CONFIG_TEMPLATE_H
#define _DRV_TMR_CONFIG_TEMPLATE_H


#error "This is a configuration template file.  Do not include it directly."

// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Mandatory configuration macros 
// *****************************************************************************
// *****************************************************************************
/* The following configuration macros are mandatory in all the build modes of 
   Timer driver. So it must be defined in 'sytem_config.h file of the application 
   */

// *****************************************************************************
/* Default module init object configuration

  Summary:
    Sets the default module init value for the  timer driver.

  Description:
    This macro sets the default module init value for the timer driver.


  Remarks:
	This value can be overridden by a run time initialization value.
*/

#define DRV_TMR_MODULE_INIT                       (SYS_MODULE_POWER_RUN_FULL)



// *****************************************************************************
/* Default timer driver index

  Summary:
    Sets the default timer module ID to be used by the timer driver.

  Description:
    This macro sets the default timer module ID to be used by the timer driver.


  Remarks:
	This value can be overridden by a run time initialization value.
*/

#define DRV_TMR_MODULE_ID                       (TMR_ID_2)

// *****************************************************************************
/* Default timer driver clock source

  Summary:
    Sets the default timer driver clock source.

  Description:
    This macro sets the default timer driver clock source.


  Remarks:
	This value can be overridden by a run time initialization value.
*/

#define DRV_TMR_CLOCK_SOURCE                       (DRV_TMR_CLKSOURCE_INTERNAL)

// *****************************************************************************
/* Default timer driver clock prescaler

  Summary:
    Sets the default timer driver clock prescaler.

  Description:
    This macro sets the default timer driver clock prescaler.


  Remarks:
	This value can be overridden by a run time initialization value.
*/

#define DRV_TMR_CLOCK_PRESCALER                       (TMR_PRESCALE_VALUE_256)

// *****************************************************************************
/* Default timer driver clock operating mode

  Summary:
    Sets the default timer driver clock operating mode.

  Description:
    This macro sets the default timer driver clock operating mode.


  Remarks:
	This value can be overridden by a run time initialization value.
*/

#define DRV_TMR_MODE                       (DRV_TMR_OPERATION_MODE_16_BIT)

// *****************************************************************************
/* Default timer driver clock interrupt source

  Summary:
    Sets the default timer driver clock interrupt source

  Description:
    This macro sets the default timer driver clock interrupt source


  Remarks:
	This value can be overridden by a run time initialization value.
*/

#define DRV_TMR_INTERRUPT_SOURCE         (INT_SOURCE_TIMER_2)


// *****************************************************************************
/* Hardware instances support

  Summary:
    Sets up the maximum number of hardware instances that can be supported by
    the dynamic driver.

  Description:
    This definition sets up the maximum number of hardware instances that can be
    supported by the dynamic driver.

  Remarks:
    None
*/

#define DRV_TMR_INSTANCES_NUMBER                    5

// *****************************************************************************
/* Client instances support

  Summary:
    Sets up the maximum number of clients that can be supported by
    an instance of the dynamic driver.

  Description:
    This definition sets up the maximum number of clients that can be
    supported by an instance of the dynamic driver.

  Remarks:
    Currently each client is required to get exclusive access
    to the timer module.
    Therfore the DRV_TMR_CLIENTS_NUMBER should always be set to 1.
*/

#define DRV_TMR_CLIENTS_NUMBER                    1


// *****************************************************************************
/* TMR Asynchronous write mode configuration

  Summary:
    Controls Asynchronous Write mode of the Timer.

  Description:
    This macro controls the Asynchronous Write mode of the Timer. This macro
    accepts the following values:

    - true  - Configures the Timer to enable asynchronous write control
    - false - Configures the Timer to disable asynchronous write control
    - DRV_CONFIG_NOT_SUPPORTED - When the feature is not supported on the
             instance.

  Remarks:
    This feature is not available in all modules/devices. Refer to the specific
    device data sheet for more information.
*/

#define DRV_TMR_ASYNC_WRITE_ENABLE                 false 

// *****************************************************************************
/* TMR Interrupt And Polled Mode Operation Control

  Summary:
    Controls operation of the driver in the interrupt or polled mode.

  Description:
    This macro controls the operation of the driver in the interrupt
    mode of operation. The possible values of this macro are:

    - true  - Select if interrupt mode of timer operation is desired
    - false - Select if polling mode of timer operation is desired

    Not defining this option to true or false will result in a build error.

  Remarks:
    None.
*/

#define DRV_TMR_INTERRUPT_MODE                      true



#endif // #ifndef _DRV_TMR_CONFIG_TEMPLATE_H

/*******************************************************************************
 End of File
*/
