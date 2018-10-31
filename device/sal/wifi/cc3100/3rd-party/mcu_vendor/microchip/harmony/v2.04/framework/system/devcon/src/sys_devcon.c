/*******************************************************************************
  Device Control System Service Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    sys_devcon.c

  Summary:
    Device Control System Service implementation.

  Description:
    The DEVCON system service provides a simple interface to manage the Device 
    Control module on PIC32M Microchip microcontrollers. This file Implements 
    the core interface routines for the Device Control system service.  While 
    building the system service from source, ALWAYS include this file in the 
    build for PIC32M devices.
*******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "system/devcon/src/sys_devcon_local.h"
#include "peripheral/int/plib_int.h"
#include "peripheral/devcon/plib_devcon.h"
#include "peripheral/osc/plib_osc.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope or Global Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Variable Definitions
// *****************************************************************************
// *****************************************************************************

/* System Device Control Instance */
SYS_DEVCON_OBJECT sysDevconObj;


// *****************************************************************************
// *****************************************************************************
// Section: SYS DEVCON Module Initialization Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ SYS_DEVCON_Initialize( const SYS_MODULE_INDEX           index,
                                       const SYS_MODULE_INIT * const    init )

  Summary:
    Initializes data for the instance of the Device Control module and opens
    the specific module instance.

  Description:
    This function initializes the instance of the Device Control module,
    using the specified initialization data. It also initializes any
    internal data structures.

  Remarks:
    This routine should only be called once during system initialization
    unless SYS_DEVCON_Deinitialize is first called to deinitialize the device
    instance before reinitializing it. If the system was already initialized
    it safely returns without causing any disturbance.
*/

SYS_MODULE_OBJ SYS_DEVCON_Initialize( const SYS_MODULE_INDEX index,
                                   const SYS_MODULE_INIT * const init )
{
    sysDevconObj.status = SYS_STATUS_READY;

    sysDevconObj.state = SYS_DEVCON_STATE_READY;

    return (SYS_MODULE_OBJ)SYS_DEVCON_INDEX_0;
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_Reinitialize( SYS_MODULE_OBJ                  object,
                               const SYS_MODULE_INIT * const   init )

   Summary:
    Reinitializes and refreshes the hardware for the instance of the Device 
    Control module.

   Description:
    This function reinitializes the instance of the Device Control module using 
    the supplied data. It modifies the internal data structure.

   Remarks:
    This operation uses the same initialization data structure as the
    SYS_DEVCON_Initialize operation. This operation can be used to change the
    power state of a DEVCON module. This function can be called multiple times
    to reinitialize the module. This operation uses the same initialization
    data structure as the Initialize operation. This operation can also be
    used to refresh the hardware registers as defined by the initialization
    data.
*/

void SYS_DEVCON_Reinitialize( SYS_MODULE_OBJ object,
                           const SYS_MODULE_INIT * const init )
{
    sysDevconObj.status = SYS_STATUS_READY;

    sysDevconObj.state = SYS_DEVCON_STATE_READY;
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_Deinitialize( SYS_MODULE_OBJ object )

  Summary:
    Deinitializes the specific module instance of the DEVCON module

  Description:
    This function deinitializes the specific module instance disabling its operation 
    (and any hardware for driver modules). Resets all of the internal data
    structures and fields for the specified instance to the default settings.

  Remarks:
    Once the Initialize operation has been called, the Deinitialize
    operation must be called before the Initialize operation can be called
    again.
*/

void SYS_DEVCON_Deinitialize( SYS_MODULE_OBJ object )
{
    sysDevconObj.status = SYS_STATUS_UNINITIALIZED;
}


// *****************************************************************************
/* Function:
    SYS_STATUS SYS_DEVCON_Status( SYS_MODULE_OBJ object )

  Summary:
    Returns status of the specific instance of the Device Control module.

  Description:
    This function returns the status of the specific module instance.

  Remarks:
    None.
*/

SYS_STATUS SYS_DEVCON_Status( SYS_MODULE_OBJ object )
{
    return sysDevconObj.status; 
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_Tasks( SYS_MODULE_OBJ object )

  Summary:
    Maintains the system Device Control state machine.

  Description:
    This function is used to maintain the system Device Control internal state machine.

  Remarks:
    This function is normally not called directly by an application.  It is
    called by the system's Tasks routine (SYS_Tasks) or by the appropriate raw
    ISR.
*/

void SYS_DEVCON_Tasks( SYS_MODULE_OBJ object )
{
    switch (sysDevconObj.state)
    {
        case SYS_DEVCON_STATE_INIT:
        case SYS_DEVCON_STATE_BUSY:
        case SYS_DEVCON_STATE_READY:
        default:
            break;
    }
}

    
// *****************************************************************************
// *****************************************************************************
// Section: SYS DEVCON Client Setup Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void SYS_DEVCON_SystemUnlock( void )

  Summary:
    Performs a system unlock sequence by writing to the SYSKEY register.

  Description:
    Performs a system unlock sequence by writing to the SYSKEY register. A 
    system unlock sequence is required before performing certain actions such
    as changing a clock frequency or IO unlocking.

  Remarks:
    None.
*/

void SYS_DEVCON_SystemUnlock( void )
{
    bool int_flag = false;

    int_flag = (bool)(PLIB_INT_GetStateAndDisable( INT_ID_0 ) & 0x01);

    PLIB_DEVCON_SystemUnlock(DEVCON_ID_0);

    if (int_flag)
    {
        PLIB_INT_Enable(INT_ID_0);
    }
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_SystemLock( void )

  Summary:
    Performs a system lock sequence by writing to the SYSKEY register.

  Description:
    Performs a system lock sequence by writing to the SYSKEY register. A 
    system lock sequence is required after performing the action that required
    a system lock sequence.

  Remarks:
    None.
*/

void SYS_DEVCON_SystemLock( void )
{
    bool int_flag = false;
   
    int_flag = (bool)(PLIB_INT_GetStateAndDisable( INT_ID_0 ) & 0x01);

    PLIB_DEVCON_SystemLock(DEVCON_ID_0);

    if (int_flag)
    {
        PLIB_INT_Enable(INT_ID_0);
    }
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_JTAGEnable( void )

  Summary:
    Enables the JTAG port on the device.

  Description:
    Enables the JTAG port on the device.

  Remarks:
    None.
*/

void SYS_DEVCON_JTAGEnable( void )
{
    PLIB_DEVCON_JTAGPortEnable(DEVCON_ID_0);
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_JTAGDisable( void )

  Summary:
    Disables the JTAG port on the device.

  Description:
    Disables the JTAG port on the device.

  Remarks:
    None.
*/

void SYS_DEVCON_JTAGDisable( void )
{
    PLIB_DEVCON_JTAGPortDisable(DEVCON_ID_0);
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_TraceEnable( void )

  Summary:
    Enables the Trace output port on the device.

  Description:
    Enables the Trace output port on the device.

  Remarks:
    None.
*/

void SYS_DEVCON_TraceEnable( void )
{
    #if defined PLIB_DEVCON_ExistsTraceOutput
        if(PLIB_DEVCON_ExistsTraceOutput(DEVCON_ID_0))
        {
            PLIB_DEVCON_TraceOutputEnable(DEVCON_ID_0);
        }
    #endif
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_TraceDisable( void )

  Summary:
    Disables the Trace output port on the device.

  Description:
    Disables the Trace output port on the device.

  Remarks:
    None.
*/

void SYS_DEVCON_TraceDisable( void )
{
    #if defined PLIB_DEVCON_ExistsTraceOutput
    if(PLIB_DEVCON_ExistsTraceOutput(DEVCON_ID_0))
    {
        PLIB_DEVCON_TraceOutputDisable(DEVCON_ID_0);
    }
    #endif
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_PowerModeEnter(SYS_POWER_MODE pwrMode)

  Summary:
    Puts the device in a low-power state.

  Description:
    This function puts the device in a low-power state.

  Remarks:
    None.
*/

void __attribute__((nomips16)) SYS_DEVCON_PowerModeEnter( SYS_POWER_MODE pwrMode )
{
    OSC_OPERATION_ON_WAIT op = SYS_POWER_MODE_IDLE;

    switch (pwrMode)
    {
        case SYS_POWER_MODE_IDLE:
            op = OSC_ON_WAIT_IDLE;
            break;
        case SYS_POWER_MODE_SLEEP:
            op = OSC_ON_WAIT_SLEEP;
            break;
        default:
            break;
    }

    
    #if defined PLIB_OSC_ExistsOnWaitAction
    if(PLIB_OSC_ExistsOnWaitAction(OSC_ID_0))
    {
        SYS_DEVCON_SystemUnlock();
        PLIB_OSC_OnWaitActionSet (OSC_ID_0, op);
        SYS_DEVCON_SystemLock();
    }
    #endif
    asm volatile ( "wait" );
}


/*******************************************************************************
 End of File
*/

