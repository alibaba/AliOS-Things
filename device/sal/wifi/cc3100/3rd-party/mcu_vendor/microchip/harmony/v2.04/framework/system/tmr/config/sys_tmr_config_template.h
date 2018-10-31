/*******************************************************************************
  Timer System Service Configuration Definitions for the Template Version

  Company:
    Microchip Technology Inc.

  File Name:
    sys_tmr_config_template.h

  Summary:
    Contains configuration definitions that are common to timer system services
    and aggregates the configuration files for the system services.

  Description:
    This file contains configuration definitions that are common to timer system
    services and aggregates the configuration files for the system services.

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


#ifndef _SYS_TMR_CONFIG_TEMPLATE_H
#define _SYS_TMR_CONFIG_TEMPLATE_H


#error "This is a configuration template file.  Do not include it directly."


// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Default module init object configuration

  Summary:
    Sets the default module init value by the system timer service.

  Description:
    This macro sets the default  module init value by the system timer service.


  Remarks:
	This value can be overridden by a run time initialization value.
*/

#define SYS_TMR_MODULE_INIT                       (SYS_MODULE_POWER_RUN_FULL)


// *****************************************************************************
/* Default timer driver object configuration

  Summary:
    Sets the default timer driver object to be used by the system timer service.

  Description:
    This macro sets the default timer driver object to be used by the system
    timer service.


  Remarks:
	This value can be overridden by a run time initialization value.
*/

#define SYS_TMR_DRIVER_INDEX                       (DRV_TMR_INDEX_0)



// *****************************************************************************
/* Maximum client timer objects configuration

  Summary:
    Sets the maximum number of timer clients that can be active at a given time.

  Description:
    This macro sets the maximum number of clients that the Timer system service
    can support.


  Remarks:
	None.
*/

#define SYS_TMR_MAX_CLIENT_OBJECTS                                 (5)



// *****************************************************************************
/* System timer running frequency

  Summary:
   Specifies the operating frequency for the system timer, Hz.

  Description:
    This macro sets the operating frequency for the system timer, in Hz.
    All the timing is maintained based on this frequency.
    This is actually the rate of the interrupt that's requested to the hardware
    timer that the system timer uses.
    
    The higher the rate, the better time keeping but the operation of the CPU
    will be interrupted more frequently (if the underlying timer driver is
    working in interrupts) and the result will be a greater overhead.
    Therefore is is not recommended to go higher than 1 KHz (below 1 ms) when
    the timer driver works in interrupts.


  Remarks:
    The system timer cannot obtain timing with a finer resolution than this
    frequency.

	This value can be overridden by a run time initialization value.

    The actual running frequency depends on the timer driver running frequency.
    Select a value that makes sense to minimize the error
    (see SYS_TMR_FREQUENCY_TOLERANCE notes).
*/


#define SYS_TMR_FREQUENCY          (1250)

// *****************************************************************************
/* Error Tolerance configuration

  Summary:
    Sets the error tolerance in %.

  Description:
    This macro sets the accepted tolerance percentage for the requested system
    timer frequency (SYS_TMR_FREQUENCY).

  
  Remarks:
    This error depends on the time base that the system timer uses
    (i.e. the underlying timer driver timing frequency).
    The actual running frequency for the system timer may be slighlty 
    different from the requested SYS_TMR_FREQUENCY.
*/

#define SYS_TMR_FREQUENCY_TOLERANCE                                     (10)


// *****************************************************************************
/* System timer internal units resolution

  Summary:
   Specifies the internal units for the time keeping.

  Description:
    This macro sets the system timer internal time units, in Hz.
    The higher the value, the better resolution but the range of the delays
    that can be obtained is shortened.

    A value too low will limit the resolution of delays that can be obtained.
    In order to be able to obtain milliseconds delays the value should
    be greater than 1000 Hz.


    The internal timing calculation is done with integer arithmetic
    so in order to do a precise enough calculation for ms values,
    a finer time unit needs to be used.

  Remarks:
    10000 Hz (meaning 0.1 ms units) is the recommended value because the 
    delays than need to be achieved are in the ms range.

    The service tries to achieve enough resolution for delays in the range of
    milliseconds. That means that this value cannot be < 1000 and the service
    will fail to initilize if such a value is attempted.

    The setting affects the range of delays that can be obtained.
    delay < (2^32 -1)/SYS_TMR_RESOLUTION 
    gives the maximum delay value, in seconds, that can be achieved
    For example:
    delay < 119.3 hours (for SYS_TMR_RESOLUTION == 10000).
*/


#define SYS_TMR_UNIT_RESOLUTION          (10000)


// *****************************************************************************
/* Client accepted Tolerance configuration

  Summary:
    Sets the accepted client tolerance in %.

  Description:
    This macro sets the percentage of the accepted tolerance for the requested
    client delay. If the system timer cannot achieve a delay within the
    specified tolerance it will reject the client request.

  
  Remarks:
    This error depends on the actual frequency that the system timer
    uses as a time base.
*/

#define SYS_TMR_CLIENT_TOLERANCE                  (10)


// *****************************************************************************
/* Interrupt Notification Mode configuration

  Summary:
    Activates the notification mechanism from within the timer interrupt.

  Description:
    This macro enables/disables the notification towards the timer clients to be
    delivered directly from the underlying time base interrupt routine(the
    routine that notifies the system timer itself that another system tick
    has elapsed).
    
    Enabling interrupt notifications provide a more accurate and responsive 
    callback towards the clients.
    However the client callbacks need to be non blocking and as short as
    possible (the client callback are meant for setting a flag to indicate that
    further processing is needed and not to execute lengthy processing inside
    the callback itself).
    
    - true  - Client notification in interrupt enabled
    - false - Client notification in interrupt disabled


  Remarks:
    None.

*/

#define SYS_TMR_INTERRUPT_NOTIFICATION                         (false)


#endif // _SYS_TMR_CONFIG_TEMPLATE_H

/*******************************************************************************
 End of File
*/

