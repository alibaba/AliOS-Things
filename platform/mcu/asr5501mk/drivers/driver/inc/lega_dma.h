#ifndef _LEGA_DMA_H_
#define _LEGA_DMA_H_
#include <stdint.h>

//#include "core_cm4.h"
//#include "core_cm4_simd.h"
//#include "core_cmFunc.h"
//#include "core_cmInstr.h"

// DRAMC Registers
#define DMA_BASE                     0x40004000
#define DMA_CLOCK_BASE               0x40000950

// ch0
#define DMA_SAR0_L                   *((volatile int *)(DMA_BASE+0x000))
#define DMA_SAR0_H                   *((volatile int *)(DMA_BASE+0x004))
#define DMA_DAR0_L                   *((volatile int *)(DMA_BASE+0x008))
#define DMA_DAR0_H                   *((volatile int *)(DMA_BASE+0x00C))

#define DMA_LLP0_L                   *((volatile int *)(DMA_BASE+0x010))
#define DMA_LLP0_L_REG               (DMA_BASE+0x010)
#define DMA_LLP0_H                   *((volatile int *)(DMA_BASE+0x014))
#define DMA_CTL0_L                   *((volatile int *)(DMA_BASE+0x018))
#define DMA_CTL0_L_REG               (DMA_BASE+0x018)
#define DMA_CTL0_H                   *((volatile int *)(DMA_BASE+0x01C))
#define DMA_CTL0_H_REG               (DMA_BASE+0x01C)

#define DMA_SSTAT0_L                 *((volatile int *)(DMA_BASE+0x020))
#define DMA_SSTAT0_H                 *((volatile int *)(DMA_BASE+0x024))
#define DMA_DSTAT0_L                 *((volatile int *)(DMA_BASE+0x028))
#define DMA_DSTAT0_H                 *((volatile int *)(DMA_BASE+0x02C))

#define DMA_SSTATAR0_L               *((volatile int *)(DMA_BASE+0x030))
#define DMA_SSTATAR0_H               *((volatile int *)(DMA_BASE+0x034))
#define DMA_DSTATAR0_L               *((volatile int *)(DMA_BASE+0x038))
#define DMA_DSTATAR0_H               *((volatile int *)(DMA_BASE+0x03C))

#define DMA_CFG0_L                   *((volatile int *)(DMA_BASE+0x040))
#define DMA_CFG0_L_REG               (DMA_BASE+0x040)
#define DMA_CFG0_H                   *((volatile int *)(DMA_BASE+0x044))
#define DMA_CFG0_H_REG               (DMA_BASE+0x044)
#define DMA_SGR0_L                   *((volatile int *)(DMA_BASE+0x048))
#define DMA_SGR0_H                   *((volatile int *)(DMA_BASE+0x04C))
#define DMA_DSR0_L                   *((volatile int *)(DMA_BASE+0x050))
#define DMA_DSR0_H                   *((volatile int *)(DMA_BASE+0x054))

// ch1
#define DMA_SAR1_L                   *((volatile int *)(DMA_BASE+0x058))
#define DMA_SAR1_H                   *((volatile int *)(DMA_BASE+0x05C))
#define DMA_DAR1_L                   *((volatile int *)(DMA_BASE+0x060))
#define DMA_DAR1_H                   *((volatile int *)(DMA_BASE+0x064))

#define DMA_LLP1_L                   *((volatile int *)(DMA_BASE+0x068))
#define DMA_LLP1_H                   *((volatile int *)(DMA_BASE+0x06C))
#define DMA_CTL1_L                   *((volatile int *)(DMA_BASE+0x070))
#define DMA_CTL1_L_REG               (DMA_BASE+0x070)
#define DMA_CTL1_H                   *((volatile int *)(DMA_BASE+0x074))

#define DMA_SSTAT1_L                 *((volatile int *)(DMA_BASE+0x078))
#define DMA_SSTAT1_H                 *((volatile int *)(DMA_BASE+0x07C))
#define DMA_DSTAT1_L                 *((volatile int *)(DMA_BASE+0x080))
#define DMA_DSTAT1_H                 *((volatile int *)(DMA_BASE+0x084))

#define DMA_SSTATAR1_L               *((volatile int *)(DMA_BASE+0x088))
#define DMA_SSTATAR1_H               *((volatile int *)(DMA_BASE+0x08C))
#define DMA_DSTATAR1_L               *((volatile int *)(DMA_BASE+0x090))
#define DMA_DSTATAR1_H               *((volatile int *)(DMA_BASE+0x094))

#define DMA_CFG1_L                   *((volatile int *)(DMA_BASE+0x098))
#define DMA_CFG1_L_REG               (DMA_BASE+0x098)
#define DMA_CFG1_H                   *((volatile int *)(DMA_BASE+0x09C))
#define DMA_CFG1_H_REG               (DMA_BASE+0x09C)
#define DMA_SGR1_L                   *((volatile int *)(DMA_BASE+0x0A0))
#define DMA_SGR1_H                   *((volatile int *)(DMA_BASE+0x0A4))
#define DMA_DSR1_L                   *((volatile int *)(DMA_BASE+0x0A8))
#define DMA_DSR1_H                   *((volatile int *)(DMA_BASE+0x0AC))

#define DMACFGREG_L                  *((volatile int *)(DMA_BASE+0x398))
#define DMACFGREG_H                  *((volatile int *)(DMA_BASE+0x39C))
#define CHENREG_L                    *((volatile int *)(DMA_BASE+0x3A0))
#define CHENREG_L_REG                (DMA_BASE+0x3A0)
#define CHENREG_H                    *((volatile int *)(DMA_BASE+0x3A4))
#define DMA_COMP_PARAMS_3_L          *((volatile int *)(DMA_BASE+0x3E0))
#define DMA_COMP_PARAMS_3_H          *((volatile int *)(DMA_BASE+0x3E4))
#define DMA_COMP_PARAMS_2_L          *((volatile int *)(DMA_BASE+0x3E8))
#define DMA_COMP_PARAMS_2_H          *((volatile int *)(DMA_BASE+0x3EC))
#define DMA_COMP_PARAMS_1_L          *((volatile int *)(DMA_BASE+0x3F0))
#define DMA_COMP_PARAMS_1_H          *((volatile int *)(DMA_BASE+0x3F4))

//DMA interrupt registers
#define DMA_RAW_TFR_L_REG        (DMA_BASE+0x2C0)
#define DMA_RAW_TFR_H_REG        (DMA_BASE+0x2C4)
#define DMA_RAW_BLOCK_L_REG      (DMA_BASE+0x2C8)
#define DMA_RAW_BLOCK_H_REG      (DMA_BASE+0x2CC)

#define DMA_STATUS_TFR_L_REG     (DMA_BASE+0x2E8)
#define DMA_STATUS_TFR_H_REG     (DMA_BASE+0x2EC)
#define DMA_STATUS_BLOCK_L_REG   (DMA_BASE+0x2F0)
#define DMA_STATUS_BLOCK_H_REG   (DMA_BASE+0x2F4)

#define DMA_MASK_TFR_L_REG       (DMA_BASE+0x310)
#define DMA_MASK_TFR_H_REG       (DMA_BASE+0x314)
#define DMA_MASK_BLOCK_L_REG     (DMA_BASE+0x318)
#define DMA_MASK_BLOCK_H_REG     (DMA_BASE+0x31C)

#define DMA_CLEAR_TFR_L_REG      (DMA_BASE+0x338)
#define DMA_CLEAR_TFR_H_REG      (DMA_BASE+0x33C)
#define DMA_CLEAR_BLOCK_L_REG    (DMA_BASE+0x340)
#define DMA_CLEAR_BLOCK_H_REG    (DMA_BASE+0x344)

#define DMA_STATUS_INT_REG       (DMA_BASE+0x360)

#define LEGA_DMA_NUM 2

typedef void (*lega_dma_callback_func)(char);


/*
 * DMA mode
 */
typedef enum {
    M2M_MODE,
    M2P_MODE,
    P2M_MODE,
    P2P_MODE
} lega_dma_mode_t;


/*
 * DMA HW handshake
 */
typedef enum {
    UART0_TX_HS,
    UART0_RX_HS,
    UART1_TX_HS,
    UART1_RX_HS,
    UART2_TX_HS,
    UART2_RX_HS,
    SPI0_TX_HS,
    SPI0_RX_HS,
    SPI1_TX_HS,
    SPI1_RX_HS,
    SPI2_TX_HS,
    SPI2_RX_HS,
    I2C0_TX_HS,
    I2C0_RX_HS,
    I2C1_TX_HS,
    I2C1_RX_HS,
} lega_dma_hs_t;

typedef struct{
    uint32_t *   DMA_SAR_L;
    uint32_t *   DMA_DAR_L;
    uint32_t *   DMA_LLP_L;
    uint32_t *   DMA_LLP_H;
    uint32_t *   DMA_CTL_H;
    uint32_t *   DMA_CFG_L;
    uint32_t     DMA_CTL_L_REG;
    uint32_t     DMA_CFG_H_REG;
} lega_dma_config_reg_t;

/*
 * DMA configuration
 */
typedef struct {
    uint32_t     src;    //src
    uint32_t     dest;    //dst
    uint8_t      ch;     //channel
    lega_dma_mode_t      mode;    //mode
    void         *priv;    /* priv data */
} lega_dma_dev_t;

/**
 * Initialises a dma interface
 *
 *
 * @param[in]  dma  the interface which should be initialised
 *
 * @return
 */
void lega_dma_init(lega_dma_dev_t *dma);

/**
 * Transmit data on a dma interface
 *
 * @param[in]  dma  the dma interface
 * @param[in]  size  number of bytes to transmit
 *
 * @return
 */
void lega_dma_send(lega_dma_dev_t *dma,/*const void *data,*/ uint32_t size, uint32_t timeout);
/**
 * Deinitialises a dma interface
 *
 * @param[in]  DMA  the interface which should be deinitialised
 *
 * @return
 */
void lega_dma_finalize(lega_dma_dev_t *dma);

#endif // _LEGA_DMA_H_

