#include <stdio.h>
#include <time.h>
#include "lega_cm4.h"
#include "lega_common.h"
#include "lega_spi.h"
lega_spi_trans_mode_t spi_trans_mode = SPI_TRANS_PIO_MODE;
//lega_spi_trans_mode_t spi_trans_mode = SPI_TRANS_DMA_MODE;

//default settings for different spi ports
lega_spi_controller_t spi_cntls[3] = {
        {
            .name = "spi0",//master only
            .left_tx_size = 0,
            .left_rx_size = 0,
            .pvtx = NULL,
            .pvrx = NULL,
            .tx_func = tx_8bit,
            .rx_func = rx_8bit,
            .bits = 8,
            .lsb = 0,//only msb
            .is_loopback = 0,
        },
        {
            .name = "spi1",//master only
            .left_tx_size = 0,
            .left_rx_size = 0,
            .pvtx = NULL,
            .pvrx = NULL,
            .tx_func = tx_8bit,
            .rx_func = rx_8bit,
            .bits = 8,
            .lsb = 0,//only msb
            .is_loopback = 0,
        },
        {
            .name = "spi2",//slave only
            .left_tx_size = 0,
            .left_rx_size = 0,
            .pvtx = NULL,
            .pvrx = NULL,
            .tx_func = tx_8bit,
            .rx_func = rx_8bit,
            .bits = 8,
            .lsb = 0,//only msb
            .is_loopback = 0,
        },
};

void tx_8bit(int port)
{
    uint8_t *tx_data;

    tx_data = (uint8_t *)spi_cntls[port].pvtx;
    if (tx_data) {
        REG_WR(SPIx_DR(port, 0), *tx_data++);
        spi_cntls[port].pvtx = tx_data;
    }
    spi_cntls[port].left_tx_size--;
}

void rx_8bit(int port)
{
    uint8_t *rx_data;

    rx_data = (uint8_t *)spi_cntls[port].pvrx;
    if (rx_data) {
        *rx_data++ = REG_RD(SPIx_DR(port, 0));
        spi_cntls[port].pvrx = rx_data;
    }
    spi_cntls[port].left_rx_size--;
}

void tx_16bit(int port)
{
    uint16_t *tx_data;

    tx_data = (uint16_t *)spi_cntls[port].pvtx;
    if (tx_data) {
        REG_WR(SPIx_DR(port, 0), *tx_data++);
        spi_cntls[port].pvtx = tx_data;
    }
    spi_cntls[port].left_tx_size--;
}

void rx_16bit(int port)
{
    uint16_t *rx_data;

    rx_data = (uint16_t *)spi_cntls[port].pvrx;
    if (rx_data) {
        *rx_data++ = REG_RD(SPIx_DR(port, 0));
        spi_cntls[port].pvrx = rx_data;
    }
    spi_cntls[port].left_rx_size--;
}

void tx_32bit(int port)
{
    uint32_t *tx_data;

    tx_data = (uint32_t *)spi_cntls[port].pvtx;
    if (tx_data) {
        REG_WR(SPIx_DR(port, 0), *tx_data++);
        spi_cntls[port].pvtx = tx_data;
    }
    spi_cntls[port].left_tx_size--;
}

void rx_32bit(int port)
{
    uint32_t *rx_data;

    rx_data = (uint32_t *)spi_cntls[port].pvrx;
    if (rx_data) {
        *rx_data++ = REG_RD(SPIx_DR(port, 0));
        spi_cntls[port].pvrx = rx_data;
    }
    spi_cntls[port].left_rx_size--;
}

/*
    spi_enable: enable/disable spi controller
    @port:    spi port index
    @enable: 1 -- enable spi controller
             0 -- disable spi controller
*/
int spi_enable(int port, int enable)
{
    if (port < 0 || port > 2)
        return -1;
    if (enable)
        REG_WR(SPIx_SSIEN(port), SPI_EN);
    else
        REG_WR(SPIx_SSIEN(port), SPI_DISABLE);
    return 0;
}

/*
    spi_pio_trans_data using PIO mode to transfer and receive data
*/
int spi_pio_trans_data(int port)
{
    REG_WR(SPIx_IM(port), REG_RD(SPIx_IM(port)) | SPI_IS_TX_EMPTY | SPI_IS_RX_FULL);
    spi_enable(port, 1);
    while (!(!(REG_RD(SPIx_ST(port)) & SPI_ST_BUSY) && spi_cntls[port].left_tx_size == 0 && spi_cntls[port].left_rx_size == 0))
        ;

    return 1;
}

int is_tx_dma_done = 0;
int is_rx_dma_done = 0;
void DMA_CTRL_IRQHandler(void)
{
    if (DMA_STATUSBLOCK & 0x1) {
        DMA_CLEARBLOCK = 0x1;
        is_tx_dma_done = 1;
    }
    if (DMA_STATUSBLOCK & 0x2) {
        DMA_CLEARBLOCK = 0x2;
        is_rx_dma_done = 1;
    }
}
/**
*    spi_dma_trans_data using DMA mode to transfer and receive data
*
*   @param[in]  spi port index
*/
int spi_dma_trans_data(int port)
{
    int dma_chan_en_reg;
    REG_WR(SPIx_IM(port), 0x0);
    is_tx_dma_done = is_rx_dma_done = 0;
    dma_chan_en_reg = CHENREG_L;
    spi_enable(port, 1);
    //check if there is DMA channel resource
    if (dma_chan_en_reg)
        return -1;
    //spi slave
    if (port == 2) {
        spi_cntls[port].tx_func(port);
        spi_cntls[port].tx_func(port);
        spi_cntls[port].tx_func(port);
        spi_cntls[port].tx_func(port);
    }

    DMA_SAR0_L = (int)(spi_cntls[port].pvtx);//spi tx data<--from mem
    DMA_DAR0_L = SPIx_DR(port, 0);
    DMA_SAR1_L = SPIx_DR(port, 0);//spi rx data-->mem
    DMA_DAR1_L = (int)(spi_cntls[port].pvrx);

    DMA_LLP0_L = 0x00;
    DMA_LLP0_H = 0x00;
    DMA_LLP1_L = 0x00;
    DMA_LLP1_H = 0x00;

    switch (spi_cntls[port].bits) {
        case 8:
            DMA_CTL0_L = 0x02104121;
            DMA_CTL0_H = spi_cntls[port].left_tx_size / 4;
            DMA_CTL1_L = 0x00A01405;
            DMA_CTL1_H = spi_cntls[port].left_rx_size;
            break;
        case 16:
            DMA_CTL0_L = 0x02104123;
            DMA_CTL0_H = spi_cntls[port].left_tx_size * 2 / 4;
            DMA_CTL1_L = 0x00A01415;
            DMA_CTL1_H = spi_cntls[port].left_rx_size;
            break;
        case 32:
            DMA_CTL0_L = 0x02104125;
            DMA_CTL0_H = spi_cntls[port].left_tx_size;
            DMA_CTL1_L = 0x00A01425;
            DMA_CTL1_H = spi_cntls[port].left_rx_size;
            break;
    }

    if (port == 0) {
        DMA_CFG0_L = 0x00000000; //31, reload DST, 19,18, interface polarity active high, 11=1 src software,10=0 dest hardware,
        DMA_CFG0_H = 0x00003302; //[11],dest ch0 [7],src ch0 [1],enable fifo mode
        DMA_CFG1_L = 0x00000000; //30, reload SRC, 19,18, interface polarity active high, 11=1 src software,10=0 dest hardware,
        DMA_CFG1_H = 0x00003B82; //[11],dest ch1 [7],src ch1 [1],enable fifo mode
    } else if (port == 1) {
        DMA_CFG0_L = 0x00000000;
        DMA_CFG0_H = 0x00004402;
        DMA_CFG1_L = 0x00000000;
        DMA_CFG1_H = 0x00004C82;
    } else if (port == 2) {
        DMA_CFG0_L = 0x00000000; //31, reload DST, 19,18, interface polarity active high, 11=1 src software,10=0 dest hardware,
        DMA_CFG0_H = 0x00005502; //[14:11]=10,dest ch0.   [10:7]=10,src ch0.  enable fifo mode
        DMA_CFG1_L = 0x00000000; //30, reload SRC, 19,18, interface polarity active high, 11=1 src software,10=0 dest hardware,
        DMA_CFG1_H = 0x00005D82; //[14:11]=11,dest ch0.   [10:7]=11,src ch0.  enable fifo mode
    }
    DMA_MASKBLOCK = 0x0303;
    DMACFGREG_L = 0x1;
    CHENREG_L = 0x0303;

    //wait until tx/rx done
    while ((is_tx_dma_done == 0 || is_rx_dma_done == 0))
        ;
    spi_cntls[port].left_tx_size = 0;
    spi_cntls[port].left_rx_size = 0;
    CHENREG_L = 0x0000;
    return 1;
}
/**
 * SPI interrupt handler
 *
 * @param[in]  spi port index
 */

void spi_isr(int port)
{
    uint32_t int_st, st, tx_level;

    int_st = REG_RD(SPIx_IS(port));
    if (int_st & (SPI_IS_RX_OF | SPI_IS_RX_UF | SPI_IS_TX_OF)) {
        REG_RD(SPIx_RXOIC(port));
        REG_RD(SPIx_RXUIC(port));
        REG_RD(SPIx_TXOIC(port));
        return;
    }
    if (int_st & SPI_IS_TX_EMPTY) {
        tx_level = REG_RD(SPIx_TXFL(port));
        //optimization here for transfer multi bytes continuesly: change from if -> while
        //spi2 as slave, spi0/spi1 as master
        if (port == 2) {
            while (spi_cntls[port].left_tx_size > 0 && ((tx_level & SPI_TXFL_MASK) != SPI_TXFL_MASK)) {
                spi_cntls[port].tx_func(port);
                tx_level = REG_RD(SPIx_TXFL(port));
            }
        } else {
            if (spi_cntls[port].left_tx_size > 0 && ((tx_level & SPI_TXFL_MASK) != SPI_TXFL_MASK)) {
                spi_cntls[port].tx_func(port);
                tx_level = REG_RD(SPIx_TXFL(port));
            }
        }
        if (0 == spi_cntls[port].left_tx_size)
            REG_WR(SPIx_IM(port), REG_RD(SPIx_IM(port)) & ~(SPI_IS_TX_EMPTY) );
    }
    int_st = REG_RD(SPIx_IS(port));
    if (int_st & SPI_IS_RX_FULL) {//rx full
        // receive at rx fifo
        st = REG_RD(SPIx_ST(port));
        while (spi_cntls[port].left_rx_size > 0 && (st & SPI_ST_RFNE)) {
            spi_cntls[port].rx_func(port);
            st = REG_RD(SPIx_ST(port));
        }
        if (0 == spi_cntls[port].left_rx_size)
            REG_WR(SPIx_IM(port), REG_RD(SPIx_IM(port)) & ~(SPI_ST_RFF) );
    }
    return;
}

void SPI0_IRQHandler(void)
{
    spi_isr(0);
}

void SPI1_IRQHandler(void)
{
    spi_isr(1);
}

void SPI2_IRQHandler(void)
{
    spi_isr(2);
}
/**
 * Initialises the SPI interface for a given SPI device
 *
 * @param[in]  spi  the spi device
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t lega_spi_init(lega_spi_dev_t *spi)
{
    uint8_t cpol, cpha;


    if (spi->port > 2 || spi->port < 0)
        return -1;

    REG_WR(0x40000830, REG_RD(0x40000830) | 0x7);

    switch (spi->port) {
    case 0:
        #if defined(LEGA_A0V1)
        REG_WR(0x40000038, REG_RD(0x40000038) | 0x11000000);
        REG_WR(0x4000003c, REG_RD(0x4000003c) | 0x00000011);
        #else
        REG_WR(0x40000004, REG_RD(0x40000004) | 0x11000000);
        REG_WR(0x40000008, REG_RD(0x40000008) | 0x00000011);
        #endif
        NVIC_SetPriority(SPI0_IRQn, 5);
        NVIC_EnableIRQ(SPI0_IRQn);
        break;
    case 1:
        #if defined(LEGA_A0V1)
        REG_WR(0x40000038, REG_RD(0x40000038) | 0x00003333);
        #else
        REG_WR(0x40000004, REG_RD(0x40000004) | 0x00003333);
        #endif
        NVIC_SetPriority(SPI1_IRQn, 2);
        NVIC_EnableIRQ(SPI1_IRQn);
        break;
    case 2:
        #if defined(LEGA_A0V1)
        REG_WR(0x4000003c, REG_RD(0x4000003c) | 0x22220000);
        #else
        REG_WR(0x40000008, REG_RD(0x40000008) | 0x22220000);
        #endif
        NVIC_SetPriority(SPI2_IRQn, 5);
        NVIC_EnableIRQ(SPI2_IRQn);
        break;
    default:
        break;
    }
    spi_enable(spi->port, 0);
    switch (spi->config.mode) {
        case 0:
            cpol = 0;
            cpha = 0;
            break;
        case 1:
            cpol = 0;
            cpha = 1;
            break;
        case 2:
            cpol = 1;
            cpha = 0;
            break;
        case 3:
            cpol = 1;
            cpha = 1;
            break;
        default:
            cpol = 0;
            cpha = 0;
            break;
    }
    switch (spi_cntls[spi->port].bits) {
        case 8:
            spi_cntls[spi->port].tx_func = tx_8bit;
            spi_cntls[spi->port].rx_func = rx_8bit;
            break;
        case 16:
            spi_cntls[spi->port].tx_func = tx_16bit;
            spi_cntls[spi->port].rx_func = rx_16bit;
            break;
        case 32:
            spi_cntls[spi->port].tx_func = tx_32bit;
            spi_cntls[spi->port].rx_func = rx_32bit;
            break;
    }
    REG_WR(SPIx_CTRL0(spi->port), ((spi_cntls[spi->port].bits - 1) << SPI_DFS32_SHIFT) | (0 << SPI_TMOD_SHIFT) | (spi_cntls[spi->port].is_loopback << SPI_SRL_SHIFT ) | (cpha << SPI_CPHA_SHIFT) | (cpol << SPI_CPOL_SHIFT) | (0 << SPI_FR_SHIFT));

    REG_WR(SPIx_TXFTL(spi->port), 0x0 & SPI_TFT_MASK);//32/4
    REG_WR(SPIx_RXFTL(spi->port), 0x0 & SPI_RFT_MASK);//32/4

    REG_WR(SPIx_BAUD(spi->port), (SYSTEM_CLOCK/spi->config.freq) & SPI_BAUD_MASK);
    if (spi->port != 2)
        REG_WR(SPIx_SE(spi->port), 0x1);
    REG_WR(SPIx_IM(spi->port), 0x0);
    spi->priv = (void *)&spi_cntls[spi->port];
    spi_enable(spi->port, 0);
    if (spi_trans_mode == 1) {
        REG_WR(SPIx_DMAC(spi->port), 0x3);
        //ensure using dma the transfer size is 4x size
        REG_WR(SPIx_DMATDL(spi->port), 0x0);
        REG_WR(SPIx_DMARDL(spi->port), 0x0);
        REG_WR(0x40000944, REG_RD(0x40000944) | 0x80);
        NVIC_SetPriority(DMA_CTRL_IRQn, 2);
        NVIC_EnableIRQ(DMA_CTRL_IRQn);
    } else {
        REG_WR(SPIx_DMAC(spi->port), 0x0);
        REG_WR(SPIx_DMATDL(spi->port), 0x0);
        REG_WR(SPIx_DMARDL(spi->port), 0x0);
    }
    return 0;
}

/**
 * Spi send
 *
 * @param[in]  spi      the spi device
 * @param[in]  data     spi send data
 * @param[in]  size     spi send data size
 * @param[in]  timeout  timeout in ms
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t lega_spi_send(lega_spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret;
    lega_spi_controller_t *spi_cntl;

    spi_cntl = (lega_spi_controller_t *)spi->priv;
    spi_cntl->left_tx_size = size;
    spi_cntl->left_rx_size = size;
    spi_cntl->pvtx = (void *)data;
    spi_cntl->pvrx = NULL;

    if (spi_trans_mode == SPI_TRANS_PIO_MODE)//pio way
        ret = spi_pio_trans_data(spi->port);
    else//dma way
        ret = spi_dma_trans_data(spi->port);
    return ret;
}
/**
 * spi_recv
 *
 * @param[in]   spi      the spi device
 * @param[out]  data     spi recv data
 * @param[in]   size     spi recv data size
 * @param[in]   timeout  timeout in ms
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t lega_spi_recv(lega_spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret;
    lega_spi_controller_t *spi_cntl;

    spi_cntl = spi->priv;
    spi_cntl->left_tx_size = size;
    spi_cntl->left_rx_size = size;
    spi_cntl->pvtx = NULL;
    spi_cntl->pvrx = data;
    if (spi_trans_mode == SPI_TRANS_PIO_MODE)//pio way
        ret = spi_pio_trans_data(spi->port);
    else//dma way
        ret = spi_dma_trans_data(spi->port);
    return ret;
}
/**
 * spi send data and recv
 *
 * @param[in]  spi      the spi device
 * @param[in]  tx_data  spi send data
 * @param[in]  rx_data  spi recv data
 * @param[in]  size  spi data to be sent and recived
 * @param[in]  timeout  timeout in ms
 *
 * @return  0, on success;  EIO : if the SPI device could not be initialised
 */
int32_t lega_spi_send_recv(lega_spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                          uint16_t size, uint32_t timeout)
{
    int32_t ret;
    lega_spi_controller_t *spi_cntl;

    spi_cntl = spi->priv;
    spi_cntl->left_tx_size = size;
    spi_cntl->left_rx_size = size;
    spi_cntl->pvtx = tx_data;
    spi_cntl->pvrx = rx_data;

    if (spi_trans_mode == SPI_TRANS_PIO_MODE)//pio way
        ret = spi_pio_trans_data(spi->port);
    else//dma way
        ret = spi_dma_trans_data(spi->port);
    return ret;
}

/**
 * De-initialises a SPI interface
 *
 *
 * @param[in]  spi the SPI device to be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred
 */
int32_t lega_spi_finalize(lega_spi_dev_t *spi)
{
    int port;

    port = spi->port;
    if (port < 0 || port > 2)
        return -1;
    REG_WR(SPIx_IM(port), ~SPI_IS_MSK);
    spi_enable(port, 0);
    switch (port) {
        case 0:
            NVIC_DisableIRQ(SPI0_IRQn);
            break;
        case 1:
            NVIC_DisableIRQ(SPI1_IRQn);
            break;
        case 2:
            NVIC_DisableIRQ(SPI2_IRQn);
            break;
        default:
            break;
    }
    return 0;
}
