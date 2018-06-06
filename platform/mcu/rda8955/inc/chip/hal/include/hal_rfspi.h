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




#ifndef _HAL_RFSPI_H_
#define _HAL_RFSPI_H_



// TODO add doc
/// @defgroup rfspi HAL RFSPI driver
///
///     description
///     operation
///     etc
///
///     ...
///
/// @{


#include "cs_types.h"
#include "chip_id.h"
#include "hal_sys.h"


// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
// HAL_RFSPI_MIN_FRAME_SIZE
// -----------------------------------------------------------------------------
/// Minimum frame size
// =============================================================================
#define HAL_RFSPI_MIN_FRAME_SIZE         3

// =============================================================================
// HAL_RFSPI_MAX_FRAME_SIZE
// -----------------------------------------------------------------------------
/// Maximum frame size
// =============================================================================
#define HAL_RFSPI_MAX_FRAME_SIZE         31

// =============================================================================
// HAL_RFSPI_MAX_CLK_DIVIDER
// -----------------------------------------------------------------------------
/// Maximum clock divider between the system clock and the FR-SPI clock
// =============================================================================
#define HAL_RFSPI_MAX_CLK_DIVIDER        0xFFFFFF

// =============================================================================
// HAL_RFSPI_CMD_MAX_BYTE_QTY
// -----------------------------------------------------------------------------
/// Maximum size of command in bytes
// =============================================================================
#define HAL_RFSPI_CMD_MAX_BYTE_QTY       0xFF

// =============================================================================
// HAL_RFSPI_GAIN_MAX_BYTE_QTY
// -----------------------------------------------------------------------------
/// Maximum size of a gaim
// =============================================================================
#define HAL_RFSPI_GAIN_MAX_BYTE_QTY      0xF

// RF SPI Software Fifo Handling:
// TODO : Dynamic allocation depending on Xcver at boot
// TODO : Find how to get good eval of max memory usage

// =============================================================================
// SPI_HWFIFO_BYTE_QTY
// -----------------------------------------------------------------------------
/// Driver's internal HW SPI FIFO. (RF-spi ... )
// =============================================================================
#define SPI_HWFIFO_BYTE_QTY 16

// =============================================================================
// RFSPI_MAX_CMD_GRP
// FIXME This is based on the HW limitation - should come from ASIC description
//  -----------------------------------------------------------------------------
// =============================================================================
#define RFSPI_MAX_CMD_GRP 20

#ifdef CHIP_DIE_8909
// =============================================================================
// HWP_RFSPI
// RFSPI HW FU base address for GSM and NBIOT
//  -----------------------------------------------------------------------------
// =============================================================================
#define HWP_RFSPI(addr)                   ((HWP_RF_SPI_T*) KSEG1(addr))

// =============================================================================
// hwp_rfSpi
// RFSPI MIPS pointer for GSM and NBIOT
//  -----------------------------------------------------------------------------
// =============================================================================
#define hwp_rfSpi                         (HWP_RFSPI(RFSPI))

// =============================================================================
// rfspi_channel
// rfspi_channel index for GSM and NBIOT
//  -----------------------------------------------------------------------------
// =============================================================================
#define rfspi_channel                     (g_rfspi_channel)

// RFSPI HW address
extern UINT32 RFSPI;

// RFSPI channel index
extern UINT8 g_rfspi_channel;

#endif

// =============================================================================
// RFSPI_DATA_BUF_SIZE
// -----------------------------------------------------------------------------
// The Queue Data Buffer must be able to contain one frame of command
// =============================================================================
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE) || \
    (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || \
    (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)

#ifdef USE_BB_DC_CANCEL
#define RFSPI_DATA_BUF_SIZE 410
#else
#define RFSPI_DATA_BUF_SIZE 284
#endif

#else // smart phones, 8809P or later feature phones
#define RFSPI_DATA_BUF_SIZE 448
#endif

// =============================================================================
// RFSPI_SW_FIFO_SIZE
// -----------------------------------------------------------------------------
/// Software FIFO size
// The SW-Fifo Data must be able to contain more than a
// full frame of commands, to allow for "pre-filling" at FrameEnd
// =============================================================================
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE) || \
    (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || \
    (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)

#ifdef USE_BB_DC_CANCEL
#define RFSPI_SW_FIFO_SIZE 452
#else
#define RFSPI_SW_FIFO_SIZE 332
#endif

#else // smart phones, 8809P or later feature phones
#define RFSPI_SW_FIFO_SIZE 500
#endif

#if(XCPU_CACHE_MODE_WRITE_BACK==1)
#define  HAL_RFSPI_SW_FIFO_LOC  HAL_UNCACHED_DATA_INTERNAL
#else
#define  HAL_RFSPI_SW_FIFO_LOC  HAL_DATA_INTERNAL
#endif

// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
// HAL_RFSPI_CMD_T
// -----------------------------------------------------------------------------
/// Commands to send through the RFSPI
// =============================================================================
typedef struct hal_RfspiCmdS
{
    /// Pointer to next SpiCommand
    struct hal_RfspiCmdS* next;
    /// Pointer to the spi-command data
    UINT8* data;
    /// Spi-command date in Qb
    UINT16 date;
    /// Index of the window to which the command belongs
    UINT8 winIdx;
    /// Size in byte of the command
    UINT8 byteQty;
    /// Tag for maskable commands
    BOOL maskable;
} HAL_RFSPI_CMD_T;


// =============================================================================
// HAL_RFSPI_CMD_QUEUE_T
// -----------------------------------------------------------------------------
/// RFSPI command queue
// =============================================================================
typedef struct
{
    HAL_RFSPI_CMD_T cmdBuf[RFSPI_MAX_CMD_GRP];
    UINT8 dataBuf[RFSPI_DATA_BUF_SIZE];

    // First command (temporally)
    HAL_RFSPI_CMD_T* firstCmd;
    // table index for commands
    UINT8 cmdIdx;
    // table index for data
    UINT16 dataIdx;
} HAL_RFSPI_CMD_QUEUE_T;


// =============================================================================
// HAL_RFSPI_SW_FIFO_T
// -----------------------------------------------------------------------------
/// Type of the software fifo.
// =============================================================================
typedef struct
{
    VOLATILE UINT8 data[RFSPI_SW_FIFO_SIZE];
    UINT16 curPos;
    UINT16 addedData;
#ifdef RFSPI_IFC_WORKAROUND
    BOOL forbidIfcStart;
#endif // RFSPI_IFC_WORKAROUND

} HAL_RFSPI_SW_FIFO_T;

// =============================================================================
// HAL_RFSPI_GAIN_T
// -----------------------------------------------------------------------------
/// Type of the gains to send through the RF-SPI
// =============================================================================
typedef struct
{
    /// Number of byte to send
    UINT32 byteQty;
    /// Gains
    UINT8 *data;
} HAL_RFSPI_GAIN_T;

// =============================================================================
// HAL_RFSPI_CLK_T
// -----------------------------------------------------------------------------
/// Possible RF-SPI clocks.
// =============================================================================
typedef enum
{
    HAL_RFSPI_26M_CLK,
    HAL_RFSPI_19_5M_CLK,
    HAL_RFSPI_13M_CLK,
    HAL_RFSPI_6_5M_CLK,
    HAL_RFSPI_3_25M_CLK,
    HAL_RFSPI_60M_CLK,
    HAL_RFSPI_61_44M_CLK
} HAL_RFSPI_CLK_T;

// =============================================================================
// HAL_RFSPI_DELAY_T
// -----------------------------------------------------------------------------
/// possible timings for SPI delays
// =============================================================================
typedef enum
{
    HAL_RFSPI_HALF_CLK_PERIOD_0,
    HAL_RFSPI_HALF_CLK_PERIOD_1,
    HAL_RFSPI_HALF_CLK_PERIOD_2,
    HAL_RFSPI_HALF_CLK_PERIOD_3,

    HAL_RFSPI_HALF_CLK_PERIOD_QTY
} HAL_RFSPI_DELAY_T;


// =============================================================================
// HAL_RFSPI_CFG_T
// -----------------------------------------------------------------------------
/// RFSPI configuration structure, used to set the SPI configuration.
// =============================================================================
typedef struct
{
    /// Enable the RFSPI ?
    BOOL enable;
    /// Is the Chip Select active high
    BOOL csActiveHi;
    /// @todo fill
    BOOL modeDigRFRead;
    BOOL modeClockedBack2back;
    /// When set to 1 the inputs are activated, else only
    /// the output is driven and no data are stored in the receive FIFO.
    BOOL inputEn;
    /// The spi clock polarity: \n
    /// - when '0' the clock disabled level is low, and the first edge is
    /// a rising edge.
    /// - When '1' the clock disabled level is high, and the first edge
    /// is a falling edge.
    BOOL clkFallEdge;
    /// Transfert start to first edge delay
    HAL_RFSPI_DELAY_T clkDelay;
    /// Transfert start to first data out delay
    HAL_RFSPI_DELAY_T doDelay;
    /// Transfer start to first data in sample delay
    HAL_RFSPI_DELAY_T diDelay;
    /// Transfer start to chip select activation delay
    HAL_RFSPI_DELAY_T csDelay;
    /// Chip select deactivation to reactivation minimum delay
    HAL_RFSPI_DELAY_T csEndHold;
    /// @todo fill
    HAL_RFSPI_DELAY_T csEndPulse;
    /// @todo fill
    UINT8 turnAroundCycles;
    /// Frame size
    UINT32 frameSize;
    /// @todo fill
    UINT32 inputFrameSize;
} HAL_RFSPI_CFG_T;


// =============================================================================
// HAL_RFSPI_STATUS_T
// -----------------------------------------------------------------------------
/// Contains the RFSPI status
// =============================================================================
typedef struct
{
    /// \c TRUE when a transfer is in progress
    BOOL active;
    /// 1' when a new command (or gain) has been requested while a
    /// command was in progress. Write '1' to clear.
    BOOL errCmd;
    /// Gain table Overflow status
    BOOL tableOvf;
    /// Gain table Underflow status
    BOOL tableUdf;
    /// Command FIFO overflow status
    BOOL cmdOvf;
    /// Command FIFO underflow status
    BOOL cmdUdf;
    /// Command data FIFO overflow status
    BOOL dataOvf;
    /// Command data FIFO underflow status
    BOOL dataUdf;
    /// Rx FIFO overflow status
    BOOL rxOvf;
    /// Rx Fifo underflow status
    BOOL rxUdf;
    /// Number of command in the command FIFO
    INT32 cmdLevel;
    /// Number of byte in the command data FIFO
    INT32 dataLevel;
    /// Rx FIFO level
    INT32 rxLevel;
} HAL_RFSPI_STATUS_T;


// =============================================================================
// HAL_RFSPI_IRQ_STATUS_T
// -----------------------------------------------------------------------------
/// @todo TODO Establish usefulness and get rid of if unnecessary
/// This structure reflects the RFSPI IRQ cause and mask status
// =============================================================================
typedef struct
{
    /// the IRQ was triggered by the end of the DMA transfer to the cmd FIFO.
    UINT32 cmdDataDmaDone:1;
    /// reserved
    UINT32 :1;
    /// the IRQ was triggered because the Cmd_FIFO is empty.
    UINT32 cmdFifoEmpty:1;
    /// the IRQ was triggered because the Cmd_FIFO level is bellow the Cmd_Threshold.
    UINT32 cmdThreshold:1;
    /// the IRQ was triggered because the Rx_Data_FIFO is full.
    UINT32 rxFifoFull:1;
    /// the IRQ was triggered because the Rx_Data_FIFO level is over the Rx_Threshold.
    UINT32 rxThreshold:1;
    /// the IRQ was triggered because an error occured. Use the #hal_RfspiGetStatus
    /// function to get the status and know what happened
    UINT32 error:1;
} HAL_RFSPI_IRQ_STATUS_T;


// =============================================================================
// HAL_RFSPI_IRQ_HANDLER_T
// -----------------------------------------------------------------------------
/// Type for the user (should there be any) RFSPI IRQ handling function
// =============================================================================
typedef VOID (*HAL_RFSPI_IRQ_HANDLER_T)(HAL_RFSPI_IRQ_STATUS_T);




// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_RfspiOpen
// -----------------------------------------------------------------------------
/// Open the RF-SPI.
/// @param cfg Configuration applied to the bus.
/// @param reqFreq Maximum frequency required for the SPI
// =============================================================================
PUBLIC VOID hal_RfspiOpen(CONST HAL_RFSPI_CFG_T* cfg, HAL_RFSPI_CLK_T reqFreq);



// =============================================================================
// hal_RfspiGetStatus
// -----------------------------------------------------------------------------
/// Get SPI status
/// @param status Pointer to the status structure where the
/// status will be written.
/// @param clear If \c TRUE, the status register is cleared.
///              If \c FALSE, it is kept as is.
// =============================================================================
PUBLIC VOID hal_RfspiGetStatus(HAL_RFSPI_STATUS_T* status, BOOL clear);



// =============================================================================
// hal_RfspiConfigure
// -----------------------------------------------------------------------------
/// Configure the RFSPI
/// @param cfg Pointer to the structure containing the configuration
/// to apply
// =============================================================================
PUBLIC VOID hal_RfspiConfigure(CONST HAL_RFSPI_CFG_T* cfg);



// =============================================================================
// hal_RfspiClose
// -----------------------------------------------------------------------------
/// Closes the RF-SPI.
// =============================================================================
PUBLIC VOID hal_RfspiClose(VOID);



// =============================================================================
// hal_RfspiForceRead
// -----------------------------------------------------------------------------
/// Performs an SPI Read access from a Xcver.
/// The read is usually preceeded by a write access
/// that must setup the Xcver in read mode.
/// @param rdSize Number of bytes to read
/// @param dataOut Buffer where the read data are stored.
// =============================================================================
PUBLIC VOID hal_RfspiForceRead(UINT8 rdSize, UINT8* dataOut);



// =============================================================================
// hal_RfspiDigRfRead
// -----------------------------------------------------------------------------
/// Performs an SPI Read access inDigRf Mode
/// @param Cmd Formatted read command (read bit, address and dummy data)
/// @param dataOut Buffer where the read data are stored.
// =============================================================================
PUBLIC VOID hal_RfspiDigRfRead(UINT8 *Cmd,UINT8* dataOut);




// =============================================================================
// hal_RfspiTriggerCmd
// -----------------------------------------------------------------------------
///  Trigger the previously sent command (like the TCU)
// =============================================================================
PUBLIC VOID hal_RfspiTriggerCmd(VOID);



// =============================================================================
// hal_RfspiRestartGain
// -----------------------------------------------------------------------------
/// Place the gain read pointer at the beginning of the gain table.
// =============================================================================
PUBLIC VOID hal_RfspiRestartGain(VOID);



// =============================================================================
// hal_RfspiReloadGain
// -----------------------------------------------------------------------------
/// Place the write pointer at the beginning of the gain table allowing to
/// fill the tab
// =============================================================================
PUBLIC VOID hal_RfspiReloadGain(VOID);



// =============================================================================
// hal_RfspiNewCmd
// -----------------------------------------------------------------------------
/// Allocate and sort a new command in the command list
/// and setup the corresponding TCU event.
/// @param date Time when the command is to be issued.
/// @param winIdx Index of the window which the command belongs to.
/// @return A pointer to the newly created command
// =============================================================================
PUBLIC HAL_RFSPI_CMD_T * hal_RfspiNewCmd(UINT16 date, UINT8 winIdx);



// =============================================================================
// hal_RfspiPushData
// -----------------------------------------------------------------------------
/// Add data to a command
/// @param cmd The data are added to this command.
/// @param data Pointer to a buffer where the data to add are located.
/// @param byteQty Size of the command to add, in number of bytes.
// =============================================================================
PUBLIC VOID hal_RfspiPushData(HAL_RFSPI_CMD_T* cmd,
                              CONST UINT8* data, UINT8 byteQty);



// =============================================================================
// hal_RfspiRemWinCmd
// -----------------------------------------------------------------------------
/// Remove all Spi commands belonging to one Window from the queue.
/// @param winIdx The commands for this window are removed from the queue.
// =============================================================================
PUBLIC VOID hal_RfspiRemWinCmd(UINT8 winIdx);

// =============================================================================
// hal_RfspiInitQueue
// -----------------------------------------------------------------------------
/// Initializes the command queue.
// =============================================================================
PUBLIC VOID hal_RfspiInitQueue(VOID);

// =============================================================================
// hal_RfspiSetupFifo
// -----------------------------------------------------------------------------
/// Configure the RFSPI FIFO
// =============================================================================
PUBLIC VOID hal_RfspiSetupFifo(VOID);


// =============================================================================
// hal_RfspiFillFifo
// -----------------------------------------------------------------------------
/// This function is meant to be called once at FrameEnd (for next frame
/// pre-programming) and once at FINT (to finish frame programming)
/// @param nextFrame \c TRUE if this command is called for next frame programming.
// =============================================================================
PUBLIC VOID hal_RfspiFillFifo(BOOL nextFrame);

// =============================================================================
// hal_RfspiNewSingleCmd
// -----------------------------------------------------------------------------
/// This function should be used only in asyncronous mode together with
/// hal_RfspiSendCmd.
/// It allocates the full Software fifo space for a single command.
/// This command can be built normally (calls to hal_RfspiPushData) and must be
/// sent by a call to hal_RfspiSendCmd.
/// Note that calling this function destroys the command queue and context.
/// Therefore it must be called only when no syncronous commands are queued (i.e
/// at startup or wakeup). The SW Fifo must be re-initialized to be used again
/// in synchronous mode after calls to hal_RfspiNewSingleCmd.
/// @return A pointer to the created command
// =============================================================================
PUBLIC HAL_RFSPI_CMD_T* hal_RfspiNewSingleCmd(VOID);

// =============================================================================
// hal_RfspiSendCmd
// -----------------------------------------------------------------------------
/// Force an SPI command to be send IMMEDIATELY (no TCU timing)
/// Used only in special Init sequences (low power w/ no TCU)
/// The SPI command must be allocated with hal_RfspiNewSingleCmd.
// =============================================================================
PUBLIC VOID hal_RfspiSendCmd(HAL_RFSPI_CMD_T *spiCmd);


// =============================================================================
// hal_RfspiSendGain
// -----------------------------------------------------------------------------
/// Send SPI gain
/// @param gain Gain to send
// =============================================================================
PUBLIC VOID hal_RfspiSendGain(CONST HAL_RFSPI_GAIN_T* gain);

// =============================================================================
// hal_RfspiFlushFifo
// -----------------------------------------------------------------------------
/// Flush CMD Fifo
// =============================================================================
PUBLIC VOID hal_RfspiFlushFifo(VOID);

// =============================================================================
// hal_RfspiIsActive
// -----------------------------------------------------------------------------
/// Check if a spi command is running
// =============================================================================
PUBLIC BOOL hal_RfspiIsActive(VOID);


// =============================================================================
// hal_RfspiIrqSetHandler
// -----------------------------------------------------------------------------
/// Set the user handler.
/// @param handler User handlerfunction.
// =============================================================================
PUBLIC VOID hal_RfspiIrqSetHandler(HAL_RFSPI_IRQ_HANDLER_T handler);

// =============================================================================
// hal_RfspiIrqSetMask
// -----------------------------------------------------------------------------
/// Set the irq mask
/// @param mask RFSPI IRQ mask
// =============================================================================
PUBLIC VOID hal_RfspiIrqSetMask(HAL_RFSPI_IRQ_STATUS_T mask);

// =============================================================================
// hal_RfspiIrqGetMask
// -----------------------------------------------------------------------------
/// Get the irq mask.
/// @return The IRQ mask for the RFSPI interrupt.
// =============================================================================
PUBLIC HAL_RFSPI_IRQ_STATUS_T hal_RfspiIrqGetMask(VOID);

// =============================================================================
// hal_RfspiImmReadWriteAvail
// -----------------------------------------------------------------------------
/// Check if RFSPI is ready for immediate read or write operation.
/// @return TRUE if ready.
// =============================================================================
PUBLIC BOOL hal_RfspiImmReadWriteAvail(VOID);

// =============================================================================
// hal_RfspiImmWrite
// -----------------------------------------------------------------------------
/// Write commands immediately via RFSPI.
/// @param Cmd Command data array.
/// @param CmdSize Command size in bytes.
// =============================================================================
PUBLIC VOID hal_RfspiImmWrite(CONST UINT8 *Cmd, UINT32 CmdSize);

/// @} // rfspi group


PUBLIC VOID hal_RfspiSetBaseAddr(UINT32 activeRAT);

#endif // _HAL_RFSPI_H_


