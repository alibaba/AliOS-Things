/**
\defgroup i2c_interface_gr I2C Interface
\brief    Driver API for I2C Bus Peripheral (%Driver_I2C.h)
\details

I<sup>2</sup>C (Inter-Integrated Circuit, referred to as I-squared-C, I-two-C, or IIC) is a multi-master serial single-ended bus and is mostly used 
on single boards, but can also connect to components which are linked via cable. 

Most significant features of the I<sup>2</sup>C bus include:
 - Only two bus lines are required
 - I<sup>2</sup>C is a true multi-master bus. Simple master/slave relationships exist between all components
 - A baud rate is not required; the master device determines a bus clock
 - Each connected device is addressable by a unique address
 - Providing arbitration and collision detection

For more information about I<sup>2</sup>C refer to the following web pages:
  - Wikipedia: <a href="http://en.wikipedia.org/wiki/I%C2%B2C" target="_blank">I<sup>2</sup>C</a>
  - <a href="http://www.i2c-bus.org" target="_blank">www.i2c-bus.org</a>.

Devices can operation in Master or Slave mode:

 - To operate in Master mode call the functions \ref ARM_I2C_MasterTransmit or \ref ARM_I2C_MasterReceive. These functions get as argument a <em>slave address</em>. 
 
 - To operate in Slave mode set the <em>slave address</em> using the function \ref ARM_I2C_Control. The functions \ref ARM_I2C_SlaveTransmit or \ref ARM_I2C_SlaveReceive are used to transfer data in Slave mode.

<b>I<sup>2</sup>C Slave Address</b>
 
Depending on the device, I<sup>2</sup>C supports 7-bit and 10-bit Slaves addresses. 
The element <em>address_10_bit</em> in \ref ARM_I2C_CAPABILITIES indicates that the driver is able to handle 10-bit addresses.
A 10-bit Slave address is ORed with \ref ARM_I2C_ADDRESS_10BIT.
 
I<sup>2</sup>C also supports a General Call to all Slaves by using the slave address value \token{0}.
A General Call is recognized by Slaves have a slave address value \ref ARM_I2C_ADDRESS_GC registered with the 
function \ref ARM_I2C_Control.

<b>Block Diagram</b>

The I2C driver allows you to connect low-speed peripherals to a motherboard, embedded system, cellphone, or other electronic device. 

\image html I2C_BlockDiagram.png  "Master/Slave connected via I2C interface"


<b>I<sup>2</sup>C API</b>

The following header files define the Application Programming Interface (API) for the I<sup>2</sup>C interface:
  - \b %Driver_I2C.h : Driver API for I2C Bus Peripheral

The driver implementation is a typical part of the Device Family Pack (DFP) that supports the 
peripherals of the microcontroller family.


<b>Driver Functions</b>

The driver functions are published in the access struct as explained in \ref DriverFunctions
  - \ref ARM_DRIVER_I2C : access struct for I2C driver functions


\anchor example <b>Example Code</b>

The following example code shows the usage of the I<sup>2</sup>C interface in Master mode.

\include I2C_Demo.c

The following example code shows the usage of the I<sup>2</sup>C interface in Slave mode.

\include I2C_SlaveDemo.c

@{
*/


/**
\struct     ARM_DRIVER_I2C
\details
The functions of the I2C interface are accessed by function pointers exposed by this structure. Refer to \ref DriverFunctions for 
overview information.

Each instance of an I2C provides such an access structure. The instance is indicated by
a postfix in the symbol name of the access structure, for example:
 - \b Driver_I2C0 is the name of the access struct of the first instance (no. 0).
 - \b Driver_I2C1 is the name of the access struct of the second instance (no. 1).


A configuration setting in the middleware allows connecting the middleware to a specific driver instance <b>%Driver_I2C<i>n</i></b>.
The default is \token{0}, which connects a middleware to the first instance of a driver.
*******************************************************************************************************************/

/**
\struct     ARM_I2C_CAPABILITIES
\details
An I2C driver can be implemented with different capabilities.
The data fields of this struct encode the capabilities implemented by this driver.

The element \em address_10_bit indicates that the driver is able to handle 10-bit addressing natively.
User can still emulate the 10-bit addressing in software if the driver does not support it.

<b>Returned by:</b>
  - \ref ARM_I2C_GetCapabilities
*******************************************************************************************************************/

/**
\struct     ARM_I2C_STATUS
\details
Structure with information about the status of the I2C.

The flag \em busy indicates that the driver is busy executing Master/Slave Transmit/Receive operation.

It is set:
 - when Master operation starts: after calling \ref ARM_I2C_MasterTransmit or \ref ARM_I2C_MasterReceive
 - when Slave operation starts: after calling \ref ARM_I2C_SlaveTransmit or \ref ARM_I2C_SlaveReceive and after being addressed by a Master as the Slave

It is cleared when Master/Slave operation has finished.

The flag \em mode indicates the current mode which is Master when Master Transmit/Receive is active or Slave otherwise.

The flag \em direction indicates either Transmitter or Receiver mode. It is updated during Master/Slave operation when the Slave is addressed by a Master.

The flag \em general_call indicates a General call (address \token{0}) when in Slave mode.

The flag \em arbitration_lost indicates that the Master has lost arbitration. The current Master operation is aborted.

The flag \em bus_error indicates that a bus error has been detected. The current Master/Slave operation is aborted.

<b>Returned by:</b>
  - \ref ARM_I2C_GetStatus
*******************************************************************************************************************/

/**
\typedef    ARM_I2C_SignalEvent_t
\details
Provides the typedef for the callback function \ref ARM_I2C_SignalEvent.

<b>Parameter for:</b>
  - \ref ARM_I2C_Initialize
*******************************************************************************************************************/

/**
\defgroup I2C_events I2C Events
\ingroup i2c_interface_gr
\brief The I2C driver generates call back events that are notified via the function \ref ARM_I2C_SignalEvent.
\details 
This section provides the event values for the \ref ARM_I2C_SignalEvent callback function.

The following call back notification events are generated:
@{
\def  ARM_I2C_EVENT_TRANSFER_DONE
\def  ARM_I2C_EVENT_TRANSFER_INCOMPLETE
\def  ARM_I2C_EVENT_SLAVE_TRANSMIT
\def  ARM_I2C_EVENT_SLAVE_RECEIVE
\def  ARM_I2C_EVENT_ADDRESS_NACK
\def  ARM_I2C_EVENT_GENERAL_CALL
\def  ARM_I2C_EVENT_ARBITRATION_LOST
\def  ARM_I2C_EVENT_BUS_ERROR
\def  ARM_I2C_EVENT_BUS_CLEAR
@}
*/


//
//  Functions
//

ARM_DRIVER_VERSION ARM_I2C_GetVersion (void)  {
  return { 0, 0 };
};
/**
\fn       ARM_DRIVER_VERSION ARM_I2C_GetVersion (void)
\details
The function \b ARM_I2C_GetVersion returns version information of the driver implementation in \ref ARM_DRIVER_VERSION
 - API version is the version of the CMSIS-Driver specification used to implement this driver.
 - Driver version is source code version of the actual driver implementation.

Example:
\code
extern ARM_DRIVER_I2C Driver_I2C0;
ARM_DRIVER_I2C *drv_info;
 
void setup_i2c (void)  {
  ARM_DRIVER_VERSION  version;
 
  drv_info = &Driver_I2C0;  
  version = drv_info->GetVersion ();
  if (version.api < 0x10A)   {      // requires at minimum API version 1.10 or higher
    // error handling
    return;
  }
}
\endcode
*******************************************************************************************************************/

ARM_I2C_CAPABILITIES ARM_I2C_GetCapabilities (void)  {
  return { 0 };
};
/**
\fn       ARM_I2C_CAPABILITIES ARM_I2C_GetCapabilities (void)
\details
The function \b ARM_I2C_GetCapabilities returns information about capabilities of this driver implementation.
The data fields of the struct \ref ARM_I2C_CAPABILITIES encodes the driver capabilities.
 
Example:
\code
extern ARM_DRIVER_I2C Driver_I2C0;
ARM_DRIVER_I2C *drv_info;
  
void read_capabilities (void)  {
  ARM_I2C_CAPABILITIES drv_capabilities;
 
  drv_info = &Driver_I2C0;  
  drv_capabilities = drv_info->GetCapabilities ();
  // interrogate capabilities
 
}
\endcode
*******************************************************************************************************************/

int32_t ARM_I2C_Initialize (ARM_I2C_SignalEvent_t cb_event) {
  return ARM_DRIVER_OK;
};
/**
\fn       int32_t ARM_I2C_Initialize (ARM_I2C_SignalEvent_t cb_event)
\details 
The function \b ARM_I2C_Initialize initializes the I2C interface. 
It is called when the middleware component starts operation.

The function performs the following operations:
  - Initializes and the I/O resources for the I2C interface.
  - Registers the \ref ARM_I2C_SignalEvent callback function.

The parameter \em cb_event is a pointer to the \ref ARM_I2C_SignalEvent callback function.
Use a NULL pointer when no callback events are required.

Can be called multiple times. If the peripheral is already initialized the function performs no operation and 
returns with \ref ARM_DRIVER_OK. Refer to \ref CallSequence for more information.

\sa ARM_I2C_PowerControl
\sa ARM_I2C_Uninitialize

\b Example:
 - refer to \ref example "Example Code"
 
*******************************************************************************************************************/

int32_t ARM_I2C_Uninitialize (void)  {
  return ARM_DRIVER_OK;
};
/**
\fn       int32_t ARM_I2C_Uninitialize (void)
\details
The function \b ARM_I2C_Uninitialize releases the I/O resources of I2C interface.

It is called when the middleware component stops operation and releases the I/O resources used by the I2C interface.
Refer to \ref CallSequence for more information.

\sa ARM_I2C_Initialize
\sa ARM_I2C_PowerControl

*******************************************************************************************************************/

int32_t ARM_I2C_PowerControl (ARM_POWER_STATE state)  {
  return ARM_DRIVER_OK;
};
/**
\fn int32_t ARM_I2C_PowerControl (ARM_POWER_STATE state)
\details
The function \b ARM_I2C_PowerControl operates the power modes of the I2C interface.

The parameter \em state can have the following values:
  - \ref ARM_POWER_FULL : set-up peripheral for data transfers, enable interrupts (NVIC) and optionally DMA. 
                          Can be called multiple times. If the peripheral is already in this mode, 
						  then the function performs no operation and returns with \ref ARM_DRIVER_OK.
  - \ref ARM_POWER_LOW : may use power saving. Returns \ref ARM_DRIVER_ERROR_UNSUPPORTED when not implemented.
  - \ref ARM_POWER_OFF : terminates any pending data transfers, disables peripheral, disables related interrupts and DMA.
      
Refer to \ref CallSequence for more information.

\sa ARM_I2C_Initialize
\sa ARM_I2C_Uninitialize

*******************************************************************************************************************/

int32_t ARM_I2C_MasterTransmit (uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)  {
  return ARM_DRIVER_OK;
};
/**
\fn int32_t ARM_I2C_MasterTransmit (uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
\details
This function \b ARM_I2C_MasterTransmit transmits data as Master to the selected Slave.

The operation consists of:
 - Master generates START condition
 - Master addresses the Slave as Master Transmitter
 - Master transmits data to the addressed Slave
 - Master generates STOP condition (if \em xfer_pending is "false")

The parameter \em addr is the address of the slave to transmit the data to. The value can be ORed with \ref ARM_I2C_ADDRESS_10BIT to 
identify a 10-bit address value. \n
The parameter \em data and \em num specify the address of a data buffer and the number of bytes to transmit. \n
Set the parameter \em xfer_pending to 'true' if another transfer operation follows. With \em xfer_pending set to 'false' a STOP condition is generated.

The function is non-blocking and returns as soon as the driver has started the operation.
During the operation it is not allowed to call any Master function again. Also the data buffer must stay allocated and the contents of data must not be modified. 
When transmit operation has finished the \ref ARM_I2C_EVENT_TRANSFER_DONE event is generated.
When not all the data is transferred then the \ref ARM_I2C_EVENT_TRANSFER_INCOMPLETE flag is set at the same time.

Number of data bytes transmitted and acknowledged is returned by the function \ref ARM_I2C_GetDataCount during and after the operation has finished.

The operation is aborted in the following cases (\ref ARM_I2C_EVENT_TRANSFER_DONE event is generated together with):
 - selected slave has not acknowledged the address: \ref ARM_I2C_EVENT_ADDRESS_NACK event
 - arbitration has been lost: \ref ARM_I2C_EVENT_ARBITRATION_LOST event
 - bus error has been detected: \ref ARM_I2C_EVENT_BUS_ERROR event

Status can be monitored by calling the \ref ARM_I2C_GetStatus and checking the flags.

Transmit operation can be aborted also by calling \ref ARM_I2C_Control with the parameter \em control \ref ARM_I2C_ABORT_TRANSFER.
*******************************************************************************************************************/

int32_t ARM_I2C_MasterReceive (uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)  {
  return ARM_DRIVER_OK;
};
/**
\fn int32_t ARM_I2C_MasterReceive (uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
\details
This function \b ARM_I2C_MasterReceive is used to receive data as Master from the selected Slave.

The operation consists of:
 - Master generates START condition
 - Master addresses the Slave as Master Receiver
 - Master receives data from the addressed Slave
 - Master generates STOP condition (if \em xfer_pending is "false")

The parameter \em addr is the address of the slave to receive the data from. The value can be ORed with \ref ARM_I2C_ADDRESS_10BIT to 
identify a 10-bit address value. \n
The parameter \em data and \em num specify the address of a data buffer and the number of bytes to receive. \n
Set the parameter \em xfer_pending to 'true' if another transfer operation follows. With \em xfer_pending set to 'false' a STOP condition is generated.

The function is non-blocking and returns as soon as the driver has started the operation.
During the operation it is not allowed to call any Master function again. Also the data buffer must stay allocated. 
When receive operation has finished the \ref ARM_I2C_EVENT_TRANSFER_DONE event is generated.
When not all the data is transferred then the \ref ARM_I2C_EVENT_TRANSFER_INCOMPLETE flag is set at the same time.

Number of data bytes received is returned by the function \ref ARM_I2C_GetDataCount during and after the operation has finished.

The operation is aborted in the following cases (\ref ARM_I2C_EVENT_TRANSFER_DONE event is generated together with):
 - selected slave has not acknowledged the address: \ref ARM_I2C_EVENT_ADDRESS_NACK event
 - arbitration has been lost: \ref ARM_I2C_EVENT_ARBITRATION_LOST event
 - bus error has been detected: \ref ARM_I2C_EVENT_BUS_ERROR event

Status can be monitored by calling the \ref ARM_I2C_GetStatus and checking the flags.

Receive operation can be aborted also by calling \ref ARM_I2C_Control with the parameter \em control = \ref ARM_I2C_ABORT_TRANSFER.
*******************************************************************************************************************/

int32_t ARM_I2C_SlaveTransmit (const uint8_t *data, uint32_t num)  {
  return ARM_DRIVER_OK;
};
/**
\fn int32_t ARM_I2C_SlaveTransmit (const uint8_t *data, uint32_t num)
\details
This function \b ARM_I2C_SlaveTransmit is used to transmit data as Slave to the Master.

The parameter \em data is a pointer to the data to transmit. \n
The parameter \em num specifies the number of bytes to transmit.

The function is non-blocking and returns as soon as the driver has registered the operation.
The actual operation will start after being addressed by the master as a Slave Transmitter. If the operation has not been registered at that point the \ref ARM_I2C_EVENT_SLAVE_TRANSMIT event is generated.
The same event is also generated if the operation has finished (specified number of bytes transmitted) but more data is requested by the master.

It is not allowed to call this function again if the operation has started until it finishes. Also the data buffer must stay allocated and the contents of data must not be modified. 
When transmit operation has finished the \ref ARM_I2C_EVENT_TRANSFER_DONE event is generated.
When not all the data is transferred then the \ref ARM_I2C_EVENT_TRANSFER_INCOMPLETE flag is set at the same time.

Number of data bytes transmitted is returned by the function \ref ARM_I2C_GetDataCount during and after the operation has finished.

In case that a General call has been detected the \ref ARM_I2C_EVENT_GENERAL_CALL flag is indicated together with the \ref ARM_I2C_EVENT_TRANSFER_DONE event (also with \ref ARM_I2C_EVENT_SLAVE_TRANSMIT event).

In case that bus error has been detected then the operation is aborted and the \ref ARM_I2C_EVENT_BUS_ERROR event is generated together with \ref ARM_I2C_EVENT_TRANSFER_DONE.

Slave will only respond to its own address (or General call if enabled) that is specified by calling \ref ARM_I2C_Control with \ref ARM_I2C_OWN_ADDRESS as control parameter. 
Using address \token{0} disables the slave.

Status can be monitored by calling the \ref ARM_I2C_GetStatus and checking the flags.

Transmit operation can be canceled or aborted by calling \ref ARM_I2C_Control with the parameter \em control = \ref ARM_I2C_ABORT_TRANSFER.
*******************************************************************************************************************/

int32_t ARM_I2C_SlaveReceive (uint8_t *data, uint32_t num)  {
  return ARM_DRIVER_OK;
};
/**
\fn int32_t ARM_I2C_SlaveReceive (uint8_t *data, uint32_t num)
\details
This function \b ARM_I2C_SlaveReceive receives data as Slave from the Master.

The parameter \em data is a pointer to the data to receive. \n
The parameter \em num specifies the number of bytes to receive.

The function is non-blocking and returns as soon as the driver has registered the operation.
The actual operation will start after being addressed by the master as a Slave Receiver. If the operation has not been registered at that point the \ref ARM_I2C_EVENT_SLAVE_RECEIVE event is generated.

It is not allowed to call this function again if the operation has started until it finishes. Also the data buffer must stay allocated. 
When receive operation has finished the \ref ARM_I2C_EVENT_TRANSFER_DONE event is generated.
When not all the data is transferred then the \ref ARM_I2C_EVENT_TRANSFER_INCOMPLETE flag is set at the same time.

Number of data bytes received and acknowledged is returned by the function \ref ARM_I2C_GetDataCount during and after the operation has finished.

In case that a General call has been detected the \ref ARM_I2C_EVENT_GENERAL_CALL flag is indicated together with the \ref ARM_I2C_EVENT_TRANSFER_DONE event (also with \ref ARM_I2C_EVENT_SLAVE_RECEIVE event).

In case that bus error has been detected then the operation is aborted and the \ref ARM_I2C_EVENT_BUS_ERROR event is generated together with \ref ARM_I2C_EVENT_TRANSFER_DONE.

Slave will only respond to its own address (or General call if enabled) that is specified by calling \ref ARM_I2C_Control with \ref ARM_I2C_OWN_ADDRESS as control parameter. 
Using address \token{0} disables the slave.

Status can be monitored by calling the \ref ARM_I2C_GetStatus and checking the flags.

Receive operation can be canceled or aborted by calling \ref ARM_I2C_Control with the parameter \em control = \ref ARM_I2C_ABORT_TRANSFER.
*******************************************************************************************************************/

int32_t ARM_I2C_GetDataCount (void)  {
  return 0;
}
/**
\fn int32_t ARM_I2C_GetDataCount (void)
\details
The function \b ARM_I2C_GetDataCount returns the number of currently transferred data bytes during and after:
 - \ref ARM_I2C_MasterTransmit : number of data bytes transmitted and acknowledged
 - \ref ARM_I2C_MasterReceive : number of data bytes received
 - \ref ARM_I2C_SlaveTransmit : number of data bytes transmitted
 - \ref ARM_I2C_SlaveReceive : number of data bytes received and acknowledged

When the Slave is not yet addressed by the Master then \token{-1} is returned.
*****************************************************************************************************************/

int32_t ARM_I2C_Control (uint32_t control, uint32_t arg)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_I2C_Control (uint32_t control, uint32_t arg)
\details
The function \b ARM_I2C_Control operates the I2C interface and executes various operations. 

The parameter \em control specifies various operations as listed in the table below.  \n
The parameter \em arg provides, depending on the operation,  additional information. \n

Parameter \em control            | Operation
:--------------------------------|:--------------------------------------------
\ref ARM_I2C_OWN_ADDRESS         | Set Own Slave Address; \em arg = slave address
\ref ARM_I2C_BUS_SPEED           | Set Bus Speed; \em arg = bus speed
\ref ARM_I2C_BUS_CLEAR           | Clear the bus by sending nine clock pulses
\ref ARM_I2C_ABORT_TRANSFER      | Aborts the data transfer between Master and Slave for Transmit or Receive 

<b>Set Own Slave Address</b>

After initialization, the I2C Device has no slave address assigned and does not accept any requests from 
an I2C Master.

The \em control operation \ref ARM_I2C_OWN_ADDRESS sets the slave address with the parameter \em arg.
The slave address can be ORed with \ref ARM_I2C_ADDRESS_10BIT to indicate a 10-bit address.  

The slave address can be ORed with \ref ARM_I2C_ADDRESS_GC to indicate that the slave accepts a General Call.
If the slave address value is only \ref ARM_I2C_ADDRESS_GC, then the slave only accepts a General Call.

The slave address value \token{0} disables Slave mode and clears any assigned slave address.

**Examples:**

Set the Slave address value \token{0x45} as 7-bit address.
\code
  I2Cdrv->Control      (ARM_I2C_OWN_ADDRESS, 0x45);
\endcode

Set the Slave address value \token{0x135} as 10-bit address and accept a General Call.
\code
  I2Cdrv->Control      (ARM_I2C_OWN_ADDRESS, 0x135 | ARM_I2C_ADDRESS_10BIT | ARM_I2C_ADDRESS_GC);
\endcode

<b>Bus Speed</b>

The \em control operation \ref ARM_I2C_BUS_SPEED sets the bus speed using the parameter \em arg.

Parameter \em arg                | Bus Speed
:--------------------------------|:--------------------------------------------
\ref ARM_I2C_BUS_SPEED_STANDARD  | Standard Speed to (\token{100 kHz})
\ref ARM_I2C_BUS_SPEED_FAST      | Fast Speed  (\token{400kHz})
\ref ARM_I2C_BUS_SPEED_FAST_PLUS | Fast + Speed (\token{1MHz})
\ref ARM_I2C_BUS_SPEED_HIGH      | High Speed  (\token{3.4MHz})

**Example:**

\code
  I2Cdrv->Control      (ARM_I2C_BUS_SPEED, I2C_BUS_SPEED_FAST);
\endcode
*****************************************************************************************************************/

ARM_I2C_STATUS ARM_I2C_GetStatus (void)  {
  return { 0 };
}
/**
\fn ARM_I2C_STATUS ARM_I2C_GetStatus (void)
\details
The function \b ARM_I2C_GetStatus returns the current I2C interface status.

Refer to \ref ARM_I2C_STATUS for details.
*****************************************************************************************************************/

void ARM_I2C_SignalEvent (uint32_t event)  {
  // function body
}
/**
\fn void ARM_I2C_SignalEvent (uint32_t event)
\details
The function \b ARM_I2C_SignalEvent is a callback function registered by the function \ref ARM_I2C_Initialize..
It is called by the I2C driver to notify the application about \ref I2C_events occured during operation.

The parameter \a event indicates one or more events that occurred during driver operation.
Each event is encoded in a separate bit and therefore it is possible to signal multiple events within the same call. 

The following events can be generated:

Parameter \em event                       |     Bit   | Description 
:---------------------------------------- |:---------:|:----------------------------------------------------------
\ref ARM_I2C_EVENT_TRANSFER_DONE          | 1UL&nbsp;<<&nbsp;0  | Occurs after Master/Slave Transmit/Receive operation has finished.
\ref ARM_I2C_EVENT_TRANSFER_INCOMPLETE    | 1UL << 1  | Occurs together with \ref ARM_I2C_EVENT_TRANSFER_DONE when less data is transferred then requested.
\ref ARM_I2C_EVENT_SLAVE_TRANSMIT         | 1UL << 2  | Occurs when addressed as Slave Transmitter and \ref ARM_I2C_SlaveTransmit has not been started.
\ref ARM_I2C_EVENT_SLAVE_RECEIVE          | 1UL << 3  | Occurs when addressed as Slave Receiver and \ref ARM_I2C_SlaveReceive has not been started.
\ref ARM_I2C_EVENT_ADDRESS_NACK           | 1UL << 4  | Occurs in master mode when address is not acknowledged from slave.
\ref ARM_I2C_EVENT_GENERAL_CALL           | 1UL << 5  | Indicates General Call in slave mode together with \ref ARM_I2C_EVENT_TRANSFER_DONE, \ref ARM_I2C_EVENT_SLAVE_TRANSMIT and \ref ARM_I2C_EVENT_SLAVE_RECEIVE.
\ref ARM_I2C_EVENT_ARBITRATION_LOST       | 1UL << 6  | Occurs in master mode when arbitration is lost.
\ref ARM_I2C_EVENT_BUS_ERROR              | 1UL << 7  | Occurs when bus error is detected.
\ref ARM_I2C_EVENT_BUS_CLEAR              | 1UL << 8  | Occurs after \ref ARM_I2C_BUS_CLEAR Control operation has finished.

**************************************************************************************************************************/


/**
\defgroup i2c_control_gr I2C Control Codes
\ingroup i2c_interface_gr
\brief Many parameters of the I2C driver are configured using the \ref ARM_I2C_Control function.
@{
\details 
The various I2C control codes define:
  - \ref i2c_control_codes           specify operation parameters and various controls
  - \ref i2c_bus_speed_ctrls         specify the I2C bus speed
  
Refer to the \ref ARM_I2C_Control function for further details.
*/

/**
\defgroup i2c_control_codes I2C Control Codes
\ingroup i2c_control_gr
\brief Specify operation parameters and various controls.
\details
@{
\def ARM_I2C_OWN_ADDRESS
\sa ARM_I2C_Control
\def ARM_I2C_BUS_SPEED
Speed is specified using the following values: \ref i2c_bus_speed_ctrls
\sa ARM_I2C_Control
\def ARM_I2C_BUS_CLEAR
\sa ARM_I2C_Control
\def ARM_I2C_ABORT_TRANSFER
\sa ARM_I2C_Control
@}
*/

/**
\defgroup i2c_bus_speed_ctrls I2C Bus Speed
\ingroup i2c_control_gr
\brief Specify the I2C bus speed.
\details
@{
\def ARM_I2C_BUS_SPEED_STANDARD
\sa ARM_I2C_Control
\def ARM_I2C_BUS_SPEED_FAST
\sa ARM_I2C_Control
\def ARM_I2C_BUS_SPEED_FAST_PLUS
\sa ARM_I2C_Control
\def ARM_I2C_BUS_SPEED_HIGH
\sa ARM_I2C_Control
@}
*/
/**
@}
*/ 

/**
\defgroup i2c_address_flags I2C Address Flags
\ingroup i2c_interface_gr
\brief Specify address flags
\details
Specifies the address type for the functions \ref ARM_I2C_MasterReceive, \ref ARM_I2C_MasterTransmit and \ref ARM_I2C_OWN_ADDRESS.
@{
\def ARM_I2C_ADDRESS_10BIT
\sa ARM_I2C_OWN_ADDRESS
\sa ARM_I2C_MasterTransmit
\sa ARM_I2C_MasterReceive
\def ARM_I2C_ADDRESS_GC     
\sa ARM_I2C_OWN_ADDRESS
@}
*/


/**
@}
*/ 
// End I2C Interface
