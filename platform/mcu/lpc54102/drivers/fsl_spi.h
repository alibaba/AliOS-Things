/*
 * Copyright (c) 2017, NXP Semiconductors, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _FSL_SPI_H_
#define _FSL_SPI_H_

#include "fsl_common.h"

/*!
 * @addtogroup spi_driver
 * @{
 */

/*! @file */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief USART driver version 2.0.0. */
#define FSL_SPI_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

#ifndef SPI_DUMMYDATA
/*! @brief SPI dummy transfer data, the data is sent while txBuff is NULL. */
#define SPI_DUMMYDATA (0xFFU)
#endif

#define SPI_ASSERT_SSELNUM(n) ((~(1U << ((n) + 16))) & 0xF0000)
#define SPI_DEASSERT_SSELNUM(n) (1U << ((n) + 16))
#define SPI_DEASSERT_ALL (0xF0000)

#define SPI_FIFO_GETRXTHRESHOLD(base)                                                                         \
    ((SPI0 == base) ?                                                                                         \
         ((VFIFO->SPI[0].CFGSPI & VFIFO_SPI_CFGSPI_RXTHRESHOLD_MASK) >> VFIFO_SPI_CFGSPI_RXTHRESHOLD_SHIFT) : \
         ((VFIFO->SPI[1].CFGSPI & VFIFO_SPI_CFGSPI_RXTHRESHOLD_MASK) >> VFIFO_SPI_CFGSPI_RXTHRESHOLD_SHIFT))
#define SPI_FIFO_GETTXTHRESHOLD(base)                                                                         \
    ((SPI0 == base) ?                                                                                         \
         ((VFIFO->SPI[0].CFGSPI & VFIFO_SPI_CFGSPI_TXTHRESHOLD_MASK) >> VFIFO_SPI_CFGSPI_TXTHRESHOLD_SHIFT) : \
         ((VFIFO->SPI[1].CFGSPI & VFIFO_SPI_CFGSPI_TXTHRESHOLD_MASK) >> VFIFO_SPI_CFGSPI_TXTHRESHOLD_SHIFT))

/*! @brief SPI transfer option.*/
typedef enum _spi_xfer_option
{
    kSPI_FrameDelay = (SPI_TXDATCTL_EOF_MASK),         /*!< Delay chip select */
    kSPI_FrameAssert = (SPI_TXDATCTL_EOT_MASK),        /*!< When transfer ends, assert chip select */
    kSPI_ReceiveIgnore = (SPI_TXDATCTL_RXIGNORE_MASK), /*!< Ignore the receive data. */
} spi_xfer_option_t;

/*! @brief SPI data shifter direction options.*/
typedef enum _spi_shift_direction
{
    kSPI_MsbFirst = 0U, /*!< Data transfers start with most significant bit. */
    kSPI_LsbFirst = 1U, /*!< Data transfers start with least significant bit. */
} spi_shift_direction_t;

/*! @brief SPI clock polarity configuration.*/
typedef enum _spi_clock_polarity
{
    kSPI_ClockPolarityActiveHigh = 0x0U, /*!< Active-high SPI clock (idles low). */
    kSPI_ClockPolarityActiveLow = 0x1U,  /*!< Active-low SPI clock (idles high). */
} spi_clock_polarity_t;

/*! @brief SPI clock phase configuration.*/
typedef enum _spi_clock_phase
{
    kSPI_ClockPhaseFirstEdge = 0x0U,  /*!< First edge on SCK occurs at the middle of the first
                                          *   cycle of a data transfer. */
    kSPI_ClockPhaseSecondEdge = 0x1U, /*!< First edge on SCK occurs at the start of the
                                   *   first cycle of a data transfer. */
} spi_clock_phase_t;

/*! @brief SPI FIFO driection. */
typedef enum _spi_fifo_direction
{
    kSPI_FifoTx = 1U, /*!< FIFO direction for transmit. */
    kSPI_FifoRx = 2U, /*!< FIFO direction for receive. */
} spi_fifo_direction_t;

/*! @brief Transfer data width */
typedef enum _spi_data_width
{
    kSPI_Data1Bits = 0,   /*!< 1 bits data width,when LEN = 0, the underrun status is not meaningful. */
    kSPI_Data2Bits = 1,   /*!< 2 bits data width */
    kSPI_Data3Bits = 2,   /*!< 3 bits data width */
    kSPI_Data4Bits = 3,   /*!< 4 bits data width */
    kSPI_Data5Bits = 4,   /*!< 5 bits data width */
    kSPI_Data6Bits = 5,   /*!< 6 bits data width */
    kSPI_Data7Bits = 6,   /*!< 7 bits data width */
    kSPI_Data8Bits = 7,   /*!< 8 bits data width */
    kSPI_Data9Bits = 8,   /*!< 9 bits data width */
    kSPI_Data10Bits = 9,  /*!< 10 bits data width */
    kSPI_Data11Bits = 10, /*!< 11 bits data width */
    kSPI_Data12Bits = 11, /*!< 12 bits data width */
    kSPI_Data13Bits = 12, /*!< 13 bits data width */
    kSPI_Data14Bits = 13, /*!< 14 bits data width */
    kSPI_Data15Bits = 14, /*!< 15 bits data width */
    kSPI_Data16Bits = 15, /*!< 16 bits data width */
} spi_data_width_t;

/*! @brief Slave select */
typedef enum _spi_ssel
{
    kSPI_Ssel0 = 0, /*!< Slave select 0 */
    kSPI_Ssel1 = 1, /*!< Slave select 1 */
    kSPI_Ssel2 = 2, /*!< Slave select 2 */
    kSPI_Ssel3 = 3, /*!< Slave select 3 */
} spi_ssel_t;

/*! @brief ssel polarity */
typedef enum _spi_spol
{
    kSPI_Spol0ActiveHigh = SPI_CFG_SPOL0(1),
    kSPI_Spol1ActiveHigh = SPI_CFG_SPOL1(1),
    kSPI_Spol2ActiveHigh = SPI_CFG_SPOL2(1),
    kSPI_Spol3ActiveHigh = SPI_CFG_SPOL3(1),
    kSPI_SpolActiveAllHigh =
        (kSPI_Spol0ActiveHigh | kSPI_Spol1ActiveHigh | kSPI_Spol2ActiveHigh | kSPI_Spol3ActiveHigh),
    kSPI_SpolActiveAllLow = 0,
} spi_spol_t;

/*! @brief SPI fifo user configure structure.*/
typedef struct _spi_fifo_config
{
    bool enableTxFifo;       /*!< Enable transmit FIFO */
    bool enableRxFifo;       /*!< Enable receive FIFO */
    uint8_t txFifoSize;      /*!< Configure txFIFO size */
    uint8_t rxFifoSize;      /*!< Configure rxFIFO size */
    uint8_t txFifoThreshold; /*!< txFIFO threshold */
    uint8_t rxFifoThreshold; /*!< rxFIFO threshold */
} spi_fifo_config_t;

/*! @brief SPI delay time configure structure.*/
typedef struct _spi_delay_config
{
    uint8_t preDelay;      /*!< Delay between SSEL assertion and the beginning of transfer. */
    uint8_t postDelay;     /*!< Delay between the end of transfer and SSEL deassertion. */
    uint8_t frameDelay;    /*!< Delay between frame to frame. */
    uint8_t transferDelay; /*!< Delay between transfer to transfer. */
} spi_delay_config_t;

/*! @brief SPI master user configure structure.*/
typedef struct _spi_master_config
{
    bool enableLoopback;             /*!< Enable loopback for test purpose */
    bool enableMaster;               /*!< Enable SPI at initialization time */
    spi_clock_polarity_t polarity;   /*!< Clock polarity */
    spi_clock_phase_t phase;         /*!< Clock phase */
    spi_shift_direction_t direction; /*!< MSB or LSB */
    uint32_t baudRate_Bps;           /*!< Baud Rate for SPI in Hz */
    spi_data_width_t dataWidth;      /*!< Width of the data */
    spi_ssel_t sselNum;              /*!< Slave select number */
    spi_spol_t sselPol;              /*!< Configure active CS polarity */
    spi_fifo_config_t fifoConfig;    /*!< Configure for fifo. */
    spi_delay_config_t delayConfig;  /*!< Configure for delay time. */
} spi_master_config_t;

/*! @brief SPI slave user configure structure.*/
typedef struct _spi_slave_config
{
    bool enableSlave;                /*!< Enable SPI at initialization time */
    spi_clock_polarity_t polarity;   /*!< Clock polarity */
    spi_clock_phase_t phase;         /*!< Clock phase */
    spi_shift_direction_t direction; /*!< MSB or LSB */
    spi_data_width_t dataWidth;      /*!< Width of the data */
    spi_spol_t sselPol;              /*!< Configure active CS polarity */
    spi_fifo_config_t fifoConfig;    /*!< Configure for fifo. */
} spi_slave_config_t;

/*! @brief SPI transfer status.*/
enum _spi_status
{
    kStatus_SPI_Busy = MAKE_STATUS(kStatusGroup_LPC_SPI, 0),  /*!< SPI bus is busy */
    kStatus_SPI_Idle = MAKE_STATUS(kStatusGroup_LPC_SPI, 1),  /*!< SPI is idle */
    kStatus_SPI_Error = MAKE_STATUS(kStatusGroup_LPC_SPI, 2), /*!< SPI  error */
    kStatus_SPI_BaudrateNotSupport =
        MAKE_STATUS(kStatusGroup_LPC_SPI, 3) /*!< Baudrate is not support in current clock source */
};

/*! @brief SPI interrupt sources.*/
enum _spi_interrupt_enable
{
    kSPI_RxReadyInterruptEnable = SPI_INTENSET_RXRDYEN_MASK,           /*!< Rx ready interrupt */
    kSPI_TxReadyInterruptEnable = SPI_INTENSET_TXRDYEN_MASK,           /*!< Tx ready interrupt */
    kSPI_RxOverrunInterruptEnable = SPI_INTENSET_RXOVEN_MASK,          /*!< Rx overrun interrupt */
    kSPI_TxUnderrunInterruptEnable = SPI_INTENSET_TXUREN_MASK,         /*!< Tx underrun interrupt */
    kSPI_SlaveSelectAssertInterruptEnable = SPI_INTENSET_SSAEN_MASK,   /*!< Slave select assert interrupt */
    kSPI_SlaveSelectDeassertInterruptEnable = SPI_INTENSET_SSDEN_MASK, /*!< Slave select deassert interrupt */
    kSPI_MasterIdleInterruptEnable = SPI_INTENSET_MSTIDLEEN_MASK,      /*!< Master idle interrupt */
    kSPI_AllInterruptEnable =
        (SPI_INTENSET_RXRDYEN_MASK | SPI_INTENSET_TXRDYEN_MASK | SPI_INTENSET_RXOVEN_MASK | SPI_INTENSET_TXUREN_MASK |
         SPI_INTENSET_SSAEN_MASK | SPI_INTENSET_SSDEN_MASK | SPI_INTENSET_MSTIDLEEN_MASK)
};

/*! @brief SPI FIFO interrupt sources.*/
enum _spi_fifo_interrupt_enable
{
    kSPI_RxFifoThresholdInterruptEnable =
        VFIFO_SPI_CTLSETSPI_RXTHINTEN_MASK, /*!< Rx FIFO reach the threshold interrupt */
    kSPI_TxFifoThresholdInterruptEnable =
        VFIFO_SPI_CTLSETSPI_TXTHINTEN_MASK, /*!< Tx FIFO reach the threshold interrupt */
    kSPI_RxFifoTimeoutInterruptEnable = VFIFO_SPI_CTLSETSPI_RXTIMEOUTINTEN_MASK, /*!< Rx FIfo timeout interrupt */
    kSPI_AllFifoInterruptEnable = (VFIFO_SPI_CTLSETSPI_RXTHINTEN_MASK | VFIFO_SPI_CTLSETSPI_TXTHINTEN_MASK |
                                   VFIFO_SPI_CTLSETSPI_RXTIMEOUTINTEN_MASK)
};

/*! @brief SPI status flags.*/
enum _spi_status_flags
{
    kSPI_RxReadyFlag = SPI_STAT_RXRDY_MASK,           /*!< Receive ready flag. */
    kSPI_TxReadyFlag = SPI_STAT_TXRDY_MASK,           /*!< Transmit ready flag. */
    kSPI_RxOverrunFlag = SPI_STAT_RXOV_MASK,          /*!< Receive overrun flag. */
    kSPI_TxUnderrunFlag = SPI_STAT_TXUR_MASK,         /*!< Transmit underrun flag. */
    kSPI_SlaveSelectAssertFlag = SPI_STAT_SSA_MASK,   /*!< Slave select assert flag. */
    kSPI_SlaveSelectDeassertFlag = SPI_STAT_SSD_MASK, /*!< slave select deassert flag. */
    kSPI_StallFlag = SPI_STAT_STALLED_MASK,           /*!< Stall flag. */
    kSPI_EndTransferFlag = SPI_STAT_ENDTRANSFER_MASK, /*!< End transfer bit. */
    kSPI_MasterIdleFlag = SPI_STAT_MSTIDLE_MASK,      /*!< Master in idle status flag. */
};

/*! @brief SPI FIFO status flags.*/
enum _spi_fifo_status_flags
{
    kSPI_RxFifoThresholdFlag = (VFIFO_SPI_STATSPI_RXTH_MASK),    /*!< Receive FIFO threshold reached flag. */
    kSPI_TxFifoThresholdFlag = (VFIFO_SPI_STATSPI_TXTH_MASK),    /*!< Transmit FIFO threshold reached flag. */
    kSPI_RxFifoTimeOutFlag = (VFIFO_SPI_STATSPI_RXTIMEOUT_MASK), /*!< Receive time out flag. */
    kSPI_FifoBusErrorFlag = (VFIFO_SPI_STATSPI_BUSERR_MASK),     /*!< FIFO bus error flag. */
    kSPI_RxFifoEmptyFlag = (VFIFO_SPI_STATSPI_RXEMPTY_MASK),     /*!< Receive fifo buffer empty flag. */
    kSPI_TxFifoEmptyFlag = (VFIFO_SPI_STATSPI_TXEMPTY_MASK),     /*!< transmit fifo buffer empty flag. */
};

/*! @brief SPI transfer structure */
typedef struct _spi_transfer
{
    uint8_t *txData;      /*!< Send buffer */
    uint8_t *rxData;      /*!< Receive buffer */
    uint32_t configFlags; /*!< Additional option to control transfer */
    size_t dataSize;      /*!< Transfer bytes */
} spi_transfer_t;

/*! @brief SPI half-duplex(master only) transfer structure */
typedef struct _spi_half_duplex_transfer
{
    uint8_t *txData;            /*!< Send buffer */
    uint8_t *rxData;            /*!< Receive buffer */
    size_t txDataSize;          /*!< Transfer bytes for transmit */
    size_t rxDataSize;          /*!< Transfer bytes */
    uint32_t configFlags;       /*!< Transfer configuration flags; set from _dspi_transfer_config_flag_for_master. */
    bool isPcsAssertInTransfer; /*!< If Pcs pin keep assert between transmit and receive. true for assert and false for
                                   deassert. */
    bool isTransmitFirst;       /*!< True for transmit first and false for receive first. */
} spi_half_duplex_transfer_t;

/*! @brief Internal configuration structure used in 'spi' and 'spi_dma' driver */
typedef struct _spi_config
{
    spi_data_width_t dataWidth;
    spi_ssel_t sselNum;
} spi_config_t;

/*! @brief  Master handle type */
typedef struct _spi_master_handle spi_master_handle_t;

/*! @brief  Slave handle type */
typedef spi_master_handle_t spi_slave_handle_t;

/*! @brief SPI master callback for finished transmit */
typedef void (*spi_master_callback_t)(SPI_Type *base, spi_master_handle_t *handle, status_t status, void *userData);

/*! @brief SPI slave callback for finished transmit */
typedef void (*spi_slave_callback_t)(SPI_Type *base, spi_slave_handle_t *handle, status_t status, void *userData);

/*! @brief SPI transfer handle structure */
struct _spi_master_handle
{
    uint8_t *volatile txData;         /*!< Transfer buffer */
    uint8_t *volatile rxData;         /*!< Receive buffer */
    volatile size_t txRemainingBytes; /*!< Number of data to be transmitted [in bytes] */
    volatile size_t rxRemainingBytes; /*!< Number of data to be received [in bytes] */
    size_t totalByteCount;            /*!< A number of transfer bytes */
    volatile uint32_t state;          /*!< SPI internal state */
    spi_master_callback_t callback;   /*!< SPI callback */
    void *userData;                   /*!< Callback parameter */
    uint8_t dataWidth;                /*!< Width of the data [Valid values: 1 to 16] */
    uint8_t sselNum;         /*!< Slave select number to be asserted when transferring data [Valid values: 0 to 3] */
    uint32_t configFlags;    /*!< Additional option to control transfer */
    bool isTxFifoEnabled;    /*!< Is transmit FIFO enabled. */
    bool isRxFifoEnabled;    /*!< Is receive FIFO enabled. */
    uint8_t txFifoThreshold; /*!< txFIFO threshold */
    uint8_t rxFifoThreshold; /*!< rxFIFO threshold */
};

#if defined(__cplusplus)
extern "C" {
#endif
/*******************************************************************************
 * API
 ******************************************************************************/

/*! @brief Returns instance number for SPI peripheral base address. */
uint32_t SPI_GetInstance(SPI_Type *base);

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief  Sets the SPI master configuration structure to default values.
 *
 * The purpose of this API is to get the configuration structure initialized for use in SPI_MasterInit().
 * User may use the initialized structure unchanged in SPI_MasterInit(), or modify
 * some fields of the structure before calling SPI_MasterInit(). After calling this API,
 * the master is ready to transfer.
 * Example:
   @code
   spi_master_config_t config;
   SPI_MasterGetDefaultConfig(&config);
   @endcode
 *
 * @param config pointer to master config structure
 */
void SPI_MasterGetDefaultConfig(spi_master_config_t *config);

/*!
 * @brief Initializes the SPI with master configuration.
 *
 * The configuration structure can be filled by user from scratch, or be set with default
 * values by SPI_MasterGetDefaultConfig(). After calling this API, the slave is ready to transfer.
 * Example
   @code
   spi_master_config_t config = {
   .baudRate_Bps = 500000,
   ...
   };
   SPI_MasterInit(SPI0, &config);
   @endcode
 *
 * @param base SPI base pointer
 * @param config pointer to master configuration structure
 * @param srcClock_Hz Source clock frequency.
 */
status_t SPI_MasterInit(SPI_Type *base, const spi_master_config_t *config, uint32_t srcClock_Hz);

/*!
 * @brief  Sets the SPI slave configuration structure to default values.
 *
 * The purpose of this API is to get the configuration structure initialized for use in SPI_SlaveInit().
 * Modify some fields of the structure before calling SPI_SlaveInit().
 * Example:
   @code
   spi_slave_config_t config;
   SPI_SlaveGetDefaultConfig(&config);
   @endcode
 *
 * @param config pointer to slave configuration structure
 */
void SPI_SlaveGetDefaultConfig(spi_slave_config_t *config);

/*!
 * @brief Initializes the SPI with slave configuration.
 *
 * The configuration structure can be filled by user from scratch or be set with
 * default values by SPI_SlaveGetDefaultConfig().
 * After calling this API, the slave is ready to transfer.
 * Example
   @code
    spi_slave_config_t config = {
    .polarity = kSPI_ClockPolarityActiveHigh;
    .phase = kSPI_ClockPhaseFirstEdge;
    .direction = kSPI_MsbFirst;
    ...
    };
    SPI_SlaveInit(SPI0, &config);
   @endcode
 *
 * @param base SPI base pointer
 * @param config pointer to slave configuration structure
 */
status_t SPI_SlaveInit(SPI_Type *base, const spi_slave_config_t *config);

/*!
 * @brief De-initializes the SPI.
 *
 * Calling this API resets the SPI module, gates the SPI clock.
 * Disable the fifo if enabled.
 * The SPI module can't work unless calling the SPI_MasterInit/SPI_SlaveInit to initialize module.
 *
 * @param base SPI base pointer
 */
void SPI_Deinit(SPI_Type *base);

/*!
 * @brief Enable or disable the SPI Master or Slave
 * @param base SPI base pointer
 * @param enable or disable ( true = enable, false = disable)
 */
static inline void SPI_Enable(SPI_Type *base, bool enable)
{
    if (enable)
    {
        base->CFG |= SPI_CFG_ENABLE_MASK;
    }
    else
    {
        base->CFG &= ~SPI_CFG_ENABLE_MASK;
    }
}

/*! @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets the status flag.
 *
 * @param base SPI base pointer
 * @return SPI Status, use status flag to AND @ref _spi_status_flags could get the related status.
 */
static inline uint32_t SPI_GetStatusFlags(SPI_Type *base)
{
    assert(NULL != base);
    return base->STAT;
}

/*!
 * @brief Gets the FIFO status flag for SPI transfer.
 *
 * @param base SPI base pointer
 * @return SPI Status, use status flag to AND @ref _spi_fifo_status_flags could get the related status.
 */
uint32_t SPI_GetFifoStatusFlags(SPI_Type *base);

/*!
 * @brief Clear the FIFO status flag for SPI transfer.
 * Only kSPI_RxFifoTimeOutFlag and kSPI_FifoBusErrorFlag can be cleared.
 * @param base SPI base pointer
 * @param mask use status flag to AND @ref _spi_status_flags could get the related status.
 */
void SPI_ClearFifoStatusFlags(SPI_Type *base, uint32_t mask);
/*! @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enables the interrupt for the SPI.
 *
 * @param base SPI base pointer
 * @param irqs SPI interrupt source. The parameter can be any combination of the following values:
 *        @arg kSPI_RxReadyInterruptEnable
 *        @arg kSPI_TxReadyInterruptEnable
 */
static inline void SPI_EnableInterrupts(SPI_Type *base, uint32_t irqs)
{
    assert(NULL != base);
    base->INTENSET = irqs;
}

/*!
 * @brief Disables the interrupt for the SPI.
 *
 * @param base SPI base pointer
 * @param irqs SPI interrupt source. The parameter can be any combination of the following values:
 *        @arg kSPI_RxReadyInterruptEnable
 *        @arg kSPI_TxReadyInterruptEnable
 */
static inline void SPI_DisableInterrupts(SPI_Type *base, uint32_t irqs)
{
    assert(NULL != base);
    base->INTENCLR = irqs;
}

/*!
 * @brief Enables the FIFO interrupt for the SPI.
 *
 * @param base SPI base pointer
 * @param irqs SPI interrupt source. The parameter can be any combination of the following values:
 *        @arg kSPI_RxFifoThresholdInterruptEnable
 *        @arg kSPI_TxFifoThresholdInterruptEnable
 */
void SPI_EnableFifoInterrupts(SPI_Type *base, uint32_t irqs);

/*!
 * @brief Disables the FIFO interrupt for the SPI.
 *
 * @param base SPI base pointer
 * @param irqs SPI interrupt source. The parameter can be any combination of the following values:
 *        @arg kSPI_RxFifoThresholdInterruptEnable
 *        @arg kSPI_TxFifoThresholdInterruptEnable
 */
void SPI_DisableFifoInterrupts(SPI_Type *base, uint32_t irqs);
/*! @} */

/*!
 * @name Bus Operations
 * @{
 */

/*!
 * @brief Returns whether the SPI module is in master mode.
 *
 * @param base SPI peripheral address.
 * @return Returns true if the module is in master mode or false if the module is in slave mode.
 */
static inline bool SPI_IsMaster(SPI_Type *base)
{
    return (bool)(((base->CFG) & SPI_CFG_MASTER_MASK) >> SPI_CFG_MASTER_SHIFT);
}

/*!
 * @brief Returns the configurations.
 *
 * @param base SPI peripheral address.
 * @return return configurations which contain datawidth and SSEL numbers.
 *         return data type is a pointer of spi_config_t.
 */
void *SPI_GetConfig(SPI_Type *base);
/*!
 * @brief Sets the baud rate for SPI transfer. This is only used in master.
 *
 * @param base SPI base pointer
 * @param baudrate_Bps baud rate needed in Hz.
 * @param srcClock_Hz SPI source clock frequency in Hz.
 */
status_t SPI_MasterSetBaud(SPI_Type *base, uint32_t baudrate_Bps, uint32_t srcClock_Hz);

/*!
 * @brief Writes a data into the SPI data register.
 *
 * @param base SPI base pointer
 * @param data needs to be write.
 * @param configFlags transfer configuration options @ref spi_xfer_option_t
 */
void SPI_WriteData(SPI_Type *base, uint16_t data, uint32_t configFlags);

/*!
 * @brief Gets a data from the SPI data register.
 *
 * @param base SPI base pointer
 * @return Data in the register.
 */
uint32_t SPI_ReadData(SPI_Type *base);

/*!
 * @brief Set delay time for transfer.
 *        the delay uint is SPI clock time, maximum value is 0xF.
 * @param base SPI base pointer
 * @param config configuration for delay option @ref spi_delay_config_t.
 */
void SPI_SetTransferDelay(SPI_Type *base, const spi_delay_config_t *config);

/*!
 * @brief Set up the dummy data.
 *
 * @param base SPI peripheral address.
 * @param dummyData Data to be transferred when tx buffer is NULL.
 */
void SPI_SetDummyData(SPI_Type *base, uint8_t dummyData);

/*! @} */

/*!
 * @name FIFO Operations
 * @{
 */

/*!
 * @brief Enable FIFO for SPI.
 *
 * This function will enable the FIFO for SPI according to pointer of the configure struct.
 * Note: If this API is called, please reset the baudrate to adapt your demand after this API
 * was called.
 *
 * @param base SPI peripheral base address.
 * @param config pointer to FIFO configuration structure.
 */
void SPI_EnableFifo(SPI_Type *base, const spi_fifo_config_t *config);

/*!
 * @brief Disable FIFO for SPI.
 *
 * This function will Disable the FIFO for SPI transfer.
 * disable interrupts, clear status flags, disable the TX/RX FIFO, set fifo size to zero.
 * But will not disable the system FIFO, because other instance like USART may using the FIFO.
 *
 * @param base SPI peripheral base address.
 * @param config pointer to FIFO configuration structure.
 */
void SPI_DisableFifo(SPI_Type *base);

/*!
 * @brief Is TX FIFO enabled.
 *
 * This function will return status if the transmit fifo is enabled. true for enabled and false for not enabled.
 *
 * @param base SPI peripheral base address.
 * @return true for enabled and false for not enabled.
 */
bool SPI_IsTxFifoEnabled(SPI_Type *base);

/*!
 * @brief Is RX FIFO enabled.
 *
 * This function will return status if the receive fifo is enabled. true for enabled and false for not enabled.
 *
 * @param base SPI peripheral base address.
 * @return true for enabled and false for not enabled.
 */
bool SPI_IsRxFifoEnabled(SPI_Type *base);

/*!
 * @brief Flush the FIFO buffer.
 *
 * This function will Flush tHE fifo buffer.
 *
 * @param base SPI peripheral base address.
 * @param direction the fifo direction need to flushed, Tx FIFO or Rx FIFO.
 */
void SPI_FifoFlush(SPI_Type *base, uint32_t direction);
/*! @} */

/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Initializes the SPI master handle.
 *
 * This function initializes the SPI master handle which can be used for other SPI master transactional APIs. Usually,
 * for a specified SPI instance, call this API once to get the initialized handle.
 *
 * @param base SPI peripheral base address.
 * @param handle SPI handle pointer.
 * @param callback Callback function.
 * @param userData User data.
 */
status_t SPI_MasterTransferCreateHandle(SPI_Type *base,
                                        spi_master_handle_t *handle,
                                        spi_master_callback_t callback,
                                        void *userData);

/*!
 * @brief Transfers a block of data using a polling method.
 *
 * @param base SPI base pointer
 * @param xfer pointer to spi_xfer_config_t structure
 * @retval kStatus_Success Successfully start a transfer.
 * @retval kStatus_InvalidArgument Input argument is invalid.
 */
status_t SPI_MasterTransferBlocking(SPI_Type *base, spi_transfer_t *xfer);

/*!
 * @brief Performs a non-blocking SPI interrupt transfer.
 *
 * @param base SPI peripheral base address.
 * @param handle pointer to spi_master_handle_t structure which stores the transfer state
 * @param xfer pointer to spi_xfer_config_t structure
 * @retval kStatus_Success Successfully start a transfer.
 * @retval kStatus_InvalidArgument Input argument is invalid.
 * @retval kStatus_SPI_Busy SPI is not idle, is running another transfer.
 */
status_t SPI_MasterTransferNonBlocking(SPI_Type *base, spi_master_handle_t *handle, spi_transfer_t *xfer);

/*!
 * @brief Transfers a block of data using a polling method.
 *
 * This function will do a half-duplex transfer for SPI master, This is a blocking function,
 * which does not retuen until all transfer have been completed. And data transfer will be half-duplex,
 * users can set transmit first or receive first.
 *
 * @param base SPI base pointer
 * @param xfer pointer to spi_half_duplex_transfer_t structure
 * @return status of status_t.
 */
status_t SPI_MasterHalfDuplexTransferBlocking(SPI_Type *base, spi_half_duplex_transfer_t *xfer);

/*!
 * @brief Performs a non-blocking SPI interrupt transfer.
 *
 * This function using polling way to do the first half transimission and using interrupts to
 * do the srcond half transimission, the transfer mechanism is half-duplex.
 * When do the second half transimission, code will return right away. When all data is transferred,
 * the callback function is called.
 *
 * @param base SPI peripheral base address.
 * @param handle pointer to spi_master_handle_t structure which stores the transfer state
 * @param xfer pointer to spi_half_duplex_transfer_t structure
 * @return status of status_t.
 */
status_t SPI_MasterHalfDuplexTransferNonBlocking(SPI_Type *base,
                                                 spi_master_handle_t *handle,
                                                 spi_half_duplex_transfer_t *xfer);

/*!
 * @brief Gets the master transfer count.
 *
 * This function gets the master transfer count.
 *
 * @param base SPI peripheral base address.
 * @param handle Pointer to the spi_master_handle_t structure which stores the transfer state.
 * @param count The number of bytes transferred by using the non-blocking transaction.
 * @return status of status_t.
 */
status_t SPI_MasterTransferGetCount(SPI_Type *base, spi_master_handle_t *handle, size_t *count);

/*!
 * @brief SPI master aborts a transfer using an interrupt.
 *
 * This function aborts a transfer using an interrupt.
 *
 * @param base SPI peripheral base address.
 * @param handle Pointer to the spi_master_handle_t structure which stores the transfer state.
 */
void SPI_MasterTransferAbort(SPI_Type *base, spi_master_handle_t *handle);

/*!
 * @brief Interrupts the handler for the SPI.
 *
 * @param base SPI peripheral base address.
 * @param handle pointer to spi_master_handle_t structure which stores the transfer state.
 */
void SPI_MasterTransferHandleIRQ(SPI_Type *base, spi_master_handle_t *handle);

/*!
 * @brief Initializes the SPI slave handle.
 *
 * This function initializes the SPI slave handle which can be used for other SPI slave transactional APIs. Usually,
 * for a specified SPI instance, call this API once to get the initialized handle.
 *
 * @param base SPI peripheral base address.
 * @param handle SPI handle pointer.
 * @param callback Callback function.
 * @param userData User data.
 */
status_t SPI_SlaveTransferCreateHandle(SPI_Type *base,
                                       spi_slave_handle_t *handle,
                                       spi_slave_callback_t callback,
                                       void *userData);

/*!
 * @brief Performs a non-blocking SPI slave interrupt transfer.
 *
 * @note The API returns immediately after the transfer initialization is finished.
 *
 * @param base SPI peripheral base address.
 * @param handle pointer to spi_master_handle_t structure which stores the transfer state
 * @param xfer pointer to spi_xfer_config_t structure
 * @retval kStatus_Success Successfully start a transfer.
 * @retval kStatus_InvalidArgument Input argument is invalid.
 * @retval kStatus_SPI_Busy SPI is not idle, is running another transfer.
 */
status_t SPI_SlaveTransferNonBlocking(SPI_Type *base, spi_slave_handle_t *handle, spi_transfer_t *xfer);

/*!
 * @brief Gets the slave transfer count.
 *
 * This function gets the slave transfer count.
 *
 * @param base SPI peripheral base address.
 * @param handle Pointer to the spi_master_handle_t structure which stores the transfer state.
 * @param count The number of bytes transferred by using the non-blocking transaction.
 * @return status of status_t.
 */
static inline status_t SPI_SlaveTransferGetCount(SPI_Type *base, spi_slave_handle_t *handle, size_t *count)
{
    return SPI_MasterTransferGetCount(base, (spi_master_handle_t *)handle, count);
}

/*!
 * @brief SPI slave aborts a transfer using an interrupt.
 *
 * This function aborts a transfer using an interrupt.
 *
 * @param base SPI peripheral base address.
 * @param handle Pointer to the spi_slave_handle_t structure which stores the transfer state.
 */
static inline void SPI_SlaveTransferAbort(SPI_Type *base, spi_slave_handle_t *handle)
{
    SPI_MasterTransferAbort(base, (spi_master_handle_t *)handle);
}

/*!
 * @brief Interrupts a handler for the SPI slave.
 *
 * @param base SPI peripheral base address.
 * @param handle pointer to spi_slave_handle_t structure which stores the transfer state
 */
void SPI_SlaveTransferHandleIRQ(SPI_Type *base, spi_slave_handle_t *handle);

/*! @} */

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_SPI_H_*/
