/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_XDMA_H__
#define __HAL_XDMA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

#define HAL_XDMA_CHAN_NONE               0xFF
#define HAL_XDMA_MAX_DESC_XFER_SIZE      0x1FFF

enum HAL_XDMA_RET_T {
    HAL_XDMA_OK,
    HAL_XDMA_ERR,
};

enum HAL_XDMA_GET_CHAN_T {
    HAL_XDMA_PRIO_SET0,
    HAL_XDMA_PRIO_SET1,
    HAL_XDMA_PRIO_SET2,
    HAL_XDMA_PRIO_SET3,
    HAL_XDMA_PRIO_QTY
};

// DMA Type of DMA controller
enum HAL_XDMA_FLOW_CONTROL_T {
    HAL_XDMA_FLOW_M2M_DMA            = 0,    /* Memory to memory - DMA control */
    HAL_XDMA_FLOW_M2P_DMA            = 1,    /* Memory to peripheral - DMA control */
    HAL_XDMA_FLOW_P2M_DMA            = 2,    /* Peripheral to memory - DMA control */
    HAL_XDMA_FLOW_P2P_DMA            = 3,    /* Source peripheral to destination peripheral - DMA control */
    HAL_XDMA_FLOW_P2P_DSTPERIPH      = 4,    /* Source peripheral to destination peripheral - destination peripheral control */
    HAL_XDMA_FLOW_M2P_PERIPH         = 5,    /* Memory to peripheral - peripheral control */
    HAL_XDMA_FLOW_P2M_PERIPH         = 6,    /* Peripheral to memory - peripheral control */
    HAL_XDMA_FLOW_P2P_SRCPERIPH      = 7,    /* Source peripheral to destination peripheral - source peripheral control */

    HAL_XDMA_FLOW_FLAG_SI            = 0x40,
    HAL_XDMA_FLOW_FLAG_DI            = 0x80,
    HAL_XDMA_FLOW_FLAG_MASK          = (HAL_XDMA_FLOW_FLAG_SI | HAL_XDMA_FLOW_FLAG_DI),

    HAL_XDMA_FLOW_M2P_DI_DMA         = HAL_XDMA_FLOW_M2P_DMA | HAL_XDMA_FLOW_FLAG_DI,
    HAL_XDMA_FLOW_P2M_SI_DMA         = HAL_XDMA_FLOW_P2M_DMA | HAL_XDMA_FLOW_FLAG_SI,
    HAL_XDMA_FLOW_P2P_DI_DMA         = HAL_XDMA_FLOW_P2P_DMA | HAL_XDMA_FLOW_FLAG_DI,
    HAL_XDMA_FLOW_P2P_SI_DMA         = HAL_XDMA_FLOW_P2P_DMA | HAL_XDMA_FLOW_FLAG_SI,
};

// DMA Burst size in Source and Destination definitions
enum HAL_XDMA_BSIZE_T {
    HAL_XDMA_BSIZE_RESERVED     = 0,
    HAL_XDMA_BSIZE_8            = 1,    /* Burst size = 8 */
    HAL_XDMA_BSIZE_16           = 2,    /* Burst size = 16 */
    HAL_XDMA_BSIZE_32           = 3,    /* Burst size = 32 */
    HAL_XDMA_BSIZE_64           = 4,    /* Burst size = 64 */
};

// Width in Source transfer width and Destination transfer width definitions
enum HAL_XDMA_WDITH_T {
    HAL_XDMA_WIDTH_RESERVED    = 0,    /* WIDTH must be 0x0 for memory-to-memory moves or companion-chip-related operations */
    HAL_XDMA_WIDTH_BYTE        = 1,    /* Width = 1 byte */
    HAL_XDMA_WIDTH_HALFWORD    = 2,    /* Width = 2 bytes */
    HAL_XDMA_WIDTH_WORD        = 3,    /* Width = 4 bytes */
};

enum HAL_XDMA_PERIPH_T {
    HAL_DSPXDMA_PERIPH_NULL       = 0,
    HAL_DSPXDMA_MEM               = 1,
/*
    HAL_DSPXDMA_AUDIO             = 2,

    HAL_DSPXDMA_CODEC_RX          = 50,
    HAL_DSPXDMA_CODEC_TX          = 51,
    HAL_DSPXDMA_BTPCM_RX          = 52,
    HAL_DSPXDMA_BTPCM_TX          = 53,
    HAL_DSPXDMA_I2S0_RX           = 54,
    HAL_DSPXDMA_I2S0_TX           = 55,
    HAL_DSPXDMA_DPD_RX            = 56,
    HAL_DSPXDMA_DPD_TX            = 57,
    HAL_DSPXDMA_SPDIF0_RX         = 58,
    HAL_DSPXDMA_SPDIF0_TX         = 59,
    HAL_DSPXDMA_SPDIF1_RX         = 60,
    HAL_DSPXDMA_SPDIF1_TX         = 61,
    HAL_DSPXDMA_DSD_RX            = 62,
    HAL_DSPXDMA_DSD_TX            = 63,
    HAL_DSPXDMA_MC_RX             = 64,
    HAL_DSPXDMA_FFT_RX            = 65,
    HAL_DSPXDMA_FFT_TX            = 66,
    HAL_DSPXDMA_FIR_RX            = 67,
    HAL_DSPXDMA_FIR_TX            = 68,
    HAL_DSPXDMA_IIR_RX            = 69,
    HAL_DSPXDMA_IIR_TX            = 70,
    HAL_DSPXDMA_BTDUMP            = 71,
    HAL_DSPXDMA_I2S1_RX           = 72,
    HAL_DSPXDMA_I2S1_TX           = 73,
    HAL_DSPXDMA_FM_RX             = 74,
    HAL_DSPXDMA_FMDUMP0           = 75,
    HAL_DSPXDMA_FMDUMP1           = 76,
    HAL_DSPXDMA_CODEC_TX2         = 77,
*/
};

struct HAL_XDMA_DESC_T;
typedef void (*HAL_XDMA_IRQ_HANDLER_T)(uint8_t chan, uint32_t remain_tsize, uint32_t error, struct HAL_XDMA_DESC_T *lli);
typedef void (*HAL_XDMA_START_CALLBACK_T)(uint8_t chan);
typedef void (*HAL_XDMA_DELAY_FUNC)(uint32_t ms);

// DMA structure using for DMA configuration
struct HAL_XDMA_CH_CFG_T {
    uint8_t ch;                         /* DMA channel number */
    uint16_t src_tsize;                 /* Length/Size of transfer */
    enum HAL_XDMA_WDITH_T width;
    enum HAL_XDMA_BSIZE_T bsize;
    enum HAL_XDMA_FLOW_CONTROL_T type;  /* Transfer Type */
    enum HAL_XDMA_PERIPH_T src_periph;
    enum HAL_XDMA_PERIPH_T dst_periph;
    uint32_t src;                       /* Physical Source Address */
    uint32_t dst;                       /* Physical Destination Address */
    HAL_XDMA_IRQ_HANDLER_T handler;
    HAL_XDMA_START_CALLBACK_T start_cb;
};

// Transfer Descriptor structure typedef
struct HAL_XDMA_DESC_T {
    uint32_t dadr;       /* Pointer to next descriptor structure */
    uint32_t sadr;       /* Source address */
    uint32_t tadr;       /* Destination address */
    uint32_t cmd;        /* Control word that has transfer size, type etc. */
};

// DMA 2D configuration structure
struct HAL_XDMA_2D_CFG_T {
    int16_t xmodify;
    uint16_t xcount;
    int16_t ymodify;
    uint16_t ycount;
};

//=============================================================

void hal_xdma_open(void);
void hal_xdma_close(void);
bool hal_xdma_chan_busy(uint8_t ch);
uint8_t hal_xdma_get_chan(enum HAL_XDMA_PERIPH_T periph, enum HAL_XDMA_GET_CHAN_T policy);
void hal_xdma_free_chan(uint8_t ch);
uint32_t hal_xdma_cancel(uint8_t ch);
uint32_t hal_xdma_stop(uint8_t ch);
enum HAL_XDMA_RET_T hal_xdma_init_desc(struct HAL_XDMA_DESC_T *desc,
                                     const struct HAL_XDMA_CH_CFG_T *cfg,
                                     const struct HAL_XDMA_DESC_T *next,
                                     int tc_irq);
enum HAL_XDMA_RET_T hal_xdma_sg_start(const struct HAL_XDMA_DESC_T *desc,
                                    const struct HAL_XDMA_CH_CFG_T *cfg);
enum HAL_XDMA_RET_T hal_xdma_sg_2d_start(const struct HAL_XDMA_DESC_T *desc,
                                       const struct HAL_XDMA_CH_CFG_T *cfg,
                                       const struct HAL_XDMA_2D_CFG_T *src_2d,
                                       const struct HAL_XDMA_2D_CFG_T *dst_2d);
enum HAL_XDMA_RET_T hal_xdma_start(const struct HAL_XDMA_CH_CFG_T *cfg);
uint32_t hal_xdma_get_cur_src_addr(uint8_t ch);
uint32_t hal_xdma_get_cur_dst_addr(uint8_t ch);
uint32_t hal_xdma_get_sg_remain_size(uint8_t ch);
enum HAL_XDMA_RET_T hal_xdma_irq_run_chan(uint8_t ch);
bool hal_xdma_busy(void);
HAL_XDMA_DELAY_FUNC hal_xdma_set_delay_func(HAL_XDMA_DELAY_FUNC new_func);
void hal_xdma_remap_periph(enum HAL_XDMA_PERIPH_T periph, int enable);
void hal_xdma_tc_irq_enable(uint8_t ch);
void hal_xdma_tc_irq_disable(uint8_t ch);
void hal_dspxdma_mem2mem_test(char *dst, char *src, uint16_t num);

//=============================================================

#define hal_dspxdma_open                  hal_xdma_open
#define hal_dspxdma_close                 hal_xdma_close
#define hal_dspxdma_chan_busy             hal_xdma_chan_busy
#define hal_dspxdma_get_chan              hal_xdma_get_chan
#define hal_dspxdma_free_chan             hal_xdma_free_chan
#define hal_dspxdma_cancel                hal_xdma_cancel
#define hal_dspxdma_stop                  hal_xdma_stop
#define hal_dspxdma_init_desc             hal_xdma_init_desc
#define hal_dspxdma_sg_start              hal_xdma_sg_start
#define hal_dspxdma_start                 hal_xdma_start
#define hal_dspxdma_get_cur_src_addr      hal_xdma_get_cur_src_addr
#define hal_dspxdma_get_cur_dst_addr      hal_xdma_get_cur_dst_addr
#define hal_dspxdma_get_sg_remain_size    hal_xdma_get_sg_remain_size
#define hal_dspxdma_irq_run_chan          hal_xdma_irq_run_chan

//=============================================================

#ifdef __cplusplus
}
#endif

#endif
