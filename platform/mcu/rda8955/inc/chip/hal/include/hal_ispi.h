/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/




#ifndef _HAL_ISPI_H_
#define _HAL_ISPI_H_

#include "cs_types.h"
#include "hal_error.h"

/// @defgroup spi HAL SPI Driver
/// This document describes the characteristics of the SPI module and how
/// to use them via its Hardware Abstraction Layer API.
///
/// One SPI with 2 chip selects is available. All of them support the
/// following functions:
/// - Supports Write only or Read Write operation
/// - Programmable receive and transmit FIFO (16 bytes deep each)
/// - DMA capabilities to allow fully automated data transfer
/// - Wide selection of programmable interrupts to allow interrupt driven data
///   transfer management.
/// - Up to 13 Mbits/s transfer rate.
/// - stream mode, associated with a pattern mode to program the reception of
/// a given number of bytes after a pattern has been read on the CS.
///
/// \b Note: SPI can only read while sending, and it reads the same number of
/// bits, if the chips interfaced does not handle this full duplex operation,
/// you'll need to write zeros when you actually need to read. And you'll also
/// need to read from FIFO the garbage read during actual write operation
/// (unless you change the CS setting to disable the read before writing).
///
/// @par I/O Signals
///
/// <TABLE BORDER >
/// <TR><TH>Signal Name</TH><TH>Direction</TH></TR>
///
/// <TR><TD> SPI_SCLK  </TD><TD> Output </TD></TR>
/// <TR><TD> SPI_SEN_0 </TD><TD> Output </TD></TR>
/// <TR><TD> SPI_SEN_1 </TD><TD> Output </TD></TR>
/// <TR><TD> SPI_DO    </TD><TD> Output </TD></TR>
/// <TR><TD> SPI_DI    </TD><TD> Input  </TD></TR>
/// </TABLE>
///
///
/// @par SPI Operation
/// A configuration structure of type BOOT_ISPI_CFG_T allows to define the SPI behavior.
/// The available configurations are as follow :
/// - @b Chip @b Select @b Choice \n
///          The SPI has three chip select, the available values are defined
///          as follow:
///          - #HAL_ISPI_CS0 &mdash; The chip select 0
///          - #HAL_ISPI_CS1 &mdash; The chip select 1
///          - #HAL_ISPI_CS2 &mdash; The chip select 2
///          - #HAL_ISPI_CS3 &mdash; The chip select 3
///          .
///          \n
/// - @b Timing @b and @b Data @b Format \n
///          The SPI module supports several configuration parameters to allow
///          connection with a wide range of devices. \n
///          \n
///          The polarity of the serial clock is configurable. The polarity of
///          both the Chip Selects are needed for operating on any one of those
///          Chip Select. Therefore, this information is recorded in the
///          target configuration structure (tgt_BoardConfig), which records
///          the polarity of both Chip Selects.
///          If the first edge after CS activation is a falling edge, set
///          the proper field to  \c TRUE.\n
///          \n
///          The configuration delays are all defined in half of SCLK period.\n
///          The delay between the CS activation and the first clock edge can
///          be 0 to 2 half clocks.\n
///          The delay between the CS activation and the output of the data can
///          be 0 to 2 half clocks.\n
///          The delay between the CS activation and the sampling of the input
///          data can be 0 to 3 half clocks.\n
///          The delay between the end of transfer and the CS deactivation can
///          be 0 to 3 half clocks.\n
///          The time when the CS must remain deactivated before a new transfer
///          can be 0 to 3 half clocks.\n
///          \n
///          The transfer size is defined by the frame size. It is the number
///          of bits per transfer, it can be 4 to 32 bits.\n
///          \n
///          The SCLK period is defined by setting the divider between the
///          system clock and the half of the SCLK (the SCLK is actually
///          divided by \c clkDivider x 2).\n
///          \n
///          Transfer sample <CODE> (clkFallEdge = true, clkDelay = 0,
///          doDelay = 0, diDelay = 1, csDelay = 1, csPulse = 2) </CODE>
///          \n
///          @image latex spi_timing.png "SPI Timing"
///          @image html spi_timing.png "SPI Timing"
///          \n
/// - @b Transfer @b Modes \n
///          To allow for an easy use of the SPI modules, a non blocking
///          Hardware Abstraction Layer interface is provided. Each transfer
///          direction (send/receive) can be configured as:
///          - @b Direct @b polling: \n
///          The application sends/receives the data directly to/from the
///          hardware module. The number of bytes actually sent/received is
///          returned. No Irq is generated. \n
///          \n
///          - @b DMA @b polling: \n
///          The application sends/receives the data through a DMA to the
///          hardware module. The function returns 0 when no DMA channel is
///          available. No bytes are sent. The function returns the number of
///          bytes to send when a DMA resource is available. They will all be
///          sent. A function allows to check if the previous DMA transfer is
///          finished. No new DMA transfer in the same direction will be
///          allowed  before the end of the previous transfer. \n
///          \n
///          .
///          \n
/// @par Configuration Structure
/// A configuration structure allows to open or change the SPI with the desired
/// parameters : #BOOT_ISPI_CFG_T.
///
/// @{

// =============================================================================
//  MACROS
// =============================================================================



// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
// HAL_ISPI_DELAY_T
// -----------------------------------------------------------------------------
/// Delays
/// Used to define the configuration delays
// =============================================================================
typedef enum
{
    /// Delay of 0 half-period
    HAL_ISPI_HALF_CLK_PERIOD_0,
    /// Delay of 1 half-period
    HAL_ISPI_HALF_CLK_PERIOD_1,
    /// Delay of 2 half-period
    HAL_ISPI_HALF_CLK_PERIOD_2,
    /// Delay of 3 half-period
    HAL_ISPI_HALF_CLK_PERIOD_3,

    HAL_ISPI_HALF_CLK_PERIOD_QTY
} HAL_ISPI_DELAY_T;





// =============================================================================
// HAL_ISPI_CS_T
// -----------------------------------------------------------------------------
/// Chip Select
/// Used to select a Chip Select
// =============================================================================
typedef enum
{
    /// Chip Select for the FM analog module.
    HAL_ISPI_CS_PMU = 0,
    /// Chip Select for the ABB analog module.
    HAL_ISPI_CS_ABB,
    /// Chip Select for the PMU analog module.
    HAL_ISPI_CS_FM,

    HAL_ISPI_CS_QTY,

    /// Chip Select activated by default out of #boot_IspiOpen(),
    /// that allow access to all ISPI driver functions for any
    /// Chip Select.
    HAL_ISPI_CS_ALL = 0xFF
} HAL_ISPI_CS_T;

// =============================================================================
// HAL_ISPI_CFG_T
// -----------------------------------------------------------------------------
/// Structure for configuration.
/// A configuration structure allows to open or change the SPI with the desired
/// parameters.
// =============================================================================
typedef struct
{
    /// Polarity of the FM CS.
    BOOL csFmActiveLow;

    /// Polarity of the ABB CS.
    BOOL csAbbActiveLow;

    /// Polarity of the PMU CS.
    BOOL csPmuActiveLow;

    /// If the first edge after the CS activation is a falling edge, set to
    /// \c TRUE.\n Otherwise, set to \c FALSE.
    BOOL clkFallEdge;

    /// The delay between the CS activation and the first clock edge,
    /// can be 0 to 2 half clocks.
    HAL_ISPI_DELAY_T clkDelay;

    /// The delay between the CS activation and the output of the data,
    /// can be 0 to 2 half clocks.
    HAL_ISPI_DELAY_T doDelay;

    /// The delay between the CS activation and the sampling of the input data,
    /// can be 0 to 3 half clocks.
    HAL_ISPI_DELAY_T diDelay;

    /// The delay between the end of transfer and the CS deactivation, can be
    /// 0 to 3 half clocks.
    HAL_ISPI_DELAY_T csDelay;

    /// The time when the CS must remain deactivated before a new transfer,
    /// can be 0 to 3 half clocks.
    HAL_ISPI_DELAY_T csPulse;

    /// Frame size in bits
    UINT32 frameSize;

    /// OE ratio - Value from 0 to 31 is the number of data out to transfert
    /// before the SPI_DO pin switches to input.
    /// Not needed in the chip,but needed for the FPGA
    UINT8 oeRatio;


    /// SPI maximum clock frequency: the SPI clock will be the highest
    /// possible value inferior to this parameter.
    UINT32 spiFreq;

} HAL_ISPI_CFG_T;



// =============================================================================
// HAL_ISPI_PIN_T
// -----------------------------------------------------------------------------
/// SPI Pin
///
/// Used to specify an SPI pin.
// =============================================================================
typedef enum
{
    /// Chip select pin for the FM analog module.
    HAL_ISPI_PIN_CS_PMU = 0,
    /// Chip select pin for the ABB analog module.
    HAL_ISPI_PIN_CS_ABB,
    /// Chip select pin for the PMU analog module.
    HAL_ISPI_PIN_CS_FM,
    /// Clock pin
    HAL_ISPI_PIN_CLK,
    /// Data out pin
    HAL_ISPI_PIN_DO,
    /// Data in pin
    HAL_ISPI_PIN_DI,
    HAL_ISPI_PIN_LIMIT
} HAL_ISPI_PIN_T;



// =============================================================================
// HAL_ISPI_PIN_STATE_T
// -----------------------------------------------------------------------------
/// SPI Pin State
///
/// Used to specify the state of an SPI pin. It can be
/// zero, one or Z (high impedence, tri-state).
// =============================================================================
typedef enum
{
    HAL_ISPI_PIN_STATE_SPI   =   0,
    HAL_ISPI_PIN_STATE_Z     =   1,
    HAL_ISPI_PIN_STATE_0     =   2,
    HAL_ISPI_PIN_STATE_1     =   3
} HAL_ISPI_PIN_STATE_T;



// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// hal_IspiOpen
// -----------------------------------------------------------------------------
/// Open the ISPI driver.
///
/// @param spiConfigPtr Pointer to the ISPI configuration to use. If \c NULL,
/// a default, romed configuration is used.
// =============================================================================
PUBLIC VOID hal_IspiOpen(CONST HAL_ISPI_CFG_T* spiConfigPtr);


// =============================================================================
// hal_IspiClose
// -----------------------------------------------------------------------------
/// Close the ISPI.
// =============================================================================
PUBLIC VOID hal_IspiClose(VOID);


// =============================================================================
// hal_IspiFlushFifos
// -----------------------------------------------------------------------------
/// Flush both ISPI Fifos.
///
/// @param csId Identifier of the Chip Select on which behalf the operation is
/// done. It must be the same as the activated Chip Select, unless the ISPI
/// is activated for all the Chip Select (#HAL_ISPI_CS_ALL)
/// @return #HAL_ERR_NO if the operation could be properly conducted, or
/// #HAL_ERR_RESOURCE_BUSY is another Chip Select is activated.
// =============================================================================
PUBLIC HAL_ERR_T hal_IspiFlushFifos(HAL_ISPI_CS_T csId);



// =============================================================================
// hal_IspiCsActivate
// -----------------------------------------------------------------------------
/// Activate a Chip Select. Activating a Chip Select means than all operation
/// that could interfere with the operation on that activated Chip Select are
/// forbidden. Namely, all read operation on other Chip Selects are forbidden,
/// as well as other activation.
///
/// @param csId Identifier of the Chip Select to activate.
/// @return \c TRUE if the Chip Select could be activated, \c FALSE if another
/// Chip Select is already activated.
// =============================================================================
PUBLIC BOOL hal_IspiCsActivate(HAL_ISPI_CS_T csId);



// =============================================================================
// hal_IspiCsDeactivate
// -----------------------------------------------------------------------------
/// Deactivate a Chip Select previously reserved by #hal_IspiCsActivate().
///
/// @param csId Identifier of the Chip Select to activate.
/// @return \c TRUE if the Chip Select could be deactivated, \c FALSE if another
/// Chip Select is already activated.
/// =============================================================================
PUBLIC BOOL hal_IspiCsDeactivate(HAL_ISPI_CS_T csId);





// =============================================================================
// hal_IspiForcePin
// -----------------------------------------------------------------------------
/// Force an ISPI pin to a certain state or release the force mode
/// and put the pin back to normal ISPI mode.
///
/// @param csId Identifier of the Chip Select on which behalf the operation is
/// done. It must be the same as the activated Chip Select, unless the ISPI
/// is activated for all the Chip Select (#HAL_ISPI_CS_ALL)
/// @param pin Identification of the pins to be forced.
/// @param state State to enforce.
/// @return #HAL_ERR_NO if the operation could be properly conducted, or
/// #HAL_ERR_RESOURCE_BUSY is another Chip Select is activated.
// =============================================================================
PUBLIC HAL_ERR_T hal_IspiForcePin(HAL_ISPI_CS_T csId, HAL_ISPI_PIN_T pin,
                                  HAL_ISPI_PIN_STATE_T state);



// =============================================================================
// hal_IspiSendData
// -----------------------------------------------------------------------------
/// Send one data.
/// This functions sends one data frame.
/// The number returned is the number of data frames actually sent.
///
/// @param csId The CS to use to send the data. This cs must be activated before
/// sending data, unless the data to send are not requiring a read operation
/// (TX only), in which case the sending is operated if room is available in
/// the Fifo.
/// @param data Frame of data to send.
/// @param read \c TRUE if the response of the device to this sent data is
/// expected to be read later and thus will be put in the read Fifo.
/// @return 1 if the data was sent, 0 otherwise.
// =============================================================================
PUBLIC UINT32 hal_IspiSendData(HAL_ISPI_CS_T csId, UINT32 data, BOOL read);

PUBLIC UINT32 hal_IspiSendDataBuffer(HAL_ISPI_CS_T csId, CONST UINT32* startAddress, UINT32 length);


// =============================================================================
// hal_IspiTxFifoAvail
// -----------------------------------------------------------------------------
/// Get available data spaces in the Spi Tx FIFO.
/// This function returns the available space in the Tx FIFO, as a number
/// of 32 bits data that can be filled into it.
///
/// @param csId Identifier of the Chip Select on which behalf the operation is
/// done. This parameter is ignored.
/// @return The size of the available space in the Tx FIFO. (In Fifo elements.)
// =============================================================================
PUBLIC UINT8 hal_IspiTxFifoAvail(HAL_ISPI_CS_T csId);



// =============================================================================
// hal_IspiTxFinished
// -----------------------------------------------------------------------------
/// Check if the last transfer is done.
/// This function returns \c TRUE when the transmit FIFO is empty and when the
/// last byte is completely sent. It should be called before closing the ISPI if
/// the last bytes of the transfer are important.\n
/// This function should not be called between transfers, in direct or DMA mode.
/// The @link #hal_IspiTxFifoLevel FIFO level @endlink for direct mode and the
/// @link #hal_IspiTxDmaDone DMA done indication @endlink for DMA allow
/// for a more optimized transmission.
///
/// @param csId Identifier of the Chip Select on which behalf the operation is
/// done. This parameter is ignored.
/// @return \c TRUE if the last tranfer is done and the Tx FIFO empty.\n
///         \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_IspiTxFinished(HAL_ISPI_CS_T csId);




// =============================================================================
// hal_IspiGetData
// -----------------------------------------------------------------------------
/// Get one datum.
///
/// This functions gets one element of 32 bits from the ISPI Rx Fifo.
/// If the Fifo was empty at the time #hal_IspiGetData() is called, 0
/// is returned and \c recData is untainted. Otherwise one datum is get
/// from the Fifo and 1 is returned
///
/// @param csId Identifier of the ISPI Chip Select on which to get data.
/// @param  recData Pointer to store the received datum.
/// @return Returns the number of received data (1 or 0, if the Fifo is empty).
// =============================================================================
PUBLIC UINT32 hal_IspiGetData(HAL_ISPI_CS_T csId, UINT32* recData);

PUBLIC UINT32 hal_IspiGetDataBuffer(HAL_ISPI_CS_T csId, UINT32* destAddress, UINT32 length);


// =============================================================================
// hal_IspiRxFifoLevel
// -----------------------------------------------------------------------------
/// Get data quantity in the Spi Rx FIFO.
///
/// @param csId Identifier of the Chip Select on which behalf the operation is
/// done. This parameter is ignored.
/// @return The number of 32 bits data items in the Rx FIFO.
// =============================================================================
PUBLIC UINT8 hal_IspiRxFifoLevel(HAL_ISPI_CS_T csId);

// =============================================================================
// hal_IspiRegRead
// -----------------------------------------------------------------------------
/// Read a register through ISPI. It is executed with interrupt disabled.
///
/// @param csId Identifier of the ISPI Chip Select on which to get data.
/// @param  address ISPI Chip register address.
/// @return Returns register value.
// =============================================================================
PUBLIC UINT16 hal_IspiRegRead(HAL_ISPI_CS_T csId, UINT16 address);

// =============================================================================
// hal_IspiRegWrite
// -----------------------------------------------------------------------------
/// Write a register through ISPI. It is executed with interrupt disabled.
///
/// @param csId Identifier of the ISPI Chip Select on which to get data.
/// @param address ISPI Chip register address.
/// @param data Register value.
// =============================================================================
PUBLIC VOID hal_IspiRegWrite(HAL_ISPI_CS_T csId, UINT16 address, UINT16 data);

///  @} <- End of SPI group


#endif //_HAL_ISPI_H_






