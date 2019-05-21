#ifndef _SDMA_H_
#define _SDMA_H_

//#define SDMA_DEBUG

#ifdef SDMA_DEBUG
    #define SDMA_PRT           os_printf
#else
    #define SDMA_PRT           os_null_printf
    #define SDMA_WPRT          warning_prf
#endif

#define SDMA_DIR_IDLE          (0)
#define SDMA_DIR_TX            (1)
#define SDMA_DIR_RX            (2)

#define SDIO_DMA_BASE        (0x00801000)

#define REG_SDMA_RESERVE     (SDIO_DMA_BASE + 00*4)

#define REG_SDMA_CONFIG      (SDIO_DMA_BASE + 01*4)
#define SDMA_CONFIG_DMA_EN        (1 << 0)
#define SDMA_CONFIG_ADDR_READY    (1 << 1)
#define SDMA_CONFIG_INTR_EN_TX    (1 << 5)
#define SDMA_CONFIG_INTR_EN_RX    (1 << 6)
#define SDMA_CONFIG_INTR_EN_CMD   (1 << 7)

#define REG_SDMA_ADDR        (SDIO_DMA_BASE + 02*4)

#define REG_SDMA_COUNT       (SDIO_DMA_BASE + 03*4)
#define SDMA_COUNT_MASK      (0xFFFF)

#define REG_SDMA_CMD_BA      (SDIO_DMA_BASE + 04*4)

#define REG_SDMA_CMD_BUF0    (SDIO_DMA_BASE + 04*4)
#define REG_SDMA_CMD_BUF1    (SDIO_DMA_BASE + 05*4)
#define REG_SDMA_CMD_BUF2    (SDIO_DMA_BASE + 06*4)
#define REG_SDMA_CMD_BUF3    (SDIO_DMA_BASE + 07*4)
#define REG_SDMA_CMD_BUF4    (SDIO_DMA_BASE + 08*4)
#define REG_SDMA_CMD_BUF5    (SDIO_DMA_BASE + 09*4)
#define REG_SDMA_CMD_BUF6    (SDIO_DMA_BASE + 10*4)
#define REG_SDMA_CMD_BUF7    (SDIO_DMA_BASE + 11*4)
#define REG_SDMA_CMD_BUF8    (SDIO_DMA_BASE + 12*4)
#define REG_SDMA_CMD_BUF9    (SDIO_DMA_BASE + 13*4)
#define REG_SDMA_CMD_BUF10   (SDIO_DMA_BASE + 14*4)
#define REG_SDMA_CMD_BUF11   (SDIO_DMA_BASE + 15*4)
#define REG_SDMA_CMD_BUF12   (SDIO_DMA_BASE + 16*4)
#define REG_SDMA_CMD_BUF13   (SDIO_DMA_BASE + 17*4)
#define REG_SDMA_CMD_BUF14   (SDIO_DMA_BASE + 18*4)
#define REG_SDMA_CMD_BUF15   (SDIO_DMA_BASE + 19*4)

#define REG_SDMA_INTR_STATUS   (SDIO_DMA_BASE + 20*4)
#define SDMA_INTR_TX_DATA_BIT      (1 << 0)
#define SDMA_INTR_RX_DATA_BIT      (1 << 1)
#define SDMA_INTR_CMD_BIT          (1 << 2)

#define REG_SDMA_INTERACTIVE_HOST   (SDIO_DMA_BASE + 21*4)
#define INTERACTIVE_BLK_LEN_POSI                (24)
#define INTERACTIVE_BLK_LEN_MASK                (0x7)
#define INTERACTIVE_RX_VALID_BIT                (1 << 23)
#define INTERACTIVE_RX_COUNT_POSI               (0)
#define INTERACTIVE_RX_COUNT_MASK               (0xFFFFF)

/*sdm driver macro*/
#define CMD_BUF_MAX_LEN        (64)

#define SDMA_BUSY              ((UINT32)-2)
#define SDMA_FAILURE           ((UINT32)-1)
#define SDMA_SUCCESS           (0

#endif // _SDMA_H_
