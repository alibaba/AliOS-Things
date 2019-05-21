#ifndef __SPI_DMA_H__
#define __SPI_DMA_H__

//#define SPI_DMA_DEBUG
#ifdef SPI_DMA_DEBUG
#define SPI_DMA_PRT                 os_printf
#else
#define SPI_DMA_PRT                 null_prf
#define SPI_DMA_WPRT                null_prf
#endif

#define SPI_DMA_BASE                    (0x00808000)

#define SPI_DMA_REG0                    (SPI_DMA_BASE + 0x00*4)
#define SPIDMA_REG0_SPI_EN                (1 << 0)
#define SPIDMA_REG0_SCK_INV               (1 << 1)
#define SPIDMA_REG0_LSB_FIRST             (1 << 2)
#define SPIDMA_REG0_WIRE3_EN              (1 << 3)
#if (CFG_SOC_NAME == SOC_BK7231)
#define SPIDMA_REG0_DMA_EN                (1 << 4)
#define SPIDMA_REG0_TIMEOUT_INTEN         (1 << 5)
#define SPIDMA_REG0_RXBUF_THRED_INTEN     (1 << 6)
#define SPIDMA_REG0_TX_FINISH_INTEN       (1 << 7)
#define SPIDMA_REG0_RXEN                  (1 << 8)
#else
#define SPIDMA_REG0_TIMEOUT_INTEN         (1 << 4)
#define SPIDMA_REG0_TX_FINISH_INTEN       (1 << 5)
#define SPIDMA_REG0_RXEN                  (1 << 6)
#endif // (CFG_SOC_NAME == SOC_BK7231)

#define SPI_DMA_REG1                    (SPI_DMA_BASE + 0x01*4)
#define SPIDMA_REG1_TIMEOUT_MASK          (0xffffffU)

#if (CFG_SOC_NAME == SOC_BK7231)
#define SPI_DMA_REG2_RXBUF_ADDR         (SPI_DMA_BASE + 0x02*4)

#define SPI_DMA_REG3_RXBUF_LEN          (SPI_DMA_BASE + 0x03*4)
#define SPIDMA_REG3_RXBUF_LEN_MASK        (0x7U)
#define SPIDMA_RXBUFLEN_1024B             (0x0U)
#define SPIDMA_RXBUFLEN_2048B             (0x1U)
#define SPIDMA_RXBUFLEN_4096B             (0x2U)
#define SPIDMA_RXBUFLEN_8192B             (0x3U)
#define SPIDMA_RXBUFLEN_16384B            (0x4U)
#define SPIDMA_RXBUFLEN_32768B            (0x5U)
#define SPIDMA_RXBUFLEN_65536B            (0x6U)

#define SPI_DMA_REG4_RXBUF_THRE         (SPI_DMA_BASE + 0x04*4)
#define SPIDMA_REG4_RXBUF_THRE_MASK       (0xffffU)

#define SPI_DMA_REG5_RXBUF_RDPTR        (SPI_DMA_BASE + 0x05*4)
#define SPIDMA_REG5_RXBUF_RDPTR_MASK       (0xffffU)

#define SPI_DMA_REG6_RXBUF_WRPTR        (SPI_DMA_BASE + 0x06*4)
#define SPIDMA_REG6_RXBUF_WRPTR_MASK       (0xffffU)

#define SPI_DMA_REG7_TXBUF_ADDR         (SPI_DMA_BASE + 0x07*4)

#define SPI_DMA_REG8_TXBUF_LEN          (SPI_DMA_BASE + 0x08*4)

#define SPI_DMA_REG9_TXBUF_VALID        (SPI_DMA_BASE + 0x09*4)

#define SPI_DMA_REG10_INT_STATUS        (SPI_DMA_BASE + 0x0a*4)
#define SPIDMA_INTSTA_RXBUF_THRE          (1 << 0)
#define SPIDMA_INTSTA_RX_TIMEOUT          (1 << 1)
#define SPIDMA_INTSTA_TXFINISH            (1 << 2)

#define SPI_DMA_REG11                   (SPI_DMA_BASE + 0x0b*4)
#define SPIDMA_REG11_RXBUF_VLDNUM_POSI    (0)
#define SPIDMA_REG11_RXBUF_VLDNUM_MASK    (0xffffU)
#define SPIDMA_REG11_TIMING_ERR_STATUS    (1 << 16)
#define SPIDMA_REG11_RXFIFO_OVERFLOW      (1 << 17)
#define SPIDMA_REG11_RXFIFO_EMPTY         (1 << 18)
#define SPIDMA_REG11_RXFIFO_FULL          (1 << 19)
#define SPIDMA_REG11_RXBUF_FULL           (1 << 20)
#define SPIDMA_REG11_RXBUF_EMPTY          (1 << 21)
#else
#define SPI_DMA_REG3_RXFIFO             (SPI_DMA_BASE + 0x03*4)
#define SPIDMA_REG3_RXFIFO_MASK           (0xffffffffU)

#define SPI_DMA_REG4_TXFIFO             (SPI_DMA_BASE + 0x04*4)
#define SPIDMA_REG4_TXFIFO_MASK           (0xffffffffU)

#define SPI_DMA_REG5_STA                (SPI_DMA_BASE + 0x05*4)
#define SPIDMA_INTSTA_RX_TIMEOUT          (1 << 0)
#define SPIDMA_INTSTA_TIMING_ERR          (1 << 1)
#define SPIDMA_STA_RXFIFO_OVERFLOW        (1 << 2)
#define SPIDMA_STA_TXFIFO_OVERFLOW        (1 << 3)
#define SPIDMA_STA_RXFIFO_EMPTY           (1 << 4)
#define SPIDMA_STA_RXFIFO_FULL            (1 << 5)
#define SPIDMA_STA_TXFIFO_EMPTY           (1 << 6)
#define SPIDMA_STA_TXFIFO_FULL            (1 << 7)
#endif // (CFG_SOC_NAME == SOC_BK7231)


#if CFG_USE_HSLAVE_SPI
static void spidma_isr(void);
static void spidma_rx_callback(UINT16 used_data_len);
static UINT32 spidma_open(UINT32 op_flag);
static UINT32 spidma_close(void);
static UINT32 spidma_ctrl(UINT32 cmd, void *param);
#endif

#endif // __SPI_DMA_H__

