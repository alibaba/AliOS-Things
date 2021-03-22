/**
\defgroup flash_interface_gr Flash Interface
\brief    Driver API for Flash Device Interface (%Driver_Flash.h)
\details
<a href="http://en.wikipedia.org/wiki/Flash_memory" target="_blank">Flash devices</a> based on NOR memory cells are the
preferred technology for embedded applications requiring a discrete non-volatile memory device. The low read latency
characteristic of these Flash devices allow a direct code execution
(<a href="http://en.wikipedia.org/wiki/Execute_in_place" target="_blank">XIP</a>) and data storage in a single memory
product.

<b>Flash API</b>

The Flash API provides a generic API suitable for Flashes with NOR memory cells independent from the actual interface
to the MCU (memory bus, SPI, ...). <a href="http://en.wikipedia.org/wiki/Flash_memory#Serial_flash" target="_blank">SPI</a>
flashes are typically not named NOR flashes but have usually same flash cell properties. 

The following header files define the Application Programming Interface (API) for the Flash interface:
  - \b %Driver_Flash.h : Driver API for Flash Device Interface

<b>Driver Functions</b>

The driver functions are published in the access struct as explained in \ref DriverFunctions
  - \ref ARM_DRIVER_FLASH : access struct for Flash driver functions
@{

A typical setup sequence for the driver is shown below:

<b>Example Code:</b>

\include Flash_Demo.c
*/
*******************************************************************************************************************/


/**
\defgroup Flash_events Flash Events
\ingroup flash_interface_gr
\brief The Flash driver generates call back events that are notified via the function \ref ARM_Flash_SignalEvent.
\details 
This section provides the event values for the \ref ARM_Flash_SignalEvent callback function.

The following call back notification events are generated:
@{
\def ARM_FLASH_EVENT_READY
\def ARM_FLASH_EVENT_ERROR
@}
*/


/**
\struct     ARM_FLASH_SECTOR 
\details  
Specifies sector start and end address.

<b>Element of</b>:
  - \ref ARM_FLASH_INFO structure
*******************************************************************************************************************/

/**
\struct     ARM_FLASH_INFO 
\details 
Stores the characteristics of a Flash device. This includes sector layout, programming size and a default value for erased memory.
This information can be obtained from the Flash device datasheet and is used by the middleware in order to properly interact with the Flash device.

Sector layout is described by specifying the \em sector_info which points to an array of sector information (start and end address) and by specifying the \em sector_count which defines the number of sectors.
The element \em sector_size is not used in this case and needs to be \em 0.
Flash sectors need not to be aligned continuously. Gaps are allowed in the device memory space in order to reserve sectors for other usage (for example application code).

When the device has uniform sector size than the sector layout can be described by specifying the \em sector_size which defines the size of a single sector and by specifying the \em sector_count which defines the number of sectors.
The element \em sector_info is not used in this case and needs to be \em NULL.

The smallest programmable unit within a sector is specified by the \em program_unit. It defines the granularity for programming data.

Optimal programming page size is specified by the \em page_size and defines the amount of data that should be programmed in one step to achieve maximum programming speed.

Contents of erased memory is specified by the \em erased_value and is typically \em 0xFF. This value can be used before erasing a sector to check if the sector is blank and erase can be skipped.

*******************************************************************************************************************/

/**
\struct     ARM_DRIVER_FLASH
\details 
The functions of the Flash driver are accessed by function pointers exposed by this structure. Refer to \ref DriverFunctions for overview information.

Each instance of a Flash interface provides such an access structure. 
The instance is identified by a postfix number in the symbol name of the access structure, for example:
 - \b Driver_Flash0 is the name of the access struct of the first instance (no. 0).
 - \b Driver_Flash1 is the name of the access struct of the second instance (no. 1).

A middleware configuration setting allows connecting the middleware to a specific driver instance \b %Driver_Flash<i>n</i>.
The default is \token{0}, which connects a middleware to the first instance of a driver.
*******************************************************************************************************************/

/**
\struct     ARM_FLASH_CAPABILITIES 
\details
A Flash driver can be implemented with different capabilities. The data fields of this struct encode 
the capabilities implemented by this driver.

The element \em event_ready indicates that the driver is able to generate the \ref ARM_FLASH_EVENT_READY event. In case that this event is not available it is possible to poll the driver status by calling the \ref ARM_Flash_GetStatus and check the \em busy flag.

The element \em data_width specifies the data access size and also defines the data type (uint8_t, uint16_t or uint32_t) for the \em data parameter in \ref ARM_Flash_ReadData and \ref ARM_Flash_ProgramData functions.

The element \em erase_chip specifies that the \ref ARM_Flash_EraseChip function is supported. Typically full chip erase is much faster than erasing the whole device sector per sector.

<b>Returned by:</b>
  - \ref ARM_Flash_GetCapabilities
*******************************************************************************************************************/

/**
\struct     ARM_FLASH_STATUS
\details
Structure with information about the status of the Flash.

The flag \em busy indicates that the driver is busy executing read/program/erase operation.

The flag \em error flag is cleared on start of read/program/erase operation and is set at the end of the current operation in case of error.

<b>Returned by:</b>
  - \ref ARM_Flash_GetStatus
*****************************************************************************************************************/

/**
\typedef    ARM_Flash_SignalEvent_t
\details
Provides the typedef for the callback function \ref ARM_Flash_SignalEvent.

<b>Parameter for:</b>
  - \ref ARM_Flash_Initialize
*******************************************************************************************************************/


//
// Functions
//

ARM_DRIVER_VERSION ARM_Flash_GetVersion (void)  {
  return { 0, 0 };
}
/**
\fn ARM_DRIVER_VERSION ARM_Flash_GetVersion (void)
\details
The function \b ARM_Flash_GetVersion returns version information of the driver implementation in \ref ARM_DRIVER_VERSION
 - API version is the version of the CMSIS-Driver specification used to implement this driver.
 - Driver version is source code version of the actual driver implementation.

Example:
\code
extern ARM_DRIVER_FLASH Driver_Flash0;
ARM_DRIVER_FLASH *drv_info;
 
void read_version (void)  {
  ARM_DRIVER_VERSION  version;
 
  drv_info = &Driver_Flash0;  
  version = drv_info->GetVersion ();
  if (version.api < 0x10A)   {      // requires at minimum API version 1.10 or higher
    // error handling
    return;
  }
}
\endcode
*******************************************************************************************************************/

ARM_FLASH_CAPABILITIES ARM_Flash_GetCapabilities (void)  {
  return { 0 };
}
/**
\fn ARM_FLASH_CAPABILITIES ARM_Flash_GetCapabilities (void)
\details
The function \b ARM_Flash_GetCapabilities returns information about capabilities in this driver implementation.
The data fields of the struct \ref ARM_FLASH_CAPABILITIES encode various capabilities, for example
if a hardware is able to create signal events using the \ref ARM_Flash_SignalEvent callback function.
 
Example:
\code
extern ARM_DRIVER_FLASH Driver_Flash0;
ARM_DRIVER_FLASH *drv_info;
  
void read_capabilities (void)  {
  ARM_FLASH_CAPABILITIES drv_capabilities;
 
  drv_info = &Driver_Flash0;  
  drv_capabilities = drv_info->GetCapabilities ();
  // interrogate capabilities
 
}
\endcode
*******************************************************************************************************************/

int32_t ARM_Flash_Initialize (ARM_Flash_SignalEvent_t cb_event)  {
  return 0;
}
/**
\fn int32_t ARM_Flash_Initialize (ARM_Flash_SignalEvent_t cb_event)
\details
The function \b ARM_Flash_Initialize initializes the Flash interface.
It is called when the middleware component starts operation.

The function performs the following operations:
  - Initializes the resources needed for the Flash interface.
  - Registers the \ref ARM_Flash_SignalEvent callback function.

The parameter \em cb_event is a pointer to the \ref ARM_Flash_SignalEvent callback function; use a NULL pointer 
when no callback signals are required.

\b Example:
 - see \ref flash_interface_gr - Driver Functions

*******************************************************************************************************************/

int32_t ARM_Flash_Uninitialize (void)  {
  return 0;
}
/**
\fn int32_t ARM_Flash_Uninitialize (void)
\details
The function \b ARM_Flash_Uninitialize de-initializes the resources of Flash interface.

It is called when the middleware component stops operation and releases the software resources used by the interface.
*******************************************************************************************************************/

int32_t ARM_Flash_PowerControl (ARM_POWER_STATE state)  {
  return 0;
}
/**
\fn int32_t ARM_Flash_PowerControl (ARM_POWER_STATE state)
\details
The function \b ARM_Flash_PowerControl operates the power modes of the Flash interface.

The parameter \em state can have the following values:
  - \ref ARM_POWER_FULL : set-up peripheral for data transfers, enable interrupts (NVIC) and optionally DMA. Can be called multiple times. 
                          If the peripheral is already in this mode, then the function performs no operation and returns with \ref ARM_DRIVER_OK.
  - \ref ARM_POWER_LOW : may use power saving. Returns \ref ARM_DRIVER_ERROR_UNSUPPORTED when not implemented.
  - \ref ARM_POWER_OFF : terminates any pending data transfers, disables peripheral, disables related interrupts and DMA.
      
Refer to \ref CallSequence for more information.
*******************************************************************************************************************/

int32_t ARM_Flash_ReadData (uint32_t addr, void *data, uint32_t cnt)  {
  return 0;
}
/**
\fn int32_t ARM_Flash_ReadData (uint32_t addr, void *data, uint32_t cnt)
\details  
This function \b ARM_Flash_ReadData reads data from the Flash device.

The parameter \em addr specifies the address from where to read data (needs to be aligned to data type size). \n
The parameter \em data specifies the pointer to a buffer storing the data read. 
The data type is \em uint8_t, \em uint16_t or \em uint32_t and is specified by the \em data_width in \ref ARM_FLASH_CAPABILITIES. \n
The parameter \em cnt specifies the number of data items to read.

The function executes in the following ways:
 - When the operation is non-blocking (typical for SPI Flash) then the function only starts the operation and returns with zero number of data items read. 
   When the operation is completed the \ref ARM_FLASH_EVENT_READY event is generated (if supported and reported by \ref ARM_Flash_GetCapabilities).
   In case of errors the \ref ARM_FLASH_EVENT_ERROR event is generated at the same time.   
   Progress of the operation can also be monitored by calling the \ref ARM_Flash_GetStatus function and checking the \em busy flag.
 - When the operation is blocking (typical for memory mapped Flash) then the function returns after the data is read and returns the number of data items read.
*******************************************************************************************************************/

int32_t ARM_Flash_ProgramData (uint32_t addr, const void *data, uint32_t cnt)  {
  return 0;
}
/**
\fn int32_t ARM_Flash_ProgramData (uint32_t addr, const void *data, uint32_t cnt)
\details  
This function \b ARM_Flash_ProgramData programs data to the Flash device.

The parameter \em addr specifies the address to where to program data (needs to be aligned to \em program_unit specified in \ref ARM_FLASH_INFO). \n
The parameter \em data specifies the pointer to a buffer containing data to be programmed. 
The data type is \em uint8_t, \em uint16_t or \em uint32_t and is specified by the \em data_width in \ref ARM_FLASH_CAPABILITIES. \n
The parameter \em cnt specifies the number of data items to program (data size needs to be a multiple of \em program_unit).

The function executes in the following ways:
 - When the operation is non-blocking (typically) then the function only starts the operation and returns with zero number of data items programmed. 
   When the operation is completed the \ref ARM_FLASH_EVENT_READY event is generated (if supported and reported by \ref ARM_Flash_GetCapabilities).
   In case of errors the \ref ARM_FLASH_EVENT_ERROR event is generated at the same time.
   Progress of the operation can also be monitored by calling the \ref ARM_Flash_GetStatus function and checking the \em busy flag.
 - When the operation is blocking then the function returns after the data is programmed and returns the number of data items programmed.
*******************************************************************************************************************/

int32_t ARM_Flash_EraseSector (uint32_t addr)  {
  return 0;
}
/**
\fn int32_t ARM_Flash_EraseSector (uint32_t addr)
\details  
This function \b ARM_Flash_EraseSector erases a flash sector specified by the parameter <i>adr</i> (points to start of the sector).

The function is non-blocking and returns as soon as the driver has started the operation.
When the operation is completed the \ref ARM_FLASH_EVENT_READY event is generated (if supported and reported by \ref ARM_Flash_GetCapabilities).
In case of errors the \ref ARM_FLASH_EVENT_ERROR event is generated at the same time.
Progress of the operation can also be monitored by calling the \ref ARM_Flash_GetStatus function and checking the \em busy flag.
*******************************************************************************************************************/

int32_t ARM_Flash_EraseChip (void)  {
  return 0;
}
/**
\fn int32_t ARM_Flash_EraseChip (void)
\details  
The optional function \b ARM_Flash_EraseChip erases the complete device.
If the device does not support global erase or only a portion of the Flash memory space is used for storing files,
then the functions returns the error value \ref ARM_DRIVER_ERROR_UNSUPPORTED.
The data field \em eras_chip = \token{1} of the structure \ref ARM_FLASH_CAPABILITIES encodes that \b ARM_Flash_EraseChip is supported.
The field can be verified with the function \ref ARM_Flash_GetCapabilities.

The function is non-blocking and returns as soon as the driver has started the operation.
When the operation is completed, the \ref ARM_FLASH_EVENT_READY event is generated (if supported and reported by \ref ARM_Flash_GetCapabilities).
In case of errors, the \ref ARM_FLASH_EVENT_ERROR event is generated at the same time.
Progress of the operation can also be monitored by calling the \ref ARM_Flash_GetStatus function and checking the \em busy flag.

<b>See also:</b>
 - ARM_Flash_SignalEvent
*******************************************************************************************************************/

ARM_FLASH_STATUS ARM_Flash_GetStatus (void)  {
  return 0;
}
/**
\fn ARM_FLASH_STATUS ARM_Flash_GetStatus (void)
\details
The function \b ARM_Flash_GetStatus returns the current Flash interface status stored in the structure \ref ARM_FLASH_STATUS.
*******************************************************************************************************************/

ARM_FLASH_INFO * ARM_Flash_GetInfo (void)  {
  return NULL;
}
/**
\fn ARM_FLASH_INFO * ARM_Flash_GetInfo (void)
\details
The function \b ARM_Flash_GetInfo returns information about the Flash device.
*******************************************************************************************************************/

void ARM_Flash_SignalEvent (uint32_t event)  {
  return 0;
}
/**
\fn void ARM_Flash_SignalEvent (uint32_t event)
\details

The function \b ARM_Flash_SignalEvent is a callback function registered by the function \ref ARM_Flash_Initialize. 
The function is called automatically after read/program/erase operation completes.

The parameter \em event indicates one or more events that occurred during driver operation. Each event is coded in a separate bit and
therefore it is possible to signal multiple events in the event call back function. 

Not every event is necessarily generated by the driver. This depends on the implemented capabilities stored in the 
data fields of the structure \ref ARM_FLASH_CAPABILITIES, which can be retrieved with the function \ref ARM_Flash_GetCapabilities.

The following events can be generated:

Parameter \em event                 | Bit | Description 
:-----------------------------------|:---:|:-----------
\ref ARM_FLASH_EVENT_READY          |  0  | Occurs after read/program/erase operation completes.
\ref ARM_FLASH_EVENT_ERROR          |  1  | Occurs together with \ref ARM_FLASH_EVENT_READY when operation completes with errors.

<b>See also:</b>
 - \ref ARM_Flash_EraseChip
*******************************************************************************************************************/

/**
@}
*/ 
// End Flash Interface
