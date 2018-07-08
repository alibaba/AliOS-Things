#ifndef _REG_DMA_H_
#define _REG_DMA_H_

#include "co_int.h"
#include "_reg_dma.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"

#define REG_DMA_COUNT 49

#define REG_DMA_DECODING_MASK 0x000000FF

/**
 * @brief CH_LLI_ROOT register definition
 */

__INLINE uint32_t dma_ch_lli_root_get(int reg_idx)
{
    return 0;
}

__INLINE void dma_ch_lli_root_set(int reg_idx, uint32_t value)
{
}

/**
 * @brief DMA_STATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     29      DOWN_STREAM_BSY   0
 *     28        UP_STREAM_BSY   0
 *     27         ARB_Q3_VALID   0
 *     26         ARB_Q2_VALID   0
 *     25         ARB_Q1_VALID   0
 *     24         ARB_Q0_VALID   0
 *  23:20        REQUEST_STATE   0x0
 *     19          CH3_STOPPED   0
 *     18          CH2_STOPPED   0
 *     17          CH1_STOPPED   0
 *     16          CH0_STOPPED   0
 *  15:00             OFT_FREE   0x0
 * </pre>
 */
#define DMA_DMA_STATUS_ADDR   0x10A00010
#define DMA_DMA_STATUS_OFFSET 0x00000010
#define DMA_DMA_STATUS_INDEX  0x00000004
#define DMA_DMA_STATUS_RESET  0x00000000

__INLINE uint32_t dma_dma_status_get(void)
{
    return 0;
}

// field definitions
#define DMA_DOWN_STREAM_BSY_BIT    ((uint32_t)0x20000000)
#define DMA_DOWN_STREAM_BSY_POS    29
#define DMA_UP_STREAM_BSY_BIT      ((uint32_t)0x10000000)
#define DMA_UP_STREAM_BSY_POS      28
#define DMA_ARB_Q3_VALID_BIT       ((uint32_t)0x08000000)
#define DMA_ARB_Q3_VALID_POS       27
#define DMA_ARB_Q2_VALID_BIT       ((uint32_t)0x04000000)
#define DMA_ARB_Q2_VALID_POS       26
#define DMA_ARB_Q1_VALID_BIT       ((uint32_t)0x02000000)
#define DMA_ARB_Q1_VALID_POS       25
#define DMA_ARB_Q0_VALID_BIT       ((uint32_t)0x01000000)
#define DMA_ARB_Q0_VALID_POS       24
#define DMA_REQUEST_STATE_MASK     ((uint32_t)0x00F00000)
#define DMA_REQUEST_STATE_LSB      20
#define DMA_REQUEST_STATE_WIDTH    ((uint32_t)0x00000004)
#define DMA_CH3_STOPPED_BIT        ((uint32_t)0x00080000)
#define DMA_CH3_STOPPED_POS        19
#define DMA_CH2_STOPPED_BIT        ((uint32_t)0x00040000)
#define DMA_CH2_STOPPED_POS        18
#define DMA_CH1_STOPPED_BIT        ((uint32_t)0x00020000)
#define DMA_CH1_STOPPED_POS        17
#define DMA_CH0_STOPPED_BIT        ((uint32_t)0x00010000)
#define DMA_CH0_STOPPED_POS        16
#define DMA_OFT_FREE_MASK          ((uint32_t)0x0000FFFF)
#define DMA_OFT_FREE_LSB           0
#define DMA_OFT_FREE_WIDTH         ((uint32_t)0x00000010)

#define DMA_DOWN_STREAM_BSY_RST    0x0
#define DMA_UP_STREAM_BSY_RST      0x0
#define DMA_ARB_Q3_VALID_RST       0x0
#define DMA_ARB_Q2_VALID_RST       0x0
#define DMA_ARB_Q1_VALID_RST       0x0
#define DMA_ARB_Q0_VALID_RST       0x0
#define DMA_REQUEST_STATE_RST      0x0
#define DMA_CH3_STOPPED_RST        0x0
#define DMA_CH2_STOPPED_RST        0x0
#define DMA_CH1_STOPPED_RST        0x0
#define DMA_CH0_STOPPED_RST        0x0
#define DMA_OFT_FREE_RST           0x0

__INLINE void dma_dma_status_unpack(uint8_t *downstreambsy, uint8_t *upstreambsy, uint8_t *arbq3valid, uint8_t *arbq2valid, uint8_t *arbq1valid, uint8_t *arbq0valid, uint8_t *requeststate, uint8_t *ch3stopped, uint8_t *ch2stopped, uint8_t *ch1stopped, uint8_t *ch0stopped, uint16_t *oftfree)
{
}

__INLINE uint8_t dma_dma_status_down_stream_bsy_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_dma_status_up_stream_bsy_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_dma_status_arb_q3_valid_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_dma_status_arb_q2_valid_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_dma_status_arb_q1_valid_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_dma_status_arb_q0_valid_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_dma_status_request_state_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_dma_status_ch3_stopped_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_dma_status_ch2_stopped_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_dma_status_ch1_stopped_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_dma_status_ch0_stopped_getf(void)
{
    return 0;
}

/**
 * @brief INT_RAWSTATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     23              CH3_EOT   0
 *     22              CH2_EOT   0
 *     21              CH1_EOT   0
 *     20              CH0_EOT   0
 *     16                ERROR   0
 *  15:00              LLI_IRQ   0x0
 * </pre>
 */
#define DMA_INT_RAWSTATUS_ADDR   0x10A00014
#define DMA_INT_RAWSTATUS_OFFSET 0x00000014
#define DMA_INT_RAWSTATUS_INDEX  0x00000005
#define DMA_INT_RAWSTATUS_RESET  0x00000000

__INLINE uint32_t dma_int_rawstatus_get(void)
{
    return 0;
}

// field definitions
#define DMA_CH3_EOT_BIT    ((uint32_t)0x00800000)
#define DMA_CH3_EOT_POS    23
#define DMA_CH2_EOT_BIT    ((uint32_t)0x00400000)
#define DMA_CH2_EOT_POS    22
#define DMA_CH1_EOT_BIT    ((uint32_t)0x00200000)
#define DMA_CH1_EOT_POS    21
#define DMA_CH0_EOT_BIT    ((uint32_t)0x00100000)
#define DMA_CH0_EOT_POS    20
#define DMA_ERROR_BIT      ((uint32_t)0x00010000)
#define DMA_ERROR_POS      16
#define DMA_LLI_IRQ_MASK   ((uint32_t)0x0000FFFF)
#define DMA_LLI_IRQ_LSB    0
#define DMA_LLI_IRQ_WIDTH  ((uint32_t)0x00000010)

#define DMA_CH3_EOT_RST    0x0
#define DMA_CH2_EOT_RST    0x0
#define DMA_CH1_EOT_RST    0x0
#define DMA_CH0_EOT_RST    0x0
#define DMA_ERROR_RST      0x0
#define DMA_LLI_IRQ_RST    0x0

__INLINE void dma_int_rawstatus_unpack(uint8_t *ch3eot, uint8_t *ch2eot, uint8_t *ch1eot, uint8_t *ch0eot, uint8_t *error, uint16_t *lliirq)
{
}

__INLINE uint8_t dma_int_rawstatus_ch3_eot_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_int_rawstatus_ch2_eot_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_int_rawstatus_ch1_eot_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_int_rawstatus_ch0_eot_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_int_rawstatus_error_getf(void)
{
    return 0;
}

__INLINE uint16_t dma_int_rawstatus_lli_irq_getf(void)
{
    return 0;
}

/**
 * @brief INT_UNMASK_SET register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     23              CH3_EOT   0
 *     22              CH2_EOT   0
 *     21              CH1_EOT   0
 *     20              CH0_EOT   0
 *     16                ERROR   0
 *  15:00              LLI_IRQ   0x0
 * </pre>
 */
#define DMA_INT_UNMASK_SET_ADDR   0x10A00018
#define DMA_INT_UNMASK_SET_OFFSET 0x00000018
#define DMA_INT_UNMASK_SET_INDEX  0x00000006
#define DMA_INT_UNMASK_SET_RESET  0x00000000

__INLINE uint32_t dma_int_unmask_get(void)
{
    return 0;
}

__INLINE void dma_int_unmask_set(uint32_t value)
{
}

// field definitions
#define DMA_CH3_EOT_BIT    ((uint32_t)0x00800000)
#define DMA_CH3_EOT_POS    23
#define DMA_CH2_EOT_BIT    ((uint32_t)0x00400000)
#define DMA_CH2_EOT_POS    22
#define DMA_CH1_EOT_BIT    ((uint32_t)0x00200000)
#define DMA_CH1_EOT_POS    21
#define DMA_CH0_EOT_BIT    ((uint32_t)0x00100000)
#define DMA_CH0_EOT_POS    20
#define DMA_ERROR_BIT      ((uint32_t)0x00010000)
#define DMA_ERROR_POS      16
#define DMA_LLI_IRQ_MASK   ((uint32_t)0x0000FFFF)
#define DMA_LLI_IRQ_LSB    0
#define DMA_LLI_IRQ_WIDTH  ((uint32_t)0x00000010)

#define DMA_CH3_EOT_RST    0x0
#define DMA_CH2_EOT_RST    0x0
#define DMA_CH1_EOT_RST    0x0
#define DMA_CH0_EOT_RST    0x0
#define DMA_ERROR_RST      0x0
#define DMA_LLI_IRQ_RST    0x0

__INLINE void dma_int_unmask_set_pack(uint8_t ch3eot, uint8_t ch2eot, uint8_t ch1eot, uint8_t ch0eot, uint8_t error, uint16_t lliirq)
{
}

__INLINE void dma_int_unmask_unpack(uint8_t *ch3eot, uint8_t *ch2eot, uint8_t *ch1eot, uint8_t *ch0eot, uint8_t *error, uint16_t *lliirq)
{
}

__INLINE uint8_t dma_int_unmask_ch3_eot_getf(void)
{
    return 0;
}

__INLINE void dma_int_unmask_ch3_eot_setf(uint8_t ch3eot)
{
}

__INLINE uint8_t dma_int_unmask_ch2_eot_getf(void)
{
    return 0;
}

__INLINE void dma_int_unmask_ch2_eot_setf(uint8_t ch2eot)
{
}

__INLINE uint8_t dma_int_unmask_ch1_eot_getf(void)
{
    return 0;
}

__INLINE void dma_int_unmask_ch1_eot_setf(uint8_t ch1eot)
{
}

__INLINE uint8_t dma_int_unmask_ch0_eot_getf(void)
{
    return 0;
}

__INLINE void dma_int_unmask_ch0_eot_setf(uint8_t ch0eot)
{
}

__INLINE uint8_t dma_int_unmask_error_getf(void)
{
    return 0;
}

__INLINE void dma_int_unmask_error_setf(uint8_t error)
{
}

__INLINE uint16_t dma_int_unmask_lli_irq_getf(void)
{
    return 0;
}

__INLINE void dma_int_unmask_lli_irq_setf(uint16_t lliirq)
{
}

/**
 * @brief INT_UNMASK_CLEAR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     23              CH3_EOT   0
 *     22              CH2_EOT   0
 *     21              CH1_EOT   0
 *     20              CH0_EOT   0
 *     16                ERROR   0
 *  15:00              LLI_IRQ   0x0
 * </pre>
 */
#define DMA_INT_UNMASK_CLEAR_ADDR   0x10A0001C
#define DMA_INT_UNMASK_CLEAR_OFFSET 0x0000001C
#define DMA_INT_UNMASK_CLEAR_INDEX  0x00000007
#define DMA_INT_UNMASK_CLEAR_RESET  0x00000000

__INLINE void dma_int_unmask_clear(uint32_t value)
{
}

// fields defined in symmetrical set/clear register
__INLINE void dma_int_unmask_clear_pack(uint8_t ch3eot, uint8_t ch2eot, uint8_t ch1eot, uint8_t ch0eot, uint8_t error, uint16_t lliirq)
{
}

__INLINE void dma_int_unmask_ch3_eot_clearf(uint8_t ch3eot)
{
}

__INLINE void dma_int_unmask_ch2_eot_clearf(uint8_t ch2eot)
{
}

__INLINE void dma_int_unmask_ch1_eot_clearf(uint8_t ch1eot)
{
}

__INLINE void dma_int_unmask_ch0_eot_clearf(uint8_t ch0eot)
{
}

__INLINE void dma_int_unmask_error_clearf(uint8_t error)
{
}

__INLINE void dma_int_unmask_lli_irq_clearf(uint16_t lliirq)
{
}

/**
 * @brief INT_ACK register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     23              CH3_EOT   0
 *     22              CH2_EOT   0
 *     21              CH1_EOT   0
 *     20              CH0_EOT   0
 *     16                ERROR   0
 *  15:00              LLI_IRQ   0x0
 * </pre>
 */
#define DMA_INT_ACK_ADDR   0x10A00020
#define DMA_INT_ACK_OFFSET 0x00000020
#define DMA_INT_ACK_INDEX  0x00000008
#define DMA_INT_ACK_RESET  0x00000000

__INLINE uint32_t dma_int_ack_get(void)
{
    return 0;
}

__INLINE void dma_int_ack_clear(uint32_t value)
{
}

// field definitions
#define DMA_CH3_EOT_BIT    ((uint32_t)0x00800000)
#define DMA_CH3_EOT_POS    23
#define DMA_CH2_EOT_BIT    ((uint32_t)0x00400000)
#define DMA_CH2_EOT_POS    22
#define DMA_CH1_EOT_BIT    ((uint32_t)0x00200000)
#define DMA_CH1_EOT_POS    21
#define DMA_CH0_EOT_BIT    ((uint32_t)0x00100000)
#define DMA_CH0_EOT_POS    20
#define DMA_ERROR_BIT      ((uint32_t)0x00010000)
#define DMA_ERROR_POS      16
#define DMA_LLI_IRQ_MASK   ((uint32_t)0x0000FFFF)
#define DMA_LLI_IRQ_LSB    0
#define DMA_LLI_IRQ_WIDTH  ((uint32_t)0x00000010)

#define DMA_CH3_EOT_RST    0x0
#define DMA_CH2_EOT_RST    0x0
#define DMA_CH1_EOT_RST    0x0
#define DMA_CH0_EOT_RST    0x0
#define DMA_ERROR_RST      0x0
#define DMA_LLI_IRQ_RST    0x0

__INLINE void dma_int_ack_pack(uint8_t ch3eot, uint8_t ch2eot, uint8_t ch1eot, uint8_t ch0eot, uint8_t error, uint16_t lliirq)
{
}

__INLINE void dma_int_ack_unpack(uint8_t *ch3eot, uint8_t *ch2eot, uint8_t *ch1eot, uint8_t *ch0eot, uint8_t *error, uint16_t *lliirq)
{
}

__INLINE uint8_t dma_int_ack_ch3_eot_getf(void)
{
    return 0;
}

__INLINE void dma_int_ack_ch3_eot_clearf(uint8_t ch3eot)
{
}

__INLINE uint8_t dma_int_ack_ch2_eot_getf(void)
{
    return 0;
}

__INLINE void dma_int_ack_ch2_eot_clearf(uint8_t ch2eot)
{
}

__INLINE uint8_t dma_int_ack_ch1_eot_getf(void)
{
    return 0;
}

__INLINE void dma_int_ack_ch1_eot_clearf(uint8_t ch1eot)
{
}

__INLINE uint8_t dma_int_ack_ch0_eot_getf(void)
{
    return 0;
}

__INLINE void dma_int_ack_ch0_eot_clearf(uint8_t ch0eot)
{
}

__INLINE uint8_t dma_int_ack_error_getf(void)
{
    return 0;
}

__INLINE void dma_int_ack_error_clearf(uint8_t error)
{
}

__INLINE uint16_t dma_int_ack_lli_irq_getf(void)
{
    return 0;
}

__INLINE void dma_int_ack_lli_irq_clearf(uint16_t lliirq)
{
}

/**
 * @brief INT_STATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     23              CH3_EOT   0
 *     22              CH2_EOT   0
 *     21              CH1_EOT   0
 *     20              CH0_EOT   0
 *     16                ERROR   0
 *  15:00              LLI_IRQ   0x0
 * </pre>
 */
#define DMA_INT_STATUS_ADDR   0x10A00024
#define DMA_INT_STATUS_OFFSET 0x00000024
#define DMA_INT_STATUS_INDEX  0x00000009
#define DMA_INT_STATUS_RESET  0x00000000

__INLINE uint32_t dma_int_status_get(void)
{
    return REG_PL_RD(DMA_INT_STATUS_ADDR);
}

// field definitions
#define DMA_CH3_EOT_BIT    ((uint32_t)0x00800000)
#define DMA_CH3_EOT_POS    23
#define DMA_CH2_EOT_BIT    ((uint32_t)0x00400000)
#define DMA_CH2_EOT_POS    22
#define DMA_CH1_EOT_BIT    ((uint32_t)0x00200000)
#define DMA_CH1_EOT_POS    21
#define DMA_CH0_EOT_BIT    ((uint32_t)0x00100000)
#define DMA_CH0_EOT_POS    20
#define DMA_ERROR_BIT      ((uint32_t)0x00010000)
#define DMA_ERROR_POS      16
#define DMA_LLI_IRQ_MASK   ((uint32_t)0x0000FFFF)
#define DMA_LLI_IRQ_LSB    0
#define DMA_LLI_IRQ_WIDTH  ((uint32_t)0x00000010)

#define DMA_CH3_EOT_RST    0x0
#define DMA_CH2_EOT_RST    0x0
#define DMA_CH1_EOT_RST    0x0
#define DMA_CH0_EOT_RST    0x0
#define DMA_ERROR_RST      0x0
#define DMA_LLI_IRQ_RST    0x0

__INLINE void dma_int_status_unpack(uint8_t *ch3eot, uint8_t *ch2eot, uint8_t *ch1eot, uint8_t *ch0eot, uint8_t *error, uint16_t *lliirq)
{
}

__INLINE uint8_t dma_int_status_ch3_eot_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_int_status_ch2_eot_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_int_status_ch1_eot_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_int_status_ch0_eot_getf(void)
{
    return 0;
}

__INLINE uint8_t dma_int_status_error_getf(void)
{
    return 0;
}

__INLINE uint16_t dma_int_status_lli_irq_getf(void)
{
    return 0;
}

/**
 * @brief CHANNEL_PRIORITY register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     16   INTERLEAVE_ENABLED   0
 *  13:12         CH3_PRIORITY   0x0
 *  09:08         CH2_PRIORITY   0x0
 *  05:04         CH1_PRIORITY   0x0
 *  01:00         CH0_PRIORITY   0x0
 * </pre>
 */
#define DMA_CHANNEL_PRIORITY_ADDR   0x10A00034
#define DMA_CHANNEL_PRIORITY_OFFSET 0x00000034
#define DMA_CHANNEL_PRIORITY_INDEX  0x0000000D
#define DMA_CHANNEL_PRIORITY_RESET  0x00000000

__INLINE uint32_t dma_channel_priority_get(void)
{
    return 0;
}

__INLINE void dma_channel_priority_set(uint32_t value)
{
    REG_PL_WR(DMA_CHANNEL_PRIORITY_ADDR, value);
}

// field definitions
#define DMA_INTERLEAVE_ENABLED_BIT    ((uint32_t)0x00010000)
#define DMA_INTERLEAVE_ENABLED_POS    16
#define DMA_CH3_PRIORITY_MASK         ((uint32_t)0x00003000)
#define DMA_CH3_PRIORITY_LSB          12
#define DMA_CH3_PRIORITY_WIDTH        ((uint32_t)0x00000002)
#define DMA_CH2_PRIORITY_MASK         ((uint32_t)0x00000300)
#define DMA_CH2_PRIORITY_LSB          8
#define DMA_CH2_PRIORITY_WIDTH        ((uint32_t)0x00000002)
#define DMA_CH1_PRIORITY_MASK         ((uint32_t)0x00000030)
#define DMA_CH1_PRIORITY_LSB          4
#define DMA_CH1_PRIORITY_WIDTH        ((uint32_t)0x00000002)
#define DMA_CH0_PRIORITY_MASK         ((uint32_t)0x00000003)
#define DMA_CH0_PRIORITY_LSB          0
#define DMA_CH0_PRIORITY_WIDTH        ((uint32_t)0x00000002)

#define DMA_INTERLEAVE_ENABLED_RST    0x0
#define DMA_CH3_PRIORITY_RST          0x0
#define DMA_CH2_PRIORITY_RST          0x0
#define DMA_CH1_PRIORITY_RST          0x0
#define DMA_CH0_PRIORITY_RST          0x0

__INLINE void dma_channel_priority_pack(uint8_t interleaveenabled, uint8_t ch3priority, uint8_t ch2priority, uint8_t ch1priority, uint8_t ch0priority)
{
}

__INLINE void dma_channel_priority_unpack(uint8_t *interleaveenabled, uint8_t *ch3priority, uint8_t *ch2priority, uint8_t *ch1priority, uint8_t *ch0priority)
{
}

__INLINE uint8_t dma_channel_priority_interleave_enabled_getf(void)
{
    return 0;
}

__INLINE void dma_channel_priority_interleave_enabled_setf(uint8_t interleaveenabled)
{
}

__INLINE uint8_t dma_channel_priority_ch3_priority_getf(void)
{
    return 0;
}

__INLINE void dma_channel_priority_ch3_priority_setf(uint8_t ch3priority)
{
}

__INLINE uint8_t dma_channel_priority_ch2_priority_getf(void)
{
    return 0;
}

__INLINE void dma_channel_priority_ch2_priority_setf(uint8_t ch2priority)
{
}

__INLINE uint8_t dma_channel_priority_ch1_priority_getf(void)
{
    return 0;
}

__INLINE void dma_channel_priority_ch1_priority_setf(uint8_t ch1priority)
{
}

__INLINE uint8_t dma_channel_priority_ch0_priority_getf(void)
{
    return 0;
}

__INLINE void dma_channel_priority_ch0_priority_setf(uint8_t ch0priority)
{
}

/**
 * @brief CHANNEL_MUTEX_SET register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     03            CH3_MUTEX   0
 *     02            CH2_MUTEX   0
 *     01            CH1_MUTEX   0
 *     00            CH0_MUTEX   0
 * </pre>
 */
#define DMA_CHANNEL_MUTEX_SET_ADDR   0x10A00038
#define DMA_CHANNEL_MUTEX_SET_OFFSET 0x00000038
#define DMA_CHANNEL_MUTEX_SET_INDEX  0x0000000E
#define DMA_CHANNEL_MUTEX_SET_RESET  0x00000000

__INLINE uint32_t dma_channel_mutex_get(void)
{
    return 0;
}

__INLINE void dma_channel_mutex_set(uint32_t value)
{
}

// field definitions
#define DMA_CH3_MUTEX_BIT    ((uint32_t)0x00000008)
#define DMA_CH3_MUTEX_POS    3
#define DMA_CH2_MUTEX_BIT    ((uint32_t)0x00000004)
#define DMA_CH2_MUTEX_POS    2
#define DMA_CH1_MUTEX_BIT    ((uint32_t)0x00000002)
#define DMA_CH1_MUTEX_POS    1
#define DMA_CH0_MUTEX_BIT    ((uint32_t)0x00000001)
#define DMA_CH0_MUTEX_POS    0

#define DMA_CH3_MUTEX_RST    0x0
#define DMA_CH2_MUTEX_RST    0x0
#define DMA_CH1_MUTEX_RST    0x0
#define DMA_CH0_MUTEX_RST    0x0

__INLINE void dma_channel_mutex_set_pack(uint8_t ch3mutex, uint8_t ch2mutex, uint8_t ch1mutex, uint8_t ch0mutex)
{
}

__INLINE void dma_channel_mutex_unpack(uint8_t *ch3mutex, uint8_t *ch2mutex, uint8_t *ch1mutex, uint8_t *ch0mutex)
{
}

__INLINE uint8_t dma_channel_mutex_ch3_mutex_getf(void)
{
    return 0;
}

__INLINE void dma_channel_mutex_ch3_mutex_setf(uint8_t ch3mutex)
{
}

__INLINE uint8_t dma_channel_mutex_ch2_mutex_getf(void)
{
    return 0;
}

__INLINE void dma_channel_mutex_ch2_mutex_setf(uint8_t ch2mutex)
{
}

__INLINE uint8_t dma_channel_mutex_ch1_mutex_getf(void)
{
    return 0;
}

__INLINE void dma_channel_mutex_ch1_mutex_setf(uint8_t ch1mutex)
{
}

__INLINE uint8_t dma_channel_mutex_ch0_mutex_getf(void)
{
    return 0;
}

__INLINE void dma_channel_mutex_ch0_mutex_setf(uint8_t ch0mutex)
{
}

/**
 * @brief CHANNEL_MUTEX_CLEAR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     03            CH3_MUTEX   0
 *     02            CH2_MUTEX   0
 *     01            CH1_MUTEX   0
 *     00            CH0_MUTEX   0
 * </pre>
 */
#define DMA_CHANNEL_MUTEX_CLEAR_ADDR   0x10A0003C
#define DMA_CHANNEL_MUTEX_CLEAR_OFFSET 0x0000003C
#define DMA_CHANNEL_MUTEX_CLEAR_INDEX  0x0000000F
#define DMA_CHANNEL_MUTEX_CLEAR_RESET  0x00000000

__INLINE void dma_channel_mutex_clear(uint32_t value)
{
}

// fields defined in symmetrical set/clear register
__INLINE void dma_channel_mutex_clear_pack(uint8_t ch3mutex, uint8_t ch2mutex, uint8_t ch1mutex, uint8_t ch0mutex)
{
}

__INLINE void dma_channel_mutex_ch3_mutex_clearf(uint8_t ch3mutex)
{
}

__INLINE void dma_channel_mutex_ch2_mutex_clearf(uint8_t ch2mutex)
{
}

__INLINE void dma_channel_mutex_ch1_mutex_clearf(uint8_t ch1mutex)
{
}

__INLINE void dma_channel_mutex_ch0_mutex_clearf(uint8_t ch0mutex)
{
}

// field definitions
#define DMA_COUNTER_MASK   ((uint32_t)0x0000FFFF)
#define DMA_COUNTER_LSB    0
#define DMA_COUNTER_WIDTH  ((uint32_t)0x00000010)

#define DMA_COUNTER_RST    0x0

/**
 * @brief DUMMY register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     00                DUMMY   0
 * </pre>
 */
#define DMA_DUMMY_ADDR   0x10A000C0
#define DMA_DUMMY_OFFSET 0x000000C0
#define DMA_DUMMY_INDEX  0x00000030
#define DMA_DUMMY_RESET  0x00000000

__INLINE uint32_t dma_dummy_get(void)
{
    return 0;
}

__INLINE void dma_dummy_set(uint32_t value)
{
}

// field definitions
#define DMA_DUMMY_BIT    ((uint32_t)0x00000001)
#define DMA_DUMMY_POS    0

#define DMA_DUMMY_RST    0x0

__INLINE uint8_t dma_dummy_getf(void)
{
    return 0;
}

__INLINE void dma_dummy_setf(uint8_t dummy)
{
}


#endif // _REG_DMA_H_

