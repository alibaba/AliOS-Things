/*******************************************************************************
  Timer Driver Interface Mapping Dynamic APIs to Static APIs           

  Company:
    Microchip Technology Inc.

  File Name:
    drv_tmr_mapping.h

  Summary:
    This file allows maintaining a single set of APIs for all Timer transactions  
    by making the type of implementation transparent to the application. In case
    where static implementation of Timer driver is selected, this file maps the 
    API functions to a particular driver instance-specific static implementation
    function, eliminating unnecessary dynamic parameters. 
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

#include "driver/tmr/drv_tmr.h"
#include "driver/tmr/drv_tmr_static.h"

// *****************************************************************************
// *****************************************************************************
// Section: Driver System Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_TMR_Initialize 
    ( 
        const SYS_MODULE_INDEX drvIndex,
        const SYS_MODULE_INIT * const init 
    )

  Summary:
    Initializes the Timer driver.

  Description:
    This function initializes the Timer driver, making it ready for clients to
    open and use it.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

SYS_MODULE_OBJ DRV_TMR_Initialize 
( 
    const SYS_MODULE_INDEX drvIndex,
    const SYS_MODULE_INIT  * const init 
)
{
	SYS_MODULE_OBJ sysObj;
	
	switch (drvIndex)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true>
        case DRV_TMR_INDEX_0:
        {
            DRV_TMR0_Initialize();
			sysObj = (SYS_MODULE_OBJ)DRV_TMR_INDEX_0;
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
        case DRV_TMR_INDEX_1:
        {
            DRV_TMR1_Initialize();
			sysObj = (SYS_MODULE_OBJ)DRV_TMR_INDEX_1;
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
        case DRV_TMR_INDEX_2:
        {
            DRV_TMR2_Initialize();
			sysObj = (SYS_MODULE_OBJ)DRV_TMR_INDEX_2;
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
        case DRV_TMR_INDEX_3:
        {
            DRV_TMR3_Initialize();
			sysObj = (SYS_MODULE_OBJ)DRV_TMR_INDEX_3;
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
        case DRV_TMR_INDEX_4:
        {
            DRV_TMR4_Initialize();
			sysObj = (SYS_MODULE_OBJ)DRV_TMR_INDEX_4;
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
        case DRV_TMR_INDEX_5:
        {
            DRV_TMR5_Initialize();
			sysObj = (SYS_MODULE_OBJ)DRV_TMR_INDEX_5;
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
        case DRV_TMR_INDEX_6:
        {
            DRV_TMR6_Initialize();
			sysObj = (SYS_MODULE_OBJ)DRV_TMR_INDEX_6;
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
        case DRV_TMR_INDEX_7:
        {
            DRV_TMR7_Initialize();
			sysObj = (SYS_MODULE_OBJ)DRV_TMR_INDEX_7;
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
        case DRV_TMR_INDEX_8:
        {
            DRV_TMR8_Initialize();
			sysObj = (SYS_MODULE_OBJ)DRV_TMR_INDEX_8;
            break;
        }
        </#if>
        default:
        {
			sysObj = SYS_MODULE_OBJ_INVALID;
            break;
        }
    }
	return sysObj;
}

// *****************************************************************************
/* Function:
    void DRV_TMR_Deinitialize ( SYS_MODULE_OBJ object )

  Summary:
    Deinitializes the specified instance of the Timer driver.

  Description:
    Deinitializes the specified instance of the Timer driver, disabling
    its operation (and any hardware). All internal data is invalidated.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_Deinitialize ( SYS_MODULE_OBJ object )
{
	switch (object)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true>
        case DRV_TMR_INDEX_0:
        {
            DRV_TMR0_DeInitialize();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
        case DRV_TMR_INDEX_1:
        {
            DRV_TMR1_DeInitialize();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
        case DRV_TMR_INDEX_2:
        {
            DRV_TMR2_DeInitialize();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
        case DRV_TMR_INDEX_3:
        {
            DRV_TMR3_DeInitialize();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
        case DRV_TMR_INDEX_4:
        {
            DRV_TMR4_DeInitialize();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
        case DRV_TMR_INDEX_5:
        {
            DRV_TMR5_DeInitialize();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
        case DRV_TMR_INDEX_6:
        {
            DRV_TMR6_DeInitialize();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
        case DRV_TMR_INDEX_7:
        {
            DRV_TMR7_DeInitialize();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
        case DRV_TMR_INDEX_8:
        {
            DRV_TMR8_DeInitialize();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }
}

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_TMR_Status ( SYS_MODULE_OBJ object )

  Summary:
    Provides the current status of the Timer driver.

  Description:
    This function provides the current status of the Timer driver.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

SYS_STATUS DRV_TMR_Status ( SYS_MODULE_OBJ object )
{
	SYS_STATUS returnValue;
	
	switch (object)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true>
        case DRV_TMR_INDEX_0:
        {
            returnValue = DRV_TMR0_Status();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
        case DRV_TMR_INDEX_1:
        {
            returnValue = DRV_TMR1_Status();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
        case DRV_TMR_INDEX_2:
        {
            returnValue = DRV_TMR2_Status();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
        case DRV_TMR_INDEX_3:
        {
            returnValue = DRV_TMR3_Status();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
        case DRV_TMR_INDEX_4:
        {
            returnValue = DRV_TMR4_Status();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
        case DRV_TMR_INDEX_5:
        {
            returnValue = DRV_TMR5_Status();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
        case DRV_TMR_INDEX_6:
        {
            returnValue = DRV_TMR6_Status();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
        case DRV_TMR_INDEX_7:
        {
            returnValue = DRV_TMR7_Status();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
        case DRV_TMR_INDEX_8:
        {
            returnValue = DRV_TMR8_Status();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }
	return returnValue;
}

// *****************************************************************************
/* Function:
    void DRV_TMR_Tasks ( SYS_MODULE_OBJ object )

  Summary:
    Maintains the driver's state machine.

  Description:
    This function is used to maintain the driver's internal state machine and
    processes the timer events.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_Tasks ( SYS_MODULE_OBJ object )
{
	switch (object)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true>
        case DRV_TMR_INDEX_0:
        {
            DRV_TMR0_Tasks();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
        case DRV_TMR_INDEX_1:
        {
            DRV_TMR1_Tasks();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
        case DRV_TMR_INDEX_2:
        {
            DRV_TMR2_Tasks();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
        case DRV_TMR_INDEX_3:
        {
            DRV_TMR3_Tasks();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
        case DRV_TMR_INDEX_4:
        {
            DRV_TMR4_Tasks();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
        case DRV_TMR_INDEX_5:
        {
            DRV_TMR5_Tasks();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
        case DRV_TMR_INDEX_6:
        {
            DRV_TMR6_Tasks();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
        case DRV_TMR_INDEX_7:
        {
            DRV_TMR7_Tasks();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
        case DRV_TMR_INDEX_8:
        {
            DRV_TMR8_Tasks();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Client Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_TMR_Open 
    ( 
        const SYS_MODULE_INDEX index,
        const DRV_IO_INTENT    intent
    )

  Summary:
    Opens the specified Timer driver instance and returns a handle to it.
  
  Description:
    This function opens the specified Timer driver instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the driver.  Timer driver does not
    support multiple clients. If two tasks want to use the timer, one should
    wait until the other one gets closed. 

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

DRV_HANDLE DRV_TMR_Open 
( 
    const SYS_MODULE_INDEX index, 
    const DRV_IO_INTENT intent 
)
{
    switch (index)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true>
        case DRV_TMR_INDEX_0:
        {
            return (DRV_HANDLE)DRV_TMR_INDEX_0;
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
        case DRV_TMR_INDEX_1:
        {
            return (DRV_HANDLE)DRV_TMR_INDEX_1;
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
        case DRV_TMR_INDEX_2:
        {
            return (DRV_HANDLE)DRV_TMR_INDEX_2;
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
        case DRV_TMR_INDEX_3:
        {
            return (DRV_HANDLE)DRV_TMR_INDEX_3;
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
        case DRV_TMR_INDEX_4:
        {
            return (DRV_HANDLE)DRV_TMR_INDEX_4;
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
        case DRV_TMR_INDEX_5:
        {
            return (DRV_HANDLE)DRV_TMR_INDEX_5;
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
        case DRV_TMR_INDEX_6:
        {
            return (DRV_HANDLE)DRV_TMR_INDEX_6;
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
        case DRV_TMR_INDEX_7:
        {
            return (DRV_HANDLE)DRV_TMR_INDEX_7;
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
        case DRV_TMR_INDEX_8:
        {
            return (DRV_HANDLE)DRV_TMR_INDEX_8;
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }
    
    return (DRV_HANDLE)NULL;
}

// *****************************************************************************
/* Function:
    void DRV_TMR_Close ( DRV_HANDLE handle )

  Summary:
    Closes an opened instance of the Timer driver.

  Description:
    This function closes an opened instance of the Timer driver, invalidating
    the handle.
  
  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_Close ( DRV_HANDLE handle )
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true>
        case DRV_TMR_INDEX_0:
        {
            DRV_TMR0_Close();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
        case DRV_TMR_INDEX_1:
        {
            DRV_TMR1_Close();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
        case DRV_TMR_INDEX_2:
        {
            DRV_TMR2_Close();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
        case DRV_TMR_INDEX_3:
        {
            DRV_TMR3_Close();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
        case DRV_TMR_INDEX_4:
        {
            DRV_TMR4_Close();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
        case DRV_TMR_INDEX_5:
        {
            DRV_TMR5_Close();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
        case DRV_TMR_INDEX_6:
        {
            DRV_TMR6_Close();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
        case DRV_TMR_INDEX_7:
        {
            DRV_TMR7_Close();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
        case DRV_TMR_INDEX_8:
        {
            DRV_TMR8_Close();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }
}

// *****************************************************************************
/* Function:
    DRV_TMR_CLIENT_STATUS DRV_TMR_ClientStatus ( DRV_HANDLE handle )

  Summary:
    Gets the status of the client operation.

  Description:
    This function gets the status of the recently completed client level
    operation.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

DRV_TMR_CLIENT_STATUS DRV_TMR_ClientStatus 
( DRV_HANDLE handle )
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true>
        case DRV_TMR_INDEX_0:
        {
            return DRV_TMR0_ClientStatus();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
        case DRV_TMR_INDEX_1:
        {
            return DRV_TMR1_ClientStatus();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
        case DRV_TMR_INDEX_2:
        {
            return DRV_TMR2_ClientStatus();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
        case DRV_TMR_INDEX_3:
        {
            return DRV_TMR3_ClientStatus();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
        case DRV_TMR_INDEX_4:
        {
            return DRV_TMR4_ClientStatus();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
        case DRV_TMR_INDEX_5:
        {
            return DRV_TMR5_ClientStatus();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
        case DRV_TMR_INDEX_6:
        {
            return DRV_TMR6_ClientStatus();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
        case DRV_TMR_INDEX_7:
        {
            return DRV_TMR7_ClientStatus();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
        case DRV_TMR_INDEX_8:
        {
            return DRV_TMR8_ClientStatus();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }

    return DRV_TMR_CLIENT_STATUS_INVALID;
}

// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Counter functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void DRV_TMR_CounterValueSet ( DRV_HANDLE handle, uint32_t counterPeriod )

  Summary:
    Updates the Timer's counter register.

  Description:
    This function updates the Timer's value in the counter register.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_CounterValueSet ( DRV_HANDLE handle, uint32_t counterPeriod )
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true>
        case DRV_TMR_INDEX_0:
        {
            DRV_TMR0_CounterValueSet(counterPeriod);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
        case DRV_TMR_INDEX_1:
        {
            DRV_TMR1_CounterValueSet(counterPeriod);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
        case DRV_TMR_INDEX_2:
        {
            DRV_TMR2_CounterValueSet(counterPeriod);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
        case DRV_TMR_INDEX_3:
        {
            DRV_TMR3_CounterValueSet(counterPeriod);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
        case DRV_TMR_INDEX_4:
        {
            DRV_TMR4_CounterValueSet(counterPeriod);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
        case DRV_TMR_INDEX_5:
        {
            DRV_TMR5_CounterValueSet(counterPeriod);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
        case DRV_TMR_INDEX_6:
        {
            DRV_TMR6_CounterValueSet(counterPeriod);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
        case DRV_TMR_INDEX_7:
        {
            DRV_TMR7_CounterValueSet(counterPeriod);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
        case DRV_TMR_INDEX_8:
        {
            DRV_TMR8_CounterValueSet(counterPeriod);
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }
}

// *****************************************************************************
/* Function:
    uint32_t DRV_TMR_CounterValueGet ( DRV_HANDLE handle )

  Summary:
    Reads the Timer's counter register.

  Description:
    This function returns the Timer's value in the counter register.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

uint32_t DRV_TMR_CounterValueGet ( DRV_HANDLE handle )
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true>
        case DRV_TMR_INDEX_0:
        {
            return DRV_TMR0_CounterValueGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
        case DRV_TMR_INDEX_1:
        {
            return DRV_TMR1_CounterValueGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
        case DRV_TMR_INDEX_2:
        {
            return DRV_TMR2_CounterValueGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
        case DRV_TMR_INDEX_3:
        {
            return DRV_TMR3_CounterValueGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
        case DRV_TMR_INDEX_4:
        {
            return DRV_TMR4_CounterValueGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
        case DRV_TMR_INDEX_5:
        {
            return DRV_TMR5_CounterValueGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
        case DRV_TMR_INDEX_6:
        {
            return DRV_TMR6_CounterValueGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
        case DRV_TMR_INDEX_7:
        {
            return DRV_TMR7_CounterValueGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
        case DRV_TMR_INDEX_8:
        {
            return DRV_TMR8_CounterValueGet();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }

    return 0;
}

// *****************************************************************************
/* Function:
    void DRV_TMR_CounterClear ( DRV_HANDLE handle )

  Summary:
    Clears the Timer's counter register.

  Description:
    This function clears the Timer's value in the counter register.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_CounterClear ( DRV_HANDLE handle )
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true>
        case DRV_TMR_INDEX_0:
        {
            DRV_TMR0_CounterClear();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
        case DRV_TMR_INDEX_1:
        {
            DRV_TMR1_CounterClear();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
        case DRV_TMR_INDEX_2:
        {
            DRV_TMR2_CounterClear();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
        case DRV_TMR_INDEX_3:
        {
            DRV_TMR3_CounterClear();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
        case DRV_TMR_INDEX_4:
        {
            DRV_TMR4_CounterClear();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
        case DRV_TMR_INDEX_5:
        {
            DRV_TMR5_CounterClear();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
        case DRV_TMR_INDEX_6:
        {
            DRV_TMR6_CounterClear();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
        case DRV_TMR_INDEX_7:
        {
            DRV_TMR7_CounterClear();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
        case DRV_TMR_INDEX_8:
        {
            DRV_TMR8_CounterClear();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Alarm functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    bool DRV_TMR_AlarmRegister 
    ( 
        DRV_HANDLE handle, 
        uint32_t divider, 
        bool isPeriodic, 
        uintptr_t context, 
        DRV_TMR_CALLBACK callBack 
    )

  Summary:
    Sets up an alarm.

  Description:
    This function sets up an alarm, allowing the client to receive a callback
    from the driver when the timer counter reaches zero.  Alarms can be one-shot
    or periodic.  A periodic alarm will reload the timer and generate alarm
    until stopped.  The alarm frequency is: DRV_TMR_CounterFrequencyGet() /
    divider;

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

bool DRV_TMR_AlarmRegister 
(
    DRV_HANDLE handle, 
    uint32_t divider, 
    bool isPeriodic, 
    uintptr_t context, 
    DRV_TMR_CALLBACK callBack 
)
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX0>
        case DRV_TMR_INDEX_0:
        {
            return DRV_TMR0_AlarmRegister(divider, isPeriodic, context, callBack);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX1>
        case DRV_TMR_INDEX_1:
        {
            return DRV_TMR1_AlarmRegister(divider, isPeriodic, context, callBack);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX2>
        case DRV_TMR_INDEX_2:
        {
            return DRV_TMR2_AlarmRegister(divider, isPeriodic, context, callBack);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX3>
        case DRV_TMR_INDEX_3:
        {
            return DRV_TMR3_AlarmRegister(divider, isPeriodic, context, callBack);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX4>
        case DRV_TMR_INDEX_4:
        {
            return DRV_TMR4_AlarmRegister(divider, isPeriodic, context, callBack);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX5>
        case DRV_TMR_INDEX_5:
        {
            return DRV_TMR5_AlarmRegister(divider, isPeriodic, context, callBack);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX6>
        case DRV_TMR_INDEX_6:
        {
            return DRV_TMR6_AlarmRegister(divider, isPeriodic, context, callBack);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX7>
        case DRV_TMR_INDEX_7:
        {
            return DRV_TMR7_AlarmRegister(divider, isPeriodic, context, callBack);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX8>
        case DRV_TMR_INDEX_8:
        {
            return DRV_TMR8_AlarmRegister(divider, isPeriodic, context, callBack);
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }

    return false;
}

// *****************************************************************************
/* Function:
    bool DRV_TMR_AlarmDisable ( DRV_HANDLE handle);

  Summary:
    Disables an alarm signal.

  Description:
    This function allows the client to disable an alarm generation.
    Use DRV_TMR_AlarmEnable to re-enable.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

bool DRV_TMR_AlarmDisable ( DRV_HANDLE handle)
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX0>
        case DRV_TMR_INDEX_0:
        {
            return DRV_TMR0_AlarmDisable();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX1>
        case DRV_TMR_INDEX_1:
        {
            return DRV_TMR1_AlarmDisable();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX2>
        case DRV_TMR_INDEX_2:
        {
            return DRV_TMR2_AlarmDisable();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX3>
        case DRV_TMR_INDEX_3:
        {
            return DRV_TMR3_AlarmDisable();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX4>
        case DRV_TMR_INDEX_4:
        {
            return DRV_TMR4_AlarmDisable();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX5>
        case DRV_TMR_INDEX_5:
        {
            return DRV_TMR5_AlarmDisable();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX6>
        case DRV_TMR_INDEX_6:
        {
            return DRV_TMR6_AlarmDisable();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX7>
        case DRV_TMR_INDEX_7:
        {
            return DRV_TMR7_AlarmDisable();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX8>
        case DRV_TMR_INDEX_8:
        {
            return DRV_TMR8_AlarmDisable();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }

    return false;
}

// *****************************************************************************
/* Function:
    void DRV_TMR_AlarmEnable ( DRV_HANDLE handle, bool enable );

  Summary:
    Re-enables an alarm signal.

  Description:
    This function allows the client to re-enable an alarm after it has been
    disabled by a DRV_TMR_AlarmDisable call.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_AlarmEnable ( DRV_HANDLE handle, bool enable )
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX0>
        case DRV_TMR_INDEX_0:
        {
            DRV_TMR0_AlarmEnable(enable);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX1>
        case DRV_TMR_INDEX_1:
        {
            DRV_TMR1_AlarmEnable(enable);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX2>
        case DRV_TMR_INDEX_2:
        {
            DRV_TMR2_AlarmEnable(enable);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX3>
        case DRV_TMR_INDEX_3:
        {
            DRV_TMR3_AlarmEnable(enable);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX4>
        case DRV_TMR_INDEX_4:
        {
            DRV_TMR4_AlarmEnable(enable);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX5>
        case DRV_TMR_INDEX_5:
        {
            DRV_TMR5_AlarmEnable(enable);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX6>
        case DRV_TMR_INDEX_6:
        {
            DRV_TMR6_AlarmEnable(enable);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX7>
        case DRV_TMR_INDEX_7:
        {
            DRV_TMR7_AlarmEnable(enable);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX8>
        case DRV_TMR_INDEX_8:
        {
            DRV_TMR8_AlarmEnable(enable);
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }
}

// *****************************************************************************
/* Function:
    void DRV_TMR_AlarmPeriodSet ( DRV_HANDLE handle, uint32_t value )

  Summary:
    Updates the Timer's period.

  Description:
    This function updates the Timer's period.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_AlarmPeriodSet ( DRV_HANDLE handle, uint32_t value )
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX0>
        case DRV_TMR_INDEX_0:
        {
            DRV_TMR0_AlarmPeriodSet(value);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX1>
        case DRV_TMR_INDEX_1:
        {
            DRV_TMR1_AlarmPeriodSet(value);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX2>
        case DRV_TMR_INDEX_2:
        {
            DRV_TMR2_AlarmPeriodSet(value);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX3>
        case DRV_TMR_INDEX_3:
        {
            DRV_TMR3_AlarmPeriodSet(value);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX4>
        case DRV_TMR_INDEX_4:
        {
            DRV_TMR4_AlarmPeriodSet(value);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX5>
        case DRV_TMR_INDEX_5:
        {
            DRV_TMR5_AlarmPeriodSet(value);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX6>
        case DRV_TMR_INDEX_6:
        {
            DRV_TMR6_AlarmPeriodSet(value);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX7>
        case DRV_TMR_INDEX_7:
        {
            DRV_TMR7_AlarmPeriodSet(value);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX8>
        case DRV_TMR_INDEX_8:
        {
            DRV_TMR8_AlarmPeriodSet(value);
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }
}

// *****************************************************************************
/* Function:
    uint32_t DRV_TMR_AlarmPeriodGet ( DRV_HANDLE handle )

  Summary:
    Provides the Timer's period.

  Description:
    This function gets the Timer's period.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

uint32_t DRV_TMR_AlarmPeriodGet ( DRV_HANDLE handle )
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX0>
        case DRV_TMR_INDEX_0:
        {
            return DRV_TMR0_AlarmPeriodGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX1>
        case DRV_TMR_INDEX_1:
        {
            return DRV_TMR1_AlarmPeriodGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX2>
        case DRV_TMR_INDEX_2:
        {
            return DRV_TMR2_AlarmPeriodGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX3>
        case DRV_TMR_INDEX_3:
        {
            return DRV_TMR3_AlarmPeriodGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX4>
        case DRV_TMR_INDEX_4:
        {
            return DRV_TMR4_AlarmPeriodGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX5>
        case DRV_TMR_INDEX_5:
        {
            return DRV_TMR5_AlarmPeriodGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX6>
        case DRV_TMR_INDEX_6:
        {
            return DRV_TMR6_AlarmPeriodGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX7>
        case DRV_TMR_INDEX_7:
        {
            return DRV_TMR7_AlarmPeriodGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX8>
        case DRV_TMR_INDEX_8:
        {
            return DRV_TMR8_AlarmPeriodGet();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }

    return 0;
}

// *****************************************************************************
/* Function:
    void DRV_TMR_AlarmDeregister ( DRV_HANDLE handle )

  Summary:
    Removes a previously set alarm.

  Description:
    This function removes a previously set alarm.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_AlarmDeregister ( DRV_HANDLE handle )
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX0>
        case DRV_TMR_INDEX_0:
        {
            DRV_TMR0_AlarmDeregister();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX1>
        case DRV_TMR_INDEX_1:
        {
            DRV_TMR1_AlarmDeregister();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX2>
        case DRV_TMR_INDEX_2:
        {
            DRV_TMR2_AlarmDeregister();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX3>
        case DRV_TMR_INDEX_3:
        {
            DRV_TMR3_AlarmDeregister();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX4>
        case DRV_TMR_INDEX_4:
        {
            DRV_TMR4_AlarmDeregister();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX5>
        case DRV_TMR_INDEX_5:
        {
            DRV_TMR5_AlarmDeregister();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX6>
        case DRV_TMR_INDEX_6:
        {
            DRV_TMR6_AlarmDeregister();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX7>
        case DRV_TMR_INDEX_7:
        {
            DRV_TMR7_AlarmDeregister();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX8>
        case DRV_TMR_INDEX_8:
        {
            DRV_TMR8_AlarmDeregister();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Operation Control functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    bool DRV_TMR_Start ( DRV_HANDLE handle )

  Summary:
    Starts the Timer counting.

  Description:
    This function starts the Timer counting.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

bool DRV_TMR_Start ( DRV_HANDLE handle )
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true>
        case DRV_TMR_INDEX_0:
        {
            return DRV_TMR0_Start();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
        case DRV_TMR_INDEX_1:
        {
            return DRV_TMR1_Start();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
        case DRV_TMR_INDEX_2:
        {
            return DRV_TMR2_Start();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
        case DRV_TMR_INDEX_3:
        {
            return DRV_TMR3_Start();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
        case DRV_TMR_INDEX_4:
        {
            return DRV_TMR4_Start();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
        case DRV_TMR_INDEX_5:
        {
            return DRV_TMR5_Start();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
        case DRV_TMR_INDEX_6:
        {
            return DRV_TMR6_Start();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
        case DRV_TMR_INDEX_7:
        {
            return DRV_TMR7_Start();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
        case DRV_TMR_INDEX_8:
        {
            return DRV_TMR8_Start();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }

    return false;
}

// *****************************************************************************
/* Function:
    void DRV_TMR_Stop ( DRV_HANDLE handle )

  Summary:
    Stops the Timer from counting.

  Description:
    This function stops the running Timer from counting.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_Stop ( DRV_HANDLE handle )
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true>
        case DRV_TMR_INDEX_0:
        {
            DRV_TMR0_Stop();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
        case DRV_TMR_INDEX_1:
        {
            DRV_TMR1_Stop();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
        case DRV_TMR_INDEX_2:
        {
            DRV_TMR2_Stop();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
        case DRV_TMR_INDEX_3:
        {
            DRV_TMR3_Stop();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
        case DRV_TMR_INDEX_4:
        {
            DRV_TMR4_Stop();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
        case DRV_TMR_INDEX_5:
        {
            DRV_TMR5_Stop();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
        case DRV_TMR_INDEX_6:
        {
            DRV_TMR6_Stop();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
        case DRV_TMR_INDEX_7:
        {
            DRV_TMR7_Stop();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
        case DRV_TMR_INDEX_8:
        {
            DRV_TMR8_Stop();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }
}

// *****************************************************************************
/* Function:
    uint32_t DRV_TMR_AlarmHasElapsed ( DRV_HANDLE handle )

  Summary:
    Provides the status of Timer's period elapse.

  Description:
    This function returns the number of times Timer's period has elapsed since
    last call to this API has made. On calling this API, the internally
    maintained counter will be cleared and count will be started again from next
    elapse.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

uint32_t DRV_TMR_AlarmHasElapsed ( DRV_HANDLE handle )
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX0>
        case DRV_TMR_INDEX_0:
        {
            return DRV_TMR0_AlarmHasElapsed();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX1>
        case DRV_TMR_INDEX_1:
        {
            return DRV_TMR1_AlarmHasElapsed();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX2>
        case DRV_TMR_INDEX_2:
        {
            return DRV_TMR2_AlarmHasElapsed();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX3>
        case DRV_TMR_INDEX_3:
        {
            return DRV_TMR3_AlarmHasElapsed();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX4>
        case DRV_TMR_INDEX_4:
        {
            return DRV_TMR4_AlarmHasElapsed();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX5>
        case DRV_TMR_INDEX_5:
        {
            return DRV_TMR5_AlarmHasElapsed();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX6>
        case DRV_TMR_INDEX_6:
        {
            return DRV_TMR6_AlarmHasElapsed();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX7>
        case DRV_TMR_INDEX_7:
        {
            return DRV_TMR7_AlarmHasElapsed();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true && CONFIG_DRV_TMR_ALARM_FUNCS_IDX8>
        case DRV_TMR_INDEX_8:
        {
            return DRV_TMR8_AlarmHasElapsed();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }

    return 0;
}

// *****************************************************************************
/* Function:
    DRV_TMR_OPERATION_MODE DRV_TMR_OperationModeGet(DRV_HANDLE handle)

  Summary:
    This function gets the currently selected operation mode.

  Description:
    This function gets the currently selected 16/32 bit operation mode.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

DRV_TMR_OPERATION_MODE DRV_TMR_OperationModeGet(DRV_HANDLE handle)
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true>
        case DRV_TMR_INDEX_0:
        {
            return DRV_TMR0_OperationModeGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
        case DRV_TMR_INDEX_1:
        {
            return DRV_TMR1_OperationModeGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
        case DRV_TMR_INDEX_2:
        {
            return DRV_TMR2_OperationModeGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
        case DRV_TMR_INDEX_3:
        {
            return DRV_TMR3_OperationModeGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
        case DRV_TMR_INDEX_4:
        {
            return DRV_TMR4_OperationModeGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
        case DRV_TMR_INDEX_5:
        {
            return DRV_TMR5_OperationModeGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
        case DRV_TMR_INDEX_6:
        {
            return DRV_TMR6_OperationModeGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
        case DRV_TMR_INDEX_7:
        {
            return DRV_TMR7_OperationModeGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
        case DRV_TMR_INDEX_8:
        {
            return DRV_TMR8_OperationModeGet();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }

    return DRV_TMR_OPERATION_MODE_NONE;
}

// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Miscellaneous information functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    bool DRV_TMR_ClockSet 
    ( 
        DRV_HANDLE handle, 
        DRV_TMR_CLK_SOURCES clockSource, 
        TMR_PRESCALE  preScale 
    )

  Summary:
    Sets the timers clock by selecting the source and prescaler.

  Description:
    This function sets the timers clock by selecting the source and prescaler.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

bool DRV_TMR_ClockSet 
( 
    DRV_HANDLE handle, 
    DRV_TMR_CLK_SOURCES clockSource, 
    TMR_PRESCALE  preScale 
)
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true>
        case DRV_TMR_INDEX_0:
        {
            return DRV_TMR0_ClockSet(clockSource, preScale);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
        case DRV_TMR_INDEX_1:
        {
            return DRV_TMR1_ClockSet(clockSource, preScale);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
        case DRV_TMR_INDEX_2:
        {
            return DRV_TMR2_ClockSet(clockSource, preScale);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
        case DRV_TMR_INDEX_3:
        {
            return DRV_TMR3_ClockSet(clockSource, preScale);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
        case DRV_TMR_INDEX_4:
        {
            return DRV_TMR4_ClockSet(clockSource, preScale);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
        case DRV_TMR_INDEX_5:
        {
            return DRV_TMR5_ClockSet(clockSource, preScale);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
        case DRV_TMR_INDEX_6:
        {
            return DRV_TMR6_ClockSet(clockSource, preScale);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
        case DRV_TMR_INDEX_7:
        {
            return DRV_TMR7_ClockSet(clockSource, preScale);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
        case DRV_TMR_INDEX_8:
        {
            return DRV_TMR8_ClockSet(clockSource, preScale);
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }

    return false;
}

// *****************************************************************************
/* Function:
    TMR_PRESCALE DRV_TMR_PrescalerGet ( DRV_HANDLE handle )

  Summary:
    This function gets the currently selected prescaler.

  Description:
    This function gets the currently selected prescaler.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

TMR_PRESCALE DRV_TMR_PrescalerGet ( DRV_HANDLE handle )
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true>
        case DRV_TMR_INDEX_0:
        {
            return DRV_TMR0_PrescalerGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
        case DRV_TMR_INDEX_1:
        {
            return DRV_TMR1_PrescalerGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
        case DRV_TMR_INDEX_2:
        {
            return DRV_TMR2_PrescalerGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
        case DRV_TMR_INDEX_3:
        {
            return DRV_TMR3_PrescalerGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
        case DRV_TMR_INDEX_4:
        {
            return DRV_TMR4_PrescalerGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
        case DRV_TMR_INDEX_5:
        {
            return DRV_TMR5_PrescalerGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
        case DRV_TMR_INDEX_6:
        {
            return DRV_TMR6_PrescalerGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
        case DRV_TMR_INDEX_7:
        {
            return DRV_TMR7_PrescalerGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
        case DRV_TMR_INDEX_8:
        {
            return DRV_TMR8_PrescalerGet();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }

    return -1;
}

// *****************************************************************************
/* Function:
    bool DRV_TMR_GateModeSet ( DRV_HANDLE handle )

  Summary:
    Enables the Gate mode.

  Description:
    This function enables the Gated mode of Timer. User can measure the duration
    of an external signal in this mode. Once the Gate mode is enabled, Timer
    will start on the raising edge of the external signal. It will keep counting
    until the next falling edge. 

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

bool DRV_TMR_GateModeSet ( DRV_HANDLE handle )
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true && CONFIG_DRV_TMR_GATED_MODE_IDX0>
        case DRV_TMR_INDEX_0:
        {
            return DRV_TMR0_GateModeSet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true && CONFIG_DRV_TMR_GATED_MODE_IDX1>
        case DRV_TMR_INDEX_1:
        {
            return DRV_TMR1_GateModeSet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true && CONFIG_DRV_TMR_GATED_MODE_IDX2>
        case DRV_TMR_INDEX_2:
        {
            return DRV_TMR2_GateModeSet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true && CONFIG_DRV_TMR_GATED_MODE_IDX3>
        case DRV_TMR_INDEX_3:
        {
            return DRV_TMR3_GateModeSet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true && CONFIG_DRV_TMR_GATED_MODE_IDX4>
        case DRV_TMR_INDEX_4:
        {
            return DRV_TMR4_GateModeSet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true && CONFIG_DRV_TMR_GATED_MODE_IDX5>
        case DRV_TMR_INDEX_5:
        {
            return DRV_TMR5_GateModeSet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true && CONFIG_DRV_TMR_GATED_MODE_IDX6>
        case DRV_TMR_INDEX_6:
        {
            return DRV_TMR6_GateModeSet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true && CONFIG_DRV_TMR_GATED_MODE_IDX7>
        case DRV_TMR_INDEX_7:
        {
            return DRV_TMR7_GateModeSet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true && CONFIG_DRV_TMR_GATED_MODE_IDX8>
        case DRV_TMR_INDEX_8:
        {
            return DRV_TMR8_GateModeSet();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }

    return false;
}

// *****************************************************************************
/* Function:
    bool DRV_TMR_GateModeClear ( DRV_HANDLE handle )

  Summary:
    Enables the Gate mode.

  Description:
    This function enables the Gated mode of Timer. User can measure the duration
    of an external signal in this mode. Once the Gate mode is enabled, Timer
    will start on the raising edge of the external signal. It will keep counting
    until the next falling edge. 

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

bool DRV_TMR_GateModeClear ( DRV_HANDLE handle )
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true && CONFIG_DRV_TMR_GATED_MODE_IDX0>
        case DRV_TMR_INDEX_0:
        {
            return DRV_TMR0_GateModeClear();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true && CONFIG_DRV_TMR_GATED_MODE_IDX1>
        case DRV_TMR_INDEX_1:
        {
            return DRV_TMR1_GateModeClear();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true && CONFIG_DRV_TMR_GATED_MODE_IDX2>
        case DRV_TMR_INDEX_2:
        {
            return DRV_TMR2_GateModeClear();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true && CONFIG_DRV_TMR_GATED_MODE_IDX3>
        case DRV_TMR_INDEX_3:
        {
            return DRV_TMR3_GateModeClear();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true && CONFIG_DRV_TMR_GATED_MODE_IDX4>
        case DRV_TMR_INDEX_4:
        {
            return DRV_TMR4_GateModeClear();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true && CONFIG_DRV_TMR_GATED_MODE_IDX5>
        case DRV_TMR_INDEX_5:
        {
            return DRV_TMR5_GateModeClear();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true && CONFIG_DRV_TMR_GATED_MODE_IDX6>
        case DRV_TMR_INDEX_6:
        {
            return DRV_TMR6_GateModeClear();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true && CONFIG_DRV_TMR_GATED_MODE_IDX7>
        case DRV_TMR_INDEX_7:
        {
            return DRV_TMR7_GateModeClear();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true && CONFIG_DRV_TMR_GATED_MODE_IDX8>
        case DRV_TMR_INDEX_8:
        {
            return DRV_TMR8_GateModeClear();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }

    return false;
}

// *****************************************************************************
/* Function:
    uint32_t DRV_TMR_CounterFrequencyGet ( DRV_HANDLE handle )

  Summary:
    Provides the Timer input frequency.

  Description:
    This function provides the Timer input frequency. Input frequency is the
    clock to the Timer register and it is considering the prescaler divisor. 

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

uint32_t DRV_TMR_CounterFrequencyGet ( DRV_HANDLE handle )
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true>
        case DRV_TMR_INDEX_0:
        {
            return DRV_TMR0_CounterFrequencyGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
        case DRV_TMR_INDEX_1:
        {
            return DRV_TMR1_CounterFrequencyGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
        case DRV_TMR_INDEX_2:
        {
            return DRV_TMR2_CounterFrequencyGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
        case DRV_TMR_INDEX_3:
        {
            return DRV_TMR3_CounterFrequencyGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
        case DRV_TMR_INDEX_4:
        {
            return DRV_TMR4_CounterFrequencyGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
        case DRV_TMR_INDEX_5:
        {
            return DRV_TMR5_CounterFrequencyGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
        case DRV_TMR_INDEX_6:
        {
            return DRV_TMR6_CounterFrequencyGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
        case DRV_TMR_INDEX_7:
        {
            return DRV_TMR7_CounterFrequencyGet();
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
        case DRV_TMR_INDEX_8:
        {
            return DRV_TMR8_CounterFrequencyGet();
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }

    return 0;
}

// *****************************************************************************
/* Function:
    DRV_TMR_OPERATION_MODE DRV_TMR_DividerRangeGet 
    ( 
        DRV_HANDLE handle, 
        DRV_TMR_DIVIDER_RANGE* pDivRange
    )

  Summary:
    Returns the Timer divider values.

  Description:
    This function provides the Timer operating mode and divider range.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

DRV_TMR_OPERATION_MODE DRV_TMR_DividerRangeGet 
( 
    DRV_HANDLE handle,
    DRV_TMR_DIVIDER_RANGE* pDivRange
)
{
    switch (handle)
    {
        <#if CONFIG_DRV_TMR_INST_0 == true>
        case DRV_TMR_INDEX_0:
        {
            return DRV_TMR0_DividerRangeGet(pDivRange);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
        case DRV_TMR_INDEX_1:
        {
            return DRV_TMR1_DividerRangeGet(pDivRange);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
        case DRV_TMR_INDEX_2:
        {
            return DRV_TMR2_DividerRangeGet(pDivRange);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
        case DRV_TMR_INDEX_3:
        {
            return DRV_TMR3_DividerRangeGet(pDivRange);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
        case DRV_TMR_INDEX_4:
        {
            return DRV_TMR4_DividerRangeGet(pDivRange);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
        case DRV_TMR_INDEX_5:
        {
            return DRV_TMR5_DividerRangeGet(pDivRange);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
        case DRV_TMR_INDEX_6:
        {
            return DRV_TMR6_DividerRangeGet(pDivRange);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
        case DRV_TMR_INDEX_7:
        {
            return DRV_TMR7_DividerRangeGet(pDivRange);
            break;
        }
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
        case DRV_TMR_INDEX_8:
        {
            return DRV_TMR8_DividerRangeGet(pDivRange);
            break;
        }
        </#if>
        default:
        {
            break;
        }
    }

    return DRV_TMR_OPERATION_MODE_NONE;
}

