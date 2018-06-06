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


#ifndef _HAL_UART_H_
#define _HAL_UART_H_


#include "cs_types.h"
#include "hal_error.h"

// =============================================================================
//  MACROS
// =============================================================================

// ============================================================================
// HAL_UART_DATA_BITS_T
// -----------------------------------------------------------------------------
/// UART data length
// =============================================================================
typedef enum
{
/// Data is 7 bits
    HAL_UART_7_DATA_BITS                        = 0x00000000,
/// Data is 8 bits
    HAL_UART_8_DATA_BITS                        = 0x00000001,
    HAL_UART_DATA_BITS_QTY                      = 0x00000002
} HAL_UART_DATA_BITS_T;


// ============================================================================
// HAL_UART_STOP_BITS_QTY_T
// -----------------------------------------------------------------------------
/// Number of stop bits
// =============================================================================
typedef enum
{
/// There is 1 stop bit
    HAL_UART_1_STOP_BIT                         = 0x00000000,
/// There are 2 stop bits
    HAL_UART_2_STOP_BITS                        = 0x00000001,
    HAL_UART_STOP_BITS_QTY                      = 0x00000002
} HAL_UART_STOP_BITS_QTY_T;


// ============================================================================
// HAL_UART_PARITY_CFG_T
// -----------------------------------------------------------------------------
/// Data parity control selection If enabled, a parity check can be performed
// =============================================================================
typedef enum
{
/// No parity check
    HAL_UART_NO_PARITY                          = 0x00000000,
/// Parity check is odd
    HAL_UART_ODD_PARITY                         = 0x00000001,
/// Parity check is even
    HAL_UART_EVEN_PARITY                        = 0x00000002,
/// Parity check is always 0 (space)
    HAL_UART_SPACE_PARITY                       = 0x00000003,
/// Parity check is always 1 (mark)
    HAL_UART_MARK_PARITY                        = 0x00000004,
    HAL_UART_PARITY_QTY                         = 0x00000005
} HAL_UART_PARITY_CFG_T;


// ============================================================================
// HAL_UART_RX_TRIGGER_CFG_T
// -----------------------------------------------------------------------------
/// Reception FIFO trigger (or treshold) level The Uarts can be configured to generate
/// an interrupt when the reception FIFO is above a configurable threshold (Rx FIFO
/// trigger
// =============================================================================
typedef enum
{
/// One data received in the Rx FIFO
    HAL_UART_RX_TRIG_1                          = 0x00000000,
/// 1/4 of the Rx FIFO is full
    HAL_UART_RX_TRIG_QUARTER                    = 0x00000007,
/// 1/2 of the Rx FIFO is full
    HAL_UART_RX_TRIG_HALF                       = 0x0000000F,
/// 3/4 of the Rx FIFO is full
    HAL_UART_RX_TRIG_3QUARTER                   = 0x00000017,
/// Rx FIFO is almost full
    HAL_UART_RX_TRIG_NEARFULL                   = 0x0000001E
} HAL_UART_RX_TRIGGER_CFG_T;


// ============================================================================
// HAL_UART_TX_TRIGGER_CFG_T
// -----------------------------------------------------------------------------
/// Tranmission FIFO trigger (or treshold) level. The Uarts can be configured to
/// generate an interrupt when the emission FIFO is below a configurable threshold
/// (Tx FIFO trigger
// =============================================================================
typedef enum
{
/// Tx FIFO empty
    HAL_UART_TX_TRIG_EMPTY                      = 0x00000000,
/// Less than 1/4 of the Tx FIFO left to send
    HAL_UART_TX_TRIG_QUARTER                    = 0x00000003,
/// Less than 1/2 of the Tx FIFO left to send
    HAL_UART_TX_TRIG_HALF                       = 0x00000007,
/// Less than 3/4 of the Tx FIFO left to send
    HAL_UART_TX_TRIG_3QUARTER                   = 0x0000000B
} HAL_UART_TX_TRIGGER_CFG_T;


// ============================================================================
// HAL_UART_AFC_MODE_T
// -----------------------------------------------------------------------------
/// Auto Flow Control. Controls the Rx Fifo level at which the Uart_RTS Auto Flow
/// Control will be set inactive high (see UART Operation for more details on AFC).
/// The Uart_RTS Auto Flow Control will be set inactive high when quantity of data
/// in Rx Fifo &amp;gt; AFC Level
// =============================================================================
typedef enum
{
/// RTS inactive with 1 data in the Rx FIFO
    HAL_UART_AFC_MODE_RX_TRIG_1                 = 0x00000000,
/// RTS inactive with 2 data in the Rx FIFO
    HAL_UART_AFC_MODE_RX_TRIG_2                 = 0x00000001,
/// RTS inactive with 3 data in the Rx FIFO
    HAL_UART_AFC_MODE_RX_TRIG_3                 = 0x00000002,
/// RTS inactive with 4 data in the Rx FIFO
    HAL_UART_AFC_MODE_RX_TRIG_4                 = 0x00000003,
/// RTS inactive with 5 data in the Rx FIFO
    HAL_UART_AFC_MODE_RX_TRIG_5                 = 0x00000004,
/// RTS inactive with 6 data in the Rx FIFO
    HAL_UART_AFC_MODE_RX_TRIG_6                 = 0x00000005,
/// RTS inactive with 7 data in the Rx FIFO
    HAL_UART_AFC_MODE_RX_TRIG_7                 = 0x00000006,
/// RTS inactive with 8 data in the Rx FIFO
    HAL_UART_AFC_MODE_RX_TRIG_8                 = 0x00000007,
/// RTS inactive with 9 data in the Rx FIFO
    HAL_UART_AFC_MODE_RX_TRIG_9                 = 0x00000008,
/// RTS inactive with 10 data in the Rx FIFO
    HAL_UART_AFC_MODE_RX_TRIG_10                = 0x00000009,
/// RTS inactive with 11 data in the Rx FIFO
    HAL_UART_AFC_MODE_RX_TRIG_11                = 0x0000000A,
/// RTS inactive with 12 data in the Rx FIFO
    HAL_UART_AFC_MODE_RX_TRIG_12                = 0x0000000B,
/// RTS inactive with 13 data in the Rx FIFO
    HAL_UART_AFC_MODE_RX_TRIG_13                = 0x0000000C,
/// RTS inactive with 14 data in the Rx FIFO
    HAL_UART_AFC_MODE_RX_TRIG_14                = 0x0000000D,
/// RTS inactive with 15 data in the Rx FIFO
    HAL_UART_AFC_MODE_RX_TRIG_15                = 0x0000000E,
/// RTS inactive with 16 data in the Rx FIFO
    HAL_UART_AFC_MODE_RX_TRIG_16                = 0x0000000F,
/// Hardware flow control is disabled. \n &amp;lt;B&amp;gt; NEVER USE THIS MODE &amp;lt;/B&amp;gt;
    HAL_UART_AFC_MODE_DISABLE                   = 0x00000010,
    HAL_UART_AFC_MODE_QTY                       = 0x00000011,
/// AFC mode is loopback \n When set, data on the Uart_Tx line is held high, while
/// the serial output is looped back to the serial input line, internally.
    HAL_UART_AFC_LOOP_BACK                      = 0x00000020
} HAL_UART_AFC_MODE_T;


// ============================================================================
// HAL_UART_IRDA_MODE_T
// -----------------------------------------------------------------------------
/// IrDA protocole enabling IrDA SIR mode is available, and can be activated when
/// the user open the Uart
// =============================================================================
typedef enum
{
/// IrDA mode disabled
    HAL_UART_IRDA_MODE_DISABLE                  = 0x00000000,
/// IrDA mode enabled
    HAL_UART_IRDA_MODE_ENABLE                   = 0x00000001,
    HAL_UART_IRDA_MODE_QTY                      = 0x00000002
} HAL_UART_IRDA_MODE_T;


// ============================================================================
// HAL_UART_BAUD_RATE_T
// -----------------------------------------------------------------------------
/// Baudrate available with the modifiable system clock UARTs are able to run at
/// a wide selection of baud rates. This must be configured at the UART opening
// =============================================================================
typedef enum
{
/// 1.2 KBaud (Serial and IrDA, only for 8955)
    HAL_UART_BAUD_RATE_1200                     = 0x000004B0,
/// 2.4 KBaud (Serial and IrDA)
    HAL_UART_BAUD_RATE_2400                     = 0x00000960,
/// 4.8 KBaud (Serial and IrDA)
    HAL_UART_BAUD_RATE_4800                     = 0x000012C0,
/// 9.6 KBaud (Serial and IrDA)
    HAL_UART_BAUD_RATE_9600                     = 0x00002580,
/// 14.4 KBaud (Serial and IrDA)
    HAL_UART_BAUD_RATE_14400                    = 0x00003840,
/// 19.2 KBaud (Serial and IrDA)
    HAL_UART_BAUD_RATE_19200                    = 0x00004B00,
/// 28.8 KBaud (Serial and IrDA)
    HAL_UART_BAUD_RATE_28800                    = 0x00007080,
/// 33.6 KBaud (Serial and IrDA)
    HAL_UART_BAUD_RATE_33600                    = 0x00008340,
/// 38.4 KBaud (Serial and IrDA)
    HAL_UART_BAUD_RATE_38400                    = 0x00009600,
/// 57.6 KBaud (Serial and IrDA)
    HAL_UART_BAUD_RATE_57600                    = 0x0000E100,
/// 115.2 KBaud (Serial and IrDA)
    HAL_UART_BAUD_RATE_115200                   = 0x0001C200,
/// 230.4 KBaud (Available only in serial mode)
    HAL_UART_BAUD_RATE_230400                   = 0x00038400,
/// 460.8 KBaud (Available only in serial mode)
    HAL_UART_BAUD_RATE_460800                   = 0x00070800,
/// 921.6 KBaud (Available only in serial mode)
    HAL_UART_BAUD_RATE_921600                   = 0x000E1000,
/// 1300.0 KBaud (Available only in serial mode)
    HAL_UART_BAUD_RATE_1300000                  = 0x0013D620,
/// 1625.0 KBaud (Available only in serial mode)
    HAL_UART_BAUD_RATE_1625000                  = 0x0018CBA8,
/// 2166.7 KBaud (Available only in serial mode)
    HAL_UART_BAUD_RATE_2166700                  = 0x00210FAC,
/// 3250.0 KBaud (Available only in serial mode)
    HAL_UART_BAUD_RATE_3250000                  = 0x00319750,
    HAL_UART_BAUD_RATE_QTY                      = 0x00319751
} HAL_UART_BAUD_RATE_T;


// ============================================================================
// HAL_UART_TRANSFERT_MODE_T
// -----------------------------------------------------------------------------
/// Data transfert mode: via DMA or direct. To allow for an easy use of the Uart
/// modules, a non blocking hardware abstraction layer interface is provided
// =============================================================================
typedef enum
{
/// Direct polling: The application sends/receives the data directly to/from the
/// hardware module. The number of bytes actually sent/received is returned. No IRQ
/// is generated.
    HAL_UART_TRANSFERT_MODE_DIRECT_POLLING      = 0x00000000,
/// Direct Irq: The application sends/receives the data directly to/from the hardware
/// module. The number of bytes actually sent/received is returned.An irq can be
/// generated when the Tx/Rx FIFO reaches the pre-programmed level.
    HAL_UART_TRANSFERT_MODE_DIRECT_IRQ          = 0x00000001,
/// DMA polling: The application sends/receives the data through a DMA to the hardware
/// module. When no DMA channel is available, the function returns 0. No byte is
/// sent. When a DMA resource is available, the function returns the number of bytes
/// to send. They will all be sent. A function allows to check if the previous DMA
/// transfer is finished. No new DMA transfer for the same Uart and in the same direction
/// is allowed until the previous transfer is finished.
    HAL_UART_TRANSFERT_MODE_DMA_POLLING         = 0x00000002,
/// The application sends/receives the data through a DMA to the hardware module.
/// When no DMA channel is available, the function returns 0. No byte is sent. When
/// a DMA resource is available, the function returns the number of bytes to send.
/// They will all be sent. An Irq is generated when the current transfer is finished.
/// No new DMA transfer for the same Uart and in the same direction is allowed until
/// the previous transfer is finished.
    HAL_UART_TRANSFERT_MODE_DMA_IRQ             = 0x00000003,
/// The transfert is off.
    HAL_UART_TRANSFERT_MODE_OFF                 = 0x00000004,
    HAL_UART_TRANSFERT_MODE_QTY                 = 0x00000005
} HAL_UART_TRANSFERT_MODE_T;


// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// HAL_UART_CFG_T
// -----------------------------------------------------------------------------
/// UART Configuration Structure This structure defines the Uart behavior
// =============================================================================
typedef struct
{
    /// Data format
    HAL_UART_DATA_BITS_T           data;                         //0x00000000
    /// Number of stop bits
    HAL_UART_STOP_BITS_QTY_T       stop;                         //0x00000004
    /// Parity check
    HAL_UART_PARITY_CFG_T          parity;                       //0x00000008
    /// Trigger for the Rx FIFO
    HAL_UART_RX_TRIGGER_CFG_T      rx_trigger;                   //0x0000000C
    /// Trigger for the Tx FIFO
    HAL_UART_TX_TRIGGER_CFG_T      tx_trigger;                   //0x00000010
    /// Hardware Flow control
    HAL_UART_AFC_MODE_T            afc;                          //0x00000014
    /// IrDA mode
    HAL_UART_IRDA_MODE_T           irda;                         //0x00000018
    /// Baud Rate
    HAL_UART_BAUD_RATE_T           rate;                         //0x0000001C
    /// Reception transfer mode
    HAL_UART_TRANSFERT_MODE_T      rx_mode;                      //0x00000020
    /// Transmission transfer mode
    HAL_UART_TRANSFERT_MODE_T      tx_mode;                      //0x00000024
    /// Auto dectected the rate baud.
#ifdef _USE_AUTO_DETECED_UART_BAUDRATE_
    BOOL                           auto_dec;
    /// ture for at, false for AT
    BOOL                           char_sel;
#endif
} HAL_UART_CFG_T; //Size : 0x28



// ============================================================================
// HAL_UART_IRQ_STATUS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT32                         txModemStatus:1;
    UINT32                         rxDataAvailable:1;
    UINT32                         txDataNeeded:1;
    UINT32                         rxTimeout:1;
    UINT32                         rxLineErr:1;
    UINT32                         txDmaDone:1;
    UINT32                         rxDmaDone:1;
    UINT32                         rxDmaTimeout:1;
    UINT32                         DTR_Rise:1;
    UINT32                         DTR_Fall:1;
} HAL_UART_IRQ_STATUS_T;
//unused
#define HAL_UART_IRQ_STATUS_TXMODEMSTATUS (1<<0)
#define HAL_UART_IRQ_STATUS_RXDATAAVAILABLE (1<<1)
#define HAL_UART_IRQ_STATUS_TXDATANEEDED (1<<2)
#define HAL_UART_IRQ_STATUS_RXTIMEOUT (1<<3)
#define HAL_UART_IRQ_STATUS_RXLINEERR (1<<4)
#define HAL_UART_IRQ_STATUS_TXDMADONE (1<<5)
#define HAL_UART_IRQ_STATUS_RXDMADONE (1<<6)
#define HAL_UART_IRQ_STATUS_RXDMATIMEOUT (1<<7)
#define HAL_UART_IRQ_STATUS_DTR_RISE (1<<8)
#define HAL_UART_IRQ_STATUS_DTR_FALL (1<<9)



// ============================================================================
// HAL_UART_ERROR_STATUS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT32                         _:16;
    UINT32                         rxOvflErr:1;
    UINT32                         txOvflErr:1;
    UINT32                         rxParityErr:1;
    UINT32                         rxFramingErr:1;
    UINT32                         rxBreakInt:1;
} HAL_UART_ERROR_STATUS_T;
//unused
#define HAL_UART_ERROR_STATUS__(n)  (((n)&0xFFFF)<<0)
#define HAL_UART_ERROR_STATUS_RXOVFLERR (1<<16)
#define HAL_UART_ERROR_STATUS_TXOVFLERR (1<<17)
#define HAL_UART_ERROR_STATUS_RXPARITYERR (1<<18)
#define HAL_UART_ERROR_STATUS_RXFRAMINGERR (1<<19)
#define HAL_UART_ERROR_STATUS_RXBREAKINT (1<<20)





// ============================================================================
// TYPES
// ============================================================================

typedef VOID (*HAL_UART_IRQ_HANDLER_T)(HAL_UART_IRQ_STATUS_T, HAL_UART_ERROR_STATUS_T, UINT32);

/// Uart 0 is the trace uart and is unavailable for this driver
/// The numbering starts at 1 for consistency.
/// The HAL_UART_QTY value is defined as the number of UARTS
/// avalaible for the chip on which the driver is running, and
/// can therefore be used for consistency checks
typedef enum
{
    HAL_UART_1                                  = 0x00000001,
    HAL_UART_2                                  = 0x00000002,
#ifdef CHIP_DIE_8909
    HAL_UART_3                                  = 0x00000003,
    HAL_UART_4                                  = 0x00000004,
#endif
    HAL_UART_QTY                                = CHIP_STD_UART_QTY+1
} HAL_UART_ID_T;


// ============================================================================
// FUNCTIONS
// ============================================================================

// ============================================================================
// hal_UartBreakIntWakeup
// ----------------------------------------------------------------------------
/// This function configures whether the system can be waked up
/// by Uart break interrupt.
/// It must be configured before calling hal_UartOpen and hal_UartClose
/// to take effect.
///
/// @param id Identifier of the UART for which the function is called.
/// @param on TRUE to enable breakIntWakeup mode, FALSE otherwise.
// ============================================================================
PUBLIC VOID hal_UartBreakIntWakeup(HAL_UART_ID_T id, BOOL on);


// ============================================================================
// hal_UartIrqEnableBreakInt
// ----------------------------------------------------------------------------
/// This function enables or disables UART break IRQ.
///
/// @param id Identifier of the UART for which the function is called.
/// @param enable TRUE to enable break IRQ, FALSE otherwise.
// ============================================================================
PUBLIC VOID hal_UartIrqEnableBreakInt(HAL_UART_ID_T id, BOOL enable);


// ============================================================================
// hal_UartOpen
// ----------------------------------------------------------------------------
/// This function enables the Uart in the mode defined by \c uartCfg
///
/// This function requests a resource of #HAL_SYS_FREQ_26M only for a pll to be on.
///
/// @param id Identifier of the UART for which the function is called.
/// @param uartCfg Uart configuration structure
// ============================================================================
PUBLIC VOID hal_UartOpen(HAL_UART_ID_T id, CONST HAL_UART_CFG_T* uartCfg);


// ============================================================================
// hal_UartClose
// ----------------------------------------------------------------------------
/// This function closes the selected UART
///
/// This function release the resource to #HAL_SYS_FREQ_32K.
///
/// @param id Identifier of the UART for which the function is called.
// ============================================================================
PUBLIC VOID hal_UartClose(HAL_UART_ID_T id);


// ============================================================================
// hal_UartAllowSleep
// ----------------------------------------------------------------------------
/// That function is called to signify that the uart needs allow or not
/// the system to go into lowpower.
/// @param id Identifier of the UART for which the function is called.
/// @param allow If \c TRUE, the system is allowed to go into lowpower mode. \n
/// If \c FALSE, this prevents the system from entering in
/// lowpower mode.
// ============================================================================
PUBLIC VOID hal_UartAllowSleep(HAL_UART_ID_T id, BOOL allow);


// ============================================================================
// hal_UartSendData
// ----------------------------------------------------------------------------
/// This functions sends \c length bytes of data starting from the address
/// \c data. The number returned is the number of bytes actually sent. In
/// DMA mode, this function returns 0 when no DMA channel is available.
/// Otherwise, it returns \c length.
///
/// @param id Identifier of the UART for which the function is called.
/// @param data Pointer on the buffer of data to send.
/// @param length Number of bytes to send.
/// @return Number of sent bytes or 0 if no DMA channel is available in case of
/// a DMA transfer.
// ============================================================================
PUBLIC UINT32 hal_UartSendData(HAL_UART_ID_T id, CONST UINT8 *data, UINT32 length);


// ============================================================================
// hal_UartTxDmaDone
// ----------------------------------------------------------------------------
/// This function checks if the DMA transmission is finished.
/// Before sending new data in DMA mode, the previous DMA transfer must be
/// finished, hence the use of this function for polling.
///
/// Note that the DMA transfer can be finished with an unempty Tx FIFO. Before
/// shutting down the Uart, one must check that the uart FIFO is empty and that
/// the last byte has been completely sent by using #hal_UartTxFinished. It
/// is not necessary to check the emptiness of the Tx FIFO to start a new DMA
/// transfer. A new DMA transfer is possible once the previous DMA transfer
/// is over (And even is the Tx FIFO is not empty).
///
/// @param id Identifier of the UART for which the function is called.
/// @return \c TRUE if the last DMA transfer is finished. \c NO otherwise.
// ============================================================================
PUBLIC BOOL hal_UartTxDmaDone(HAL_UART_ID_T id);


// ============================================================================
// hal_UartTxFinished
// ----------------------------------------------------------------------------
/// This functions checks if the data transfer is completely finished before
/// closing.
/// This function returns \c TRUE when the transmit FIFO is empty and when the
/// last byte is completely sent. It should be called before closing the Uart
/// if the last bytes of the transfer are important.
///
/// This function should not be called between transfers, in direct or DMA mode.
/// The \link #hal_UartTxFifoAvailable FIFO available \endlink for direct
/// mode and the \link #hal_UartTxDmaDone DMA done indication \endlink for
/// DMA allow for a more optimized transmission.
///
/// @param id Identifier of the UART for which the function is called.
/// @return \c TRUE if the Tx FIFO is empty. \c NO otherwise
// ============================================================================
PUBLIC BOOL hal_UartTxFinished(HAL_UART_ID_T id);


// ============================================================================
// hal_UartGetData
// ----------------------------------------------------------------------------
/// This function gets \c length bytes from the Uart and stores them starting
/// from the address \c dest_address. The number returned is the number of bytes
/// actually received.
/// In DMA mode, this function returns 0 when no DMA channel is available. It
/// returns length otherwise.
/// If you use the Rx Uart (to get some data from the Uart) in DMA mode, you
/// will have to call the #hal_SysInvalidateCache function to invalidate the
/// cache on your reception buffer.
///
/// @param id Identifier of the UART for which the function is called.
/// @param destAddress Pointer to a buffer to store the received data
/// @param length Number of byte to receive
/// @return The number of actually received bytes, or 0 when no DMA channel is
/// available in case of a DMA transfer.
// ============================================================================
PUBLIC UINT32 hal_UartGetData(HAL_UART_ID_T id, UINT8* destAddress, UINT32 length);


// ============================================================================
// hal_UartRxFifoLevel
// ----------------------------------------------------------------------------
/// This function returns the number of bytes in the Rx FIFO
///
/// @param id Identifier of the UART for which the function is called.
/// @returns The number of bytes in the Rx FIFO
// ============================================================================
PUBLIC UINT8 hal_UartRxFifoLevel(HAL_UART_ID_T id);


// ============================================================================
// hal_UartRxDmaDone
// ----------------------------------------------------------------------------
/// Check if the reception is finished.
///
/// Before being able to receive new data in DMA mode, the previous transfer
/// must be finished, hence the use of this function for polling.
///
/// @param id Identifier of the UART for which the function is called.
/// @returns \c TRUE if the last DMA transfer is finished. \c NO otherwise
// ============================================================================
PUBLIC BOOL hal_UartRxDmaDone(HAL_UART_ID_T id);


// ============================================================================
// hal_UartRxDmaStop
// ----------------------------------------------------------------------------
/// This functions terminates the current Rx transfer and releases the
/// current Rx IFC DMA channel used by identified UART.
/// If the IFC channel fifo is not empty the transfer is not terminated.
/// This is useful when the DMA Rx Timeout rises.
///
/// @param id Identifier of the UART for which the function is called.
/// @return \c TRUE if the transfer has been released or if the current
/// UART doesn't have an IFC DMA channel attributed. \c FALSE if the
/// transfer cannot be released, i.e. the IFC DMA channel fifo is not empty.
// ============================================================================
PUBLIC BOOL hal_UartRxDmaStop(HAL_UART_ID_T id);


// ============================================================================
// hal_UartRxDmaLevel
// ----------------------------------------------------------------------------
/// This function returns the number of free room remaining in the IFC DMA Rx
/// buffer. It can be useful to know when you can get data from the buffer.
///
/// @param id Identifier of the UART for which the function is called.
/// @return Number of data (in bytes) remaining in the DMA buffer.
// ============================================================================
PUBLIC UINT32 hal_UartRxDmaLevel(HAL_UART_ID_T id);


// ============================================================================
// hal_UartRxDmaFifoEmpty
// ----------------------------------------------------------------------------
/// This function returns \c TRUE when the fifo of the DMA is empty.
///
/// @param id Identifier of the UART for which the function is called.
/// @returns \c TRUE when the fifo of the DMA is empty.
// ============================================================================
PUBLIC BOOL hal_UartRxDmaFifoEmpty(HAL_UART_ID_T id);


// ============================================================================
// hal_UartRxDmaPurgeFifo
// ----------------------------------------------------------------------------
/// Sends all the remaining data in the IFC fifo to the RAM buffer.
///
/// After the IFC DMA channel has been purged, it is NOT released.
/// After calling this function, the software should wait for the
/// actual end of the purging the fifo, by calling the function
/// hal_UartRxDmaFifoEmpty().
/// After the IFC DMA channel has been purged (or is being purged),
/// the data from the UART will not be read by the IFC DMA channel
/// anymore. This means that after calling this function, the user must
/// stop the current transfer and restart a new transfer.
///
/// @param id Identifier of the UART for which the function is called.
// ============================================================================
PUBLIC VOID hal_UartRxDmaPurgeFifo(HAL_UART_ID_T id);


// ============================================================================
// hal_UartSetRi
// ----------------------------------------------------------------------------
/// Sets the RI bit to the value given as a parameter
///
/// @param id Identifier of the UART for which the function is called.
/// @param value If \c TRUE, the RI bit is set.
/// If \c FALSE, the RI bit is cleared.
// ============================================================================
PUBLIC VOID hal_UartSetRi(HAL_UART_ID_T id, BOOL value);


// ============================================================================
// hal_UartSetDsr
// ----------------------------------------------------------------------------
/// Sets the DSR bit to the value given as a parameter
///
/// @param id Identifier of the UART for which the function is called.
/// @param value If \c TRUE, the DSR bit is set.
/// If \c FALSE, the DSR bit is cleared.
// ============================================================================
PUBLIC VOID hal_UartSetDsr(HAL_UART_ID_T id, BOOL value);


// ============================================================================
// hal_UartSetDcd
// ----------------------------------------------------------------------------
/// Sets the DCD bit to the value given as a parameter
///
/// @param id Identifier of the UART for which the function is called.
/// @param value If \c TRUE, the DCD bit is set.
/// If \c FALSE, the DCD bit is cleared.
// ============================================================================
PUBLIC VOID hal_UartSetDcd(HAL_UART_ID_T id, BOOL value);


// ============================================================================
// hal_UartGetDtr
// ----------------------------------------------------------------------------
/// Read the current value of the DTR line
/// @param id Identifier of the UART for which the function is called.
/// @return The current value of the DTR line. (\c TRUE or \c FALSE)
// ============================================================================
PUBLIC BOOL hal_UartGetDtr(HAL_UART_ID_T id);


// ============================================================================
// hal_UartSetBreak
// ----------------------------------------------------------------------------
/// Enable or disable the break (hold the Tx line low).
///
/// With this function, it is possible to generate a break: when the booLean
/// parameter \c enable is set to \c TRUE, the Uart_Tx line is held low, thus
/// generating a break on the line. It is held low until this function is
/// called again with the parameter \c enable set to \c FALSE.
///
/// @param id Identifier of the UART for which the function is called.
/// @param enable When set to \c TRUE, enables a break. When set to \c FALSE,
/// disables a break.
// ============================================================================
PUBLIC VOID hal_UartSetBreak(HAL_UART_ID_T id, BOOL enable);


// ============================================================================
// hal_UartTxFifoAvailable
// ----------------------------------------------------------------------------
/// This function returns the number of bytes available in the TX FIFO
///
/// @param id Identifier of the UART for which the function is called.
/// @return The number of available bytes in the TX FIFO.
// ============================================================================
PUBLIC UINT8 hal_UartTxFifoAvailable(HAL_UART_ID_T id);


// ============================================================================
// hal_UartGetErrorStatus
// ----------------------------------------------------------------------------
/// Gives the status of the UART.
/// This function returns the line error status. The \c UINT16 value returned
/// is a bitfield containing the errors detected during the transfer. The
/// errors can be: #HAL_ERR_UART_RX_OVERFLOW, #HAL_ERR_UART_TX_OVERFLOW,
/// #HAL_ERR_UART_PARITY, #HAL_ERR_UART_FRAMING, or #HAL_ERR_UART_BREAK_INT.
///
/// @param id Identifier of the UART for which the function is called.
/// @return The line error status
// ============================================================================
PUBLIC HAL_ERR_T hal_UartGetErrorStatus(HAL_UART_ID_T id);


// ============================================================================
// hal_UartClearErrorStatus
// ----------------------------------------------------------------------------
/// This function clears all the line error status. The error status is cleared
/// automatically by the interrupt dispatcher code, but if you do not use the
/// interrupt, the error status is not cleared. This means that if you check
/// the error status manually (by polling it with the #hal_UartGetErrorStatus
/// function), you'll need to call the #hal_UartClearErrorStatus function to
/// clear it, manually.
///
/// @param id Identifier of the UART for which the function is called.
// ============================================================================
PUBLIC VOID hal_UartClearErrorStatus(HAL_UART_ID_T id);


// ============================================================================
// hal_UartFifoFlush
// ----------------------------------------------------------------------------
/// This function resets the reception and the transmission FIFOs. All bytes
/// that are waiting to be sent in the Tx FIFO and that have been received,
/// waiting to be read in the Rx FIFO, will be removed.
///
/// @param id Identifier of the UART for which the function is called.
// ============================================================================
PUBLIC VOID hal_UartFifoFlush(HAL_UART_ID_T id);


// ============================================================================
// hal_UartSetRts
// ----------------------------------------------------------------------------
/// This function controls the RTS line of the UART. When the function is
/// called with a \c ready parameter set to \c FALSE, that means that the CPU
/// is not ready to handle the data of the UART. The remote device will stop
/// sending data to the UART.
///
/// To resume the data transfer (i.e. to let the remote device send data to the
/// UART again), call this function with a \c ready parameter set to \c TRUE.
///
/// If the hardware flow control is enabled, the RTS line will be controlled as
/// follows: RTS Line Ready = CPU Ready AND UART Hardware Flow Control Ready.
/// It means that if the UART or the CPU is not ready, the RTS line will ask
/// the remote device to stop sending data.
///
/// When the flow control is handled by the #hal_UartSetRts function, it is
/// called software flow control. The software flow control can be used in
/// parallel to the hardware flow control. Please refer to previous chapters for
/// details about @link #HAL_UART_AFC_MODE_T hardware flow control @endlink.
///
/// @param id Identifier of the UART for which the function is called.
/// @param ready Ready to receive data from the remote
/// device (&amp;lt;CODE&amp;gt; ready = TRUE &amp;lt;/CODE&amp;gt;) or not (&amp;lt;CODE&amp;gt; ready = FALSE &amp;lt;/CODE&amp;gt;)
// ============================================================================
PUBLIC VOID hal_UartSetRts(HAL_UART_ID_T id, BOOL ready);


// ============================================================================
// hal_UartGetCts
// ----------------------------------------------------------------------------
/// This function returns the value of the CTS line of the UART. When the
/// function returns \c TRUE, that means that the remote peripheral is
/// ready to receive data on its serial interface. The UART should stop sending
/// data.
///
/// If the hardware flow control is enabled and the CTS line says that the
/// remote device is not ready to receive data, the UART hardware will
/// automatically stop sending data.
///
/// When the flow control is handled by the #hal_UartGetCts, it is called
/// software flow control. The software flow control can be used in parallel to
/// the hardware flow control. Please refer to previous chapters for details
/// about @link #HAL_UART_AFC_MODE_T hardware flow control @endlink.
///
/// @param id Identifier of the UART for which the function is called.
/// @returns Value of the CTS line : \c TRUE if the remote device is ready to
/// receive data, \c FALSE otherwise.
// ============================================================================
PUBLIC BOOL hal_UartGetCts(HAL_UART_ID_T id);


// ============================================================================
// hal_UartIrqSetHandler
// ----------------------------------------------------------------------------
/// Set the user handler called when an Irq is triggered for the
/// defined UART.
/// @param id Identifier of the UART for which the function is called.
/// @param handler User function called in case of an interrupt on this UART
/// occurs.
// ============================================================================
PUBLIC VOID hal_UartIrqSetHandler(HAL_UART_ID_T id, HAL_UART_IRQ_HANDLER_T handler, UINT32 param);


// ============================================================================
// hal_UartIrqSetMask
// ----------------------------------------------------------------------------
/// Set the IRQ mask for the specified uart
/// @param id Identifier of the UART for which the function is called.
/// @param mask The mask to apply
// ============================================================================
PUBLIC VOID hal_UartIrqSetMask(HAL_UART_ID_T id, HAL_UART_IRQ_STATUS_T mask);


// ============================================================================
// hal_UartIrqGetMask
// ----------------------------------------------------------------------------
/// Get the IRQ mask set for an UART
/// @param id Identifier of the UART for which the function is called.
/// @return The Irq mask of this uart
// ============================================================================
PUBLIC HAL_UART_IRQ_STATUS_T hal_UartIrqGetMask(HAL_UART_ID_T id);


// ============================================================================
// hal_UartForcePinsLow
// ----------------------------------------------------------------------------
/// This function force all output pins of the UART to low. This can be
/// called for any UART it will only change the state of the pins actually
/// configured in UART mode (IO config).
///
/// This function must only be called when the UART is closed.
///
/// @param id Identifier of the UART for which the function is called.
// ============================================================================
PUBLIC VOID hal_UartForcePinsLow(HAL_UART_ID_T id);

#ifdef CHIP_DIE_8955

// ============================================================================
// hal_UartAutoEnable
// ----------------------------------------------------------------------------
///
/// @param id Identifier of the UART for which the function is called.
/// @param lowcase If \c TRUE, enable auto detect
///              If \c FALSE, disable auto detect
/// note: Call hal_UartAutoConfig first then call hal_UartAutoEnable
// ============================================================================
PUBLIC VOID hal_UartAutoEnable(HAL_UART_ID_T id, BOOL enable, BOOL lowcase);

// ============================================================================
// hal_UartAutoConfig
// ----------------------------------------------------------------------------
///
/// @param id Identifier of the UART for which the function is called.
/// @param lowcase If \c TRUE, char is lowcase(at)
///              If \c FALSE, char is upcase(AT)
/// @param highspd If \c TRUE, UART Clock is  52M
///              If \c FALSE, UART Clock is 32K
// ============================================================================
PUBLIC VOID hal_UartAutoConfig(HAL_UART_ID_T id, BOOL highspd);

#ifdef _USE_AUTO_DETECED_UART_BAUDRATE_
PUBLIC BOOL hal_UartCheckRateIsLock(HAL_UART_ID_T id);
#endif

#endif

// ============================================================================
// hal_UartEnable
// ----------------------------------------------------------------------------
///
/// @param id Identifier of the UART for which the function is called.
/// @param cts Enable cts or not
/// @param rts Enable rts or not
// ============================================================================
extern void hal_UartEnable(UINT8 id, BOOL cts, BOOL rts);

// ============================================================================
// hal_UartDisable
// ----------------------------------------------------------------------------
///
/// @param id Identifier of the UART for which the function is called.
// ============================================================================
extern void hal_UartDisable(UINT8 id);

/// @} --> End of uart group



#endif

