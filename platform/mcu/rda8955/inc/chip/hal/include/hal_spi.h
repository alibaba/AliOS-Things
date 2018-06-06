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




#ifndef _HAL_SPI_H_
#define _HAL_SPI_H_

#include "cs_types.h"

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
/// A configuration structure of type HAL_SPI_CFG_T allows to define the SPI behavior.
/// The available configurations are as follow :
/// - @b Chip @b Select @b Choice \n
///          The SPI has three chip select, the available values are defined
///          as follow:
///          - #HAL_SPI_CS0 &mdash; The chip select 0
///          - #HAL_SPI_CS1 &mdash; The chip select 1
///          - #HAL_SPI_CS2 &mdash; The chip select 2
///          - #HAL_SPI_CS3 &mdash; The chip select 3
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
/// - @b FIFO @b Controls \n
///          The SPI module can be setup to generate an interrupt when the
///          reception FIFO is above a configurable threshold (Rx FIFO trigger)
///          or when the emission FIFO is below a configurable threshold (Tx
///          FIFO trigger).\n
///          \n
///          The available triggers for the reception FIFO are:
///          - #HAL_SPI_RX_TRIGGER_1_BYTE &mdash; One Data received in the Rx
///          FIFO
///          - #HAL_SPI_RX_TRIGGER_4_BYTE &mdash; 4 Data received in the Rx FIFO
///          - #HAL_SPI_RX_TRIGGER_8_BYTE &mdash; 8 Data received in the Rx FIFO
///          - #HAL_SPI_RX_TRIGGER_12_BYTE &mdash; 12 Data received in the Rx
///          FIFO
///          .
///          \n
///          The available triggers for the transmission FIFO are:
///          - #HAL_SPI_TX_TRIGGER_1_EMPTY &mdash; One Data spot is empty in the
///          Tx FIFO
///          - #HAL_SPI_TX_TRIGGER_4_EMPTY &mdash; 4 Data spots are empty in the
///          Tx FIFO
///          - #HAL_SPI_TX_TRIGGER_8_EMPTY &mdash; 8 Data spots are empty in the
///          Tx FIFO
///          - #HAL_SPI_TX_TRIGGER_12_EMPTY &mdash; 12 Data spots are empty in
///          the Tx FIFO
///          .
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
///          - @b Direct @b IRQ: \n
///          The application sends/receives the data directly to/from the
///          hardware module. The number of bytes actually sent/received is
///          returned. An Irq can be generated when the Tx/Rx FIFO reaches the
///          pre-programmed level. \n
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
///          - @b DMA @b IRQ: \n
///          The application sends/receives the data through a DMA to the
///          hardware module. The function returns 0 when no DMA channel is
///          available. No bytes are sent. The function returns the number of
///          bytes to send when a DMA resource is available. They will all be
///          sent. An IRQ is generated when the current transfer is finished.
///          No new DMA transfer in the same direction will be allowed before
///          the end of the previous transfer.
///          .
///          \n
///
/// @par Interruption Mask
/// To set the interruption mask and enable the interrupt, use the dedicated
/// functions: #hal_SpiIrqSetMask and #hal_SpiIrqSetHandler.
///
/// @par
/// The mask can be set up with a #HAL_SPI_IRQ_STATUS_T structure. See
/// this type description for details on the different masking available.
///
/// @par Configuration Structure
/// A configuration structure allows to open or change the SPI with the desired
/// parameters : #HAL_SPI_CFG_T.
///
/// @par Pattern Mode
/// The infinite transfer done by #hal_SpiStartInfiniteWrite provides the clock
/// coping with the trashing reads before the pattern is finally seen).
/// When the pattern mode is enabled, data received on the SPI bus will only
/// be put in the Rx FIFO after a given pattern has been received.
/// The infinite transfert can be stopped by two ways:
/// - manually, by a call to #hal_SpiStopInfiniteWrite;
/// - automatically (prefered behaviour): the infinite transfer is stopped
/// by the RxDmaDone IRQ generated by the end of the reception done parallelly,
/// if the Rx mode is configured as #HAL_SPI_DMA_POLLING or #HAL_SPI_DMA_IRQ.
/// See the documentation associated with this function and its parameters for
/// more details.
///
/// @{

// =============================================================================
//  MACROS
// =============================================================================
/// Defines the SPI minimum frame size
#define HAL_SPI_MIN_FRAME_SIZE          4

/// Defines the SPI maximum frame size
#define HAL_SPI_MAX_FRAME_SIZE          32


/// Defines the maximum number of Chip Select
/// possible on a same given SPI.
#define     HAL_SPI_CS_MAX_QTY      4






// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
// HAL_SPI_ID_T
// -----------------------------------------------------------------------------
/// Defines which SPI is used by a function of the SPI. Depending on the
/// hardware you are running on, some of those SPIs might not exist
// =============================================================================
#ifdef CHIP_DIE_8955
typedef enum
{
    HAL_SPI,
    HAL_SPI_2,
    HAL_SPI_QTY = 2
} HAL_SPI_ID_T;
#else
typedef enum
{
    HAL_SPI,
    HAL_SPI_2,
#if (CHIP_SPI2_CS_QTY == 0)
    HAL_SPI_QTY = 1
#else
    HAL_SPI_QTY = 2
#endif
} HAL_SPI_ID_T;
#endif




// =============================================================================
// HAL_SPI_DELAY_T
// -----------------------------------------------------------------------------
/// Delays
/// Used to define the configuration delays
// =============================================================================
typedef enum
{
    /// Delay of 0 half-period
    HAL_SPI_HALF_CLK_PERIOD_0,
    /// Delay of 1 half-period
    HAL_SPI_HALF_CLK_PERIOD_1,
    /// Delay of 2 half-period
    HAL_SPI_HALF_CLK_PERIOD_2,
    /// Delay of 3 half-period
    HAL_SPI_HALF_CLK_PERIOD_3,

    HAL_SPI_HALF_CLK_PERIOD_QTY
} HAL_SPI_DELAY_T;



// =============================================================================
// HAL_SPI_CS_T
// -----------------------------------------------------------------------------
/// Chip Select
/// Used to select a Chip Select
// =============================================================================
typedef enum
{
    /// Chip Select 0
    HAL_SPI_CS0 = 0,
    /// Chip Select 1
    HAL_SPI_CS1,
    /// Chip Select 2
    HAL_SPI_CS2,
    /// Chip Select 3 - It doesn't necessarily exists for a given SPI.
    HAL_SPI_CS3,

    HAL_SPI_CS_QTY
} HAL_SPI_CS_T;



// =============================================================================
// HAL_SPI_RX_TRIGGER_CFG_T
// -----------------------------------------------------------------------------
/// Reception  trigger (or treshold) level.
/// The SPI module can be setup to generate an interrupt when the reception FIFO
/// is above a configurable threshold (Rx fifo trigger)
// =============================================================================
typedef enum
{
    /// 1 Data received in the Rx FIFO
    HAL_SPI_RX_TRIGGER_1_BYTE,
    /// 2 Data received in the Rx FIFO
    HAL_SPI_RX_TRIGGER_4_BYTE,
    /// 3 Data received in the Rx FIFO
    HAL_SPI_RX_TRIGGER_8_BYTE,
    /// 4 Data received in the Rx FIFO
    HAL_SPI_RX_TRIGGER_12_BYTE,

    HAL_SPI_RX_TRIGGER_QTY
} HAL_SPI_RX_TRIGGER_CFG_T;



// =============================================================================
// HAL_SPI_TX_TRIGGER_CFG_T
// -----------------------------------------------------------------------------
/// Tranmission FIFO trigger (or treshold) level.
/// The SPI module can be setup to generate an interrupt when the emission FIFO
/// is above a configurable threshold (Tx FIFO trigger)
// =============================================================================
typedef enum
{
    /// 1 Data spot is empty in the Tx FIFO
    HAL_SPI_TX_TRIGGER_1_EMPTY,
    /// 2 Data spots are empty in the Tx FIFO
    HAL_SPI_TX_TRIGGER_4_EMPTY,
    /// 8 Data spots are empty in the Tx FIFO
    HAL_SPI_TX_TRIGGER_8_EMPTY,
    /// 12 Data spots are empty in the Tx FIFO
    HAL_SPI_TX_TRIGGER_12_EMPTY,

    HAL_SPI_TX_TRIGGER_QTY
} HAL_SPI_TX_TRIGGER_CFG_T;



// =============================================================================
// HAL_SPI_TRANSFERT_MODE_T
// -----------------------------------------------------------------------------
/// Data transfert mode: via DMA or direct.
/// To allow for an easy use of the SPI modules, a non blocking Hardware
/// Abstraction Layer interface is provided. Each transfer direction
/// (send/receive) can be configured as:
// =============================================================================
typedef enum
{
    /// Direct polling: The application sends/receives the data directly to/from
    /// the hardware module. The number of bytes actually sent/received is
    /// returned. No Irq is generated.
    HAL_SPI_DIRECT_POLLING = 0,

    /// Direct IRQ: The application sends/receives the data directly to/from
    /// the hardware module. The number of bytes actually sent/received is
    /// returned. An Irq can be generated when the Tx/Rx FIFO reaches the
    /// pre-programmed level.
    HAL_SPI_DIRECT_IRQ,

    /// DMA polling: The application sends/receives the data through a DMA to
    /// the hardware module. The function returns 0 when no DMA channel is
    /// available. No bytes are sent. The function returns the number of bytes
    /// to send when a DMA resource is available. They will all be sent. A
    /// function allows to check if the previous DMA transfer is finished. No
    /// new DMA transfer in the same direction will be allowed before the end
    /// of the previous transfer.
    HAL_SPI_DMA_POLLING,

    /// DMA IRQ: The application sends/receives the data through a DMA to the
    /// hardware module. The function returns 0 when no DMA channel is
    /// available. No bytes are sent. The function returns the number of bytes
    /// to send when a DMA resource is available. They will all be sent. An
    /// IRQ is generated when the current transfer is finished. No new DMA
    /// transfer in the same direction will be allowed before the end of the
    /// previous transfer.
    HAL_SPI_DMA_IRQ,

    /// The SPI is off
    HAL_SPI_OFF,

    HAL_SPI_TM_QTY
} HAL_SPI_TRANSFERT_MODE_T;



// =============================================================================
// HAL_SPI_IRQ_STATUS_T
// -----------------------------------------------------------------------------
/// This structure is used to represent the IRQ status and mask
/// of the SPI module.
// =============================================================================
typedef struct
{
    /// receive FIFO overflow irq
    UINT32 rxOvf:1;
    /// transmit FIFO threshold irq
    UINT32 txTh:1;
    /// transmit Dma Done irq
    UINT32 txDmaDone:1;
    /// receive FIFO threshold irq
    UINT32 rxTh:1;
    /// receive Dma Done irq
    UINT32 rxDmaDone:1;
} HAL_SPI_IRQ_STATUS_T;



// =============================================================================
// HAL_SPI_IRQ_HANDLER_T
// -----------------------------------------------------------------------------
/// Type of the user IRQ handler
// =============================================================================
typedef VOID (*HAL_SPI_IRQ_HANDLER_T)(HAL_SPI_IRQ_STATUS_T);




// =============================================================================
// HAL_SPI_CFG_T
// -----------------------------------------------------------------------------
/// Structure for configuration.
/// A configuration structure allows to open or change the SPI with the desired
/// parameters.
// =============================================================================
typedef struct
{
    /// Select the Chip Select
    HAL_SPI_CS_T enabledCS;

    /// Polarity of this CS
    BOOL csActiveLow;

    /// When \c TRUE, the emission commands will fill the Rx FIFO with read
    /// data, thus enabling the ability to receive data. \n
    /// When \c FALSE, nothing is written in the Rx FIFO when data are sent.
    /// It is not possible to read received data, which are discarded.
    BOOL inputEn;

    /// If the first edge after the CS activation is a falling edge, set to
    /// \c TRUE.\n Otherwise, set to \c FALSE.
    BOOL clkFallEdge;

    /// The delay between the CS activation and the first clock edge,
    /// can be 0 to 2 half clocks.
    HAL_SPI_DELAY_T clkDelay;

    /// The delay between the CS activation and the output of the data,
    /// can be 0 to 2 half clocks.
    HAL_SPI_DELAY_T doDelay;

    /// The delay between the CS activation and the sampling of the input data,
    /// can be 0 to 3 half clocks.
    HAL_SPI_DELAY_T diDelay;

    /// The delay between the end of transfer and the CS deactivation, can be
    /// 0 to 3 half clocks.
    HAL_SPI_DELAY_T csDelay;

    /// The time when the CS must remain deactivated before a new transfer,
    /// can be 0 to 3 half clocks.
    HAL_SPI_DELAY_T csPulse;

    /// Frame size in bits
    UINT32 frameSize;

    /// OE ratio - Value from 0 to 31 is the number of data out to transfert
    /// before the SPI_DO pin switches to input. When 0m the SPI_DO pin switching
    /// direction mode is not enabled.
    UINT8 oeRatio;

    /// inputSel- Value from 0 to 31 is the number of data out to transfert
    UINT8 inputSel;

    /// SPI maximum clock frequency: the SPI clock will be the highest
    /// possible value inferior to this parameter.
    UINT32 spiFreq;

    /// Value for the reception FIFO above which an interrupt may be generated.
    HAL_SPI_RX_TRIGGER_CFG_T rxTrigger;

    /// Value for the emission FIFO above which an interrupt may be generated.
    HAL_SPI_TX_TRIGGER_CFG_T txTrigger;

    /// Reception transfer mode
    HAL_SPI_TRANSFERT_MODE_T rxMode;

    /// Emission transfer mode
    HAL_SPI_TRANSFERT_MODE_T txMode;

    /// IRQ mask for this CS
    HAL_SPI_IRQ_STATUS_T mask;

    /// IRQ handler for this CS;
    HAL_SPI_IRQ_HANDLER_T handler;

} HAL_SPI_CFG_T;



// =============================================================================
// HAL_SPI_PATTERN_MODE_T
// -----------------------------------------------------------------------------
/// The pattern mode can be used in two: record data after the pattern has been
/// received on the SPI, or data are not recorded while the pattern is seens on
/// the bus. HAL_SPI_PATTERN_WHILE feature is not available for every chip.
// =============================================================================
typedef enum
{
    /// Standard mode, we don't care about the pattern
    HAL_SPI_PATTERN_NO,
    /// No data written until the pattern is read on the bus
    HAL_SPI_PATTERN_UNTIL,
    /// No data are written while the pattern is read on the bus
    HAL_SPI_PATTERN_WHILE
} HAL_SPI_PATTERN_MODE_T;

// =============================================================================
// HAL_SPI_INFINITE_TX_T
// -----------------------------------------------------------------------------
/// That type describes the characteristics and configuration of an infinite
/// transfer on a SPI CS. That is typically used for devices like MMC which use
/// the pattern mode read feature. (The infinite transfer provides the clock
/// coping with the trashing reads before the pattern is finally seen).
/// When the pattern mode is enabled, data received on the SPI bus will only
/// be put in the Rx FIFO after a given pattern has been received.
/// The SPI will continue transfering data (zeros or ones, depending on the
/// configuration of sendOne) after the Tx fifo is empty (i.e. the IFC DMA
/// is finished) and until the Rx transfer is completed (i.e. the pattern
/// has been received and the Rx transfer size is reached).
/// After you start the inifinite write mode, you have to use the usual
/// send and get data functions.
/// The infinite transfert can be stopped by two ways:
/// - manually, by a call to #hal_SpiStopInfiniteWrite;
/// - automatically (prefered behaviour): the infinite transfer is stopped
/// by the RxDmaDone IRQ generated by the end of the reception done parallelly,
/// if the Rx mode is configured as #HAL_SPI_DMA_POLLING or #HAL_SPI_DMA_IRQ.
// =============================================================================
typedef struct
{
    /// \c TRUE if the infinite transfer is done by sending ones,
    /// \c FALSE if it is done by sending zeroes.
    BOOL sendOne;

    /// \c TRUE enable the auto-stopping feature of the infinite transfer.
    BOOL autoStop;

    /// Described the pattern mode. That is the read buffer
    /// will only be filled with data received after a given pattern,
    /// specified by the following field, is seen on the line,
    /// or after the pattern is no more read on it (while the pattern
    /// is seen, no data is recorded).
    HAL_SPI_PATTERN_MODE_T patternMode;

    /// Value of the pattern waited before useful data are received.
    UINT32 pattern;
} HAL_SPI_INFINITE_TX_T;

// =============================================================================
// HAL_SPI_PIN_T
// -----------------------------------------------------------------------------
/// SPI Pin
///
/// Used to specify an SPI pin.
// =============================================================================
typedef enum
{
    /// Chip select 0 pin
    HAL_SPI_PIN_CS0 = 0,
    /// Chip select 1 pin
    HAL_SPI_PIN_CS1,
    /// Chip select 2 pin, if available
    HAL_SPI_PIN_CS2,
    /// Chip select 3 pin, if available
    HAL_SPI_PIN_CS3,
    /// Clock pin
    HAL_SPI_PIN_CLK,
    /// Data out pin
    HAL_SPI_PIN_DO,
    /// Data in pin
    HAL_SPI_PIN_DI,
    HAL_SPI_PIN_LIMIT
} HAL_SPI_PIN_T;



// =============================================================================
// HAL_SPI_PIN_STATE_T
// -----------------------------------------------------------------------------
/// SPI Pin State
///
/// Used to specify the state of an SPI pin. It can be
/// zero, one or Z (high impedence, tri-state).
// =============================================================================
typedef enum
{
    HAL_SPI_PIN_STATE_SPI   =   0,
    HAL_SPI_PIN_STATE_Z     =   1,
    HAL_SPI_PIN_STATE_0     =   2,
    HAL_SPI_PIN_STATE_1     =   3
} HAL_SPI_PIN_STATE_T;



// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// hal_SpiIrqSetHandler
// -----------------------------------------------------------------------------
/// Set the user irq handler function
/// @param id Identifier of the SPI for which the function is called.
/// @param handler User function called in case of SPI-related IRQ.
// =============================================================================
PUBLIC VOID hal_SpiIrqSetHandler(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum, HAL_SPI_IRQ_HANDLER_T handler);



// =============================================================================
// hal_SpiIrqSetMask
// -----------------------------------------------------------------------------
/// Set the irq mask
/// @param id Identifier of the SPI for which the function is called.
/// @param mask Mask to set.
// =============================================================================
PUBLIC VOID hal_SpiIrqSetMask(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum, HAL_SPI_IRQ_STATUS_T mask);



// =============================================================================
// hal_SpiIrqGetMask
// -----------------------------------------------------------------------------
/// Get the irq mask.
/// @param id Identifier of the SPI for which the function is called.
/// @return The IRQ mask.
// =============================================================================
PUBLIC HAL_SPI_IRQ_STATUS_T hal_SpiIrqGetMask(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum);



// =============================================================================
// hal_SpiOpen
// -----------------------------------------------------------------------------
/// Open a SPI CS driver.
/// This function enables the SPI in the mode selected by \c spiCfg.
/// <B> The polarity of both the Chip Select must be set in the configuration
/// structure of HAL in the tgt_BoardConfig structure. The knowledge about those
/// two polarities
/// is needed by the driver, and it cannot operate without them. </B>
///
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum Chip Select for which this configuration applies.
/// @param spiCfg The configuration for SPI
// =============================================================================
PUBLIC VOID hal_SpiOpen(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum, CONST HAL_SPI_CFG_T* spiCfg);



// =============================================================================
// hal_SpiActivateCs
// -----------------------------------------------------------------------------
/// Activate a Chip Select previously opened by a call to #hal_SpiOpen. Once
/// the CS is no more used, it must be deactivated by a call to
/// #hal_SpiDeActivateCs.
///
/// This is a non blocking function, the return value *MUST* be checked
///
/// If an attempt is made to activate a previously opened CS, two case can
/// happen:
/// - the SPI bus is free: the CS will be activated immediatly and
/// the function returns TRUE.
/// - another CS is activated: the new activation is not done and
/// the function returns FALSE, the calling task must retry later.
///
/// The hal_SpiActivateCs function will always exit immediately.
///
/// A CS must be activated before being used, and only one CS at a time can be
/// activated.
///
/// This function requests a resource corresponding to the needed frequency.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum CS to activate.
/// @return \c TRUE if the cs has been successfully activated
/// \c FALSE when another cs is already active
// =============================================================================
PUBLIC BOOL hal_SpiActivateCs(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum);




// =============================================================================
// hal_SpiDeActivateCs
// -----------------------------------------------------------------------------
/// Deactivate a Chip Select. This is the opposite operation to #hal_SpiActivateCs.
/// It must be called after #hal_SpiActivateCs to allow the activation of another
/// CS. The deactivation of a non-activated CS will trig an ASSERT.
///
/// This function release the resource to #HAL_SYS_FREQ_32K.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum CS to deactivate.
// =============================================================================
PUBLIC VOID hal_SpiDeActivateCs(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum);

// =============================================================================
// hal_SpiRxIfcChannelRelease
// -----------------------------------------------------------------------------
/// Force the release of the RxIfc channel owned by
/// the request HAL_IFC_SPI_RX.
// =============================================================================
PUBLIC VOID hal_SpiRxIfcChannelRelease(HAL_SPI_ID_T id);


// =============================================================================
// hal_SpiTxIfcChannelRelease
// -----------------------------------------------------------------------------
/// Force the release of the TxIfc channel owned by
/// the request HAL_IFC_SPI_TX.
// =============================================================================
PUBLIC VOID hal_SpiTxIfcChannelRelease(HAL_SPI_ID_T id);


// =============================================================================
// hal_SpiClose
// -----------------------------------------------------------------------------
/// Close the SPI CS \c csNum.  To use it again,
/// it must be opened and activated again.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum SPI CSto close.
// =============================================================================
PUBLIC VOID hal_SpiClose(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum);



// =============================================================================
// hal_SpiSendData
// -----------------------------------------------------------------------------
/// Send a bunch of data.
/// This functions sends \c length bytes starting from the address \c
/// start_address. The number returned is the number of bytes actually sent.
/// In DMA mode, this function returns 0 when no DMA channel is available, it
/// returns length otherwise. This function is not to be used for the infinite
/// mode. Use instead the dedicated driver function #hal_SpiStartInfiniteWrite.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum The CS to use to send the data. This cs must be activated before
/// sending data.
/// @param startAddress Pointer on the buffer to send
/// @param length number of bytes to send (Up to 4 kB) .
/// @return When in DMA mode, returns 0 if no DMA channel is available. \n
///         In direct mode or DMA mode with an available channel, returns the
///         number of sent bytes.
// =============================================================================
PUBLIC UINT32 hal_SpiSendData(
    HAL_SPI_ID_T id,
    HAL_SPI_CS_T csNum,
    CONST UINT8* startAddress,
    UINT32 length);

/*
PUBLIC UINT16 hal_SpiSendData(
                HAL_SPI_ID_T id,
                HAL_SPI_CS_T csNum,
                UINT8* startAddress,
                UINT16 length);
*/


// =============================================================================
// hal_SpiTxFifoAvail
// -----------------------------------------------------------------------------
/// Get available data spaces in the Spi Tx FIFO.
/// This function returns the size of the available space in the Tx FIFO.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param  csNum   Chip select
/// @return Tthe size of the available space in the Tx FIFO.
// =============================================================================
PUBLIC UINT8 hal_SpiTxFifoAvail(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum);



// =============================================================================
// hal_SpiTxDmaDone
// -----------------------------------------------------------------------------
/// Check if the transmission is finished.
///
/// This function returns \c TRUE when the last DMA transfer is finished.
/// Before sending new data in DMA mode, the previous transfer must be finished,
/// hence the use of this function for polling.\n
/// Note that the DMA transfer can be finished but the Tx FIFO of the SPI is
/// not empty. Before shutting down the SPI, one must check that the SPI FIFO
/// is empty and that the last byte has been completely sent by using
/// #hal_SpiTxFinished.\n
/// Even if the Tx FIFO is not empty, if a previous DMA transfer is over, one
/// can start a new DMA transfert
///
/// @param id Identifier of the SPI for which the function is called.
/// @param  csNum   Chip select
/// @return \c TRUE is the previous DMA transfert is finshed.\n
///          \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_SpiTxDmaDone(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum);



// =============================================================================
// hal_SpiTxFinished
// -----------------------------------------------------------------------------
/// Check if the last transfer is done
/// This function returns \c TRUE when the transmit FIFO is empty and when the
/// last byte is completely sent. It should be called before closing the SPI if
/// the last bytes of the transfer are important.\n
/// This function should not be called between transfers, in direct or DMA mode.
/// The @link #hal_SpiTxFifoAvail FIFO availability @endlink for direct mode and the
/// @link #hal_SpiTxDmaDone DMA done indication @endlink for DMA allow
/// for a more optimized transmission.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param  csNum   Chip select
/// @return \c TRUE if the last tranfer is done and the Tx FIFO empty.\n
///         \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_SpiTxFinished(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum);


// =============================================================================
// hal_SpiGetData
// -----------------------------------------------------------------------------
/// Get a bunch of data.
///
/// This functions gets \c length bytes from the SPI and stores them starting
/// from the address \c dest_address. The number returned is the number of bytes
/// actually received. In DMA mode, this function returns 0 when no DMA channel
/// is available. It returns length otherwise.
/// A pattern mode is available.
/// When enabled, the actual reception and count of received bytes starts
/// only after a precised pattern has been read on the SPI. This pattern is not
/// copied into the reception buffer.
/// This feature is only enabled by using #hal_SpiStartInfiniteWrite.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param  csNum   Chip select
/// @param destAddress Pointer on the buffer to store received data
/// @param length Number of byte to receive.
/// @return When in DMA mode, returns 0 if no DMA channel is available. \n
///         In direct mode or DMA mode with an available channel, returns the
///         number of received bytes.
// =============================================================================
PUBLIC UINT32 hal_SpiGetData(
    HAL_SPI_ID_T id,
    HAL_SPI_CS_T csNum,
    UINT8*      destAddress,
    UINT32      length
);

// =============================================================================
// hal_SpiRxFifoLevel
// -----------------------------------------------------------------------------
/// Get data quantity in the Spi Rx FIFO.
///
/// Returns the number of bytes in the Rx FIFO.
/// @param id Identifier of the SPI for which the function is called.
/// @param  csNum   Chip select
/// @return The number of bytes in the Rx FIFO
// =============================================================================
PUBLIC UINT8 hal_SpiRxFifoLevel(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum);



// =============================================================================
// hal_SpiRxDmaDone
// -----------------------------------------------------------------------------
/// Check if the reception is finished.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param  csNum   Chip select
/// @return This function returns \c TRUE when the last DMA transfer is finished.
/// Before receiving new data in DMA mode, the previous transfer must be
/// finished, hence the use of this function for polling.
// =============================================================================
PUBLIC BOOL hal_SpiRxDmaDone(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum);





// =============================================================================
// hal_SpiStartInfiniteWriteMode
// -----------------------------------------------------------------------------
/// Do a stream write on the given CS.
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum CS on which send the stream.
/// @param infTx Pointer to the structure describing the streaming write.
// =============================================================================
PUBLIC VOID hal_SpiStartInfiniteWriteMode(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum, CONST HAL_SPI_INFINITE_TX_T* infTx);




// =============================================================================
// hal_SpiStopInfiniteWriteMode
// -----------------------------------------------------------------------------
/// Stop the stream write on the given CS.
/// This function is useless when the auto-stop mode is enabled.
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum CS on which send the stream.
// =============================================================================
PUBLIC VOID hal_SpiStopInfiniteWriteMode(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum);



// =============================================================================
// hal_SpiFlushFifos
// -----------------------------------------------------------------------------
/// Flush both SPI Fifos.
/// @param id Identifier of the SPI for which the function is called.
// =============================================================================
PUBLIC VOID hal_SpiFlushFifos(HAL_SPI_ID_T id);


// =============================================================================
// hal_SpiClearRxDmaDone
// -----------------------------------------------------------------------------
/// clear the RX DMA Done status
/// @param id Identifier of the SPI for which the function is called.
// =============================================================================
PUBLIC VOID hal_SpiClearRxDmaDone(HAL_SPI_ID_T id);


// =============================================================================
// hal_SpiClearTxDmaDone
// -----------------------------------------------------------------------------
/// clear the TX DMA Done status
/// @param id Identifier of the SPI for which the function is called.
// =============================================================================
PUBLIC VOID hal_SpiClearTxDmaDone(HAL_SPI_ID_T id);


// =============================================================================
// hal_SpiIfcTc
// -----------------------------------------------------------------------------
/// @param id Identifier of the SPI for which the function is called.
/// @return ifcTc
// =============================================================================
PUBLIC UINT16 hal_SpiGetRxIfcTc(HAL_SPI_ID_T id);




// =============================================================================
// hal_SpiForcePin
// -----------------------------------------------------------------------------
/// Force an SPI pin to a certain state or release the force mode
/// and put the pin back to normal SPI mode.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param pin Identification of the pins to be forced.
/// @param state State to enforce.
// =============================================================================
PUBLIC VOID hal_SpiForcePin(HAL_SPI_ID_T id, HAL_SPI_PIN_T pin,  HAL_SPI_PIN_STATE_T state);



///  @} <- End of SPI group


#endif //_HAL_SPI_H_






