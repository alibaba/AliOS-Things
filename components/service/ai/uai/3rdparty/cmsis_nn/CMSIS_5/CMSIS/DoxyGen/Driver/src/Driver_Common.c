/**
\defgroup common_drv_gr Common Driver Definitions
\brief Definitions common in all driver interfaces (%Driver_Common.h)
\details
The following definitions are common in all CMSIS-Driver interfaces. Refer to \ref DriverFunctions
for a general overview.
@{
*/


/**
\enum ARM_POWER_STATE
\details 
The access structure of each CMSIS-Driver provides the function \b PowerControl, which handles the power
profile for a peripheral using the parameter \ref ARM_POWER_STATE. Depending on this parameter, 
the peripheral will operate at full speed, detect just events, or is completely un-powered.

Refer to \ref CallSequence for more information.
*/

/**
\struct ARM_DRIVER_VERSION
\details
The access structure of each CMSIS-Driver provides the function \b GetVersion, which returns in the struct ARM_DRIVER_VERSION:
 - API version, which is the version of the CMSIS-Driver specification used to implement this driver.
 - Driver version, which is the source code version of the actual driver implementation.

The version is encoded as 16-bit unsigned value (uint16_t) with:
 - high-byte: major version.
 - low-byte: minor version.

For example, version 1.12 is encoded as 0x10C.


\defgroup execution_status Status Error Codes
\ingroup common_drv_gr
\brief Negative return values of functions indicate errors occurred during execution. 
\details 
Most functions return a status information using negative return values.
The following list provides the status error codes that are common in all drivers.
The drivers may return also status error codes that are specific to the peripheral.
\sa
\ref spi_execution_status for SPI driver;
\ref usart_execution_status for USART driver;
\ref nand_execution_status for NAND driver;
@{
\def ARM_DRIVER_OK
The value 0 or positive values indicate that the function execution is completed without any errors.
Note that positive values are used to provide for example the number of data items.

\def ARM_DRIVER_ERROR
The function did not execute correct and an unspecified error occurred during execution.

\def ARM_DRIVER_ERROR_BUSY
The function cannot be executed because the driver is busy with the execution of a conflicting operation.

\def ARM_DRIVER_ERROR_TIMEOUT
The function execution is terminated because a peripheral did not react within a specific timeout limit.

\def ARM_DRIVER_ERROR_UNSUPPORTED
The function requested an operation (for example by using an illegal control code) that is not supported.

\def ARM_DRIVER_ERROR_PARAMETER
A function parameter is incorrect.

\def ARM_DRIVER_ERROR_SPECIFIC
This value indicates the start of status error codes that are specific to the peripheral driver.
\sa
\ref spi_execution_status for SPI driver;
\ref usart_execution_status for USART driver;
@}
*/

/**
@}
*/
