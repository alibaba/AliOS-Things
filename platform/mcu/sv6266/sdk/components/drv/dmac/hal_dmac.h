#ifndef _HAL_DMAC_H_
#define _HAL_DMAC_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef enum _DMAC_TR_WIDTH_E {
    DMAC_TR_WIDTH_32        = 2         ///< 32 bits (4 Bytes)
} DMAC_TR_WIDTH_E;

typedef enum _DMAC_INC_E {
    DMAC_ADDR_INCREMENT     = 0,        ///< Address Increment
    DMAC_ADDR_DECREMENT     = 1,        ///< Address Decrement
    DMAC_ADDR_NO_CHANGE     = 2         ///< Address No change
} DMAC_INC_E;

typedef enum _DMAC_MSIZE_E {
    DMAC_MSIZE_1            = 0,        ///< Burst Transaction of Length 1
    DMAC_MSIZE_4            = 1         ///< Burst Transaction of Length 4
} DMAC_MSIZE_E;

typedef enum _DMAC_TT_FC_E {
    DMAC_TT_FC_M2M_Dma      = 0,        ///< Memory to Memory
    DMAC_TT_FC_M2P_Dma      = 1,        ///< Memory to Peripheral
    DMAC_TT_FC_P2M_Dma      = 2         ///< Peripheral to Memory
} DMAC_TT_FC_E;

typedef enum _DMAC_CH_PRIOR_E {
    DMAC_CH_PRIOR_0         = 0,        ///< Lowest Priority
    DMAC_CH_PRIOR_1         = 1         ///< Highest Priority
} DMAC_CH_PRIOR_E;

typedef enum _DMAC_HS_SEL_E {
    DMAC_HS_SEL_HARDWARE    = 0,        ///< Handshaking Interface Select Hardware
    DMAC_HS_SEL_SOFTWARE    = 1         ///< Handshaking Interface Select Software
} DMAC_HS_SEL_E;

typedef enum _DMAC_PER_E {
    DMAC_HW_HS_IF_I2S_RX    = 0,        ///< Hardware Handshaking Interface Select I2S Receiver
    DMAC_HW_HS_IF_I2S_TX    = 1,        ///< Hardware Handshaking Interface Select I2S Transmitter
    DMAC_HW_HS_IF_SPI_RX    = 2,        ///< Hardware Handshaking Interface Select SPI Master-Receiver
    DMAC_HW_HS_IF_SPI_TX    = 3,        ///< Hardware Handshaking Interface Select SPI Master-Transmitter
    DMAC_HW_HS_IF_I2C_RX    = 4,        ///< Hardware Handshaking Interface Select I2C Master-Receiver
    DMAC_HW_HS_IF_I2C_TX    = 5         ///< Hardware Handshaking Interface Select I2C Master-Transmitter
} DMAC_PER_E;

typedef enum _DMAC_INT_TYPE_E {
    DMAC_INT_TYPE_Tfr       = 0,        ///< Transfer Complete Interrupt Type
    DMAC_INT_TYPE_Err       = 4         ///< Error Interrupt Type
} DMAC_INT_TYPE_E;

typedef enum _DMAC_CH_E {
    DMAC_CH_0               = 0,        ///< Channel 0
    DMAC_CH_1               = 1,        ///< Channel 1
    DMAC_CH_MAX                ,        ///< Maximum number of channels
    DMAC_CH_NOT_FOUND       = -14       ///< Free channel was not found
} DMAC_CH_E;

typedef enum _DMAC_BLOCKING_MODE_E {
    DMAC_NON_BLOCKING       = 0,        ///< Sets the DMAC HAL driver to operate in non-blocking mode
    DMAC_BLOCKING           = 1         ///< Sets the DMAC HAL driver to operate in blocking mode
} DMAC_BLOCKING_MODE_E;

extern uint32_t gDMAC_VALUE[DMAC_CH_MAX];

/**
 * Prototype for the DMAC interrupt service routine.
 *
 * @param   CH                          Specifies the channel number.
 * @param   INT_TYPE                    Specifies the type of interrupt source.
 */
typedef
void
(*DMAC_ISR) (
    DMAC_CH_E               CH,
    DMAC_INT_TYPE_E         INT_TYPE
    );

/**
 * Registers an interrupt service routine to be called from the DMAC interrupt handler.
 *
 * If ISR is NULL, then the DMAC interrupt is disabled.
 *
 * @param   CH                          Specifies the channel number.
 * @param   INT_EN                      Specifies the type of interrupt enable.
 * @param   ISR                         A pointer to an ISR function that is called when a DMAC interrupt handler occurs.
 *                                      NULL to disable interrupt.
 *
 * @retval  -2                          The CH or INT_EN of the parameters are incorrect.
 * @retval   0                          The operation completed successfully.
 */
int32_t
hal_dmac_register_isr (
    DMAC_CH_E               CH,
    DMAC_INT_TYPE_E         INT_EN,
    DMAC_ISR                ISR
    );

/**
 * This service clears the status that indicates the DMAC interrupt source.
 *
 * @param   CH                          Specifies the channel number.
 * @param   INT_TYPE                    Specifies the type of interrupt source.
 *
 * @retval  -2                          The CH or INT_EN of the parameters are incorrect.
 * @retval   0                          The operation completed successfully.
 */
int32_t
hal_dmac_clear_int_status (
    DMAC_CH_E               CH,
    DMAC_INT_TYPE_E         INT_TYPE
    );

/**
 * Initialize the DMAC hardware.
 */
void
hal_dmac_init (
    void
    );

/**
 * Deinitialize the DMAC hardware.
 */
void
hal_dmac_deinit (
    void
    );

/**
 * Start the DMAC transfer.
 *
 * @param   DAR                         Specifies the start destination address.
 * @param   SAR                         Specifies the start source address.
 * @param   BLOCK_TS                    Specifies the block transfer size.
 * @param   CH                          Specifies the channel number.
 * @param   DST_TR_WIDTH                Specifies the destination transfer width.
 * @param   SRC_TR_WIDTH                Specifies the source transfer width.
 * @param   DINC                        Specifies the destination address increment, decrement, or no change.
 * @param   SINC                        Specifies the source address increment, decrement, or no change.
 * @param   DST_MSIZE                   Specifies the destination burst transaction length.
 * @param   SRC_MSIZE                   Specifies the source burst transaction length.
 * @param   TT_FC                       Specifies the transfer type and flow control.
 * @param   CH_PRIOR                    Specifies the channel priority.
 * @param   HS_SEL_DST                  Specifies the type of destination handshaking interface.
 * @param   DST_PER                     Specifies the hardware of destination handshaking interface.
 * @param   HS_SEL_SRC                  Specifies the type of source handshaking interface.
 * @param   SRC_PER                     Specifies the hardware of source handshaking interface.
 */
void
hal_dmac_start (
    void                    *DAR,
    void const              *SAR,
    uint32_t                BLOCK_TS,
    DMAC_CH_E               CH,
    DMAC_TR_WIDTH_E         DST_TR_WIDTH,
    DMAC_TR_WIDTH_E         SRC_TR_WIDTH,
    DMAC_INC_E              DINC,
    DMAC_INC_E              SINC,
    DMAC_MSIZE_E            DST_MSIZE,
    DMAC_MSIZE_E            SRC_MSIZE,
    DMAC_TT_FC_E            TT_FC,
    DMAC_CH_PRIOR_E         CH_PRIOR,
    DMAC_HS_SEL_E           HS_SEL_DST,
    DMAC_PER_E              DST_PER,
    DMAC_HS_SEL_E           HS_SEL_SRC,
    DMAC_PER_E              SRC_PER
    );

/**
 * Stop the DMAC transfer.
 *
 * @param   CH                          Specifies the channel number.
 *
 * @retval  -2                          The CH of the parameters is incorrect.
 * @retval   0                          The operation completed successfully.
 */
int32_t
hal_dmac_stop (
    DMAC_CH_E               CH
    );

/**
 * Sets the DMAC to 32-bit memory copy operation.
 *
 * @param   DAR                         Specifies the start destination address.
 * @param   SAR                         Specifies the start source address.
 * @param   COUNT                       Specifies the byte transfer size.
 * @param   CH                          Specifies the channel number.
 *
 * @retval  -19                         The DMAC has not been started.
 * @retval    0                         The operation completed successfully.
 */
int32_t
hal_dmac_m2mcpy32 (
    uint8_t                 *DAR,
    uint8_t const           *SAR,
    uint32_t                COUNT,
    DMAC_CH_E               CH
    );

/**
 * Sets the DMAC to 8-bit memory fill operation.
 *
 * @param   DAR                         Specifies the start destination address.
 * @param   SAR                         Specifies the start source address.
 * @param   COUNT                       Specifies the byte transfer size.
 * @param   CH                          Specifies the channel number.
 *
 * @retval  -19                         The DMAC has not been started.
 * @retval    0                         The operation completed successfully.
 */
int32_t
hal_dmac_m2mset8 (
    uint8_t                 *DAR,
    uint32_t                *SAR,
    uint32_t                COUNT,
    DMAC_CH_E               CH
    );

/**
 * Sets the DMAC to 16-bit memory fill operation.
 *
 * @param   DAR                         Specifies the start destination address.
 * @param   SAR                         Specifies the start source address.
 * @param   COUNT                       Specifies the byte transfer size.
 * @param   CH                          Specifies the channel number.
 *
 * @retval  -19                         The DMAC has not been started.
 * @retval    0                         The operation completed successfully.
 */
int32_t
hal_dmac_m2mset16 (
    uint8_t                 *DAR,
    uint32_t                *SAR,
    uint32_t                COUNT,
    DMAC_CH_E               CH
    );

/**
 * Sets the DMAC to 32-bit memory fill operation.
 *
 * @param   DAR                         Specifies the start destination address.
 * @param   SAR                         Specifies the start source address.
 * @param   COUNT                       Specifies the byte transfer size.
 * @param   CH                          Specifies the channel number.
 *
 * @retval  -19                         The DMAC has not been started.
 * @retval    0                         The operation completed successfully.
 */
int32_t
hal_dmac_m2mset32 (
    uint8_t                 *DAR,
    uint32_t                *SAR,
    uint32_t                COUNT,
    DMAC_CH_E               CH
    );

/**
 * This service finds a free DMAC channel.
 *
 * @param   BLOCKING_MODE               Sets the DMAC HAL driver to operate in blocking or non-blocking mode.
 *
 * @retval  DMAC_CH_NOT_FOUND           The free channel was not found.
 * @return  Returns the free channel number.
 */
DMAC_CH_E
hal_dmac_find_free_channel (
    DMAC_BLOCKING_MODE_E    BLOCKING_MODE
    );

/**
 * This service reads the DMAC interrupt raw status value.
 *
 * @param   CH                          Specifies the channel number.
 * @param   STATUS_TYPE                 Specifies the type of interrupt status.
 *
 * @retval  -2                          The CH or STATUS_TYPE of the parameters are incorrect.
 * @retval   1                          The corresponding interrupt is pending.
 * @retval   0                          The corresponding interrupt is not pending.
 */
int32_t
hal_dmac_get_int_raw_status (
    DMAC_CH_E               CH,
    DMAC_INT_TYPE_E         STATUS_TYPE
    );

#endif  // #ifndef _HAL_DMAC_H_
