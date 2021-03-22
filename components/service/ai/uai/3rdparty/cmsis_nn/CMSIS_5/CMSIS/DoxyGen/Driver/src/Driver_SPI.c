/**
\defgroup spi_interface_gr SPI Interface
\brief Driver API for SPI Bus Peripheral (%Driver_SPI.h)
\details 
The <b>Serial Peripheral Interface Bus</b> (SPI) implements a synchronous serial bus for data exchange. In microcontroller (MCU) applications,
the interface is often used to connect peripheral components at board (PCB) level. SPI devices can operate as Master (SCLK and SS are outputs) or 
Slave (SCLK and SS are inputs). Wikipedia offers more information about 
the <a href="http://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus" target="_blank"><b>Serial Peripheral Interface Bus</b></a>.

<b>Block Diagram</b>

The SPI Driver API defines a <b>SPI</b> interface for middleware components. The SPI Driver supports multiple
slaves, but if only one slave is connected, then the Slave Select signal can be omitted.


\image html SPI_Master1Slaves.png  "SPI Master connected to a single slave"


<p>&nbsp;</p>
\image html SPI_Master3Slaves.png  "SPI Master connected to 3 slaves"

The SPI Driver functions control the following SPI signal lines. 

Signal | Name                                | Description
-------|-------------------------------------|------------------------------------------------------------------------------
SS     | Slave Select (active low)           | Selects the slave. This signal can be part of the SPI peripheral or implemented using a GPIO pin.
MOSI   | Master&nbsp;Out,&nbsp;Slave&nbsp;In | MOSI output of the Master connects to MOSI input of the Slave.
SCLK   | Serial Clock                        | Serial clock output from Master. Controls the transfer speed and when data are sent and read.
MISO   | Master&nbsp;In,&nbsp;Slave&nbsp;Out | MISO input of the Master connects to MISO output of the Slave.


<b>SPI API</b>

The following header files define the Application Programming Interface (API) for the SPI interface:
  - \b %Driver_SPI.h : Driver API for SPI Bus Peripheral

The driver implementation is a typical part of the Device Family Pack (DFP) that supports the 
peripherals of the microcontroller family.


<b>Driver Functions</b>

The driver functions are published in the access struct as explained in \ref DriverFunctions
  - \ref ARM_DRIVER_SPI : access struct for SPI driver functions

  
<b>Example Code</b>

The following example code shows the usage of the SPI interface.

\include SPI_Demo.c
  
@{
*/


/**
\struct     ARM_DRIVER_SPI 
\details 
The functions of the SPI driver are accessed by function pointers exposed by this structure.
Refer to \ref DriverFunctions for overview information.

Each instance of a SPI interface provides such an access structure. 
The instance is identified by a postfix number in the symbol name of the access structure, for example:
 - \b Driver_SPI0 is the name of the access struct of the first instance (no. 0).
 - \b Driver_SPI1 is the name of the access struct of the second instance (no. 1).

A middleware configuration setting allows connecting the middleware to a specific driver instance \b %Driver_SPI<i>n</i>.
The default is \token{0}, which connects a middleware to the first instance of a driver.
**************************************************************************************************************************/

/** 
\struct     ARM_SPI_CAPABILITIES
\details
A SPI driver can be implemented with different capabilities.
The data fields of this structure encode the capabilities implemented by this driver.

<b>Returned by:</b>
  - \ref ARM_SPI_GetCapabilities
**************************************************************************************************************************/

/**
\struct     ARM_SPI_STATUS
\details
Structure with information about the status of the SPI. The data fields encode busy flag and error flags.

<b>Returned by:</b>
  - \ref ARM_SPI_GetStatus
*****************************************************************************************************************/


/**
\typedef    ARM_SPI_SignalEvent_t
\details
Provides the typedef for the callback function \ref ARM_SPI_SignalEvent.

<b>Parameter for:</b>
  - \ref ARM_SPI_Initialize
*******************************************************************************************************************/

/**
\defgroup spi_execution_status Status Error Codes
\ingroup common_drv_gr
\brief Negative values indicate errors (SPI has specific codes in addition to common \ref execution_status). 
\details 
The SPI driver has additional status error codes that are listed below.
Note that the SPI driver also returns the comon \ref execution_status. 
  
@{
\def ARM_SPI_ERROR_MODE
The \b mode requested with the function \ref ARM_SPI_Control is not supported by this driver.

\def ARM_SPI_ERROR_FRAME_FORMAT
The <b>frame format</b> requested with the function \ref ARM_SPI_Control is not supported by this driver.

\def ARM_SPI_ERROR_DATA_BITS
The number of <b>data bits</b> requested with the function \ref ARM_SPI_Control is not supported by this driver.

\def ARM_SPI_ERROR_BIT_ORDER
The <b>bit order</b> requested with the function \ref ARM_SPI_Control is not supported by this driver.

\def ARM_SPI_ERROR_SS_MODE
The <b>slave select mode</b> requested with the function \ref ARM_SPI_Control is not supported by this driver.
@}
*/

/**
\defgroup SPI_events SPI Events
\ingroup spi_interface_gr
\brief The SPI driver generates call back events that are notified via the function \ref ARM_SPI_SignalEvent.
\details 
This section provides the event values for the \ref ARM_SPI_SignalEvent callback function.

The following call back notification events are generated:
@{
\def  ARM_SPI_EVENT_TRANSFER_COMPLETE
\def  ARM_SPI_EVENT_DATA_LOST
\def  ARM_SPI_EVENT_MODE_FAULT
@}
*/

//
//  Functions
//

ARM_DRIVER_VERSION ARM_SPI_GetVersion (void)  {
  return { 0, 0 };
}
/**
\fn       ARM_DRIVER_VERSION ARM_SPI_GetVersion (void)
\details
The function \b ARM_SPI_GetVersion returns version information of the driver implementation in \ref ARM_DRIVER_VERSION
 - API version is the version of the CMSIS-Driver specification used to implement this driver.
 - Driver version is source code version of the actual driver implementation.

Example:
\code
extern ARM_DRIVER_SPI Driver_SPI0;
ARM_DRIVER_SPI *drv_info;
 
void setup_spi (void)  {
  ARM_DRIVER_VERSION  version;
 
  drv_info = &Driver_SPI0;  
  version = drv_info->GetVersion ();
  if (version.api < 0x10A)   {      // requires at minimum API version 1.10 or higher
    // error handling
    return;
  }
}
\endcode
**************************************************************************************************************************/

ARM_SPI_CAPABILITIES ARM_SPI_GetCapabilities (void)  {
  return { 0 };
}
/**
\fn       ARM_SPI_CAPABILITIES ARM_SPI_GetCapabilities (void)
\details
The function \b ARM_SPI_GetCapabilities returns information about the capabilities in this driver implementation.
The data fields of the structure \ref ARM_SPI_CAPABILITIES encode various capabilities, for example
supported modes.
 
Example:
\code
extern ARM_DRIVER_SPI Driver_SPI0;
ARM_DRIVER_SPI *drv_info;
  
void read_capabilities (void)  {
  ARM_SPI_CAPABILITIES drv_capabilities;
 
  drv_info = &Driver_SPI0;  
  drv_capabilities = drv_info->GetCapabilities ();
  // interrogate capabilities
 
}
\endcode

**************************************************************************************************************************/

int32_t ARM_SPI_Initialize (ARM_SPI_SignalEvent_t cb_event)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_SPI_Initialize (ARM_SPI_SignalEvent_t cb_event)
\details
The function \b ARM_SPI_Initialize initializes the SPI interface. 

The parameter \em cb_event is a pointer to the \ref ARM_SPI_SignalEvent callback function; use a NULL pointer 
when no callback signals are required.

The function is called when the middleware component starts operation and performs the following:
  - Initializes the resources needed for the SPI interface.
  - Registers the \ref ARM_SPI_SignalEvent callback function.


\b Example:
 - see \ref spi_interface_gr - Driver Functions

**************************************************************************************************************************/

int32_t ARM_SPI_Uninitialize (void)  {
  return ARM_DRIVER_OK;
}
/**
\fn       int32_t ARM_SPI_Uninitialize (void)
\details
The function \b ARM_SPI_Uninitialize de-initializes the resources of SPI interface.

It is called when the middleware component stops operation and releases the software resources used by the interface.
**************************************************************************************************************************/

int32_t ARM_SPI_PowerControl (ARM_POWER_STATE state)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_SPI_PowerControl (ARM_POWER_STATE state)
\details
The function \b ARM_SPI_PowerControl controls the power modes of the SPI interface.  

The parameter \em state sets the operation and can have the following values:
  - \ref ARM_POWER_FULL : set-up peripheral for data transfers, enable interrupts (NVIC) and optionally DMA. 
                          Can be called multiple times. If the peripheral is already in this mode the function performs 
                          no operation and returns with \ref ARM_DRIVER_OK.
  - \ref ARM_POWER_LOW : may use power saving. Returns \ref ARM_DRIVER_ERROR_UNSUPPORTED when not implemented.
  - \ref ARM_POWER_OFF : terminates any pending data transfers, disables peripheral, disables related interrupts and DMA.
      
Refer to \ref CallSequence for more information.
**************************************************************************************************************************/

int32_t ARM_SPI_Send (const void *data, uint32_t num)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_SPI_Send (const void *data, uint32_t num)
\details 
This function \b ARM_SPI_Send is used to send data to the SPI transmitter (received data is ignored).

The parameter \em data specifies the data buffer. \n
The parameter \em num specifies the number of items to send. \n
The item size is defined by the data type, which depends on the configured number of data bits.

Data type is:
 - \em uint8_t when configured for 1..8 data bits
 - \em uint16_t when configured for 9..16 data bits
 - \em uint32_t when configured for 17..32 data bits
 
Calling the function \b ARM_SPI_Send only starts the send operation.
When in slave mode, the operation is only registered and started when the master starts the transfer.
The function is non-blocking and returns as soon as the driver has started the operation 
(driver typically configures DMA or the interrupt system for continuous transfer).
During the operation it is not allowed to call this function or any other data transfer function again. 
Also the data buffer must stay allocated and the contents of unsent data must not be modified.
When send operation is completed (requested number of items sent), the \ref ARM_SPI_EVENT_TRANSFER_COMPLETE event is generated.
Progress of send operation can also be monitored by reading the number of items already sent by calling \ref ARM_SPI_GetDataCount. 

Status of the transmitter can also be monitored by calling the \ref ARM_SPI_GetStatus and checking the \em busy data field,
which indicates if transmission is still in progress or pending. 

When in master mode and configured to monitor slave select and the slave select gets deactivated during transfer,
then the SPI mode changes to inactive and the \ref ARM_SPI_EVENT_MODE_FAULT event is generated (instead of \ref ARM_SPI_EVENT_TRANSFER_COMPLETE).

When in slave mode but send/receive/transfer operation is not started and data is sent/requested by the master, 
then the \ref ARM_SPI_EVENT_DATA_LOST event is generated. 

Send operation can be aborted by calling \ref ARM_SPI_Control with \ref ARM_SPI_ABORT_TRANSFER as the control parameter.
**************************************************************************************************************************/

int32_t ARM_SPI_Receive (void *data, uint32_t num)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_SPI_Receive (void *data, uint32_t num)
\details 
The function \b ARM_SPI_Receive is used to receive data 
(transmits the default value as specified by \ref ARM_SPI_Control with \ref ARM_SPI_SET_DEFAULT_TX_VALUE as control parameter). 

The parameter \em data specifies the data buffer. \n
The parameter \em num specifies the number of items to receive. \n
The item size is defined by the data type, which depends on the configured number of data bits.

Data type is:
 - \em uint8_t when configured for 1..8 data bits
 - \em uint16_t when configured for 9..16 data bits
 - \em uint32_t when configured for 17..32 data bits
 
Calling the function \b ARM_SPI_Receive only starts the receive operation.
The function is non-blocking and returns as soon as the driver has started the operation 
(driver typically configures DMA or the interrupt system for continuous transfer).
When in slave mode, the operation is only registered and started when the master starts the transfer.
During the operation it is not allowed to call this function or any other data transfer function again. Also the data buffer must stay allocated.
When receive operation is completed (requested number of items received), the \ref ARM_SPI_EVENT_TRANSFER_COMPLETE event is generated.
Progress of receive operation can also be monitored by reading the number of items already received by calling \ref ARM_SPI_GetDataCount. 

Status of the receiver can also be monitored by calling the \ref ARM_SPI_GetStatus and checking the \em busy data field,
which indicates if reception is still in progress or pending. 

When in master mode and configured to monitor slave select and the slave select gets deactivated during transfer,
then the SPI mode changes to inactive and the \ref ARM_SPI_EVENT_MODE_FAULT event is generated (instead of \ref ARM_SPI_EVENT_TRANSFER_COMPLETE).

When in slave mode but send/receive/transfer operation is not started and data is sent/requested by the master, 
then the \ref ARM_SPI_EVENT_DATA_LOST event is generated. 

Receive operation can be aborted by calling \ref ARM_SPI_Control with \ref ARM_SPI_ABORT_TRANSFER as the control parameter.
**************************************************************************************************************************/

int32_t ARM_SPI_Transfer (const void *data_out, void *data_in, uint32_t num)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_SPI_Transfer (const void *data_out, void *data_in, uint32_t num)
\details 
The function \b ARM_SPI_Transfer transfers data via SPI. It synchronously sends data to the SPI transmitter and receives data from the SPI receiver. 

The parameter \em data_out is a pointer to the buffer with data to send. \n
The parameter \em data_in is a pointer to the buffer which receives data. \n
The parameter \em num specifies the number of items to transfer. \n
The item size is defined by the data type which depends on the configured number of data bits.

Data type is:
 - \em uint8_t when configured for 1..8 data bits
 - \em uint16_t when configured for 9..16 data bits
 - \em uint32_t when configured for 17..32 data bits
 
Calling the function \b ARM_SPI_Transfer only starts the transfer operation.
The function is non-blocking and returns as soon as the driver has started the operation 
(driver typically configures DMA or the interrupt system for continuous transfer).
When in slave mode, the operation is only registered and started when the master starts the transfer.
During the operation it is not allowed to call this function or any other data transfer function again. 
Also the data buffers must stay allocated and the contents of unsent data must not be modified.
When transfer operation is completed (requested number of items transferred), the \ref ARM_SPI_EVENT_TRANSFER_COMPLETE event is generated.
Progress of transfer operation can also be monitored by reading the number of items already transferred by calling \ref ARM_SPI_GetDataCount. 

Status of the transmitter and receiver can also be monitored by calling the \ref ARM_SPI_GetStatus and checking the \em busy flag. 

When in master mode and configured to monitor slave select and the slave select gets deactivated during transfer,
then the SPI mode changes to inactive and the \ref ARM_SPI_EVENT_MODE_FAULT event is generated (instead of \ref ARM_SPI_EVENT_TRANSFER_COMPLETE).

When in slave mode but send/receive/transfer operation is not started and data is sent/requested by the master,
 then the \ref ARM_SPI_EVENT_DATA_LOST event is generated. 

Transfer operation can also be aborted by calling \ref ARM_SPI_Control with \ref ARM_SPI_ABORT_TRANSFER as the control parameter.
**************************************************************************************************************************/

uint32_t ARM_SPI_GetDataCount (void)  {
  return 0;
}
/**
\fn uint32_t ARM_SPI_GetDataCount (void)
\details
The function \b ARM_SPI_GetDataCount returns the number of currently transferred data items 
during \ref ARM_SPI_Send, \ref ARM_SPI_Receive and \ref ARM_SPI_Transfer operation.
*****************************************************************************************************************/

int32_t ARM_SPI_Control (uint32_t control, uint32_t arg)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_SPI_Control (uint32_t control, uint32_t arg)
\details
The function \b ARM_SPI_Control controls the SPI interface settings and executes various operations.

The parameter \em control is a bit mask that specifies various operations. 
  - Controls form different categories can be ORed. 
  - If one control is omitted, then the default value of that category is used.
  - Miscellaneous controls cannot be combined.  

The parameter \em arg provides (depending on the parameter \em control) additional information, for example the Bus Speed.


<table class="cmtable" summary="">
<tr><th> Parameter \em control    </th>
    <th style="text-align:right"> Bit          </th>
    <th> Category      </th>
    <th> Description          
	</th></tr>
<tr><td> \ref ARM_SPI_MODE_INACTIVE       </td>
    <td rowspan="5" style="text-align:right"> 0..7   </td>
    <td rowspan="5"> \anchor spi_mode_tab Mode Controls    </td>
    <td> Set SPI to inactive. 
	</td></tr>
<tr><td> \ref ARM_SPI_MODE_MASTER         </td>
    <td> Set the SPI Master (Output on MOSI, and the Input on MISO); \em arg = Bus Speed in \token{bps} 
	</td></tr>
<tr><td> \ref ARM_SPI_MODE_MASTER_SIMPLEX </td>
    <td> Set the SPI Master (Output and Input on MOSI); \em arg = Bus Speed in \token{bps}              
	</td></tr>
<tr><td> \ref ARM_SPI_MODE_SLAVE          </td>
    <td> Set the SPI Slave  (Output on MISO, and the Input on MOSI)                                     
	</td></tr>
<tr><td> \ref ARM_SPI_MODE_SLAVE_SIMPLEX  </td>
    <td> Set the SPI Slave  (Output and Input on MISO)                                                  
	</td></tr> 
<tr><td> \ref ARM_SPI_CPOL0_CPHA0  (default)  </td>
    <td rowspan="6" style="text-align:right"> 8..11 </td>
    <td rowspan="6"> Clock Polarity <br> (Frame Format) </td><td> CPOL=\token{0} and CPHA=\token{0}: Clock Polarity 0, Clock Phase 0 </td>
	</tr>
<tr><td> \ref ARM_SPI_CPOL0_CPHA1             </td>
    <td> CPOL=\token{0} and CPHA=\token{1}: Clock Polarity 0, Clock Phase 1                 
    </td></tr>  
<tr><td> \ref ARM_SPI_CPOL1_CPHA0             </td>
    <td> CPOL=\token{1} and CPHA=\token{0}: Clock Polarity 1, Clock Phase 0                 
	</td></tr> 
<tr><td> \ref ARM_SPI_CPOL1_CPHA1             </td>
    <td> CPOL=\token{1} and CPHA=\token{1}: Clock Polarity 1, Clock Phase 1                 
	</td></tr> 
<tr><td> \ref ARM_SPI_TI_SSI                  </td>
    <td> Specifies that the frame format corresponds to the Texas Instruments Frame Format  
	</td></tr> 
<tr><td> \ref ARM_SPI_MICROWIRE               </td>
    <td> Specifies that the frame format corresponds to the National Microwire Frame Format 
	</td></tr> 
<tr><td> \ref ARM_SPI_DATA_BITS(n)       </td>
    <td style="text-align:right"> 12..17 </td>
	<td> Data Bits </td>
    <td> Set the number of bits per SPI frame; range for \em n = \token{1..32}. 
         This is the minimum required parameter. 
    </td></tr> 
<tr><td> \ref ARM_SPI_MSB_LSB   (default) </td>
    <td rowspan="2" style="text-align:right"> 18 </td>
    <td rowspan="2"> Bit Order </td>
    <td> Set the bit order from MSB to LSB   
	</td></tr>
<tr><td> \ref ARM_SPI_LSB_MSB             </td>
    <td> Set the bit order from LSB to MSB   
	</td></tr>
<tr><td nowrap>\ref ARM_SPI_SS_MASTER_UNUSED   (default) </td>
    <td rowspan="6" style="text-align:right"> 19..21 </td>
    <td rowspan="6"> Slave Select 
	                 <br>when Master 
	                 <div style="min-height:200px">&nbsp;</div>
                     Must be used with the corresponding master or slave controls from category <a href="#spi_mode_tab"><b>Mode Controls</b></a>. 
	                 <div style="min-height:200px">&nbsp;</div>
					 Slave Select 
					 <br>when Slave
	</td>
    <td>Set the Slave Select mode for the master to  <b>Not used</b>. Used with Mode Control ARM_SPI_MODE_MASTER.
	    Master does not drive or monitor the SS line. For example, when connecting to a single slave, 
		which has the SS line connected to a fixed low level.  
	</td></tr>
<tr><td>\ref ARM_SPI_SS_MASTER_SW</td>
    <td>Set the Slave Select mode for the master to  <b>Software controlled</b>. Used with Mode Control ARM_SPI_MODE_MASTER.
	    The Slave Select line is configured as output and controlled via the Miscellaneous Control \ref ARM_SPI_CONTROL_SS. 
		By default, the line it is not active (high), and is not affected by transfer-, send-, or receive functions.
    </td></tr>
<tr><td>\ref ARM_SPI_SS_MASTER_HW_OUTPUT</td>
    <td>Set the Slave Select mode for the master to <b>Hardware controlled Output</b>. Used with Mode Control ARM_SPI_MODE_MASTER.
	    The Slave Select line is configured as output and controlled by hardware. 
		The line gets activated or deactivated automatically by the hardware for transfers and is not controlled by the Miscellaneous Control \ref ARM_SPI_CONTROL_SS.
		When exactly the line is activated or deactivated is hardware dependent. Typically, the hardware will activate the line before starting the transfer 
		and deactivate it after the transfer completes. Some hardware will leave the line activated until the SPI stays master. 
		\note Some devices require that the SS signal is strictly defined regarding transfers. Refer to the documentaiton of your device.
    </td></tr>
<tr>
    <td>\ref ARM_SPI_SS_MASTER_HW_INPUT</td>
    <td>Set the Slave Select mode for the master to <b>Hardware monitored Input</b>. Used with Mode Control ARM_SPI_MODE_MASTER.
	    Used in multi-master configuration where a master does not drive the Slave Select when driving the bus, but rather monitors it. 
		When another master activates this line, the active master backs off. This is called Mode Fault. Slave Select is configured as input 
		and hardware only monitors the line. When the line is deactivated externally while we are master, 
		it presents a Mode Fault (\ref ARM_SPI_EVENT_MODE_FAULT) and the SPI switches to inactive mode.
    </td></tr>
<tr><td>\ref ARM_SPI_SS_SLAVE_HW (default)</td>
    <td>Set the Slave Select mode for the slave to <b>Hardware monitored</b>. Used with Mode Control ARM_SPI_MODE_SLAVE.
	    Hardware monitors the Slave Select line and accepts transfers only when the line is active. Transfers are ignored while the Slave Select line is inactive.
    </td></tr>
<tr><td>\ref ARM_SPI_SS_SLAVE_SW</td>
    <td>Set the Slave Select mode for the slave to <b>Software controlled</b>. Used with Mode Control ARM_SPI_MODE_SLAVE.
	    Used only when the Slave Select line is not used. For example, when a single master and slave are connected in the system 
		then the Slave Select line is not needed. Software controls if the slave is responding or not (by default it is not responding). 
		Software enables or disables transfers by using the Miscellaneous Control \ref ARM_SPI_CONTROL_SS.
    </td></tr>
<tr><td> \ref ARM_SPI_SET_BUS_SPEED  </td>
    <td rowspan="5" style="text-align:right"> 0..21 </td>
    <td rowspan="5"> Miscellaneous Controls <br>(cannot be ORed)</td>
    <td>Set the bus speed; \em arg= Bus Speed in \token{bps}
    </td></tr>
<tr><td> \ref ARM_SPI_GET_BUS_SPEED         </td>
    <td> Get the bus speed; Retrun values >= \token{0} reperesent the bus speed in \token{bps}. Negative values are \ref spi_execution_status.
    </td></tr>
<tr><td> \ref ARM_SPI_SET_DEFAULT_TX_VALUE  </td>
    <td> Set the default transmission value; the parameter \em arg sets the value
    </td></tr>
<tr><td> \ref ARM_SPI_CONTROL_SS            </td>
    <td> Control the Slave Select signal (SS); the values for the parameter \em arg are: \token{ARM_SPI_SS_INACTIVE; ARM_SPI_SS_ACTIVE} 
	</td></tr>
<tr><td> \ref ARM_SPI_ABORT_TRANSFER        </td>
    <td> Abort the current data transfer    
	</td></tr>
</table>



\b Example 

\code
  extern ARM_DRIVER_SPI Driver_SPI0;
  
  // configure: SPI master | clock polarity=1, clock phase=1 | bits per frame=16 | bus speed : 1000000 
  status = Driver_SPI0.Control(ARM_SPI_MODE_MASTER   | 
                               ARM_SPI_CPOL1_CPHA1   | 
                               ARM_SPI_DATA_BITS(16), 1000000);
\endcode
*****************************************************************************************************************/

ARM_SPI_STATUS ARM_SPI_GetStatus (void)  {
  return { 0 };
}
/**
\fn ARM_SPI_STATUS ARM_SPI_GetStatus (void)
\details
The function \b ARM_SPI_GetStatus returns the current SPI interface status.
*****************************************************************************************************************/

void ARM_SPI_SignalEvent (uint32_t event)  {
  // function body
}
/**
\fn void ARM_SPI_SignalEvent (uint32_t event)
\details
The function \b ARM_SPI_SignalEvent is a callback function registered by the function \ref ARM_SPI_Initialize. 

The parameter \em event indicates one or more events that occurred during driver operation.
Each event is encoded in a separate bit and therefore it is possible to signal multiple events within the same call. 

Not every event is necessarily generated by the driver. This depends on the implemented capabilities stored in the 
data fields of the structure \ref ARM_SPI_CAPABILITIES, which can be retrieved with the function \ref ARM_SPI_GetCapabilities.

The following events can be generated:

<table class="cmtable" summary="">
<tr>
  <th> Parameter \em event                  </th><th> Bit </th><th> Description </th>                                              
  <th> supported when ARM_SPI_CAPABILITIES </th>
</tr>
<tr>
  <td> \ref ARM_SPI_EVENT_TRANSFER_COMPLETE </td><td>  0  </td><td> Occurs after call to \ref ARM_SPI_Send, \ref ARM_SPI_Receive, 
                                                                    or \ref ARM_SPI_Transfer 
                                                                    to indicate that all the data has been transferred. 
																	The driver is ready for the next transfer operation. </td>     
  <td> <i>always supported</i> </td>
</tr>
<tr>
  <td> \ref ARM_SPI_EVENT_DATA_LOST         </td><td>  1  </td><td> Occurs in slave mode when data is requested/sent by master 
                                                                    but send/receive/transfer operation has not been started and 
                                                                    indicates that data is lost. Occurs also in master mode when
                                                                    driver cannot transfer data fast enough.             </td>     
  <td> <i>always supported</i> </td>
</tr>
<tr>
  <td> \ref ARM_SPI_EVENT_MODE_FAULT        </td><td>  2  </td><td> Occurs in master mode when Slave Select is deactivated and 
                                                                    indicates Master Mode Fault.  
																	The driver is ready for the next transfer operation. </td>     
  <td> data field \em event_mode_fault = \token{1} </td>
</tr>
</table>
**************************************************************************************************************************/


/**
\defgroup SPI_control SPI Control Codes
\ingroup spi_interface_gr
\brief Many parameters of the SPI driver are configured using the \ref ARM_SPI_Control function.
\details 
@{
The various SPI control codes define:
  
  - \ref spi_mode_ctrls               specifies SPI mode
  - \ref spi_frame_format_ctrls       defines the frame format
  - \ref spi_data_bits_ctrls          defines the number of data bits
  - \ref spi_bit_order_ctrls          defines the bit order
  - \ref spi_slave_select_mode_ctrls  specifies slave select mode
  - \ref spi_misc_ctrls               specifies additional miscellaneous controls

Refer to the \ref ARM_SPI_Control function for further details.
*/

/**
\defgroup spi_mode_ctrls SPI Mode Controls
\ingroup SPI_control
\brief Specifies SPI mode.
\details
@{
\def ARM_SPI_MODE_INACTIVE
\sa ARM_SPI_Control
\def ARM_SPI_MODE_MASTER
\sa ARM_SPI_Control
\def ARM_SPI_MODE_SLAVE
\sa ARM_SPI_Control
\def ARM_SPI_MODE_MASTER_SIMPLEX
\sa ARM_SPI_Control
\def ARM_SPI_MODE_SLAVE_SIMPLEX
\sa ARM_SPI_Control
@}
*/

/**
\defgroup spi_frame_format_ctrls SPI Frame Format 
\ingroup SPI_control
\brief Defines the frame format.
\details
@{
\def ARM_SPI_CPOL0_CPHA0
\sa ARM_SPI_Control
\def ARM_SPI_CPOL0_CPHA1
\sa ARM_SPI_Control
\def ARM_SPI_CPOL1_CPHA0
\sa ARM_SPI_Control
\def ARM_SPI_CPOL1_CPHA1
\sa ARM_SPI_Control
\def ARM_SPI_TI_SSI
\sa ARM_SPI_Control
\def ARM_SPI_MICROWIRE
\sa ARM_SPI_Control
@}
*/

/**
\defgroup spi_data_bits_ctrls SPI Data Bits
\ingroup SPI_control
\brief Defines the number of data bits.
\details
@{
\def ARM_SPI_DATA_BITS(n)
\sa ARM_SPI_Control
@}
*/

/**
\defgroup spi_bit_order_ctrls  SPI Bit Order
\ingroup SPI_control
\brief Defines the bit order.
\details
@{
\def ARM_SPI_MSB_LSB
\sa ARM_SPI_Control
\def ARM_SPI_LSB_MSB
\sa ARM_SPI_Control
@}
*/

/**
\defgroup spi_slave_select_mode_ctrls  SPI Slave Select Mode
\ingroup SPI_control
\brief Specifies SPI slave select mode.
\details
\b SPI \b Slave \b Select \b Mode configures the behavior of the \b Slave \b Select \b (SS) signal. The configuration is
separate for \b Master (ARM_SPI_SS_MASTER_*) and for \b Slave (\ref ARM_SPI_SS_SLAVE_HW, \ref ARM_SPI_SS_SLAVE_SW). The
active configuration depends on the current state (Master/Slave).

@{
\def ARM_SPI_SS_MASTER_UNUSED
An SPI master does not drive or monitor the SS line. For example, when connecting to a single slave, the SS line can be connected
to a fixed low level.
\sa ARM_SPI_Control
\def ARM_SPI_SS_MASTER_SW
SS is configured as an output and controlled via \ref ARM_SPI_Control (\ref ARM_SPI_CONTROL_SS). By default, it is not active
(high). It is activated (low) by \ref ARM_SPI_Control (\ref ARM_SPI_CONTROL_SS, \ref ARM_SPI_SS_ACTIVE) and deactivated by
\ref ARM_SPI_Control (\ref ARM_SPI_CONTROL_SS, \ref ARM_SPI_SS_INACTIVE). It is not affected by transfer/send/receive
functions.
\sa ARM_SPI_Control
\def ARM_SPI_SS_MASTER_HW_OUTPUT
Here, SS is configured as an output. It will be automatically activated/deactivated for the transfers by hardware (not
controlled by \ref ARM_SPI_Control (\ref ARM_SPI_CONTROL_SS)). The activation/deactivation of the line is completely hardware
dependent. Typically, the hardware will activate it before starting a transfer and deactivate it after a transfer completes.
Some hardware will leave it activated as long as the SPI stays master. Due to different hardware behavior, this mode is
typically not useful because certain devices require that the SS signal is strictly defined with regards to transfers.
\sa ARM_SPI_Control
\def ARM_SPI_SS_MASTER_HW_INPUT
This is normally used in a multi-master configuration, where a master does not drive the SS line when driving the bus but only
monitors it. When another master activates this line, the active master backs off. This is called \b mode \b fault. SS is
configured as input and the hardware only monitors it. When it is externally deactivated while being the master, it presents
a mode fault and the SPI switches to \b inactive mode.
\sa ARM_SPI_Control
\def ARM_SPI_SS_SLAVE_HW
Hardware monitors the SS line and accepts transfers only when SS line is activate. Transfers while SS is not active are
ignored.
\sa ARM_SPI_Control
\def ARM_SPI_SS_SLAVE_SW
Used only when SS line is not used. For example, when a single master and slave are connected in a system, the SS line is not
needed (reduces the number of lines and pins used). Slave responses are controlled by software (by default, it is not
responding). Software enables/disables transfers by calling \ref ARM_SPI_Control (\ref ARM_SPI_CONTROL_SS, \ref ARM_SPI_SS_ACTIVE / \ref ARM_SPI_SS_INACTIVE).
\sa ARM_SPI_Control
@}
*/

/**
\defgroup spi_misc_ctrls  SPI Miscellaneous Controls
\ingroup SPI_control
\brief Specifies additional miscellaneous controls.
\details
@{
\def ARM_SPI_SET_BUS_SPEED
\sa ARM_SPI_Control
\def ARM_SPI_GET_BUS_SPEED
\sa ARM_SPI_Control
\def ARM_SPI_SET_DEFAULT_TX_VALUE
\sa ARM_SPI_Control
\def ARM_SPI_CONTROL_SS
\sa ARM_SPI_Control
\def ARM_SPI_ABORT_TRANSFER
\sa ARM_SPI_Control
@}
*/

/**
@} 
*/
// end group SPI_control 

/**
@}
*/ 
// End SPI Interface
