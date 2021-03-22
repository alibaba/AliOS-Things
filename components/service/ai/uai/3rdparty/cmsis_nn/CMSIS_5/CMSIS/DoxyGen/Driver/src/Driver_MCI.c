/**
\defgroup mci_interface_gr MCI Interface
\brief    Driver API for Memory Card Interface using SD/MMC interface (%Driver_MCI.h)

\details
The <b>Memory Card Interface</b> (MCI) implements the hardware abstraction layer for Secure Digital (SD) and Multi Media Card (MMC)
memory that is typically used as file storage. For embedded systems, SD/MMC devices are available as memory cards in several 
forms (SD, miniSD, microSD, MMC, MMCmicro) or as non-removable devic
es that are directly soldered to the PCB (eMMC).  

\b References:
- Wikipedia offers more information about the <a href="http://en.wikipedia.org/wiki/SD_card" target="_blank"><b>Secure Digital</b> memory</a>.
- Wikipedia offers more information about the <a href="http://en.wikipedia.org/wiki/MultiMediaCard" target="_blank"><b>MultiMediaCard</b></a>.
- The SD Association provides detailed documentation under <a href="http://www.sdcard.org">www.sdcard.org</a>.
- The MultiMediaCard Association (merged with JEDEC) provides detailed documentation under <a href="http://www.jedec.org">www.jedec.org</a>.

<b>Block Diagram</b>

The MCI driver allows you to exchange data of the SD/MMC memory via SD/MMC interface.

The following modes are supported by SD/MMC memory cards:

- SPI bus mode: Serial Peripheral Interface Bus supported by most microcontrollers.
- 1-bit SD/MMC Bus mode: proprietary data transfer protocol supported by SD/MMC interfaces.
- 4-bit SD/MMC Bus mode: high-speed version of the SD/MMC interface using 4 data I/O pins. 
- 8-bit SD/MMC Bus mode: high-speed version of the SD/MMC interface using 8 data I/O pins. 

\image html SPI_BusMode.png  "SD memory connected via SPI interface"
<p>&nbsp;</p>
\image html SD_1BitBusMode.png  "SD memory connected via 1-bit SD Bus Mode"
<p>&nbsp;</p>
\image html SD_4BitBusMode.png  "SD memory connected via 4-bit SD Bus Mode"


<b>MCI API</b>

The following header files define the Application Programming Interface (API) for the MCI interface:
  - \b %Driver_MCI.h : Driver API for Memory Card Interface using SD/MMC interface

The driver implementation is a typical part of the Device Family Pack (DFP) that supports the 
peripherals of the microcontroller family.

\note
For parameters, the value marked with (default) is the setting after the driver initialization.

 
<b>Driver Functions</b>

The driver functions are published in the access struct as explained in \ref DriverFunctions
  - \ref ARM_DRIVER_MCI : access struct for MCI driver functions


<b>Example Code</b>

The following example code shows the usage of the MCI interface.

\include MCI_Demo.c

@{
*/


/*************   Structures ******************************************************************************************************/
/** 
\struct     ARM_DRIVER_MCI 
\details 
The functions of the MCI are accessed by function pointers exposed by this structure. Refer to \ref DriverFunctions for overview information.

Each instance of an MCI provides such an access structure. 
The instance is identified by a postfix number in the symbol name of the access structure, for example:
 - \b Driver_MCI0 is the name of the access struct of the first instance (no. 0).
 - \b Driver_MCI1 is the name of the access struct of the second instance (no. 1).

A configuration setting in the middleware allows connecting the middleware to a specific driver instance <b>Driver_MCI<i>n</i></b>.
The default is \token{0}, which connects a middleware to the first instance of a driver.
*******************************************************************************************************************/

/** 
\struct     ARM_MCI_CAPABILITIES 
\details 
A MCI driver can be implemented with different capabilities.  
The data fields of this struct encode the capabilities implemented by this driver.

<b>Returned by:</b>
  - \ref ARM_MCI_GetCapabilities
*******************************************************************************************************************/

/**
\defgroup mci_event_gr MCI Events
\brief The MCI driver generates call back events that are notified via the function \ref ARM_MCI_SignalEvent.
\details
This section provides the event values for the \ref ARM_MCI_SignalEvent callback function.

The following call back notification events are generated:
@{
\def ARM_MCI_EVENT_CARD_INSERTED   
\sa \ref ARM_MCI_SignalEvent
\def ARM_MCI_EVENT_CARD_REMOVED    
\sa \ref  ARM_MCI_SignalEvent
\def ARM_MCI_EVENT_COMMAND_COMPLETE 
\sa \ref ARM_MCI_SignalEvent
\def ARM_MCI_EVENT_COMMAND_TIMEOUT  
\sa \ref ARM_MCI_SignalEvent
\def ARM_MCI_EVENT_COMMAND_ERROR    
\sa \ref ARM_MCI_SignalEvent
\def ARM_MCI_EVENT_TRANSFER_COMPLETE
\sa \ref ARM_MCI_SignalEvent
\def ARM_MCI_EVENT_TRANSFER_TIMEOUT 
\sa \ref ARM_MCI_SignalEvent
\def ARM_MCI_EVENT_TRANSFER_ERROR   
\sa \ref ARM_MCI_SignalEvent
\def ARM_MCI_EVENT_SDIO_INTERRUPT   
\sa \ref ARM_MCI_SignalEvent
\def ARM_MCI_EVENT_CCS              
\sa \ref ARM_MCI_SignalEvent
\def ARM_MCI_EVENT_CCS_TIMEOUT      
\sa \ref ARM_MCI_SignalEvent
@}
*******************************************************************************************************************/

//open mci_contorl_gr
/**  
@{
*/
/**
\defgroup mci_control_gr MCI Control Codes
\ingroup mci_interface_gr
\brief Configure and control the MCI using the \ref ARM_MCI_Control.
\details
@{
Many parameters of the MCI driver are configured using the \ref ARM_MCI_Control function.

The various MCI control codes define:
  - \ref mci_mode_ctrls configures and controls the MCI interface
  - \ref mci_bus_speed_ctrls specifies the bus speed mode
  - \ref mci_bus_data_width_ctrls specifies the data bus width
  - \ref mci_cmd_line_ctrls specifies the CMD line mode
  - \ref mci_driver_strength_ctrls specifies the driver strength

Refer to the function \ref ARM_MCI_Control for further details.
@}
*******************************************************************************************************************/


/**
\defgroup mci_mode_ctrls MCI Controls
\ingroup mci_control_gr
\brief Configure and control the MCI interface.
\details
The following codes are used as values for the parameter \em control of the function \ref ARM_MCI_Control to setup the MCI interface.
@{
\def ARM_MCI_BUS_SPEED       
\def ARM_MCI_BUS_SPEED_MODE       
\def ARM_MCI_BUS_CMD_MODE         
\def ARM_MCI_BUS_DATA_WIDTH       
\def ARM_MCI_DRIVER_STRENGTH       
\def ARM_MCI_CONTROL_RESET         
\def ARM_MCI_CONTROL_CLOCK_IDLE    
\def ARM_MCI_UHS_TUNING_OPERATION 
\def ARM_MCI_UHS_TUNING_RESULT
\def ARM_MCI_DATA_TIMEOUT          
\def ARM_MCI_CSS_TIMEOUT            
\def ARM_MCI_MONITOR_SDIO_INTERRUPT 
\def ARM_MCI_CONTROL_READ_WAIT      
\def ARM_MCI_SUSPEND_TRANSFER       
\def ARM_MCI_RESUME_TRANSFER        
@}
*******************************************************************************************************************/


/**
\defgroup mci_bus_speed_ctrls MCI Bus Speed Mode
\ingroup mci_control_gr
\brief Specify the bus speed mode.
\details
@{
The function \ref ARM_MCI_Control with \em control = \ref ARM_MCI_BUS_SPEED configures the bus speed of the MCI to the
requested bits/s specified with \em arg.

The function \ref ARM_MCI_Control with \em control = \ref ARM_MCI_BUS_SPEED_MODE configures the bus speed mode of the MCI
as specified with \em arg listed bellow.

The function \ref ARM_MCI_GetCapabilities lists the supported bus speed modes. Initially, all SD cards use a 3.3 volt electrical interface. 
Some SD cards can switch to 1.8 volt operation. For example, the use of ultra-high-speed (UHS) 
SD cards requires 1.8 volt operation and a 4-bit bus data width. The data field \em uhs_signaling of the structure ARM_MCI_CAPABILITIES encodes 
whether the driver supports 1.8 volt UHS signaling.

\sa 
 - \ref mci_driver_strength_ctrls

The following codes are defined:

\def ARM_MCI_BUS_DEFAULT_SPEED     
\def ARM_MCI_BUS_HIGH_SPEED        
\def ARM_MCI_BUS_UHS_SDR12         
\def ARM_MCI_BUS_UHS_SDR25         
\def ARM_MCI_BUS_UHS_SDR50         
\def ARM_MCI_BUS_UHS_SDR104        
\def ARM_MCI_BUS_UHS_DDR50         
@}
*******************************************************************************************************************/


/**
\defgroup mci_bus_data_width_ctrls MCI Bus Data Width
\ingroup mci_control_gr
\brief Specify the data bus width.
\details
@{
The function \ref ARM_MCI_Control with \em control = \ref ARM_MCI_BUS_DATA_WIDTH specifies with \em arg the number of data I/O pins on the SD/MMC interface.

For high-speed memory cards, a 4-bit bus data width should be used (or 8-bit for eMMC). The data fields \em data_width_4 and \em data_width_8 
of the structure ARM_MCI_CAPABILITIES encode whether the driver supports a specific bus data with.

The following codes are defined:

\def ARM_MCI_BUS_DATA_WIDTH_1        
\def ARM_MCI_BUS_DATA_WIDTH_4        
\def ARM_MCI_BUS_DATA_WIDTH_8        
\def ARM_MCI_BUS_DATA_WIDTH_4_DDR    
\def ARM_MCI_BUS_DATA_WIDTH_8_DDR    
@}
*******************************************************************************************************************/


/**
\defgroup mci_cmd_line_ctrls MCI CMD Line Mode
\ingroup mci_control_gr
\brief Specify the CMD line mode (Push-Pull or Open Drain).
\details
@{
Set the CMD line type with the function \ref ARM_MCI_Control. 
The CMD line mode is push-pull (default) or open drain (needed for older MMC).

\def ARM_MCI_BUS_CMD_PUSH_PULL       
\def ARM_MCI_BUS_CMD_OPEN_DRAIN      
@}
*******************************************************************************************************************/


/**
\defgroup mci_driver_strength_ctrls MCI Driver Strength
\ingroup mci_control_gr
\brief Specify the driver strength.
\details
@{
The function \ref ARM_MCI_Control with \em control = \ref ARM_MCI_DRIVER_STRENGTH specifies with \em arg the driver type of the SD interface.

\sa
  - \ref mci_bus_speed_ctrls

The following codes are defined:

\def ARM_MCI_DRIVER_TYPE_A      
\def ARM_MCI_DRIVER_TYPE_B      
\def ARM_MCI_DRIVER_TYPE_C      
\def ARM_MCI_DRIVER_TYPE_D      
@}
*******************************************************************************************************************/

/**
@}
*/   // close group mci_control_gr

/**
\defgroup mci_send_command_flags_ctrls MCI Send Command Flags
\ingroup mci_interface_gr
\brief Specify various options for sending commands to the card and the expected response.
\details
\b ARM_MCI_xxx flags are sent with the function \ref ARM_MCI_SendCommand as the parameter \em flag. 
It controls the behavior of the command sent to the card and provides information about the expected response from the card.

The following codes are defined:
@{
\def ARM_MCI_RESPONSE_NONE           
\def ARM_MCI_RESPONSE_SHORT          
\def ARM_MCI_RESPONSE_SHORT_BUSY     
\def ARM_MCI_RESPONSE_LONG           
\def ARM_MCI_RESPONSE_INDEX          
\def ARM_MCI_RESPONSE_CRC            
\def ARM_MCI_WAIT_BUSY             
\def ARM_MCI_TRANSFER_DATA         
\def ARM_MCI_CARD_INITIALIZE       
\def ARM_MCI_INTERRUPT_COMMAND     
\def ARM_MCI_INTERRUPT_RESPONSE    
\def ARM_MCI_BOOT_OPERATION        
\def ARM_MCI_BOOT_ALTERNATIVE      
\def ARM_MCI_BOOT_ACK              
\def ARM_MCI_CCSD                  
\def ARM_MCI_CCS                   
@}
*******************************************************************************************************************/

/**
\defgroup mci_transfer_ctrls MCI Transfer Controls
\ingroup mci_interface_gr
\brief  Specify data transfer mode.
\details
Data transfer codes specifies the transfer direction and type and are used with the function \ref ARM_MCI_SetupTransfer as the parameter \em mode.

The following codes are defined:
@{
\def ARM_MCI_TRANSFER_READ          
\def ARM_MCI_TRANSFER_WRITE         
\def ARM_MCI_TRANSFER_BLOCK         
\def ARM_MCI_TRANSFER_STREAM        
@}
*******************************************************************************************************************/

/**
\defgroup mci_card_power_ctrls MCI Card Power Controls
\ingroup mci_interface_gr
\brief Specify Memory Card Power supply voltage
\details
Specifies the power supply volatge for a memory card. Used with the function \ref ARM_MCI_CardPower as the parameter \em voltage.

The following codes are defined:
@{
\def ARM_MCI_POWER_VDD_OFF      
\def ARM_MCI_POWER_VDD_3V3      
\def ARM_MCI_POWER_VDD_1V8      
\def ARM_MCI_POWER_VCCQ_OFF     
\def ARM_MCI_POWER_VCCQ_3V3     
\def ARM_MCI_POWER_VCCQ_1V8     
\def ARM_MCI_POWER_VCCQ_1V2     
@}
*******************************************************************************************************************/


/**
\struct   ARM_MCI_STATUS
\details 
Structure with information about the status of the MCI.

<b>Returned by:</b>
  - \ref ARM_MCI_GetStatus
*******************************************************************************************************************/

/**
\typedef    ARM_MCI_SignalEvent_t
\details
Provides the typedef for the callback function \ref ARM_MCI_SignalEvent.

<b>Parameter for:</b>
  - \ref ARM_MCI_Initialize
*******************************************************************************************************************/


//
//   Functions 
//
ARM_DRIVER_VERSION ARM_MCI_GetVersion (void)  {
  return { 0, 0 };
}
/**
\fn       ARM_DRIVER_VERSION ARM_MCI_GetVersion (void)
\details
The function \b ARM_MCI_GetVersion returns version information of the driver implementation in \ref ARM_DRIVER_VERSION
 - API version is the version of the CMSIS-Driver specification used to implement this driver.
 - Driver version is source code version of the actual driver implementation.

Example:
\code
extern ARM_DRIVER_MCI Driver_MCI0;
ARM_DRIVER_MCI *drv_info;
 
void setup_mci (void)  {
  ARM_DRIVER_VERSION  version;
 
  drv_info = &Driver_MCI0;
  version = drv_info->GetVersion ();
  if (version.api < 0x10A)   {      // requires at minimum API version 1.10 or higher
    // error handling
    return;
  }
}
\endcode
*******************************************************************************************************************/

ARM_MCI_CAPABILITIES ARM_MCI_GetCapabilities (void)  {
  return { 0 };
}
/**
\fn       ARM_MCI_CAPABILITIES ARM_MCI_GetCapabilities (void)
\details
The function \b ARM_MCI_GetCapabilities returns information about capabilities in this driver implementation.
The data fields of the structure \ref ARM_MCI_CAPABILITIES encode various capabilities, for example
supported bus modes ...
 
Example:
\code
extern ARM_DRIVER_MCI Driver_MCI0;
ARM_DRIVER_MCI *drv_info;
  
void read_capabilities (void)  {
  ARM_MCI_CAPABILITIES drv_capabilities;
 
  drv_info = &Driver_MCI0;  
  drv_capabilities = drv_info->GetCapabilities ();
  // interrogate capabilities
 
}
\endcode
*******************************************************************************************************************/

int32_t ARM_MCI_Initialize (ARM_MCI_SignalEvent_t cb_event)  {
  return ARM_DRIVER_OK;
}
/**
\fn       int32_t ARM_MCI_Initialize (ARM_MCI_SignalEvent_t cb_event)
\details
The function \b ARM_MCI_Initialize initializes the MCI interface. 
It is called when the middleware component starts operation.

The function performs the following operations:
  - Initializes the resources needed for the MCI interface.
  - Registers the \ref ARM_MCI_SignalEvent callback function.

The parameter \em cb_event is a pointer to the \ref ARM_MCI_SignalEvent callback function; use a NULL pointer 
when no callback signals are required.

\b Example:
 - see \ref mci_interface_gr - Driver Functions

*******************************************************************************************************************/

int32_t ARM_MCI_Uninitialize (void)  {
  return ARM_DRIVER_OK;
}
/**
\fn       int32_t ARM_MCI_Uninitialize (void)
\details
The function \b ARM_MCI_Uninitialize de-initializes the resources of I2C interface.

It is called when the middleware component stops operation and releases the software resources used by the interface.
*******************************************************************************************************************/

int32_t ARM_MCI_PowerControl (ARM_POWER_STATE state)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_MCI_PowerControl (ARM_POWER_STATE state)
\details     
The function \b ARM_MCI_PowerControl operates the power modes of the MCI interface.  

The parameter \em state can have the following values:
  - \ref ARM_POWER_FULL : set-up peripheral for data transfers, enable interrupts (NVIC) and optionally DMA. Can be called multiple times. 
                          If the peripheral is already in this mode, then the function performs no operation and returns with \ref ARM_DRIVER_OK.
  - \ref ARM_POWER_LOW : may use power saving. Returns \ref ARM_DRIVER_ERROR_UNSUPPORTED when not implemented.
  - \ref ARM_POWER_OFF : terminates any pending data transfers, disables peripheral, disables related interrupts and DMA.
      
Refer to \ref CallSequence for more information.
*******************************************************************************************************************/

int32_t ARM_MCI_CardPower (uint32_t voltage)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_MCI_CardPower (uint32_t voltage)
\details
The function \b ARM_MCI_CardPower operates the memory card power supply voltage. 

The parameter \em voltage sets the voltage. Not every voltage might be supported by the driver implementation.
The structure \ref ARM_MCI_CAPABILITIES encodes the supported voltage. Retrieve the information with the function \ref ARM_MCI_GetCapabilities and
verify the data fields.

The following values:

Parameter \em voltage                 | Description                   | supported when ARM_MCI_CAPABILITIES
:-------------------------------------|:------------------------------|-----------------------------------------
\ref ARM_MCI_POWER_VDD_OFF            | VDD (VCC) turned off          | <i>always supported</i>
\ref ARM_MCI_POWER_VDD_3V3            | VDD (VCC) = \token{3.3V}      | data field \em vdd = \token{1}
\ref ARM_MCI_POWER_VDD_1V8            | VDD (VCC) = \token{1.8V}      | data field \em vdd_1v8 = \token{1} 
\ref ARM_MCI_POWER_VCCQ_OFF           | eMMC VCCQ turned off          | <i>always supported</i>
\ref ARM_MCI_POWER_VCCQ_3V3           | eMMC VCCQ = \token{3.3V}      | data field \em vccq = \token{1}
\ref ARM_MCI_POWER_VCCQ_1V8           | eMMC VCCQ = \token{1.8V}      | data field \em vccq_1v8 = \token{1}
\ref ARM_MCI_POWER_VCCQ_1V2           | eMMC VCCQ = \token{1.2V}      | data field \em vccq_1v2 = \token{1}

*******************************************************************************************************************/

int32_t ARM_MCI_ReadCD (void)  {
  return 0;
}
/**
\fn int32_t ARM_MCI_ReadCD (void)
\details
The function \b ARM_MCI_ReadCD reads the status of the Card Detect (CD) pin.
*******************************************************************************************************************/

int32_t ARM_MCI_ReadWP (void)  {
  return 0;
}
/**
\fn int32_t ARM_MCI_ReadWP (void)
\details
The function \b ARM_MCI_ReadWP reads the status of the Write Protect (WP) pin.
*******************************************************************************************************************/

int32_t ARM_MCI_SendCommand (uint32_t cmd, uint32_t arg, uint32_t flags, uint32_t *response)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_MCI_SendCommand (uint32_t cmd, uint32_t arg, uint32_t flags, uint32_t *response)
\details
The function \b ARM_MCI_SendCommand 
 - sends commands to the memory card 
 - retrieve the response from the card 
 - optionally, start the data transfer.

The parameter \em cmd is the command sent to the card. \n
The parameter \em arg contains arguments for the command \em cmd. \n
The parameter \em flags controls the behavior of the operation and takes predefined values listed in the table below. \n
The parameter \em response is a pointer to receive data.

The parameter \em flags can have the following values:

Parameter \em flags                   | Description
:-------------------------------------|:------------
\ref ARM_MCI_RESPONSE_NONE            | No response expected (default)
\ref ARM_MCI_RESPONSE_SHORT           | Short response (\token{48}-bit) expected
\ref ARM_MCI_RESPONSE_SHORT_BUSY      | Short response with busy signal (\token{48}-bit) expected
\ref ARM_MCI_RESPONSE_LONG            | Long response (\token{136}-bit) expected
\ref ARM_MCI_RESPONSE_INDEX           | Check command index in response
\ref ARM_MCI_RESPONSE_CRC             | Check CRC in response
\ref ARM_MCI_WAIT_BUSY                | Wait until busy before sending the command
\ref ARM_MCI_TRANSFER_DATA            | Activate Data transfer
\ref ARM_MCI_CARD_INITIALIZE          | Execute Memory Card initialization sequence
\ref ARM_MCI_INTERRUPT_COMMAND        | Send Interrupt command (CMD40 - MMC only)
\ref ARM_MCI_INTERRUPT_RESPONSE       | Send Interrupt response (CMD40 - MMC only)
\ref ARM_MCI_BOOT_OPERATION           | Execute Boot operation (MMC only)
\ref ARM_MCI_BOOT_ALTERNATIVE         | Execute Alternative Boot operation (MMC only)
\ref ARM_MCI_BOOT_ACK                 | Expect Boot Acknowledge (MMC only)
\ref ARM_MCI_CCSD                     | Send Command Completion Signal Disable (CCSD) for CE-ATA device
\ref ARM_MCI_CCS                      | Expect Command Completion Signal (CCS) for CE-ATA device

Calling the function <b>ARM_MCI_SendCommand</b> only starts the operation.
The function is non-blocking and returns as soon as the driver has started the operation.
It is not allowed to call this function again until the operation is in progress.

After the command is sent the response is retrieved if specified with <b>ARM_MCI_RESPONSE_xxx</b> flags.
When the command completes successfully (requested response is received without errors) the \ref ARM_MCI_EVENT_COMMAND_COMPLETE event is generated.
In case that response is requested but not received the \ref ARM_MCI_EVENT_COMMAND_TIMEOUT event is generated instead.
In case of invalid response (or CRC error) the \ref ARM_MCI_EVENT_COMMAND_ERROR event is generated instead.
Progress of command operation can be monitored by calling the \ref ARM_MCI_GetStatus and checking the \em command_active flag.

After the command operation the data transfer operation is started if specified with <b>ARM_MCI_TRANSFER_DATA</b> flag.
The data transfer needs to be configured before that by calling the \ref ARM_MCI_SetupTransfer.
When the data transfer completes successfully the \ref ARM_MCI_EVENT_TRANSFER_COMPLETE event is generated.
In case that data transfer is not completed in-time (specified by \ref ARM_MCI_DATA_TIMEOUT) the \ref ARM_MCI_EVENT_TRANSFER_TIMEOUT event is generated instead.
In case of CRC errors the \ref ARM_MCI_EVENT_TRANSFER_ERROR event is generated instead.
Progress of data transfer operation can be monitored by calling the \ref ARM_MCI_GetStatus and checking the \em transfer_active flag.

<b>See also:</b>
 - \ref ARM_MCI_SignalEvent
*******************************************************************************************************************/

int32_t ARM_MCI_SetupTransfer (uint8_t  *data, uint32_t block_count, uint32_t block_size, uint32_t mode)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_MCI_SetupTransfer (uint8_t  *data, uint32_t block_count, uint32_t block_size, uint32_t mode) 
\details
The function \b ARM_MCI_SetupTransfer prepares the data transfer operation that is initiated 
by calling the function \ref ARM_MCI_SendCommand with the parameter \em flags = \ref ARM_MCI_TRANSFER_DATA.

The parameter \em data is a pointer to the data to transfer. \n
The parameter \em block_count is the number of blocks to transfer. \n
The parameter \em block_size is the size of a block. \n
The parameter \em mode sets the transfer mode and can have the values liste in the table below:

Transfer Directions                   | Description
:-------------------------------------|:------------
\ref ARM_MCI_TRANSFER_READ            | Read data from MCI
\ref ARM_MCI_TRANSFER_WRITE           | Write data to MCI
\ref ARM_MCI_TRANSFER_BLOCK (default) | Block Data transfer 
\ref ARM_MCI_TRANSFER_STREAM          | Stream Data transfer (MMC only)
                                    
*******************************************************************************************************************/

int32_t ARM_MCI_AbortTransfer (void)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_MCI_AbortTransfer (void)
\details
The function \b ARM_MCI_AbortTransfer aborts the active data transfer operation initiated with \ref ARM_MCI_SendCommand.
*******************************************************************************************************************/


int32_t ARM_MCI_Control (uint32_t control, uint32_t arg)  {
  return ARM_DRIVER_OK;
}
/**
\fn            int32_t ARM_MCI_Control (uint32_t control, uint32_t arg)
\details
Th function \b ARM_MCI_Control controls the MCI interface and executes various operations.

The parameter \em control specifies the operation.  
Values for \em control cannot be ORed, but must be called separately in the code. \n
The parameter \em arg provides, depending on the operation, additional information or sets values.

\note
For parameters, the values marked with (default) are the setting after the driver initialization.

The table lists values for the parameter \em control.

Parameter \em control                 | Operation
:-------------------------------------|:------------
\ref ARM_MCI_BUS_SPEED                | Set the Bus Speed. The parameter \em arg specifies the speed in bits/s; The function returns the bus speed configured in bits/s.
\ref ARM_MCI_BUS_SPEED_MODE           | Set the Bus Speed Mode. Predefined values for \em arg are listed in the table <b>Bus Speed Mode</b>.
\ref ARM_MCI_BUS_CMD_MODE             | Set the CMD Line Mode. Predefined values for \em arg are listed in the table <b>Bus CMD Line Mode</b>.
\ref ARM_MCI_BUS_DATA_WIDTH           | Set data bus width. Predefined values for \em arg are encoded in <b>Bus Data Width</b>.
\ref ARM_MCI_DRIVER_STRENGTH          | Set driver strength. Predefined values for \em arg are listed in the table <b>Driver Type</b>
\ref ARM_MCI_CONTROL_RESET            | Control optional RST_n Pin (eMMC). The parameter \em arg can have the values \token{[0:inactive(default); 1:active]}
\ref ARM_MCI_CONTROL_CLOCK_IDLE       | Control clock generation on CLK Pin when idle. The parameter \em arg  can have the values \token{[0:disabled; 1:enabled]}
\ref ARM_MCI_UHS_TUNING_OPERATION     | Sampling clock Tuning operation (SD UHS-I). The parameter \em arg  can have the values  \token{[0:reset; 1:execute]}
\ref ARM_MCI_UHS_TUNING_RESULT        | Sampling clock Tuning result (SD UHS-I). Returns \token{[0:done; 1:in progress; -1:error]}
\ref ARM_MCI_DATA_TIMEOUT             | Set Data timeout;  The parameter \em arg sets the timeout in bus cycles.
\ref ARM_MCI_CSS_TIMEOUT              | Set Command Completion Signal (CCS) timeout. The parameter \em arg sets timeout in bus cycles.
\ref ARM_MCI_MONITOR_SDIO_INTERRUPT   | Monitor SD I/O interrupt. The parameter \em arg  can have the values \token{[0:disabled(default); 1:enabled]}. Monitoring is automatically disabled when an interrupt is recognized.
\ref ARM_MCI_CONTROL_READ_WAIT        | Control Read/Wait states for SD I/O. The parameter \em arg  can have the values \token{[0:disabled(default); 1:enabled]}.
\ref ARM_MCI_SUSPEND_TRANSFER         | Suspend Data transfer (SD I/O). Returns the number of remaining bytes to transfer.
\ref ARM_MCI_RESUME_TRANSFER          | Resume Data transfer (SD I/O).
 
 
<b>Bus Speed Mode</b>
 
The function \ref ARM_MCI_GetCapabilities lists the supported bus speed modes. Initially, all SD cards use a 3.3 volt electrical interface. 
Some SD cards can switch to 1.8 volt operation. For example, the use of ultra-high-speed (UHS) 
SD cards requires 1.8 volt operation and a 4-bit bus data width. The bit field ARM_MCI_CAPABILITIES.uhs_signaling encodes 
whether the driver supports 1.8 volt UHS signaling.

The \em control operation \b ARM_MCI_BUS_SPEED_MODE  sets the bus speed mode using the parameter \em arg.

Parameter \em arg                                             | Bus Speed Mode
:-------------------------------------------------------------|:------------------------------------------
\ref  ARM_MCI_BUS_DEFAULT_SPEED (default)                     | Set the bus speed for SD/MMC cards: Default Speed mode up to \token{[25;26]MHz} 
\ref  ARM_MCI_BUS_HIGH_SPEED                                  | Set the bus speed for SD/MMC: High    Speed mode up to \token{[50;52]MHz}
\ref  ARM_MCI_BUS_UHS_SDR12                                   | Set the bus speed for SD: SDR12  (Single Data Rate) up to  \token{25MHz,  12.5MB/s: UHS-I (Ultra High Speed) 1.8V signalling}
\ref  ARM_MCI_BUS_UHS_SDR25                                   | Set the bus speed for SD: SDR25  (Single Data Rate) up to  \token{50MHz,  25  MB/s: UHS-I (Ultra High Speed) 1.8V signalling}
\ref  ARM_MCI_BUS_UHS_SDR50                                   | Set the bus speed for SD: SDR50  (Single Data Rate) up to \token{100MHz,  50  MB/s: UHS-I (Ultra High Speed) 1.8V signalling}
\ref  ARM_MCI_BUS_UHS_SDR104                                  | Set the bus speed for SD: SDR104 (Single Data Rate) up to \token{208MHz, 104  MB/s: UHS-I (Ultra High Speed) 1.8V signalling}
\ref  ARM_MCI_BUS_UHS_DDR50                                   | Set the bus speed for SD: DDR50  (Dual Data Rate)   up to  \token{50MHz,  50  MB/s: UHS-I (Ultra High Speed) 1.8V signalling}
 
 
<b>Bus CMD Line Mode</b> 
 
The \em control operation \b ARM_MCI_BUS_CMD_MODE sets the bus command line mode using the parameter \em arg.

Parameter \em arg                                             | Bus CMD Line Mode  
:-------------------------------------------------------------|:------------------------------------------
\ref ARM_MCI_BUS_CMD_PUSH_PULL  (default)                     | Set the Push-Pull CMD line
\ref ARM_MCI_BUS_CMD_OPEN_DRAIN                               | Set the Open Drain CMD line (MMC only)
 
 
<b>Bus Data Width</b> 
 
Specifies the bus data width (the number of data I/O pins on the SD/MMC interface).
 
For high speed memory cards, a 4-bit bus data width should be used (or 8-bit for eMMC). The bit fields ARM_MCI_CAPABILITIES.data_width_4 and
ARM_MCI_CAPABILITIES.data_width_8 encode whether the driver supports a specific bus data with.

The \em control operation \b ARM_MCI_BUS_DATA_WIDTH  sets the bus data width using the parameter \em arg.

Parameter \em arg                                             | Bus Data Width
:-------------------------------------------------------------|:------------------------------------------
\ref ARM_MCI_BUS_DATA_WIDTH_1 (default)                       | Set the Bus data width to \token{1 bit} 
\ref ARM_MCI_BUS_DATA_WIDTH_4                                 | Set the Bus data width to \token{4 bits}
\ref ARM_MCI_BUS_DATA_WIDTH_8                                 | Set the Bus data width to \token{8 bits}
\ref ARM_MCI_BUS_DATA_WIDTH_4_DDR                             | Set the Bus data width to \token{4 bits}, DDR (Dual Data Rate) - MMC only
\ref ARM_MCI_BUS_DATA_WIDTH_8_DDR                             | Set the Bus data width to \token{8 bits}, DDR (Dual Data Rate) - MMC only
 
 
<b>Driver Type</b> 
 
Specifies the interface driver type.

The \em control operation \b ARM_MCI_DRIVER_STRENGTH  sets the interface driver type using the parameter \em arg.

Parameter \em arg                                             | Driver Type 
:-------------------------------------------------------------|:------------------------------------------
\ref ARM_MCI_DRIVER_TYPE_A                                    | Set the interface to SD UHS-I Driver Type A
\ref ARM_MCI_DRIVER_TYPE_B  (default)                         | Set the interface to SD UHS-I Driver Type B 
\ref ARM_MCI_DRIVER_TYPE_C                                    | Set the interface to SD UHS-I Driver Type C
\ref ARM_MCI_DRIVER_TYPE_D                                    | Set the interface to SD UHS-I Driver Type D

\b Examples:
\code
// Set Bus Speed to 25MHz
MCIdrv->Control(ARM_MCI_BUS_SPEED, 25000000);
 
// Set High Speed mode
MCIdrv->Control(ARM_MCI_BUS_SPEED_MODE, ARM_MCI_BUS_HIGH_SPEED);
 
// Configure CMD line as Open Drain (MMC only)
MCIdrv->Control(ARM_MCI_BUS_CMD_MODE, ARM_MCI_BUS_CMD_OPEN_DRAIN);
 
// Set Bus Data Width = 4bits
MCIdrv->Control(ARM_MCI_BUS_DATA_WIDTH, ARM_MCI_BUS_DATA_WIDTH_4);
 
// Set SD UHS-I Driver Type B
MCIdrv->Control(ARM_MCI_DRIVER_STRENGTH, ARM_MCI_DRIVER_TYPE_B);
 
// RTS_n Pin is not active by default
// Assert RTS_n Pin (eMMC)
MCIdrv->Control(ARM_MCI_CONTROL_RESET, 1);
// De-assert RTS_n Pin (eMMC)
MCIdrv->Control(ARM_MCI_CONTROL_RESET, 0);
 
// Clock generation on CLK when Idle: hardware specific default behavior
// Enable Clock generation on CLK when Idle
MCIdrv->Control(ARM_MCI_CONTROL_CLOCK_IDLE, 1);
// Disable Clock generation on CLK when Idle
MCIdrv->Control(ARM_MCI_CONTROL_CLOCK_IDLE, 0);
 
// UHS Tuning
MCIdrv->Control(ARM_MCI_UHS_TUNING_OPERATION, 1);  // start tuning
do {
  status = MCIdrv->Control(ARM_MCI_UHS_TUNING_RESULT, 0/*argument not used*/);
  if (status == -1) { break; /* tuning failed */ }
} while (status == 1);
 
// Set Data Timeout to 12500000 bus cycles (0.5s @25MHz Bus Speed)
// Default value is hardware specific (typically 2^32-1)
MCIdrv->Control(ARM_MCI_DATA_TIMEOUT, 12500000);
  
// Set CSS Timeout to 1000000 bus cycles
// Default value is hardware specific
MCIdrv->Control(ARM_MCI_CSS_TIMEOUT, 1000000);
 
// SD I/O Interrupt Monitoring is disabled by default
// Enable SD I/O Interrupt Monitoring
MCIdrv->Control(ARM_MCI_MONITOR_SDIO_INTERRUPT, 1);
// Disable SD I/O Interrupt Monitoring
MCIdrv->Control(ARM_MCI_MONITOR_SDIO_INTERRUPT, 0);
 
// Read/Wait for SD I/O is disabled by default
// Enable Read/Wait for SD I/O
MCIdrv->Control(ARM_MCI_CONTROL_READ_WAIT, 1);
// Disable Read/Wait for SD I/O
MCIdrv->Control(ARM_MCI_CONTROL_READ_WAIT, 0);
 
// Suspend Data transfer (SD I/O)
MCIdrv->Control(ARM_MCI_SUSPEND_TRANSFER, 0/*argument not used*/);
 
// Resume Data transfer (SD I/O)
MCIdrv->Control(ARM_MCI_RESUME_TRANSFER, 0/*argument not used*/);
\endcode
*******************************************************************************************************************/

ARM_MCI_STATUS ARM_MCI_GetStatus (void)  {
  return ARM_DRIVER_OK;
}
/**
\fn            ARM_MCI_STATUS ARM_MCI_GetStatus (void)
\details
The function \b ARM_MCI_GetStatus returns the current MCI interface status.
*******************************************************************************************************************/

void ARM_MCI_SignalEvent (uint32_t event)  {
 // function body
}
/**
\fn void ARM_MCI_SignalEvent (uint32_t event)
\details
The function \b ARM_MCI_SignalEvent is a callback function registered by the function \ref ARM_MCI_Initialize. 

The parameter \em event indicates one or more events that occurred during driver operation.
Each event is encoded in a separate bit and therefore it is possible to signal multiple events within the same call. 

Not every event is necessarily generated by the driver. This depends on the implemented capabilities stored in the 
data fields of the structure \ref ARM_NAND_CAPABILITIES, which can be retrieved with the function \ref ARM_NAND_GetCapabilities.

The following events can be generated:

Parameter \em event                        |Bit | Description                                                           | supported when \ref ARM_NAND_CAPABILITIES
:------------------------------------------|---:|:----------------------------------------------------------------------|:---------------------------------------------
\ref ARM_MCI_EVENT_CARD_INSERTED           | 0  | Occurs after Memory Card inserted                                     | <i>always supported</i>
\ref ARM_MCI_EVENT_CARD_REMOVED            | 1  | Occurs after Memory Card removal                                      | <i>always supported</i>
\ref ARM_MCI_EVENT_COMMAND_COMPLETE        | 2  | Occurs after command completed successfully                           | <i>always supported</i>
\ref ARM_MCI_EVENT_COMMAND_TIMEOUT         | 3  | Occurs after command timeout                                          | <i>always supported</i>
\ref ARM_MCI_EVENT_COMMAND_ERROR           | 4  | Occurs after command response error (CRC error or invalid response)   | <i>always supported</i>
\ref ARM_MCI_EVENT_TRANSFER_COMPLETE       | 5  | Occurs after data transfer completed successfully                     | <i>always supported</i>
\ref ARM_MCI_EVENT_TRANSFER_TIMEOUT        | 6  | Occurs after data transfer timeout                                    | <i>always supported</i>
\ref ARM_MCI_EVENT_TRANSFER_ERROR          | 7  | Occurs after data transfer error (CRC failed)                         | <i>always supported</i>
\ref ARM_MCI_EVENT_SDIO_INTERRUPT          | 8  | Indicates SD I/O Interrupt                                            | data field \em sdio_interrupt = \token{1}
\ref ARM_MCI_EVENT_CCS                     | 9  | Indicates a Command Completion Signal (CCS)                           | data field \em ccs = \token{1}
\ref ARM_MCI_EVENT_CCS_TIMEOUT             |10  | Indicates a Command Completion Signal (CCS) Timeout                   | data field \em css_timeout = \token{1}

<b>See also:</b>
 - \ref ARM_MCI_SendCommand
 
*******************************************************************************************************************/


/**
@}
*/ 
// End MCI Interface
