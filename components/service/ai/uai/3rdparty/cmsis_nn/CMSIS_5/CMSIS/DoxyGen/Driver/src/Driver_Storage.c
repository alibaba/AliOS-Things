/**
\defgroup storage_interface_gr Storage Interface
\brief    Driver API for Storage Device Interface (%Driver_Storage.h)
\details
This is an abstraction for a storage controller. It offers an interface to
access an address space of storage locations, comprising APIs for
initialization, erase, access, program, and status-fetch operations. It also
offers APIs to iterate over the available Storage Blocks (\ref
ARM_STORAGE_BLOCK), allowing the discovery of block attributes such as
write/erase granularities. Using the Storage abstraction, it becomes possible to
write generic algorithms, such as block copy, to operate on any conforming
storage device.

\note The storage abstraction layer is not responsible for storage management.
Algorithms such as block-allocation, wear-leveling, erase-before-write and other
storage-management policies are the responsibility of modules external to the
storage abstraction layer. In essence, the storage interface is the lowest
abstraction upon which block management policies can be implemented.

Here's a picture to help locate the storage abstraction in the software stack.
The part below the box labeled 'Storage abstraction layer' is implemented by a
storage driver.

\image html storage_sw_stack.png

<b>Storage API</b>

The following header files define the Application Programming Interface (API) for the Flash interface:
  - \b %Driver_Storage.h : Driver API for Storage Device Interface


<b>Driver Functions</b>

The driver functions are published in the access struct as explained in \ref StorageDriverFunctions
  - \ref ARM_DRIVER_STORAGE : access struct for Storage driver functions

A sample use for the driver can be found at: \ref SampleUseOfStorageDriver
*******************************************************************************************************************/



/**
\addtogroup storage_interface_gr
@{
*******************************************************************************************************************/

/**
\struct     ARM_STORAGE_BLOCK_ATTRIBUTES
<b>Contained in:</b>
  - \ref ARM_STORAGE_BLOCK
*******************************************************************************************************************/

/**
\struct     ARM_STORAGE_BLOCK
\details Storage blocks combine to make up the address map of a storage controller.
*******************************************************************************************************************/

/**
\struct     ARM_STORAGE_INFO
\details
It describes the characteristics of a Storage device. This includes total
storage, programming size, a default value for erased memory etc. This
information can be obtained from the Storage device datasheet and is used by the
middleware in order to properly interact with the Storage device.

Total available storage (in bytes) is contained in \em total_storage. Minimum
programming size (in bytes) is described by \em program_unit (applicable only if
the \em programmable attribute is set for a block). It defines the granularity
for programming data. The offset of the start of a program-range and the size
should also be aligned with \em program_unit.
\note: setting \em program_unit to 0 has the effect of disabling the size and
alignment restrictions (setting it to 1 also has the same effect).

Optimal programming page-size (in bytes) is specified by \em
optimal_program_unit. Some storage controllers have internal buffers into which
to receive data. Writing in chunks of \em optimal_program_unit would achieve
maximum programming speed. Like with \em program_unit, this is applicable only
if the \em programmable attribute is set for the underlying storage block(s).

\em program_cycles is a measure of endurance for reprogramming.
A value of \em ARM_STORAGE_PROGRAM_CYCLES_INFINITE may be used to signify
infinite or unknown endurance.

Contents of erased memory is specified by the \em erased_value. It is usually
\token{1} to indicate erased bytes with state 0xFF.

\em memory_mapped can be set to \token{1} to indicate that the storage device
has a mapping onto the processor's memory address space.
\note: For a memory-mapped block which isn't erasable but is programmable,
writes should be possible directly to the memory-mapped storage without going
through the \ref ARM_Storage_ProgramData operation.

The field \em programmability holds a value to indicate storage programmability.
Similarly, \em retention_level holds a for encoding data-retention levels for
all storage blocks.

\note
These fields serve a different purpose than the ones contained in
\ref ARM_STORAGE_CAPABILITIES, which is another structure containing device-level
metadata. ARM_STORAGE_CAPABILITIES describes the API capabilities, whereas
ARM_STORAGE_INFO describes the device. Furthermore ARM_STORAGE_CAPABILITIES fits
within a single word, and is designed to be passed around by value;
ARM_STORAGE_INFO, on the other hand, contains metadata which doesn't fit into a
single word and requires the use of pointers to be moved around.

<b>Returned by:</b>
  - \ref ARM_Storage_GetInfo
*******************************************************************************************************************/

/**
\struct ARM_DRIVER_STORAGE
\details
This is the set of operations constituting the Storage driver. Their
implementation is platform-specific, and needs to be supplied by the porting
effort. The functions of the Storage driver are accessed by function pointers
exposed by this structure. Refer to \ref StorageDriverFunctions for overview
information.

Each instance of a Storage interface provides such an access structure.
The instance is identified by a postfix number in the symbol name of the access structure, for example:
 - \b Driver_Storage0 is the name of the access struct of the first instance (no. 0).
 - \b Driver_Storage1 is the name of the access struct of the second instance (no. 1).

A middleware configuration setting allows connecting the middleware to a specific driver instance \b %Driver_Flash<i>n</i>.
The default is \token{0}, which connects a middleware to the first instance of a driver.
*******************************************************************************************************************/

/**
\defgroup StorageDriverFunctions Use of Storage APIs

Function pointers within \ref ARM_DRIVER_STORAGE form the set of operations
constituting the Storage driver. Their implementation is platform-specific, and
needs to be supplied by the porting effort.

Some of these APIs will always operate synchronously:
- \ref ARM_Storage_GetVersion
- \ref ARM_Storage_GetCapabilities
- \ref ARM_Storage_GetStatus
- \ref ARM_Storage_GetInfo
- \ref ARM_Storage_ResolveAddress
- \ref ARM_Storage_GetNextBlock and
- \ref ARM_Storage_GetBlock.

This means that control returns to the caller with a relevant status code only after the completion of the operation (or
the discovery of a failure condition).

The remainder of the APIs:
- \ref ARM_Storage_Initialize
- \ref ARM_Storage_Uninitialize
- \ref ARM_Storage_PowerControl
- \ref ARM_Storage_ReadData
- \ref ARM_Storage_ProgramData
- \ref ARM_Storage_Erase and
- \ref ARM_Storage_EraseAll

can function asynchronously if the underlying controller supports it; that is if ARM_STORAGE_CAPABILITIES::asynchronous_ops
is set. In the case of asynchronous operation, the invocation returns early (with ARM_DRIVER_OK) and results in a completion
callback later. If ARM_STORAGE_CAPABILITIES::asynchronous_ops is not set, then all such APIs execute synchronously, and
control returns to the caller with a status code only after the completion of the operation (or the discovery of a failure
condition).

If ARM_STORAGE_CAPABILITIES::asynchronous_ops is set, a storage driver may
still choose to execute asynchronous operations in a synchronous manner. If
so, the driver returns a positive value to indicate successful synchronous
completion (or an error code in case of failure) and no further invocation of
completion callback should be expected. The expected return value for
synchronous completion of such asynchronous operations varies depending on
the operation. For operations involving data access, it often equals the
amount of data transferred or affected. For non data-transfer operations,
such as EraseAll or Initialize, it is usually 1.

Here's a code snippet to suggest how asynchronous APIs might be used by
callers to handle both synchronous and asynchronous execution by the
underlying storage driver:
\code
    ASSERT(ARM_DRIVER_OK == 0); // this is a precondition; it doesn't need to be put in code
    
    int32_t returnValue = drv->asynchronousAPI(...);
         
    if (returnValue < ARM_DRIVER_OK) {
        // handle error.
        
    } else if (returnValue == ARM_DRIVER_OK) {
        ASSERT(drv->GetCapabilities().asynchronous_ops == 1);
        // handle early return from asynchronous execution; remainder of the work is done in the callback handler.
        
    } else {
        ASSERT(returnValue == EXPECTED_RETURN_VALUE_FOR_SYNCHRONOUS_COMPLETION);
        // handle synchronous completion.
    }
\endcode

THis example is mixing synchronous and asynchronous APIs: \ref SampleUseOfStorageDriver
*******************************************************************************************************************/

/**
\struct     ARM_STORAGE_CAPABILITIES
\details
A Storage driver can be implemented with different capabilities. The data fields
of this struct encode the API capabilities implemented by this driver.

The element \em asynchronous_ops indicates if APIs like initialize, read, erase,
program, etc. can operate in asynchronous mode. Having this bit set to 1 means
that the driver is capable of launching asynchronous operations; command
completion for asynchronous operations is signaled by the invocation of a
completion callback. If set to 1, drivers may still complete asynchronous
operations synchronously as necessary--in which case they return a positive
error code to indicate synchronous completion.  If \em asynchronous_ops is not
set, then all such APIs execute synchronously, and control returns to the caller
with a status code only after the completion of the operation (or the discovery
of a failure condition).

The element \em erase_all specifies that the \ref ARM_Storage_EraseAll function
is supported. Typically full chip erase is much faster than erasing the whole
device using \em ARM_Storage_Erase.

<b>Returned by:</b>
  - \ref ARM_Storage_GetCapabilities

\note
This data structure is designed to fit within a single word so that it can be
fetched cheaply using a call to driver->GetCapabilities().
*******************************************************************************************************************/

/**
\struct     ARM_STORAGE_STATUS
\details
Structure with information about the status of the Storage device.

The flag \em busy indicates that the driver is busy executing read/program/erase operation.

The flag \em error flag is cleared on start of read/program/erase operation and is set at the end of the current operation in case of error.

<b>Returned by:</b>
  - \ref ARM_Storage_GetStatus
*****************************************************************************************************************/

/**
\enum       ARM_STORAGE_OPERATION
\details
Command opcodes for the Storage interface. Completion callbacks use these codes
to refer to completing commands. Refer to \ref ARM_Storage_Callback_t.
*****************************************************************************************************************/

/**
\typedef    ARM_Storage_Callback_t
\details
Provides the typedef for the callback function \ref ARM_Storage_Callback_t.

\param [in] status
              A code to indicate the status of the completed operation. For data
              transfer operations, the status field is overloaded in case of
              success to return the count of bytes successfully transferred; this
              can be done safely because error codes are negative values.

\param [in] operation
              The command op-code. This value isn't essential, but it is expected that
              this information could be a quick and useful filter for the handler.

<b>Parameter for:</b>
  - \ref ARM_Storage_Initialize
*******************************************************************************************************************/


//
// Functions
//

ARM_DRIVER_VERSION ARM_Storage_GetVersion (void)  {
  return { 0, 0 };
}
/**
\fn ARM_DRIVER_VERSION ARM_Storage_GetVersion (void)
\details
The function \b ARM_Storage_GetVersion returns version information of the driver implementation in \ref ARM_DRIVER_VERSION.
 - API version is the version of the CMSIS-Driver specification used to implement this driver.
 - Driver version is source code version of the actual driver implementation.

Example:
\code
extern ARM_DRIVER_STORAGE *drv_info;
 
void read_version (void)  {
  ARM_DRIVER_VERSION  version;
 
  version = drv_info->GetVersion ();
  if (version.api < 0x10A)   {      // requires at minimum API version 1.10 or higher
    // error handling
    return;
  }
}
\endcode

\note This API returns synchronously--it does not result in an invocation
   of a completion callback.

\note The function GetVersion() can be called any time to obtain the
   required information from the driver (even before initialization). It
   always returns the same information.
*******************************************************************************************************************/

ARM_STOR_CAPABILITIES ARM_Storage_GetCapabilities (void)  {
  return { 0 };
}
/**
\fn ARM_STORAGE_CAPABILITIES ARM_Storage_GetCapabilities (void)

\details
The function \b ARM_Storage_GetCapabilities returns information about
capabilities in this driver implementation. The data fields of the struct
ARM_STORAGE_CAPABILITIES encode various capabilities, for example if the device
is able to execute operations asynchronously.

Example:
\code
extern ARM_DRIVER_STORAGE *drv_info;
 
void read_capabilities (void)  {
  ARM_STORAGE_CAPABILITIES drv_capabilities;
 
  drv_capabilities = drv_info->GetCapabilities ();
  // interrogate capabilities

}
\endcode

\note This API returns synchronously--it does not result in an invocation
   of a completion callback.

\note The function GetCapabilities() can be called any time to obtain the
   required information from the driver (even before initialization). It
   always returns the same information.
*******************************************************************************************************************/

int32_t ARM_Storage_Initialize (ARM_Storage_Callback_t callback)  {
  return 0;
}
/**
\fn int32_t ARM_Storage_Initialize (ARM_Storage_Callback_t callback)
\details
The function \b ARM_Storage_Initialize is called when the middleware component starts
operation. In addition to bringing the controller to a ready state,
Initialize() receives a callback handler to be invoked upon completion of
asynchronous operations.

ARM_Storage_Initialize() needs to be called explicitly before
powering the peripheral using ARM_Storage_PowerControl(), and before initiating other
accesses to the storage controller.

The function performs the following operations:
 - Initializes the resources needed for the Storage interface.
 - Registers the \ref ARM_Storage_Callback_t callback function.

To start working with a peripheral the functions ARM_Storage_Initialize and ARM_Storage_PowerControl() need to be called in this order:
\code
   drv->Initialize (...);              // Allocate I/O pins
   drv->PowerControl (ARM_POWER_FULL); // Power up peripheral, setup IRQ/DMA
\endcode

- ARM_Storage_Initialize() typically allocates the I/O resources (pins) for the
 peripheral. The function can be called multiple times; if the I/O resources
 are already initialized it performs no operation and just returns with
 ARM_DRIVER_OK.

- ARM_Storage_PowerControl (ARM_POWER_FULL) sets the peripheral registers including
 interrupt (NVIC) and optionally DMA. The function can be called multiple
 times; if the registers are already set it performs no operation and just
 returns with ARM_DRIVER_OK.

To stop working with a peripheral the functions ARM_Storage_PowerControl() and ARM_Storage_Uninitialize() need to be called in this order:
\code
   drv->PowerControl (ARM_POWER_OFF); // Terminate any pending transfers, reset IRQ/DMA, power off peripheral
   drv->Uninitialize (...);           // Release I/O pins
\endcode

The functions ARM_Storage_PowerControl() and ARM_Storage_Uninitialize() always execute and can be used
to put the peripheral into a Safe State, for example after any data
transmission errors. To restart the peripheral in an error condition,
you should first execute the Stop Sequence and then the Start Sequence.

\note This API may execute asynchronously if
   ARM_STORAGE_CAPABILITIES::asynchronous_ops is set. Asynchronous
   execution is optional even if 'asynchronous_ops' is set.
*******************************************************************************************************************/

int32_t ARM_Storage_Uninitialize (void)  {
  return 0;
}
/**
\fn int32_t ARM_Storage_Uninitialize (void)
\details
It is called when the middleware component stops operation, and wishes to
release the software resources used by the interface.

\note This API may execute asynchronously if
   ARM_STORAGE_CAPABILITIES::asynchronous_ops is set. Asynchronous
   execution is optional even if 'asynchronous_ops' is set.
*******************************************************************************************************************/

int32_t ARM_Storage_PowerControl (ARM_POWER_STATE state)  {
  return 0;
}
/**
\fn int32_t ARM_Storage_PowerControl (ARM_POWER_STATE state)
\details
The function \b ARM_Storage_PowerControl operates the power modes of the Storage interface.

To start working with a peripheral the functions Initialize and PowerControl need to be called in this order:
\code
   drv->Initialize (...);                 // Allocate I/O pins
   drv->PowerControl (ARM_POWER_FULL);    // Power up peripheral, setup IRQ/DMA
\endcode

- ARM_Storage_Initialize() typically allocates the I/O resources (pins) for the
 peripheral. The function can be called multiple times; if the I/O resources
 are already initialized it performs no operation and just returns with
 ARM_DRIVER_OK.

- PowerControl (ARM_POWER_FULL) sets the peripheral registers including
 interrupt (NVIC) and optionally DMA. The function can be called multiple
 times; if the registers are already set it performs no operation and just
 returns with ARM_DRIVER_OK.

To stop working with a peripheral the functions PowerControl and Uninitialize need to be called in this order:
\code
   drv->PowerControl (ARM_POWER_OFF);     // Terminate any pending transfers, reset IRQ/DMA, power off peripheral
   drv->Uninitialize (...);               // Release I/O pins
\endcode

The functions ARM_Storage_PowerControl and ARM_Storage_Uninitialize always execute and can be used
to put the peripheral into a Safe State, for example after any data
transmission errors. To restart the peripheral in an error condition,
you should first execute the Stop Sequence and then the Start Sequence.

The parameter \em state can have the following values:
  - \ref ARM_POWER_FULL : set-up the Storage device for data transfers, enable interrupts (NVIC) and optionally DMA. Can be called multiple times.
                          If the device is already in this mode, then the function performs no operation and returns with \ref ARM_DRIVER_OK.
  - \ref ARM_POWER_LOW : may use power saving. Returns \ref ARM_DRIVER_ERROR_UNSUPPORTED when not implemented.
  - \ref ARM_POWER_OFF : terminates any pending data transfers, disables peripheral, disables related interrupts and DMA.

\note This API may execute asynchronously if
   ARM_STORAGE_CAPABILITIES::asynchronous_ops is set. Asynchronous
   execution is optional even if 'asynchronous_ops' is set.
*******************************************************************************************************************/

int32_t ARM_Storage_ReadData (uint64_t addr, void *data, uint32_t size)  {
  return 0;
}
/**
\fn int32_t ARM_Storage_ReadData (uint64_t addr, void *data, uint32_t size)
\details
Read the contents of a range of storage memory into a buffer
supplied by the caller. The buffer is owned by the caller and should
remain accessible for the lifetime of this command.

\note This API may execute asynchronously if
   ARM_STORAGE_CAPABILITIES::asynchronous_ops is set. Asynchronous
   execution is optional even if 'asynchronous_ops' is set.
*******************************************************************************************************************/

int32_t ARM_Storage_ProgramData (uint64_t addr, const void *data, uint32_t size)  {
  return 0;
}
/**
\fn int32_t ARM_Storage_ProgramData (uint64_t addr, const void *data, uint32_t size)
\details
Write the contents of a given memory buffer into a range of
storage memory. In the case of flash memory, the destination range in
storage memory typically has its contents in an erased state from a
preceding erase operation. The source memory buffer is owned by the
caller and should remain accessible for the lifetime of this command.

\note It is best for the middleware to write in units of
   'optimal_program_unit' (\ref ARM_STORAGE_INFO) of the device.

\note This API may execute asynchronously if
   ARM_STORAGE_CAPABILITIES::asynchronous_ops is set. Asynchronous
   execution is optional even if 'asynchronous_ops' is set.
*******************************************************************************************************************/

int32_t ARM_Storage_Erase (uint64_t addr, uint32_t size)  {
  return 0;
}
/**
\fn int32_t ARM_Storage_Erase (uint64_t addr, uint32_t size)

\details
This function erases a range of storage specified by [addr, addr +
size). Both 'addr' and 'addr + size' should align with the
'erase_unit'(s) of the respective owning storage block(s) (see \ref
ARM_STORAGE_BLOCK and \ref ARM_STORAGE_BLOCK_ATTRIBUTES). The range to
be erased will have its contents returned to the un-programmed state--
i.e. to \ref ARM_STORAGE_INFO::erased_value, which
is usually 1 to indicate the pattern of all ones: 0xFF.

\note This API may execute asynchronously if
   ARM_STORAGE_CAPABILITIES::asynchronous_ops is set. Asynchronous
   execution is optional even if 'asynchronous_ops' is set.

\note Erase() may return a smaller (positive) value than the size of the
   requested range. The returned value indicates the actual number of bytes
   erased. It is the caller's responsibility to follow up with an appropriate
   request to complete the operation.

\note in the case of a failed erase (except when
   ARM_DRIVER_ERROR_PARAMETER, ARM_STORAGE_ERROR_PROTECTED, or
   ARM_STORAGE_ERROR_NOT_ERASABLE is returned synchronously), the
   requested range should be assumed to be in an unknown state. The
   previous contents may not be retained.
*******************************************************************************************************************/

int32_t ARM_Storage_EraseAll (void)  {
  return 0;
}
/**
\fn int32_t ARM_Storage_EraseAll (void)
\details
This optional function erases the complete device. If the device does not
support global erase then the function returns the error value \ref
ARM_DRIVER_ERROR_UNSUPPORTED. The data field \em 'erase_all' =
\token{1} of the structure \ref ARM_STORAGE_CAPABILITIES encodes that
\ref ARM_Storage_EraseAll is supported.

\note This API may execute asynchronously if
   ARM_STORAGE_CAPABILITIES::asynchronous_ops is set. Asynchronous
   execution is optional even if 'asynchronous_ops' is set.
*******************************************************************************************************************/

ARM_Storage_STATUS ARM_Storage_GetStatus (void)  {
  return 0;
}
/**
\fn ARM_STORAGE_STATUS ARM_Storage_GetStatus (void)
\details
Get the status of the current (or previous) command executed by the
storage controller; stored in the structure \ref ARM_STORAGE_STATUS.

\note This API returns synchronously--it does not result in an invocation
   of a completion callback.
*******************************************************************************************************************/

int32_t ARM_Storage_GetInfo (ARM_STORAGE_INFO *info)  {
  return 0;
}
/**
\fn int32_t ARM_Storage_GetInfo (ARM_STORAGE_INFO *info)
\details
Get information about the Storage device; stored in the structure \ref ARM_STORAGE_INFO.

\note It is the caller's responsibility to ensure that the buffer passed in
       is able to be initialized with a \ref ARM_STORAGE_INFO.

\note This API returns synchronously--it does not result in an invocation
   of a completion callback.
*******************************************************************************************************************/

uint32_t ARM_Storage_ResolveAddress(uint64_t addr) {
  return 0;
}
/**
\fn uint32_t ARM_Storage_ResolveAddress(uint64_t addr)
\details
Only applicable to devices with memory-mapped storage.

\note This API returns synchronously. The invocation should return quickly,
   and result in a resolved address.
*******************************************************************************************************************/

int32_t ARM_Storage_GetNextBlock(const ARM_STORAGE_BLOCK* prev_block, ARM_STORAGE_BLOCK *next_block) {
  return 0;
}
/**
\fn int32_t ARM_Storage_GetNextBlock(const ARM_STORAGE_BLOCK* prev_block, ARM_STORAGE_BLOCK *next_block);
\details
This helper function fetches (an iterator to) the next block (or
the first block if 'prev_block' is passed in as NULL). In the failure
case, a terminating, invalid block iterator is filled into the out
parameter: 'next_block'. In combination with \ref
ARM_STORAGE_VALID_BLOCK, it can be used to iterate over the sequence
of blocks within the storage map:

\code
  ARM_STORAGE_BLOCK block;
  for (drv->GetNextBlock(NULL, &block); ARM_STORAGE_VALID_BLOCK(&block); drv->GetNextBlock(&block, &block)) {
      // make use of block
  }
\endcode

\note This API returns synchronously--it does not result in an invocation
    of a completion callback.
*******************************************************************************************************************/

int32_t ARM_Storage_GetBlock(uint64_t addr, ARM_STORAGE_BLOCK *block) {
  return 0;
}
/**
\fn int32_t ARM_Storage_GetBlock(uint64_t addr, ARM_STORAGE_BLOCK *block);
\note This API returns synchronously--it does not result in an invocation
    of a completion callback.
*******************************************************************************************************************/

/**
@}
*/

/**
\defgroup SampleUseOfStorageDriver Sample Use of Storage Driver
\ingroup storage_interface_gr
@{
<b>Example Code:</b>

The following is a generic algorithm to erase
and program one \ref ARM_STORAGE_BLOCK_ATTRIBUTES::erase_unit worth of storage
and then read it back to be verified. It handles both synchronous and
asynchronous driver implementations.

\code
// Copyright (c) 2006-2016, Arm Limited, All Rights Reserved
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http:// www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Driver_Storage.h"
#include <stdio.h>
#include <string.h>
 
#define TEST_ASSERT(Expr)                       if (!(Expr)) { printf("%s:%u: assertion failure\n", __FUNCTION__, __LINE__); while (1) ;}
#define TEST_ASSERT_EQUAL(expected, actual)     if ((expected) != (actual)) {printf("%s:%u: assertion failure\n", __FUNCTION__, __LINE__); while (1) ;}
#define TEST_ASSERT_NOT_EQUAL(expected, actual) if ((expected) == (actual)) {printf("%s:%u: assertion failure\n", __FUNCTION__, __LINE__); while (1) ;}
 
// forward declarations
void callbackHandler(int32_t status, ARM_STORAGE_OPERATION operation);
void progressStateMachine(void);
 
static enum {
    NEEDS_INITIALIZATION,
    NEEDS_ERASE,
    NEEDS_PROGRAMMING,
    NEEDS_READ,
    NEEDS_VERIFICATION_FOLLOWING_READ,
    FINISHED
} state;
 
extern ARM_DRIVER_STORAGE ARM_Driver_Storage_(0);
ARM_DRIVER_STORAGE *drv = &ARM_Driver_Storage_(0);
 
static const unsigned BUFFER_SIZE = 16384;
static uint8_t buffer[BUFFER_SIZE];
 
void main(int argc __unused, char** argv __unused)
{
    state = NEEDS_INITIALIZATION;
 
    progressStateMachine();
    while (true) {
        // WFE(); // optional low-power sleep
    }
}
 
void progressStateMachine(void)
{
    int32_t rc;
 
    static ARM_STORAGE_BLOCK firstBlock;
    if (!ARM_STORAGE_VALID_BLOCK(&firstBlock)) {
        // Get the first block. This block is entered only once.
        rc = drv->GetNextBlock(NULL, &firstBlock); // get first block
        TEST_ASSERT_EQUAL(ARM_DRIVER_OK, rc);
    }
    TEST_ASSERT(ARM_STORAGE_VALID_BLOCK(&firstBlock));
    TEST_ASSERT(firstBlock.size > 0);
 
    switch (state) {
        case NEEDS_INITIALIZATION:
            rc = drv->Initialize(callbackHandler);
            TEST_ASSERT(rc >= ARM_DRIVER_OK);
            if (rc == ARM_DRIVER_OK) {
                TEST_ASSERT_EQUAL(1, drv->GetCapabilities().asynchronous_ops);
                state = NEEDS_ERASE;
                return; // there is pending asynchronous activity which will lead to a completion callback later.
            }
            TEST_ASSERT_EQUAL(1, rc); // synchronous completion
 
            // intentional fall-through
 
        case NEEDS_ERASE:
            TEST_ASSERT(firstBlock.attributes.erase_unit > 0);
            rc = drv->Erase(firstBlock.addr, firstBlock.attributes.erase_unit);
            TEST_ASSERT(rc >= ARM_DRIVER_OK);
            if (rc == ARM_DRIVER_OK) {
                TEST_ASSERT_EQUAL(1, drv->GetCapabilities().asynchronous_ops);
                state = NEEDS_PROGRAMMING;
                return; // there is pending asynchronous activity which will lead to a completion callback later.
            }
            TEST_ASSERT_EQUAL(firstBlock.attributes.erase_unit, (uint32_t)rc); // synchronous completion
 
            // intentional fall-through
 
        case NEEDS_PROGRAMMING:
            TEST_ASSERT(BUFFER_SIZE >= firstBlock.attributes.erase_unit);
            #define PATTERN 0xAA
            memset(buffer, PATTERN, firstBlock.attributes.erase_unit);
            rc = drv->ProgramData(firstBlock.addr, buffer, firstBlock.attributes.erase_unit);
            TEST_ASSERT(rc >= ARM_DRIVER_OK);
            if (rc == ARM_DRIVER_OK) {
                TEST_ASSERT_EQUAL(1, drv->GetCapabilities().asynchronous_ops);
                state = NEEDS_READ;
                return;  // there is pending asynchronous activity which will lead to a completion callback later.
            }
            TEST_ASSERT_EQUAL(firstBlock.attributes.erase_unit, (uint32_t)rc); // synchronous completion
 
            // intentional fall-through
 
        case NEEDS_READ:
            rc = drv->ReadData(firstBlock.addr, buffer, firstBlock.attributes.erase_unit);
            TEST_ASSERT(rc >= ARM_DRIVER_OK);
            if (rc == ARM_DRIVER_OK) {
                TEST_ASSERT_EQUAL(1, drv->GetCapabilities().asynchronous_ops);
                state = NEEDS_VERIFICATION_FOLLOWING_READ;
                return;  // there is pending asynchronous activity which will lead to a completion callback later.
            }
            TEST_ASSERT_EQUAL(firstBlock.attributes.erase_unit, (uint32_t)rc);
 
            // intentional fall-through
 
        case NEEDS_VERIFICATION_FOLLOWING_READ:
            printf("verifying data\r\n");
            for (unsigned i = 0; i < firstBlock.attributes.erase_unit; i++) {
                TEST_ASSERT_EQUAL(PATTERN, buffer[i]);
            }
            state = FINISHED;
            printf("done\r\n");
            break;
 
        case FINISHED:
            break;
    } // switch (state)
}
 
void callbackHandler(int32_t status, ARM_STORAGE_OPERATION operation)
{
    (void)status;
    (void)operation;
    switch (operation) {
        case ARM_STORAGE_OPERATION_INITIALIZE:
        case ARM_STORAGE_OPERATION_READ_DATA:
        case ARM_STORAGE_OPERATION_PROGRAM_DATA:
        case ARM_STORAGE_OPERATION_ERASE:
            progressStateMachine();
            break;
 
        default:
            printf("callbackHandler: unexpected callback for opcode %u with status %ld\r\n", operation, status);
            break;
    }
}
\endcode
@}
*******************************************************************************************************************/
// End Storage Interface
