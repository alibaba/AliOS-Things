/*******************************************************************************
  Device Control System Service Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    sys_devcon.h

  Summary:
    Device Control (DEVCON) System Service interface definition.

  Description:
    This file contains the interface definition for the Device Control System
    Service.  It provides a way to interact with the Device Control subsystem
    to manage the device control requests supported by the system.
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

#ifndef _SYS_DEVCON_H
#define _SYS_DEVCON_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "system/system.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: SYS DEVCON Data Types
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* SYS Device Control Module Index Numbers

  Summary:
    Device Control System Service index definitions.

  Description:
    These constants provide Device Control System Service index definitions.

  Remarks:
    These constants should be used in place of hard-coded numeric literals.

*/

#define SYS_DEVCON_INDEX_0                 0


//************************************************************************
/*
  Summary:
    Identifies a particular registered event instance.

  Description:
    SYS DEVCON Handle.
    This event handle identifies a registered instance of an event. Every
    time the application that tries to access the parameters with respect
    to a particular event, shall used this event handle to refer to that
    event.

  Remarks:
    None.
*/

typedef int8_t SYS_DEVCON_HANDLE;


// *****************************************************************************
/* SYS DEVCON Initialize structure

  Summary:
    Identifies the system device control initialize structure.

  Description:
    This structure identifies the system device control initialize structure.

  Remarks:
    None.
*/

typedef struct
{
    /* System module initialization */
    SYS_MODULE_INIT                 moduleInit;

} SYS_DEVCON_INIT;


// *****************************************************************************
/* SYS_CACHE_COHERENCY enumeration

   Summary:
    System L1 cache coherency settings.

   Description:
    This enumeration defines the supported system L1 cache coherency settings.

   Remarks:
    Used to read or write cache coherency policy for kseg0.
*/

typedef enum
{
    SYS_CACHE_WRITETHROUGH_NO_WRITEALLOCATE = 0,

    SYS_CACHE_WRITETHROUGH_WRITEALLOCATE = 1,

    SYS_CACHE_DISABLE = 2,

    SYS_CACHE_WRITEBACK_WRITEALLOCATE = 3

} SYS_CACHE_COHERENCY;


// *****************************************************************************
/* SYS_POWER_MODE enumeration

   Summary:
    System power mode settings.

   Description:
    This enumeration defines the supported system power mode settings.

   Remarks:
    None.
*/

typedef enum
{
    SYS_POWER_MODE_IDLE,

    SYS_POWER_MODE_SLEEP

} SYS_POWER_MODE;

// *****************************************************************************
// *****************************************************************************
// Section: SYS DEVCON Module Initialization Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ SYS_DEVCON_Initialize( const SYS_MODULE_INDEX index,
                                       const SYS_MODULE_INIT * const init )

  Summary:
    Initializes data for the instance of the Device Control module and opens
    the specific module instance.

  Description:
    This function initializes the instance of the Device Control module,
    using the specified initialization data. It also initializes any
    internal data structures.

  Precondition:
    None.

  Parameters:
    index           - Index for the instance to be initialized
    init            - Pointer to a data structure containing any data necessary
                      to initialize the Device Control module. This pointer may
                      be null if no data is required because static overrides
                      have been provided.

  Returns:
    If successful, returns a valid handle to an object.  Otherwise, it
    returns SYS_MODULE_OBJ_INVALID. The returned object must be passed as
    argument to SYS_DEVCON_Reinitialize, SYS_DEVCON_Deinitialize, SYS_DEVCON_Tasks and
    SYS_DEVCON_Status routines.

  Example:
    <code>
    SYS_MODULE_OBJ  objectHandle;
    SYS_DEVCON_INIT    initConfig;

    // Populate the DEVCON initialization structure
    initConfig.moduleInit.value     = SYS_MODULE_POWER_RUN_FULL;

    objectHandle = SYS_DEVCON_Initialize (SYS_DEVCON_INDEX_0, (SYS_MODULE_INIT*)&initConfig);
    if (SYS_MODULE_OBJ_INVALID == objectHandle)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine should only be called once during system initialization
    unless SYS_DEVCON_Deinitialize is first called to deinitialize the device
    instance before reinitializing it. If the system was already initialized
    it safely returns without causing any disturbance.
*/

SYS_MODULE_OBJ SYS_DEVCON_Initialize( const SYS_MODULE_INDEX index,
                                   const SYS_MODULE_INIT * const init );


// *****************************************************************************
/* Function:
    void SYS_DEVCON_Reinitialize( SYS_MODULE_OBJ object,
                               const SYS_MODULE_INIT * const init )

   Summary:
    Reinitializes and refreshes the hardware for the instance of the Device
    Control module.

   Description:
    This function reinitializes the instance of the Device Control module using
    the supplied data. It modifies the internal data structure.

  Precondition:
    The SYS_DEVCON_Initialize function should have been called before calling this
    function.

  Parameters:
    object          - Identifies the SYS DEVCON Object returned by the Initialize
                      interface
    init            - Pointer to the data structure containing any data
                      necessary to initialize the hardware

   Returns:
    None

   Example:
    <code>
    SYS_MODULE_OBJ  objectHandle;
    SYS_DEVCON_INIT    initConfig;
    SYS_STATUS      devconStatus;

    // Populate the device control initialization structure
    initConfig.moduleInit.value     = SYS_MODULE_POWER_RUN_FULL;

    SYS_DEVCON_Reinitialize (objectHandle, (SYS_MODULE_INIT*)&initConfig);

    devconStatus = SYS_DEVCON_Status (object);
    if (SYS_STATUS_ERROR >= devconStatus)
    {
        // Handle error
    }
    </code>

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
                           const SYS_MODULE_INIT * const init );


// *****************************************************************************
/* Function:
    void SYS_DEVCON_Deinitialize( SYS_MODULE_OBJ object )

  Summary:
    Deinitializes the specific module instance of the DEVCON module

  Description:
    This function deinitializes the specific module instance disabling its operation
    (and any hardware for driver modules). Resets all of the internal data
    structures and fields for the specified instance to the default settings.

  Precondition:
    The SYS_DEVCON_Initialize function should have been called before calling this
    function.

  Parameters:
    object          - SYS DEVCON object handle, returned from SYS_DEVCON_Initialize

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     //  Returned from SYS_DEVCON_Initialize
    SYS_STATUS          status;

    SYS_DEVCON_Deinitialize (object);

    status = SYS_DEVCON_Status (object);
    if (SYS_MODULE_DEINITIALIZED == status)
    {
        // Check again later if you need to know
        // when the SYS DEVCON is deinitialized.
    }
    </code>

  Remarks:
    Once the Initialize operation has been called, the Deinitialize
    operation must be called before the Initialize operation can be called
    again.
*/

void SYS_DEVCON_Deinitialize( SYS_MODULE_OBJ object );


// *******************************************************************************
/* Function:
    SYS_STATUS SYS_DEVCON_Status( SYS_MODULE_OBJ object )

  Summary:
    Returns status of the specific instance of the Device Control module.

  Description:
    This function returns the status of the specific module instance
    disabling its operation (and any hardware for driver modules).

  Conditions:
    The SYS_DEVCON_Initialize function should have been called before
    calling this function.

  Input:
    object -  SYS DEVCON object handle, returned from SYS_DEVCON_Initialize

  Return:
    - SYS_STATUS_READY - Indicates that any previous operations have
	                     succeeded and the module is ready for additional operations.
						 Any value greater than SYS_STATUS_READY is also a normal
						 running state in which the driver is ready to accept new operations.
	- SYS_STATUS_BUSY  - Indicates that the driver is busy with a previous system
                         level operation and cannot start another.
    - SYS_STATUS_ERROR - Indicates that the driver is in an error state. Any value
	                     less than SYS_STATUS_ERROR is also an error state.
    - SYS_MODULE_DEINITIALIZED - Indicates that the driver has been deinitialized.
	                             This value is less than SYS_STATUS_ERROR. Once the
								 Initialize operation has been called, the Deinitialize
								 operation must be called before the Initialize operation
								 can be called again.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from SYS_DEVCON_Initialize
    SYS_STATUS          tmrStatus;

    devconStatus = SYS_DEVCON_Status (object);
    else if (SYS_STATUS_ERROR >= devconStatus)
    {
        // Handle error
    }
    </code>

  Remarks:
    None.
*/

SYS_STATUS SYS_DEVCON_Status( SYS_MODULE_OBJ object );


// **************************************************************************
/*  Function:
    void SYS_DEVCON_Tasks( SYS_MODULE_OBJ object )

  Summary:
    Maintains the system Device Control state machine.

  Description:
    This function is used to maintain the system Device Control internal
    state machine.

  Preconditions:
    The SYS_DEVCON_Initialize function must have been called for the
    specified DEVCON driver instance.

  Parameters:
    object -  SYS DEVCON object handle, returned from SYS_DEVCON_Initialize

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from SYS_DEVCON_Initialize

    while (true)
    {
        SYS_DEVCON_Tasks (object);

        // Do other tasks
    }
    </code>

  Remarks:
    This function is normally not called directly by an application. It is
    called by the system's Tasks routine (SYS_Tasks) or by the appropriate
    raw ISR.
*/

void SYS_DEVCON_Tasks( SYS_MODULE_OBJ object );


// *****************************************************************************
// *****************************************************************************
// Section: SYS DEVCON Client Setup Routines
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Function:
    void SYS_DEVCON_PerformanceConfig( unsigned int sysclk )

  Summary:
    Configures the PFM wait states and Prefetch Cache module for maximum
    performance.

  Description:
    This function configures the PFM wait states and Prefetch Cache module
    for maximum performance.

  Precondition:
    The SYS_DEVCON_Initialize function should have been called before calling this
    function.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_PerformanceConfig(SYS_FREQUENCY);
    </code>

  Remarks:
    None.
*/

void __attribute__((nomips16)) SYS_DEVCON_PerformanceConfig( unsigned int sysclk );

// *****************************************************************************
/* Function:
    void SYS_DEVCON_SystemUnlock( void )

  Summary:
    Performs a system unlock sequence by writing to the SYSKEY register.

  Description:
    Performs a system unlock sequence by writing to the SYSKEY register. A
    system unlock sequence is required before performing certain actions such
    as changing a clock frequency or I/O unlocking.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_SystemUnlock();
    // Perform some action
    SYS_DEVCON_SystemLock();
    </code>

  Remarks:
    None.
*/

void SYS_DEVCON_SystemUnlock( void );


// *****************************************************************************
/* Function:
    void SYS_DEVCON_SystemLock( void )

  Summary:
    Performs a system lock sequence by writing to the SYSKEY register.

  Description:
    Performs a system lock sequence by writing to the SYSKEY register. A
    system lock sequence is required after performing the action that required
    a system lock sequence.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_SystemUnlock();
    // Perform some action
    SYS_DEVCON_SystemLock();
    </code>

  Remarks:
    None.
*/

void SYS_DEVCON_SystemLock( void );


// *****************************************************************************
/* Function:
    void SYS_DEVCON_JTAGEnable( void )

  Summary:
    Enables the JTAG port on the device.

  Description:
    This function enables the JTAG port on the device.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_JTAGEnable();
    </code>

  Remarks:
    None.
*/

void SYS_DEVCON_JTAGEnable( void );


// *****************************************************************************
/* Function:
    void SYS_DEVCON_JTAGDisable( void )

  Summary:
    Disables the JTAG port on the device.

  Description:
    This function disables the JTAG port on the device.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_JTAGDisable();
    </code>

  Remarks:
    None.
*/

void SYS_DEVCON_JTAGDisable( void );


// *****************************************************************************
/* Function:
    void SYS_DEVCON_TraceEnable( void )

  Summary:
    Enables the trace output port on the device.

  Description:
    This function enables the trace output port on the device.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_TraceEnable();
    </code>

  Remarks:
    None.
*/

void SYS_DEVCON_TraceEnable( void );


// *****************************************************************************
/* Function:
    void SYS_DEVCON_TraceDisable( void )

  Summary:
    Disables the trace output port on the device.

  Description:
    This function disables the trace output port on the device.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_TraceDisable();
    </code>

  Remarks:
    None.
*/

void SYS_DEVCON_TraceDisable( void );


// *****************************************************************************
/* Function:
    void SYS_DEVCON_CacheInit( SYS_CACHE_COHERENCY cacheCoherency )

  Summary:
    Initializes the L1 cache.

  Description:
    This function initializes both instruction and data caches. Invalidates all entries
	and zeros all tags. Sets coherency attribute for kseg0.

  Precondition:
    None.

  Parameters:
    cacheCoherency  -   The desired kseg0 coherency attribute.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_CacheInit(SYS_CACHE_WRITEBACK_WRITEALLOCATE);
    </code>

  Remarks:
    Use with caution. Invalidates all cache lines without writing data
    back to memory. Any dirty data in the cache will be lost.
*/

void SYS_DEVCON_CacheInit(SYS_CACHE_COHERENCY cacheCoherency);


// *****************************************************************************
/* Function:
    void SYS_DEVCON_CacheFlush( void )

  Summary:
    Flushes the L1 cache.

  Description:
    This function flushes both instruction and data caches. Invalidate entire
	 instruction cache; writes back and invalidates the entire data cache.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_CacheFlush();
    </code>

  Remarks:
    Simplest way to synchronize caches with memory, but not necessarily
    the most efficient.
*/

void SYS_DEVCON_CacheFlush(void);


// *****************************************************************************
/* Function:
    void SYS_DEVCON_DataCacheFlush( void )

  Summary:
    Flushes the L1 data cache.

  Description:
    This function writes back and invalidates the entire data cache.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_DataCacheFlush();
    </code>

  Remarks:
    None.
*/

void SYS_DEVCON_DataCacheFlush(void);


// *****************************************************************************
/* Function:
    void SYS_DEVCON_InstructionCacheFlush( void )

  Summary:
    Flushes (invalidates) the L1 instruction cache.

  Description:
    This function invalidates the entire instruction cache.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_InstructionCacheFlush();
    </code>

  Remarks:
    None.
*/

void SYS_DEVCON_InstructionCacheFlush(void);


// *****************************************************************************
/* Function:
    void SYS_DEVCON_CacheClean( uint32_t addr, size_t len )

  Summary:
    Writes back and invalidates an address range in either cache.

  Description:
    This function writes back (data) and invalidates (data and address) an address
	range in either cache.

  Precondition:
    None.

  Parameters:
    addr    -   Starting address of data block.
    len     -   Size of data block.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_CacheClean(&myData, sizeof(myData));
    </code>

  Remarks:
    None.
*/

void SYS_DEVCON_CacheClean(uint32_t addr, size_t len);


// *****************************************************************************
/* Function:
    void SYS_DEVCON_DataCacheClean( uint32_t addr, size_t len )

  Summary:
    Writes back and invalidates an address range in the data cache.

  Description:
    This function writes back and invalidates an address range in the data cache.

  Precondition:
    None.

  Parameters:
    addr    -   Starting address of data block.
    len     -   Size of data block.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_DataCacheClean(&myData, sizeof(myData));
    </code>

  Remarks:
    None.
*/

void SYS_DEVCON_DataCacheClean(uint32_t addr, size_t len);


// *****************************************************************************
/* Function:
    void SYS_DEVCON_DataCacheInvalidate( uint32_t addr, size_t len )

  Summary:
    Invalidates an address range in the data cache.

  Description:
    This function invalidates an address range in the data cache.

  Precondition:
    None.

  Parameters:
    addr    -   Starting address of data block.
    len     -   Size of data block.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_DataCacheInvalidate(&myData, sizeof(myData));
    </code>

  Remarks:
    Use caution. Any dirty data in the cache will be lost.
*/
void SYS_DEVCON_DataCacheInvalidate(uint32_t addr, size_t len);


// *****************************************************************************
/* Function:
    void SYS_DEVCON_InstructionCacheInvalidate( uint32_t addr, size_t len )

  Summary:
    Invalidates an address range in the instruction cache.

  Description:
    This function invalidates an address range in the instruction cache.

  Precondition:
    None.

  Parameters:
    addr    -   Starting address of code block.
    len     -   Size of code block.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_InstructionCacheInvalidate(&myCode, myCodeSize);
    </code>

  Remarks:
    None.
*/

void SYS_DEVCON_InstructionCacheInvalidate(uint32_t addr, size_t len);


// *****************************************************************************
/* Function:
    void SYS_DEVCON_InstructionCacheLock( uint32_t addr, size_t len )

  Summary:
    Fetches and locks a block of instructions in the instruction cache.

  Description:
    This function fetches and locks a block of instructions in the instruction cache.

  Precondition:
    None.

  Parameters:
    addr    -   Starting address of code block.
    len     -   Size of code block.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_InstructionCacheLock(&myCode, myCodeSize);
    </code>

  Remarks:
    Global flush functions will invalidate and unlock any cache lines
    locked with this function.
*/

void SYS_DEVCON_InstructionCacheLock(uint32_t addr, size_t len);


// *****************************************************************************
/* Function:
    void SYS_DEVCON_DataCacheLock( uint32_t addr, size_t len )

  Summary:
    Fetches and locks a block of data in the data cache.

  Description:
    This function fetches and locks a block of data in the data cache.

  Precondition:
    None.

  Parameters:
    addr    -   Starting address of data block.
    len     -   Size of data block.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_DataCacheLock(&myData, sizeof(myData));
    </code>

  Remarks:
    Global flush functions will invalidate and unlock any cache lines
    locked with this function.
*/

void SYS_DEVCON_DataCacheLock(uint32_t addr, size_t len);


// *****************************************************************************
/* Function:
    void SYS_DEVCON_CacheSync( uint32_t addr, size_t len )

  Summary:
    Synchronizes the instruction and data caches.

  Description:
    This function synchronizes the instruction and data caches. Used when modifying
    the instruction stream (breakpoints, self-modifying code, relocating
    executable code to RAM). Flushes an address range from the data cache
    and invalidates that same range from the instruction cache.

  Precondition:
    None.

  Parameters:
    addr    -   Starting address of code block.
    len     -   Size of code block.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_CacheSync(&myCode, myCodeSize);
    </code>

  Remarks:
    None.
*/
void SYS_DEVCON_CacheSync(uint32_t addr, size_t len);


// *****************************************************************************
/* Function:
    void SYS_DEVCON_CacheCoherencySet( SYS_CACHE_COHERENCY cacheCoherency )

  Summary:
    Sets the cache coherency attribute for kseg0.

  Description:
    This function sets the cache coherency attribute for kseg0.

  Precondition:
    None.

  Parameters:
    cacheCoherency  -   The desired coherency attribute.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_CacheCoherencySet(SYS_CACHE_WRITEBACK_WRITEALLOCATE);
    </code>

  Remarks:
    Use with caution. May cause unpredictable behavior in a running
    system.
*/

void SYS_DEVCON_CacheCoherencySet(SYS_CACHE_COHERENCY cacheCoherency);


// *****************************************************************************
/* Function:
    SYS_CACHE_COHERENCY SYS_DEVCON_CacheCoherencyGet( void )

  Summary:
    Returns the current cache coherency attribute for kseg0.

  Description:
    This function returns the current cache coherency attribute for kseg0.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    The current cache coherency attribute for kseg0.

  Example:
    <code>
    SYS_CACHE_COHERENCY cacheCoherency;
    cacheCoherency = SYS_DEVCON_CacheCoherencyGet();
    </code>

  Remarks:
    None.
*/

SYS_CACHE_COHERENCY SYS_DEVCON_CacheCoherencyGet(void);


// *****************************************************************************
/* Function:
    size_t SYS_DEVCON_DataCacheAssociativityGet(void)

  Summary:
    Returns the number of ways in the data cache.

  Description:
    This function returns the number of ways in the data cache.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    The number of ways in the data cache.

  Example:
    <code>
    unsigned int size;
    size = SYS_DEVCON_DataCacheAssociativityGet();
    </code>

  Remarks:
    None.
*/

size_t SYS_DEVCON_DataCacheAssociativityGet(void);


// *****************************************************************************
/* Function:
    size_t SYS_DEVCON_InstructionCacheAssociativityGet(void)

  Summary:
    Returns the number of ways in the instruction cache.

  Description:
    This function returns the number of ways in the instruction cache.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    The number of ways in the instruction cache.

  Example:
    <code>
    unsigned int size;
    size = SYS_DEVCON_InstructionCacheAssociativityGet();
    </code>

  Remarks:
    None.
*/

size_t SYS_DEVCON_InstructionCacheAssociativityGet(void);


// *****************************************************************************
/* Function:
    size_t SYS_DEVCON_DataCacheLineSizeGet(void)

  Summary:
    Returns the data cache line size.

  Description:
    This function returns the data cache line size.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    The number of bytes per line in the data cache.

  Example:
    <code>
    unsigned int size;
    size = SYS_DEVCON_DataCacheLineSizeGet();
    </code>

  Remarks:
    None.
*/

size_t SYS_DEVCON_DataCacheLineSizeGet(void);


// *****************************************************************************
/* Function:
    size_t SYS_DEVCON_InstructionCacheLineSizeGet(void)

  Summary:
    Returns the instruction cache line size.

  Description:
    This function returns the instruction cache line size.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    The number of bytes per line in the instruction cache.

  Example:
    <code>
    unsigned int size;
    size = SYS_DEVCON_InstructionCacheLineSizeGet();
    </code>

  Remarks:
    None.
*/

size_t SYS_DEVCON_InstructionCacheLineSizeGet(void);


// *****************************************************************************
/* Function:
    size_t SYS_DEVCON_DataCacheLinesPerWayGet(void)

  Summary:
    Returns the number of lines per way in the data cache.

  Description:
    This function returns the number of lines per way in the data cache.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    The number of lines per way in the data cache.

  Example:
    <code>
    unsigned int size;
    size = SYS_DEVCON_DataCacheLinesPerWayGet();
    </code>

  Remarks:
    None.
*/

size_t SYS_DEVCON_DataCacheLinesPerWayGet(void);


// *****************************************************************************
/* Function:
    size_t SYS_DEVCON_InstructionCacheLinesPerWayGet(void)

  Summary:
    Returns the number of lines per way in the instruction cache.

  Description:
    This function returns the number of lines per way in the instruction cache.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    The number of lines per way in the instruction cache.

  Example:
    <code>
    unsigned int size;
    size = SYS_DEVCON_InstructionCacheLinesPerWayGet();
    </code>

  Remarks:
    None.
*/

size_t SYS_DEVCON_InstructionCacheLinesPerWayGet(void);


// *****************************************************************************
/* Function:
    size_t SYS_DEVCON_DataCacheSizeGet(void)

  Summary:
    Returns the total number of bytes in the data cache.

  Description:
    This function returns the total number of bytes in the data cache.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    The total number of bytes in the data cache.

  Example:
    <code>
    unsigned int size;
    size = SYS_DEVCON_DataCacheSizeGet();
    </code>

  Remarks:
    None.
*/

size_t SYS_DEVCON_DataCacheSizeGet(void);


// *****************************************************************************
/* Function:
    size_t SYS_DEVCON_InstructionCacheSizeGet(void)

  Summary:
    Returns the total number of bytes in the instruction cache.

  Description:
    This function returns the total number of bytes in the instruction cache.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    The total number of bytes in the instruction cache.

  Example:
    <code>
    unsigned int size;
    size = SYS_DEVCON_InstructionCacheSizeGet();
    </code>

  Remarks:
    None.
*/

size_t SYS_DEVCON_InstructionCacheSizeGet(void);


// *****************************************************************************
/* Function:
    void SYS_DEVCON_PowerModeEnter(SYS_POWER_MODE pwrMode)

  Summary:
    Puts the device in a low-power state.

  Description:
    This function places the device in a low-power state.

  Precondition:
    None.

  Parameters:
    pwrMode     - The low power state to enter.

  Returns:
    None.

  Example:
    <code>
    SYS_DEVCON_PowerModeEnter(SYS_POWER_MODE_SLEEP);
    </code>

  Remarks:
    None.
*/
void __attribute__((nomips16)) SYS_DEVCON_PowerModeEnter(SYS_POWER_MODE pwrMode);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif //_SYS_DEVCON_H

/*******************************************************************************
 End of File
*/
