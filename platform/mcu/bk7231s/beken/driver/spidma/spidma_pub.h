#ifndef __SPI_DMA_PUB_H__
#define __SPI_DMA_PUB_H__

#if CFG_USE_HSLAVE_SPI
#include "rtos_pub.h"

#define SPIDMA_FAILURE                (1)
#define SPIDMA_SUCCESS                (0)

#define SPIDMA_DEV_NAME                "spidma"

#define SPIDMA_CMD_MAGIC              (0x0BCD0000)

enum
{
    SPIDMA_CMD_CONF_SCK_INV = SPIDMA_CMD_MAGIC + 1,
    SPIDMA_CMD_CONF_LSB_FIRST,
    SPIDMA_CMD_CONF_3WIRE_EN,
    SPIDMA_CMD_CONF_TIMEOUT_VAL,
    SPIDMA_CMD_CONF_RXBUF_ADDR,
    SPIDMA_CMD_CONF_RXBUF_LEN,
    SPIDMA_CMD_CONF_RXDATA_THRE,
    SPIDMA_CMD_GET_RXDATA_THRE,
    SPIDMA_CMD_GET_RXBUF_RDPTR,
    SPIDMA_CMD_SET_RXBUF_RDPTR,
    SPIDMA_CMD_GET_RXBUF_WRPTR,
    SPIDMA_CMD_CONF_TXBUF_ADDR,
    SPIDMA_CMD_CONF_TXBUF_LEN,
    SPIDMA_CMD_CONF_TXBUF_VALID,
    SPIDMA_CMD_GET_RXBUF_VALID_DATALEN,
};

#define SPIDMA_DESC_SCK_MODE_MASK    (0x1)
#define SPIDMA_DESC_SCK_MODE_POSI    (0)
#define SPIDMA_DESC_LSB_FIRST_MASK   (0x1)
#define SPIDMA_DESC_LSB_FIRST_POSI   (1)
#define SPIDMA_DESC_3WIRE_MODE_MASK  (0x1)
#define SPIDMA_DESC_3WIRE_MODE_POSI  (2)
#define SPIDMA_DESC_RX_THRED_MASK    (0xffffU)
#define SPIDMA_DESC_RX_THRED_POSI    (3)
#define SPIDMA_DESC_CALLBK_RD_NEWP_MASK    (0xffffU)
#define SPIDMA_DESC_CALLBK_CLR_RXTHRE_POSI   (16)
#define SPIDMA_DESC_CALLBK_CLR_RXTIO_POSI    (17)
#define SPIDMA_DESC_CALLBK_CLR_TXFIN_POSI    (18)

#if (CFG_SOC_NAME == SOC_BK7231)
typedef struct spidma_desc
{
    UINT8 *rxbuf;
    UINT8 *txbuf;
    UINT16 rxbuf_len;
    UINT16 txbuf_len;
    void (*rx_handler)(void *curptr, UINT32 newlen);
    void (*rx_timeout)(void);
    void (*tx_handler)(void);
    /* mode:     SPIDMA mode
     * bit[0]: SPIDMA sck clock invert
     *          0:  posedge sample data
     *          1:  negedge sample data
     * bit[1]: SPIDMA LSB transfer first
     *          0:  MSB transfer first
     *          1:  LSB transfer first
     * bit[2]: SPIDMA 3-wire mode
     *          0:  4-wire mode
     *          1:  3-wire mode
     * bit[3:19]: 16bit SPIDMA rxbuf threshold val
     */
    UINT32 mode;
    UINT32 timeout_val;
    UINT32 node_len;
} SPIDMA_DESC_ST, *SPIDMA_DESC_PTR;
#else
typedef struct spidma_desc
{
    UINT8 *rxbuf;
    
    void (*node_full_handler)(void *curptr, UINT32 newlen, UINT32 is_eof, UINT32 frame_len);
    void (*data_end_handler)(void);   

    UINT16 rxbuf_len;
    UINT16 rx_read_len;
    UINT32 node_len;
    

    UINT8 *txbuf;
    void (*tx_handler)(void);
   
    /* mode:     SPIDMA mode
     * bit[0]: SPIDMA sck clock invert
     *          0:  posedge sample data
     *          1:  negedge sample data
     * bit[1]: SPIDMA LSB transfer first
     *          0:  MSB transfer first
     *          1:  LSB transfer first
     * bit[2]: SPIDMA 3-wire mode
     *          0:  4-wire mode
     *          1:  3-wire mode
     * bit[3:19]: 16bit SPIDMA rxbuf threshold val
     */
    UINT32 mode;
    UINT32 timeout_val;
    UINT32 txbuf_len;
    void (*end_frame_handler)(void);
#if CFG_GENERAL_DMA
    void (*dma_rx_handler)(UINT32);
    UINT32 dma_rx_channel;
    void (*dma_tx_handler)(UINT32);
    UINT32 dma_tx_channel;
    
#endif
} SPIDMA_DESC_ST, *SPIDMA_DESC_PTR;
#endif

#define SPIDMA_DEF_RXDATA_TIMEOUT_VAL       (800)  //  Unit is Tahb_clk
#define SPIDMA_DEF_RXDATA_THRE_INT          (4096) //(1024)

void spidma_init(void);
void spidma_uninit(void);

#endif // CFG_USE_HSLAVE_SPI

#endif

