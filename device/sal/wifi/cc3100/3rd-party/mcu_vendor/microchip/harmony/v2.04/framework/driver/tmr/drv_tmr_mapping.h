/*******************************************************************************
  Timer Device Driver Interface Names Mapping

  Company:
    Microchip Technology Inc.

  File Name:
    drv_tmr_mapping.h

  Summary:
    Timer device driver interface names mapping.

  Description:
    This header file contains the data type definitions.
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

#ifndef _DRV_TMR_MAPPING_H
#define _DRV_TMR_MAPPING_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
/* Note:  See the bottom of file for implementation header include files.
*/

#include <stdint.h>
#include <stdbool.h>

#include "driver/tmr/drv_tmr_static.h"


// *****************************************************************************
// *****************************************************************************
// Section: Build Parameter Checking
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Macro: DRV_TMR_INSTANCES_NUMBER Check

  Summary:
    Checks the DRV_TMR_INSTANCES_NUMBER definition.

  Description:
    If DRV_TMR_INSTANCES_NUMBER is greater than the number of the Timer module
    instances available on the device, an error is generated.

  Remarks:
    _TMR_EXISTS is a processor-specific value defined by the processor
    headers in the peripheral library.
    
    If the configuration does not specify the number of driver instances to 
    allocate it defaults to the number of Timer module instances on the device.
*/

#if defined(DRV_TMR_INSTANCES_NUMBER)

    #if (DR_TMR_DRIVER_OBJECTS_NUMBER > _TMR_EXISTS )

        #error "The number of TMR instances configured is more than the available TMRs on the part"

    #endif

#endif



// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Static API Name Generation
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Macro: _DRV_TMR_MAKE_NAME(name)

  Summary:
    Creates an instance-specific static interface name.

  Description:
     This macro creates the instance-specific name of the specified static interface
     routine by inserting the index number into the name.
     
     Example: DRV_TMR_Initialize becomes DRV_TMR1_Initialize for an index of 1.
     
  Remarks:
    Multi-client configurations add the word "multi" to the API name, whereas 
    single-client configurations do not.
    
    Example: DRV_TMR_Initialize becomes DRV_TMR1multi_Initialize for an index
    of 1.
*/


#if !defined(DRV_TMR_INSTANCES_NUMBER)

    // Static builds use static naming to reduce calling overhead. 
    #if !defined( DRV_TMR_CLIENTS_NUMBER)
    
        // Static Single-Client Interface Name Generation
        #define _DRV_TMR_STATIC_API_SINGLE(index, name)     DRV_TMR ## index ## _ ## name
    
    #else // ( DRV_TMR_CLIENTS_NUMBER == 1 )

        #error "The TMR driver does not support multi-client static configurations"

    #endif

    // Static naming Macros
    #define _DRV_TMR_STATIC_API(index, name)                _DRV_TMR_STATIC_API_SINGLE(index, name)
    #define _DRV_TMR_MAKE_NAME(name)                        _DRV_TMR_STATIC_API(DRV_TMR_INDEX, name)
        
#else // (DRV_TMR_CONFIG_BUILD_TYPE == DRV_BUILD_TYPE_DYNAMIC)

        // Dynamic builds use dynamic naming to add the correct prefix to the operation.
        #define _DRV_TMR_MAKE_NAME(name)                    DRV_TMR_ ## name
    
#endif


// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Static API Mapping
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Static Interface Mapping

  Summary:
    Maps the dynamic interface calls to appropriate static interface.

  Description:
    These macros map calls to the dynamic interface routines to calls to the 
    appropriate instance-specific static interface routine when a static build
    (DRV_TMR_INSTANCES_NUMBER is not defined) is configured.
    
    Example: DRV_TMR_Status(DRV_TMR_INDEX_1);
        
    Becomes: DRV_TMR_Status();
        
  Remarks:
    Static configuration eliminates the need to pass the object parameter.
    However, the index is "returned" as the object handle (from the 
    "Initialize" routine) to allow code written for the dynamic API to continue
    to build when using a static configuration.
    
    Example:
    
        object = DRV_TMR_Initialize(DRV_TMR_INDEX_1, &initData);
        
    Becomes:
    
        object = ( DRV_TMR1_Initialize(&initData), DRV_TMR_INDEX );
        
    Static single-client configurations also eliminate the client handle 
    parameter.  However, the index (the driver-object index) is "returned" from
    the "Open" routine to allow code written for multi-client drivers to build
    when using a static single-open configuration.
    
    Example: handle = DRV_TMR_Open(DRV_TMR_INDEX_1, intent);
        
    Becomes: handle = ( DRV_TMR_Open(intent), DRV_TMR_INDEX );
*/

#if !defined(DRV_TMR_INSTANCES_NUMBER) // static

    #define DRV_TMR_Initialize(sysID, inData)           (_DRV_TMR_MAKE_NAME(Initialize)(inData), DRV_TMR_INDEX)

    #define DRV_TMR_Reinitialize(sysObj, inData)        _DRV_TMR_MAKE_NAME(Reinitialize)(inData)

    #define DRV_TMR_Deinitialize(sysObj)                _DRV_TMR_MAKE_NAME(Deinitialize)()

    #define DRV_TMR_Status(sysObj)                      _DRV_TMR_MAKE_NAME(Status)()

    #define DRV_TMR_Tasks(sysObj)                       _DRV_TMR_MAKE_NAME(Tasks)()

    #define DRV_TMR_Open(sysID, intent)                 (_DRV_TMR_MAKE_NAME(Open)(intent), DRV_TMR_INDEX)

    #define DRV_TMR_Close(handle)                       _DRV_TMR_MAKE_NAME(Close)()

    #define DRV_TMR_ClientStatus(handle)                _DRV_TMR_MAKE_NAME(ClientStatus)()

    #define DRV_TMR_Period8BitSet(handle,value)         _DRV_TMR_MAKE_NAME(Period8BitSet)(value)

    #define DRV_TMR_Period8BitGet(handle)               _DRV_TMR_MAKE_NAME(Period8BitGet)()

    #define DRV_TMR_Counter8BitSet(handle,value)        _DRV_TMR_MAKE_NAME(Counter8BitSet)(value)

    #define DRV_TMR_Counter8BitGet(handle)              _DRV_TMR_MAKE_NAME(Counter8BitGet)()

    #define DRV_TMR_Period16BitSet(handle,value)        _DRV_TMR_MAKE_NAME(Period16BitSet)(value)

    #define DRV_TMR_Period16BitGet(handle)              _DRV_TMR_MAKE_NAME(Period16BitGet)()

    #define DRV_TMR_Counter16BitSet(handle,value)       _DRV_TMR_MAKE_NAME(Counter16BitSet)(value)

    #define DRV_TMR_Counter16BitGet(handle)             _DRV_TMR_MAKE_NAME(Counter16BitGet)()

    #define DRV_TMR_Period32BitSet(handle,value)        _DRV_TMR_MAKE_NAME(Period32BitSet)(value)

    #define DRV_TMR_Period32BitGet(handle)              _DRV_TMR_MAKE_NAME(Period32BitGet)()

    #define DRV_TMR_Counter32BitSet(handle,value)       _DRV_TMR_MAKE_NAME(Counter32BitSet)(value)

    #define DRV_TMR_Counter32BitGet(handle)             _DRV_TMR_MAKE_NAME(Counter32BitGet)()

    #define DRV_TMR_SyncModeSet(handle, mode)           _DRV_TMR_MAKE_NAME(SyncModeSet)(mode)

    #define DRV_TMR_SyncModeGet(handle)                 _DRV_TMR_MAKE_NAME(SyncModeGet)()

    #define DRV_TMR_Start(handle)                       _DRV_TMR_MAKE_NAME(Start)()

    #define DRV_TMR_Stop(handle)                        _DRV_TMR_MAKE_NAME(Stop)()

    #define DRV_TMR_OperatingFrequencyGet(handle)       _DRV_TMR_MAKE_NAME(OperatingFrequencyGet)()

    #define DRV_TMR_AlarmSet(handle, conf)              _DRV_TMR_MAKE_NAME(AlarmSet)(conf)

    #define DRV_TMR_AlarmCountGet(handle)               _DRV_TMR_MAKE_NAME(AlarmCountGet)()

    #define DRV_TMR_AlarmCountClear(handle)             _DRV_TMR_MAKE_NAME(AlarmCountClear)()

    #define DRV_TMR_TickFrequencyGet(handle, freq)      _DRV_TMR_MAKE_NAME(TickFrequencyGet)(freq)

    #define DRV_TMR_ElapsedStatusGetAndClear(handle)    _DRV_TMR_MAKE_NAME(ElapsedStatusGetAndClear)()

#else // Dynamic Build

    // No Change in the naming convention

#endif


#endif // #ifndef _DRV_TMR_MAPPING_H

/*******************************************************************************
 End of File
*/

