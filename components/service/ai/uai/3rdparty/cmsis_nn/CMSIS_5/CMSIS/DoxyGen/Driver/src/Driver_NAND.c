/**
\defgroup nand_interface_gr NAND Interface
\brief    Driver API for NAND Flash Device Interface (%Driver_NAND.h).

\details
<b>NAND</b> devices are a type of non-volatile storage and do not require power to hold data. 
Wikipedia offers more information about 
the <a href="http://en.wikipedia.org/wiki/Flash_memory#ARM_NAND_memories" target="_blank"><b>Flash Memories</b></a>, including NAND.

<b>Block Diagram</b>

<p>&nbsp;</p>
\image html NAND_Schematics.png "Simplified NAND Flash Schematic"
<p>&nbsp;</p>


<b>NAND API</b>

The following header files define the Application Programming Interface (API) for the NAND interface:
  - \b %Driver_NAND.h : Driver API for NAND Flash Device Interface

The driver implementation is a typical part of the Device Family Pack (DFP) that supports the 
peripherals of the microcontroller family.

NAND Flash is organized in pages, grouped into blocks as the smallest erasable unit. The addressing
of data is archieved by `byte_address = block * block_size + page_in_block * page_size + offset_in_page`.
In terms of this NAND API blocks and pages are referrd to as `row` and the byte offset within the page as `col`.
Thus one can calculate the `byte_address = row * page_size + col`. The parameters `page_size` and `block_size` 
are device specific and must be handled by the driver user appropriately.

<b>Driver Functions</b>

The driver functions are published in the access struct as explained in \ref DriverFunctions
  - \ref ARM_DRIVER_NAND : access struct for NAND driver functions

@{
\anchor example <b>Example Code:</b>

\include NAND_Demo.c
*/
/*******************************************************************************************************************/


/**
\defgroup nand_execution_status Status Error Codes
\ingroup common_drv_gr
\brief Negative values indicate errors (NAND has specific codes in addition to common \ref execution_status). 
\details 
The NAND driver has additional status error codes that are listed below.
Note that the NAND driver also returns the common \ref execution_status. 
  
@{
\def ARM_NAND_ERROR_ECC
ECC generation or correction failed during \ref ARM_NAND_ReadData, \ref ARM_NAND_WriteData or \ref ARM_NAND_ExecuteSequence.
@}
*/


/**
\defgroup NAND_events NAND Events
\ingroup nand_interface_gr
\brief The NAND driver generates call back events that are notified via the function \ref ARM_NAND_SignalEvent.
\details 
This section provides the event values for the \ref ARM_NAND_SignalEvent callback function.

The following call back notification events are generated:
@{
\def ARM_NAND_EVENT_DEVICE_READY
\def ARM_NAND_EVENT_DRIVER_READY
\def ARM_NAND_EVENT_DRIVER_DONE 
\def ARM_NAND_EVENT_ECC_ERROR   
@}
*/


/**
\defgroup nand_driver_flag_codes NAND Flags
\ingroup nand_interface_gr
\brief Specify Flag codes.
\details
The defines can be used in the function \ref ARM_NAND_ReadData and \ref ARM_NAND_WriteData for the parameter \em mode
and in the function \ref ARM_NAND_ExecuteSequence for the parameter \em code.
@{
\def ARM_NAND_DRIVER_DONE_EVENT   
@}
*/


/**
\defgroup nand_control_gr NAND Control Codes
\ingroup nand_interface_gr
\brief Many parameters of the NAND driver are configured using the \ref ARM_NAND_Control function.
@{
\details 
Refer to the function \ref ARM_NAND_Control for further details.
*/

/**
\defgroup nand_control_codes NAND Mode Controls
\ingroup nand_control_gr
\brief Specify operation modes of the NAND interface.
\details
These controls can be used in the function \ref ARM_NAND_Control for the parameter \em control.
@{
\def ARM_NAND_BUS_MODE             
\def ARM_NAND_BUS_DATA_WIDTH       
\def ARM_NAND_DRIVER_STRENGTH      
\def ARM_NAND_DEVICE_READY_EVENT   
\def ARM_NAND_DRIVER_READY_EVENT   
@}
*/

/**
\defgroup nand_bus_mode_codes NAND Bus Modes
\ingroup nand_control_gr
\brief Specify bus mode of the NAND interface.
\details
The defines can be used in the function \ref ARM_NAND_Control for the parameter \em arg and with the \ref ARM_NAND_BUS_MODE as the \em control code.
@{
\def ARM_NAND_BUS_SDR               
\def ARM_NAND_BUS_DDR               
\def ARM_NAND_BUS_DDR2              
\def ARM_NAND_BUS_TIMING_MODE_0     
\def ARM_NAND_BUS_TIMING_MODE_1     
\def ARM_NAND_BUS_TIMING_MODE_2     
\def ARM_NAND_BUS_TIMING_MODE_3     
\def ARM_NAND_BUS_TIMING_MODE_4     
\def ARM_NAND_BUS_TIMING_MODE_5     
\def ARM_NAND_BUS_TIMING_MODE_6     
\def ARM_NAND_BUS_TIMING_MODE_7     
\def ARM_NAND_BUS_DDR2_DO_WCYC_0    
\def ARM_NAND_BUS_DDR2_DO_WCYC_1    
\def ARM_NAND_BUS_DDR2_DO_WCYC_2    
\def ARM_NAND_BUS_DDR2_DO_WCYC_4    
\def ARM_NAND_BUS_DDR2_DI_WCYC_0    
\def ARM_NAND_BUS_DDR2_DI_WCYC_1    
\def ARM_NAND_BUS_DDR2_DI_WCYC_2    
\def ARM_NAND_BUS_DDR2_DI_WCYC_4    
\def ARM_NAND_BUS_DDR2_VEN          
\def ARM_NAND_BUS_DDR2_CMPD         
\def ARM_NAND_BUS_DDR2_CMPR         
@}
*/

/**
\defgroup nand_data_bus_width_codes NAND Data Bus Width
\ingroup nand_control_gr
\brief Specify data bus width of the NAND interface.
\details
The defines can be used in the function \ref ARM_NAND_Control for the parameter \em arg and with the \ref ARM_NAND_BUS_DATA_WIDTH as the \em control code.
@{
\def ARM_NAND_BUS_DATA_WIDTH_8   
\def ARM_NAND_BUS_DATA_WIDTH_16  
@}
*/

/**
\defgroup nand_driver_strength_codes NAND Driver Strength
\ingroup nand_control_gr
\brief Specify driver strength of the NAND interface.
\details
The defines can be used in the function \ref ARM_NAND_Control for the parameter \em arg and with the \ref ARM_NAND_DRIVER_STRENGTH as the \em control code.
@{
\def ARM_NAND_DRIVER_STRENGTH_18 
\def ARM_NAND_DRIVER_STRENGTH_25 
\def ARM_NAND_DRIVER_STRENGTH_35 
\def ARM_NAND_DRIVER_STRENGTH_50 
@}
*/

/**
@}
*/


/**
\defgroup nand_driver_ecc_codes NAND ECC Codes
\ingroup nand_interface_gr
\brief Specify ECC codes.
\details
The defines can be used in the function \ref ARM_NAND_ReadData and \ref ARM_NAND_WriteData for the parameter \em mode
and in the function \ref ARM_NAND_ExecuteSequence for the parameter \em code.
@{
\def ARM_NAND_ECC(n)
\def ARM_NAND_ECC0
\def ARM_NAND_ECC1
@}
*/


/**
\defgroup nand_driver_seq_exec_codes NAND Sequence Execution Codes
\ingroup nand_interface_gr
\brief Specify execution codes
\details
The defines can be used in the function \ref ARM_NAND_ExecuteSequence for the parameter \em code.
@{
\def ARM_NAND_CODE_SEND_CMD1       
\def ARM_NAND_CODE_SEND_ADDR_COL1  
\def ARM_NAND_CODE_SEND_ADDR_COL2  
\def ARM_NAND_CODE_SEND_ADDR_ROW1  
\def ARM_NAND_CODE_SEND_ADDR_ROW2  
\def ARM_NAND_CODE_SEND_ADDR_ROW3  
\def ARM_NAND_CODE_INC_ADDR_ROW    
\def ARM_NAND_CODE_WRITE_DATA      
\def ARM_NAND_CODE_SEND_CMD2       
\def ARM_NAND_CODE_WAIT_BUSY       
\def ARM_NAND_CODE_READ_DATA       
\def ARM_NAND_CODE_SEND_CMD3       
\def ARM_NAND_CODE_READ_STATUS     
@}
*/


/*------------   Structures --------------------------------------------------------------------------------------*/
/**
\struct     ARM_NAND_STATUS
\details
Structure with information about the status of a NAND. The data fields encode flags for the driver.

<b>Returned by:</b>
  - \ref ARM_NAND_GetStatus
*****************************************************************************************************************/

/**
\struct     ARM_DRIVER_NAND 
\details
The functions of the NAND driver are accessed by function pointers exposed by this structure. Refer to \ref DriverFunctions for overview information.

Each instance of a NAND interface provides such an access structure. 
The instance is identified by a postfix number in the symbol name of the access structure, for example:
 - \b Driver_NAND0 is the name of the access struct of the first instance (no. 0).
 - \b Driver_NAND1 is the name of the access struct of the second instance (no. 1).

A middleware configuration setting allows connecting the middleware to a specific driver instance <b>Driver_NAND<i>n</i></b>.
The default is \token{0}, which connects a middleware to the first instance of a driver.
*******************************************************************************************************************/

/**
\struct     ARM_NAND_CAPABILITIES 
\details
A NAND driver can be implemented with different capabilities. The data fields of this struct encode 
the capabilities implemented by this driver.

<b>Returned by:</b>
  - \ref ARM_NAND_GetCapabilities
*******************************************************************************************************************/


/**
\typedef    ARM_NAND_SignalEvent_t
\details
Provides the typedef for the callback function \ref ARM_NAND_SignalEvent.

<b>Parameter for:</b>
  - \ref ARM_NAND_Initialize
*******************************************************************************************************************/


/**
\struct     ARM_NAND_ECC_INFO

\details
Stores the characteristics of a ECC (Error Correction Code) algorithm and provides the information about necessary
application data handling in order to protect stored data from NAND bit errors.

ECC algorithms applied on NAND memory typically operate on NAND device page level which is virtually divided to multiple
main and spare areas. Data from main and spare area is taken into account when generating ECC data which is also stored
into spare area. ECC codeword defines how much data will be protected and how much ECC data will be generated.

To describe how application data must be organized, ECC information structure specifies protection \em type which
defines the protected part of data. As main and spare are of different size, two different algorithms could be
provided, we can describe them as ECC0 and ECC1. Type can then have the following values:

Type| Description 
:---|:-----------
 0  | ECC algorithm not used
 1  | ECC0 algorithm protects main data
 2  | ECC0 algorithm protects main and spare data
 3  | ECC0 algorithm protects main and ECC1 algorithm protects spare data

Virtual page division is described with page layout (\em page_layout), number of pages (\em page_count) and
virtual page size (\em page_size or \em virtual_page_size). Virtual page size used by ECC algorithm can be defined
by either \em page_size or \em virtual_page_size, depending on the \em page_size values:

Value| Main + Spare size
:----|:-----------
 0   | 512 + 16
 1   | 1024 + 32
 2   | 2048 + 64
 3   | 4096 + 128
 4   | 8192 + 256
 8   | 512 + 28
 9   | 1024 + 56
 10  | 2048 + 112
 11  | 4096 + 224
 12  | 8192 + 448
 15  | Not used, use virtual_page_size

Structure member \em virtual_page_size is an array of two 16-bit values. First field of array (i.e. \em virtual_page_size[0])
contains main area size while second (i.e. \em virtual_page_size[1]) contains spare area size. Number of virtual pages N
is defined with \em page_count and must be calculated as N = 2 ^ page_count.

Page layout defines main and spare ordering and two different page layouts are possible. First ordering assumes that
spare area follows after every main area, while in second case all main areas build one contiguous region followed by
contiguous region of spare areas. This is defined by member \em page_layout:

Layout| Description 
:-----|:-----------
 0    | Single spare follows after single main: Main0,Spare0 ... MainN-1,SpareN-1
 1    | Contiguous spare follows after contiguous main: Main0 ... MainN-1,Spare0 ... SpareN-1

ECC codeword size defines the size of data that is protected by ECC algorithm and is different for main and spare
area. All structure members that define the codeword are therefore arrays of two 16-bit values. Codeword offset defines
where ECC protected data starts in main (\em codeword_offset[0]) or spare (\em codeword_offset[1]) area, codeword
size (\em codeword_size) defines the number of data that is protected i.e. data over which ECC is calculated and
codeword gap (\em codeword_gap) defines the space between two consecutive codeword regions.

Generated ECC data is stored into spare area and is described similar as codeword, with offset from start of spare area
(\em ecc_offset), size of generated data (\em ecc_size) and gap (\em ecc_gap) between two consecutive ECC data regions.

Number of bits that ECC algorithm can correct per codeword is defined with \em correctable_bits.

<b>Parameter for:</b>
  - \ref ARM_NAND_InquireECC
*****************************************************************************************************************/


//
//  Functions 
//

ARM_DRIVER_VERSION ARM_NAND_GetVersion (void)  {
  return { 0, 0 };
}
/**
\fn       ARM_DRIVER_VERSION ARM_NAND_GetVersion (void)
\details
The function \b ARM_NAND_GetVersion returns version information of the driver implementation in \ref ARM_DRIVER_VERSION
 - API version is the version of the CMSIS-Driver specification used to implement this driver.
 - Driver version is source code version of the actual driver implementation.

Example:
\code
extern ARM_DRIVER_NAND Driver_NAND0;
ARM_DRIVER_NAND *drv_info;
 
void setup_nand (void)  {
  ARM_DRIVER_VERSION  version;
 
  drv_info = &Driver_NAND0;  
  version = drv_info->GetVersion ();
  if (version.api < 0x10A)   {      // requires at minimum API version 1.10 or higher
    // error handling
    return;
  }
}
\endcode
*******************************************************************************************************************/

ARM_NAND_CAPABILITIES ARM_NAND_GetCapabilities (void)  {
  return { 0 };
}
/**
\fn       ARM_NAND_CAPABILITIES ARM_NAND_GetCapabilities (void)
\details
The function \b ARM_NAND_GetCapabilities retrieves information about capabilities in this driver implementation.
The data fields of the structure \ref ARM_NAND_CAPABILITIES encode various capabilities, for example
if a hardware is able to create signal events using the \ref ARM_NAND_SignalEvent 
callback function.
 
Example:
\code
extern ARM_DRIVER_NAND Driver_NAND0;
ARM_DRIVER_NAND *drv_info;
  
void read_capabilities (void)  {
  ARM_NAND_CAPABILITIES drv_capabilities;
 
  drv_info = &Driver_NAND0;  
  drv_capabilities = drv_info->GetCapabilities ();
  // interrogate capabilities
 
}
\endcode
*******************************************************************************************************************/

int32_t ARM_NAND_Initialize (ARM_NAND_SignalEvent_t  cb_event)  {
  return 0;
}
/**
\fn int32_t ARM_NAND_Initialize (ARM_NAND_SignalEvent_t  cb_event)
\details
The function \b ARM_NAND_Initialize initializes the NAND interface. 
It is called when the middleware component starts operation.

The function performs the following operations:
  - Initializes the resources needed for the NAND interface.
  - Registers the \ref ARM_NAND_SignalEvent callback function.

The parameter \em cb_event is a pointer to the \ref ARM_NAND_SignalEvent callback function; use a NULL pointer 
when no callback signals are required.

\b Example:
 - see \ref nand_interface_gr - Driver Functions

*******************************************************************************************************************/

int32_t ARM_NAND_Uninitialize (void)  {
  return 0;
}
/**
\fn int32_t ARM_NAND_Uninitialize (void)
\details
The function \b ARM_NAND_Uninitialize de-initializes the resources of NAND interface.

It is called when the middleware component stops operation and releases the software resources used by the interface.
*******************************************************************************************************************/

int32_t ARM_NAND_PowerControl (ARM_POWER_STATE state)  {
  return 0;
}
/**
\fn int32_t ARM_NAND_PowerControl (ARM_POWER_STATE state)
\details
The function \b ARM_NAND_PowerControl controls the power modes of the NAND interface.  

The parameter \em state sets the operation and can have the following values:
  - \ref ARM_POWER_FULL : set-up peripheral for data transfers, enable interrupts (NVIC) and optionally DMA. 
                          Can be called multiple times. If the peripheral is already in this mode the function performs 
						  no operation and returns with \ref ARM_DRIVER_OK.
  - \ref ARM_POWER_LOW : may use power saving. Returns \ref ARM_DRIVER_ERROR_UNSUPPORTED when not implemented.
  - \ref ARM_POWER_OFF : terminates any pending data transfers, disables peripheral, disables related interrupts and DMA.
      
Refer to \ref CallSequence for more information.
*******************************************************************************************************************/


int32_t ARM_NAND_DevicePower (uint32_t voltage)  {
  return 0;
}
/**
\fn int32_t ARM_NAND_DevicePower (uint32_t voltage)
\details
The function \b ARM_NAND_DevicePower controls the power supply of the NAND device.

The parameter \em voltage sets the device supply voltage as defined in the table.

\b AMR_NAND_POWER_xxx_xxx specifies power settings.

Device Power Bits                | Description
:--------------------------------|:--------------------------------------------
\ref ARM_NAND_POWER_VCC_OFF      | Set VCC Power off
\ref ARM_NAND_POWER_VCC_3V3      | Set VCC = 3.3V
\ref ARM_NAND_POWER_VCC_1V8      | Set VCC = 1.8V
\ref ARM_NAND_POWER_VCCQ_OFF     | Set VCCQ I/O Power off
\ref ARM_NAND_POWER_VCCQ_3V3     | Set VCCQ = 3.3V
\ref ARM_NAND_POWER_VCCQ_1V8     | Set VCCQ = 1.8V
\ref ARM_NAND_POWER_VPP_OFF      | Set VPP off
\ref ARM_NAND_POWER_VPP_ON       | Set VPP on

*******************************************************************************************************************/

int32_t ARM_NAND_WriteProtect (uint32_t dev_num, bool enable)  {
  return 0;
}
/**
\fn int32_t ARM_NAND_WriteProtect (uint32_t dev_num, bool enable)
\details
The function \b ARM_NAND_WriteProtect controls the Write Protect (WPn) pin of a NAND device.

The parameter \em dev_num is the device number. \n 
The parameter \em enable specifies whether to enable or disable write protection.
*******************************************************************************************************************/

int32_t ARM_NAND_ChipEnable (uint32_t dev_num, bool enable)  {
  return 0;
}
/**
\fn int32_t ARM_NAND_ChipEnable (uint32_t dev_num, bool enable)
\details
The function \b ARM_NAND_ChipEnable control the Chip Enable (CEn) pin of a NAND device.

The parameter \em dev_num is the device number. \n 
The parameter \em enable specifies whether to enable or disable the device.

This function is optional and supported only when the data field \em ce_manual = \token{1} in the structure \ref ARM_NAND_CAPABILITIES.
Otherwise, the Chip Enable (CEn) signal is controlled automatically by SendCommand/Address, Read/WriteData and ExecuteSequence 
(for example when the NAND device is connected to a memory bus).
*******************************************************************************************************************/

int32_t ARM_NAND_GetDeviceBusy (uint32_t dev_num)  {
  return 0;
}
/**
\fn int32_t ARM_NAND_GetDeviceBusy (uint32_t dev_num)
\details
The function \b ARM_NAND_GetDeviceBusy returns the status of the Device Busy pin: [\token{1=busy; 0=not busy or error}].

The parameter \em dev_num is the device number.
*******************************************************************************************************************/

int32_t ARM_NAND_SendCommand (uint32_t dev_num, uint8_t cmd)  {
  return 0;
}
/**
\fn int32_t ARM_NAND_SendCommand (uint32_t dev_num, uint8_t cmd)
\details
The function \b ARM_NAND_SendCommand sends a command to the NAND device.

The parameter \em dev_num is the device number. \n
The parameter \em cmd is the command sent to the NAND device.
*******************************************************************************************************************/

int32_t ARM_NAND_SendAddress (uint32_t dev_num, uint8_t addr)  {
  return 0;
}
/**
\fn int32_t ARM_NAND_SendAddress (uint32_t dev_num, uint8_t addr)
\details
Send an address to the NAND device.
The parameter \em dev_num is the device number.
The parameter \em addr is the address.
*******************************************************************************************************************/

int32_t ARM_NAND_ReadData (uint32_t dev_num, void *data, uint32_t cnt, uint32_t mode)  {
  return 0;
}
/**
\fn int32_t ARM_NAND_ReadData (uint32_t dev_num, void *data, uint32_t cnt, uint32_t mode)
\details
The function \b ARM_NAND_ReadData reads data from a NAND device.

The parameter \em dev_num is the device number. \n
The parameter \em data is a pointer to the buffer that stores the data read from a NAND device. \n
The parameter \em cnt is the number of data items to read. \n
The parameter \em mode defines the operation mode as listed in the table below.

Read Data Mode                     | Description
:----------------------------------|:--------------------------------------------
\ref ARM_NAND_ECC(n)               | Select ECC
\ref ARM_NAND_ECC0                 | Use ECC0 of selected ECC
\ref ARM_NAND_ECC1                 | Use ECC1 of selected ECC
\ref ARM_NAND_DRIVER_DONE_EVENT    | Generate \ref ARM_NAND_EVENT_DRIVER_DONE

The data item size is defined by the data type, which depends on the configured data bus width.

Data type is:
 - \em uint8_t for 8-bit data bus
 - \em uint16_t for 16-bit data bus

The function executes in the following ways:
 - When the operation is blocking (typical for devices connected to memory bus when not using DMA), 
   then the function returns after all data is read and returns the number of data items read.
 - When the operation is non-blocking (typical for NAND controllers), then the function only starts the operation and returns with zero number of data items read.
   After the operation is completed, the \ref ARM_NAND_EVENT_DRIVER_DONE event is generated (if enabled by \b ARM_NAND_DRIVER_DONE_EVENT).
   Progress of the operation can also be monitored by calling the \ref ARM_NAND_GetStatus function and checking the \em busy data field.
   Operation is automatically aborted if ECC is used and ECC correction fails, which generates the \ref ARM_NAND_EVENT_ECC_ERROR event 
   (together with \ref ARM_NAND_DRIVER_DONE_EVENT if enabled).

*******************************************************************************************************************/

int32_t ARM_NAND_WriteData (uint32_t dev_num, const void *data, uint32_t cnt, uint32_t mode)  {
  return 0;
}
/**
\fn int32_t ARM_NAND_WriteData (uint32_t dev_num, const void *data, uint32_t cnt, uint32_t mode)
\details
The function \b ARM_NAND_WriteData writes data to a NAND device.

The parameter \em dev_num is the device number. \n
The parameter \em data is a pointer to the buffer with data to write. \n
The parameter \em cnt is the number of data items to write. \n
The parameter \em mode defines the operation mode as listed in the table below.

Write Data Mode                    | Description
:----------------------------------|:--------------------------------------------
\ref ARM_NAND_ECC(n)               | Select ECC
\ref ARM_NAND_ECC0                 | Use ECC0 of selected ECC
\ref ARM_NAND_ECC1                 | Use ECC1 of selected ECC
\ref ARM_NAND_DRIVER_DONE_EVENT    | Generate \ref ARM_NAND_EVENT_DRIVER_DONE

The data item size is defined by the data type, which depends on the configured data bus width.

Data type is:
 - \em uint8_t for 8-bit data bus
 - \em uint16_t for 16-bit data bus

The function executes in the following ways:
 - When the operation is blocking (typical for devices connected to memory bus when not using DMA), 
   then the function returns after all data is written and returns the number of data items written.
 - When the operation is non-blocking (typical for NAND controllers), then the function only starts the operation 
   and returns with zero number of data items written. After the operation is completed, 
   the \ref ARM_NAND_EVENT_DRIVER_DONE event is generated (if enabled by \b ARM_NAND_DRIVER_DONE_EVENT).
   Progress of the operation can also be monitored by calling the \ref ARM_NAND_GetStatus function and checking the \em busy data field.
   Operation is automatically aborted if ECC is used and ECC generation fails, 
   which generates the \ref ARM_NAND_EVENT_ECC_ERROR event (together with \ref ARM_NAND_DRIVER_DONE_EVENT if enabled).
*******************************************************************************************************************/

int32_t ARM_NAND_ExecuteSequence (uint32_t dev_num, uint32_t code, uint32_t cmd,
                                  uint32_t addr_col, uint32_t addr_row,
                                  void *data, uint32_t data_cnt,
                                  uint8_t *status, uint32_t *count)  {
  return 0;
}
/**
\fn int32_t ARM_NAND_ExecuteSequence (uint32_t dev_num, uint32_t code, uint32_t cmd, uint32_t addr_col, uint32_t addr_row, void *data, uint32_t data_cnt, uint8_t *status, uint32_t *count)
\details
The function \b ARM_NAND_ExecuteSequence executes a sequence of operations for a NAND device.

The parameter \em dev_num is the device number. \n
The parameter \em code is the sequence encoding as defined in the table <b>Sequence execution Code</b>. \n
The parameter \em cmd is the command or a series of commands. \n
The parameter \em addr_col is the column address. \n
The parameter \em addr_row is the row address. \n
The parameter \em data is a pointer to the buffer that stores the data to or loads the data from. \n
The parameter \em data_cnt is the number of data items to read or write in one iteration. \n
The parameter \em status is a pointer to the buffer that stores the status read. \n
The parameter \em count is a pointer to the number of iterations. \n

\b ARM_NAND_CODE_xxx specifies sequence execution codes.

Sequence Execution Code            | Description
:----------------------------------|:--------------------------------------------
\ref ARM_NAND_CODE_SEND_CMD1       | Send Command 1 (cmd[7..0])
\ref ARM_NAND_CODE_SEND_ADDR_COL1  | Send Column Address 1 (addr_col[7..0])
\ref ARM_NAND_CODE_SEND_ADDR_COL2  | Send Column Address 2 (addr_col[15..8])
\ref ARM_NAND_CODE_SEND_ADDR_ROW1  | Send Row Address 1 (addr_row[7..0])
\ref ARM_NAND_CODE_SEND_ADDR_ROW2  | Send Row Address 2 (addr_row[15..8])
\ref ARM_NAND_CODE_SEND_ADDR_ROW3  | Send Row Address 3 (addr_row[23..16])
\ref ARM_NAND_CODE_INC_ADDR_ROW    | Auto-increment Row Address
\ref ARM_NAND_CODE_WRITE_DATA      | Write Data
\ref ARM_NAND_CODE_SEND_CMD2       | Send Command 2 (cmd[15..8])
\ref ARM_NAND_CODE_WAIT_BUSY       | Wait while R/Bn busy
\ref ARM_NAND_CODE_READ_DATA       | Read Data
\ref ARM_NAND_CODE_SEND_CMD3       | Send Command 3 (cmd[23..16])
\ref ARM_NAND_CODE_READ_STATUS     | Read Status byte and check FAIL bit (bit 0)
\ref ARM_NAND_ECC(n)               | Select ECC
\ref ARM_NAND_ECC0                 | Use ECC0 of selected ECC
\ref ARM_NAND_ECC1                 | Use ECC1 of selected ECC
\ref ARM_NAND_DRIVER_DONE_EVENT    | Generate \ref ARM_NAND_EVENT_DRIVER_DONE

The data item size is defined by the data type, which depends on the configured data bus width.

Data type is:
 - \em uint8_t for 8-bit data bus
 - \em uint16_t for 16-bit data bus

The function is non-blocking and returns as soon as the driver has started executing the specified sequence.
When the operation is completed, the \ref ARM_NAND_EVENT_DRIVER_DONE event is generated (if enabled by \b ARM_NAND_DRIVER_DONE_EVENT).
Progress of the operation can also be monitored by calling the \ref ARM_NAND_GetStatus function and checking the \em busy data field.

Driver executes the number of specified iterations where in each iteration 
items specified by \b ARM_NAND_CODE_xxx are executed in the order as listed in the table <b>Sequence execution Code</b>.
The parameter \em count is holding the current number of iterations left.

Execution is automatically aborted and \ref ARM_NAND_EVENT_DRIVER_DONE event is generated (if enabled by \b ARM_NAND_DRIVER_DONE_EVENT):
 - if Read Status is enabled and the FAIL bit (bit 0) is set
 - if ECC is used and ECC fails (also sets \ref ARM_NAND_EVENT_ECC_ERROR event)

\note
\ref ARM_NAND_CODE_WAIT_BUSY can only be specified if the Device Ready event can be generated (reported by \em event_device_ready in \ref ARM_NAND_CAPABILITIES).
The event \ref ARM_NAND_EVENT_DEVICE_READY is not generated during sequence execution but rather used internally by the driver.
*******************************************************************************************************************/

int32_t ARM_NAND_AbortSequence (uint32_t dev_num)  {
  return 0;
}
/**
\fn int32_t ARM_NAND_AbortSequence (uint32_t dev_num)
\details
The function \b ARM_NAND_AbortSequence aborts execution of the current sequence for a NAND device.

The parameter \em dev_num is the device number.
*******************************************************************************************************************/

int32_t ARM_NAND_Control (uint32_t dev_num, uint32_t control, uint32_t arg)  {
  return 0;
}
/**
\fn int32_t ARM_NAND_Control (uint32_t dev_num, uint32_t control, uint32_t arg)
\details
The function \b ARM_NAND_Control controls the NAND interface and executes operations.

The parameter \em dev_num is the device number. \n
The parameter \em control specifies the operation. \n
The parameter \em arg provides (depending on the \em control) additional information or sets values.

The table lists the operations for the parameter \em control.

Parameter \em control            | Operation
:--------------------------------|:--------------------------------------------
\ref ARM_NAND_BUS_MODE           | Set the bus mode. The parameter \em arg sets the \ref bus_mode_tab "\b Bus Mode".
\ref ARM_NAND_BUS_DATA_WIDTH     | Set the data bus width. The parameter \em arg sets the \ref bus_data_width_tab "\b Bus Data Width".
\ref ARM_NAND_DRIVER_STRENGTH    | Set the driver strength. The parameter \em arg sets the \ref driver_strength_tab "\b Driver Strength".
\ref ARM_NAND_DRIVER_READY_EVENT | Control generation of callback event \ref ARM_NAND_EVENT_DRIVER_READY. Enable: \em arg = \token{1}. Disable: \em arg = \token{0}.
\ref ARM_NAND_DEVICE_READY_EVENT | Control generation of callback event \ref ARM_NAND_EVENT_DEVICE_READY; Enable: \em arg = \token{1}. Disable: \em arg = \token{0}.

<b>See Also</b>
- \ref ARM_NAND_GetCapabilities returns information about supported operations, which are stored in the structure \ref ARM_NAND_CAPABILITIES.
- \ref ARM_NAND_SignalEvent provides information about the callback events \ref ARM_NAND_EVENT_DRIVER_READY and \ref ARM_NAND_EVENT_DEVICE_READY

The table lists values for the parameter \em arg used with the \em control operation \ref ARM_NAND_BUS_MODE, \ref ARM_NAND_BUS_DATA_WIDTH, and
\ref ARM_NAND_DRIVER_STRENGTH. Values from different categories can be ORed.

\anchor bus_mode_tab
<table class="cmtable" summary="">
<tr><th> Parameter \em arg  <br> for <i>control</i> = \ref ARM_NAND_BUS_MODE   </th>
    <th> Bit                  </th>
	<th> Category             </th>
    <th> Description          </th>
    <th width="30%"> Supported when \ref ARM_NAND_CAPABILITIES            </th></tr>
<tr><td> \ref ARM_NAND_BUS_TIMING_MODE_0 (default)            </td>
    <td rowspan="8" style="text-align:right"> 0..3            </td>
    <td rowspan="8"> \anchor bus_timing_tab  Bus Timing Mode  </td>
	<td>             \token{0}                                </td>
    <td rowspan="8"> The maximum timing mode that can be applied to a specific \ref bus_data_interface_tab  "\b Bus Data Interface"
	                 is stored in the data fields: <br><br>
					 <i>sdr_timing_mode</i> - for SDR <br>
					 <i>ddr_timing_mode</i> - for NV-DDR <br>
					 <i>ddr2_timing_mode</i> - for NV_DDR2                                 </td></tr>
<tr><td> \ref ARM_NAND_BUS_TIMING_MODE_1            </td><td>  \token{1}                    </td></tr>
<tr><td> \ref ARM_NAND_BUS_TIMING_MODE_2            </td><td>  \token{2}                    </td></tr>
<tr><td> \ref ARM_NAND_BUS_TIMING_MODE_3            </td><td>  \token{3}                    </td></tr>
<tr><td> \ref ARM_NAND_BUS_TIMING_MODE_4            </td><td>  \token{4} (SDR EDO capable)  </td></tr>
<tr><td> \ref ARM_NAND_BUS_TIMING_MODE_5            </td><td>  \token{5} (SDR EDO capable)  </td></tr>
<tr><td> \ref ARM_NAND_BUS_TIMING_MODE_6            </td><td>  \token{6} (NV-DDR2 only)     </td></tr>
<tr><td> \ref ARM_NAND_BUS_TIMING_MODE_7            </td><td>  \token{7} (NV-DDR2 only)     </td></tr>
<tr><td> \ref ARM_NAND_BUS_SDR (default)      \anchor bus_data_interface_tab      </td>
    <td rowspan="3" style="text-align:right"> 4..7  </td>
    <td rowspan="3">   Bus Data Interface   </td>
	<td>             SDR  (Single Data Rate) - Traditional interface      </td>
    <td>     <i>always supported</i>                </td></tr>
<tr><td> \ref ARM_NAND_BUS_DDR                      </td><td>  NV-DDR  (Double Data Rate)  </td><td>  data field <i>ddr</i> = \token{1} </td></tr>
<tr><td> \ref ARM_NAND_BUS_DDR2                     </td><td>  NV-DDR2 (Double Data Rate)  </td><td>  data field <i>ddr2</i> = \token{1} </td></tr>
<tr><td style="white-space: nowrap"> \ref ARM_NAND_BUS_DDR2_DO_WCYC_0 (default) </td>
    <td rowspan="4" style="text-align:right"> 8..11 </td>
    <td rowspan="4" style="white-space: nowrap"> Data Output Warm-up   \anchor bus_output_tab  </td>
	<td> Set the DDR2 Data Output Warm-up to \token{0} cycles      </td>
    <td rowspan="4">  <b>Data Output Warm-up</b> cycles are dummy cycles for interface calibration with no incremental data transfer
                      and apply to NV-DDR2 of the \ref bus_data_interface_tab "\b Bus Data Interface".	
	</td></tr>
<tr><td> \ref ARM_NAND_BUS_DDR2_DO_WCYC_1           </td><td> Set the DDR2 Data Output Warm-up to \token{1} cycles </td></tr>
<tr><td> \ref ARM_NAND_BUS_DDR2_DO_WCYC_2           </td><td> Set the DDR2 Data Output Warm-up to \token{2} cycles </td></tr>
<tr><td> \ref ARM_NAND_BUS_DDR2_DO_WCYC_4           </td><td> Set the DDR2 Data Output Warm-up to \token{4} cycles </td></tr>
<tr><td style="white-space: nowrap"> \ref ARM_NAND_BUS_DDR2_DI_WCYC_0 (default) \anchor bus_input_tab</td>
    <td rowspan="4" style="text-align:right"> 12..15 </td>
    <td rowspan="4" style="white-space: nowrap">   Data Input Warm-up   </td>
	<td> Set the DDR2 Data Input Warm-up to \token{0} cycles      </td>
    <td rowspan="4">  <b>Data Input Warm-up</b> cycles are dummy cycles for interface calibration with no incremental data transfer
                      and apply to NV-DDR2 of the \ref bus_data_interface_tab "\b Bus Data Interface".	
	</td></tr>
<tr><td> \ref ARM_NAND_BUS_DDR2_DI_WCYC_1           </td><td> Set the DDR2 Data Input Warm-up to \token{1} cycles </td></tr>
<tr><td> \ref ARM_NAND_BUS_DDR2_DI_WCYC_2           </td><td> Set the DDR2 Data Input Warm-up to \token{2} cycles </td></tr>
<tr><td> \ref ARM_NAND_BUS_DDR2_DI_WCYC_4           </td><td> Set the DDR2 Data Input Warm-up to \token{4} cycles </td></tr>
<tr><td style="white-space: nowrap"> \ref ARM_NAND_BUS_DDR2_VEN \anchor bus_misc_tab </td>
    <td style="text-align:right"> 16 </td>
    <td rowspan="3" style="white-space: nowrap">   Miscellaneous   </td>
	<td> Set the DDR2 Enable external VREFQ as reference      </td>
    <td rowspan="3">  &nbsp;
	</td></tr>
<tr><td> \ref ARM_NAND_BUS_DDR2_CMPD    </td><td style="text-align:right"> 17 </td><td> Set the DDR2 Enable complementary DQS (DQS_c) signal </td></tr>
<tr><td> \ref ARM_NAND_BUS_DDR2_CMPR    </td><td style="text-align:right"> 18 </td><td> Set the DDR2 Enable complementary RE_n (RE_c) signal </td></tr>
<tr><th> Parameter \em arg  <br> for <i>control</i> = \ref ARM_NAND_BUS_DATA_WIDTH   </th>
    <th> Bit                  </th>
	<th> Category      \anchor bus_data_width_tab       </th>
    <th> Description          </th>
    <th width="30%"> Supported when \ref ARM_NAND_CAPABILITIES            </th></tr>
<tr><td style="white-space: nowrap"> \ref ARM_NAND_BUS_DATA_WIDTH_8 (default) </td>
    <td rowspan="2" style="text-align:right"> 0..1 </td>
    <td rowspan="2" style="white-space: nowrap">   Bus Data Width        </td>
	<td> Set to \token{8 bit}        </td>
    <td>  <i>always supported</i>
	</td></tr>
<tr><td> \ref ARM_NAND_BUS_DATA_WIDTH_16  </td><td> Set to \token{16 bit}    </td><td> data field <i>data_width_16</i> = \token{1}   </td></tr>
<tr><th style="white-space: nowrap"> Parameter \em arg  <br> for <i>control</i> = \ref ARM_NAND_DRIVER_STRENGTH   </th>
    <th> Bit                  </th>
	<th> Category   \anchor driver_strength_tab        </th>
    <th> Description          </th>
    <th width="30%"> Supported when \ref ARM_NAND_CAPABILITIES            </th></tr>
<tr><td style="white-space: nowrap"> \ref ARM_NAND_DRIVER_STRENGTH_18 </td>
    <td rowspan="4" style="text-align:right"> 0..3 </td>
    <td rowspan="4" style="white-space: nowrap"> Driver Strength     </td>
	<td> Set the Driver Strength 2.0x = 18 Ohms     </td>
    <td> data field <i>driver_strength_18</i> = \token{1} 
	</td></tr>
<tr><td> \ref ARM_NAND_DRIVER_STRENGTH_25            </td><td> Set the Driver Strength 1.4x = 25 Ohms </td><td> data field <i>driver_strength_25</i> = \token{1} </td></tr>
<tr><td> \ref ARM_NAND_DRIVER_STRENGTH_35 (default)  </td><td> Set the Driver Strength 1.0x = 35 Ohms </td><td> <i>always supported</i>  </td></tr>
<tr><td> \ref ARM_NAND_DRIVER_STRENGTH_50            </td><td> Set the Driver Strength 0.7x = 50 Ohms </td><td> data field <i>driver_strength_50</i> = \token{1} </td></tr>
</table>

<b>Example</b>
\code
extern ARM_DRIVER_NAND Driver_NAND0;
 
status = Driver_NAND0.Control (0, ARM_NAND_BUS_MODE, ARM_NAND_BUS_TIMING_MODE_5 | 
                                                     ARM_NAND_BUS_DDR2          | 
                                                     ARM_NAND_BUS_DDR2_VEN);
											    
status = Driver_NAND0.Control (0, ARM_NAND_BUS_DATA_WIDTH,  ARM_NAND_BUS_DATA_WIDTH_16); 
 
status = Driver_NAND0.Control (0, ARM_NAND_DRIVER_STRENGTH, ARM_NAND_DRIVER_STRENGTH_50);
\endcode

*******************************************************************************************************************/

ARM_NAND_STATUS ARM_NAND_GetStatus (uint32_t dev_num)  {
  return 0;
}
/**
\fn ARM_NAND_STATUS ARM_NAND_GetStatus (uint32_t dev_num)
\details
The function \b ARM_NAND_GetStatus returns the current NAND device status.

The parameter \em dev_num is the device number.
*******************************************************************************************************************/

int32_t ARM_NAND_InquireECC (int32_t index, ARM_NAND_ECC_INFO *info)  {
  return 0;
}
/**
\fn int32_t ARM_NAND_InquireECC (int32_t index, ARM_NAND_ECC_INFO *info)
\details
The function \b ARM_NAND_InquireECC reads error correction code information.

The parameter \em index is the ECC index and is used to retrieve different ECC configurations. \n
The parameter \em info is a pointer of type \ref ARM_NAND_ECC_INFO. The data fields store the information.

When multiple different ECC configurations exist, ARM_NAND_ECC_INFO structure exists for each configuration. Parameter
\em index denotes which configuration will be retrieved. Value of index should start with zero to retrieve first ECC
configuration and should be incremented in order to retrieve next ECC configuration. When index is out of range function
ARM_NAND_InquireECC returns with error.

Parameter \em index is used by \ref ARM_NAND_ECC(n) in \ref ARM_NAND_ReadData, \ref ARM_NAND_WriteData and
\ref ARM_NAND_ExecuteSequence to select suitable ECC configuration.

<b>Example</b>
\code
extern ARM_DRIVER_NAND Driver_NAND0; 

ARM_NAND_ECC_INFO ecc;
int32_t idx;
 
idx = 0;
while (Driver_NAND0.InquireECC (idx, &ecc) == ARM_DRIVER_OK) {
  // Examine retrieved ECC configuration
  if (ecc.type == 2) {
    // Algorithm ECC0 protects Main+Spare
  }
  // ..
  idx++;
}
\endcode
*******************************************************************************************************************/

void ARM_NAND_SignalEvent (uint32_t dev_num, uint32_t event)  {
  return 0;
}
/**
\fn void ARM_NAND_SignalEvent (uint32_t dev_num, uint32_t event)
\details
The function \b ARM_NAND_SignalEvent is a callback function registered by the function \ref ARM_NAND_Initialize.

The parameter \em dev_num is the device number. \n
The parameter \em event indicates one or more events that occurred during driver operation.
Each event is encoded in a separate bit and therefore it is possible to signal multiple events within the same call. 

Not every event is necessarily generated by the driver. This depends on the implemented capabilities stored in the 
data fields of the structure \ref ARM_NAND_CAPABILITIES, which can be retrieved with the function \ref ARM_NAND_GetCapabilities.

The following events can be generated:

Parameter \em event               | Bit | Description
:---------------------------------|-----|:---------------------------
\ref ARM_NAND_EVENT_DEVICE_READY  | 0   | Occurs when rising edge is detected on R/Bn (Ready/Busy) pin indicating that the device is ready.
\ref ARM_NAND_EVENT_DRIVER_READY  | 1   | Occurs to indicate that commands can be executed (after previously being busy and not able to start the requested operation).
\ref ARM_NAND_EVENT_DRIVER_DONE   | 2   | Occurs after an operation completes. An operation was successfully started before with \ref ARM_NAND_ReadData, \ref ARM_NAND_WriteData, \ref ARM_NAND_ExecuteSequence.
\ref ARM_NAND_EVENT_ECC_ERROR     | 3   | Occurs when ECC generation failed or ECC correction failed. An operation was successfully started before with \ref ARM_NAND_ReadData, \ref ARM_NAND_WriteData, \ref ARM_NAND_ExecuteSequence. 

The event \ref ARM_NAND_EVENT_DEVICE_READY occurs after complete execution of commands 
(initiated with the functions \ref ARM_NAND_SendCommand, \ref ARM_NAND_SendAddress, \ref ARM_NAND_ReadData, \ref ARM_NAND_WriteData, \ref ARM_NAND_ExecuteSequence).
It is useful to indicate completion of complex operations (such as erase). 
The event is only generated when \ref ARM_NAND_GetCapabilities returns data field \em event_device_ready = \token{1}
and was enabled by calling \ref ARM_NAND_Control (\ref ARM_NAND_DEVICE_READY_EVENT, 1).
If the event is not available, poll the \em busy data field using the function \ref ARM_NAND_GetStatus.

The event \ref ARM_NAND_EVENT_DRIVER_READY occurs when previously a function 
(\ref ARM_NAND_SendCommand, \ref ARM_NAND_SendAddress, \ref ARM_NAND_ReadData, \ref ARM_NAND_WriteData, \ref ARM_NAND_ExecuteSequence) 
returned with \ref ARM_DRIVER_ERROR_BUSY. It is useful when functions are called simultaneously from independent threads 
(for example to control multiple devices) and the threads have no knowledge about each other (driver rejects reentrant calls with return of \ref ARM_DRIVER_ERROR_BUSY). 
\em dev_num indicates the device that returned previously busy. 
*******************************************************************************************************************/

/**
@}
*/ 
// End NAND Interface
