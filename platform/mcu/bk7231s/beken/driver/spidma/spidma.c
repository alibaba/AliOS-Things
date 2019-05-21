#include "include.h"
#include "arm_arch.h"

#if CFG_USE_HSLAVE_SPI

#include "uart_pub.h"
#include "spidma_pub.h"
#include "spidma.h"

#include "mem_pub.h"
#include "intc_pub.h"

#include "icu_pub.h"
#include "gpio_pub.h"

#if CFG_GENERAL_DMA
#include "general_dma_pub.h"
#endif

#define USE_SPI_DMA_GPIO_14_17      (0)
#define USE_SPI_DMA_GPIO_30_33      (1)
#define USE_SPI_DMA_GPIO_NUM        USE_SPI_DMA_GPIO_14_17


SPIDMA_DESC_PTR p_spidma_desc;
static DD_OPERATIONS spidma_op =
{
    spidma_open,
    spidma_close,
    NULL,
    NULL,
    spidma_ctrl
};

static void spidma_config_sck_invert(UINT32 negedge)
{
    UINT32 reg;
    reg = REG_READ(SPI_DMA_REG0);
    if(negedge)
        reg |= SPIDMA_REG0_SCK_INV;
    else
        reg &= ~(SPIDMA_REG0_SCK_INV);
    REG_WRITE(SPI_DMA_REG0, reg);
}

static void spidma_config_LSB_first_transfer(UINT32 ture)
{
    UINT32 reg;
    reg = REG_READ(SPI_DMA_REG0);
    if(ture)
        reg |= SPIDMA_REG0_LSB_FIRST;
    else
        reg &= ~(SPIDMA_REG0_LSB_FIRST);
    REG_WRITE(SPI_DMA_REG0, reg);
}

static void spidma_config_3wire_mode(UINT32 ture)
{
    UINT32 reg;
    reg = REG_READ(SPI_DMA_REG0);
    if(ture)
        reg |= SPIDMA_REG0_WIRE3_EN;
    else
        reg &= ~(SPIDMA_REG0_WIRE3_EN);
    REG_WRITE(SPI_DMA_REG0, reg);
}

static void spidma_config_rxdata_timeout_val(UINT32 timeout)
{
    REG_WRITE(SPI_DMA_REG1, timeout & SPIDMA_REG1_TIMEOUT_MASK);
}

#if (CFG_SOC_NAME == SOC_BK7231)
static void spidma_config_rxbuf_start_addr(UINT8 *rxbufptr)
{
    REG_WRITE(SPI_DMA_REG2_RXBUF_ADDR, (UINT32)rxbufptr);
}

#ifdef SPIDMA_GET_RX_BUF_ADDR
static UINT8 *spidma_get_rxbuf_start_addr(void)
{
    return (UINT8 *)REG_READ(SPI_DMA_REG2_RXBUF_ADDR);
}
#endif

static void spidma_config_rxbuf_length(UINT32 rxbuflen)
{
    UINT32 reg;
    switch(rxbuflen)
    {
    case 1024:
        reg = SPIDMA_RXBUFLEN_1024B;
        break;
    case 2048:
        reg = SPIDMA_RXBUFLEN_2048B;
        break;
    case 4096:
        reg = SPIDMA_RXBUFLEN_4096B;
        break;
    case 8192:
        reg = SPIDMA_RXBUFLEN_8192B;
        break;
    case 16384:
        reg = SPIDMA_RXBUFLEN_16384B;
        break;
    case 32768:
        reg = SPIDMA_RXBUFLEN_32768B;
        break;
    case 65536:
        reg = SPIDMA_RXBUFLEN_65536B;
        break;
    default:
        reg = SPIDMA_RXBUFLEN_1024B;
        break;
    }

    REG_WRITE(SPI_DMA_REG3_RXBUF_LEN, reg);
}

static void spidma_config_rx_threshold_int(UINT32 threshold)
{
    REG_WRITE(SPI_DMA_REG4_RXBUF_THRE, (threshold - 1)&SPIDMA_REG4_RXBUF_THRE_MASK);
}

static UINT32 spidma_get_rx_threshold_val(void)
{
    return (REG_READ(SPI_DMA_REG4_RXBUF_THRE)&SPIDMA_REG4_RXBUF_THRE_MASK) + 1;
}

static UINT32 spidma_config_get_rxbuf_readptr(void)
{
    return REG_READ(SPI_DMA_REG5_RXBUF_RDPTR) & SPIDMA_REG5_RXBUF_RDPTR_MASK;
}

static void spidma_config_set_rxbuf_readptr(UINT32 rdptr)
{
    REG_WRITE(SPI_DMA_REG5_RXBUF_RDPTR, rdptr & SPIDMA_REG5_RXBUF_RDPTR_MASK);
}

static UINT32 spidma_get_rxbuf_writeptr(void)
{
    return REG_READ(SPI_DMA_REG6_RXBUF_WRPTR)&SPIDMA_REG6_RXBUF_WRPTR_MASK;
}

static void spidma_config_txbuf_start_addr(UINT8 *txbufptr)
{
    REG_WRITE(SPI_DMA_REG7_TXBUF_ADDR, (UINT32)txbufptr);
}

static void spidma_config_txbuf_length(UINT32 txbuflen)
{
    REG_WRITE(SPI_DMA_REG8_TXBUF_LEN, txbuflen);
}

static void spidma_set_txbuf_valid(UINT32 ture)
{
    if(ture)
        REG_WRITE(SPI_DMA_REG9_TXBUF_VALID, 0x1);
    else
        REG_WRITE(SPI_DMA_REG9_TXBUF_VALID, 0x0);
}

static UINT32 spidma_set_rxbuf_valid_datalen(void)
{
    return REG_READ(SPI_DMA_REG11) & SPIDMA_REG11_RXBUF_VLDNUM_MASK;
}
#endif // (CFG_SOC_NAME == SOC_BK7231)

static void spidma_enable_interrupt(void)
{
    UINT32 param;
    param = (FIQ_SPI_DMA_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);
}

static void spidma_disable_interrupt(void)
{
    UINT32 param;
    param = (FIQ_SPI_DMA_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
}

static void spidma_gpio_config(void)
{
    UINT32 param;
    
    #if (CFG_SOC_NAME == SOC_BK7231)
    param = GFUNC_MODE_SPI_DMA;
    #else
    #if (USE_SPI_DMA_GPIO_NUM == USE_SPI_DMA_GPIO_14_17)
    param = GFUNC_MODE_SPI_DMA;
    #elif (USE_SPI_DMA_GPIO_NUM == USE_SPI_DMA_GPIO_30_33)
    param = GFUNC_MODE_SPI_DMA1;
    #else
    #error "spidma must set to gpio14-17 or gpio30-33"
    #endif
    #endif // #if (CFG_SOC_NAME == SOC_BK7231)
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
}

#if (CFG_SOC_NAME != SOC_BK7231)
#if CFG_GENERAL_DMA
static void spidma_config_rxdma(void)
{
    GDMACFG_TPYES_ST cfg;
    GDMA_CFG_ST en_cfg;
    os_memset(&cfg, 0, sizeof(GDMACFG_TPYES_ST));
    
    cfg.dstdat_width = 32;
    cfg.srcdat_width = 8;
    cfg.dstptr_incr = 1;
    cfg.srcptr_incr = 0;
    cfg.src_start_addr = (void*)SPI_DMA_REG3_RXFIFO;
    cfg.dst_start_addr = &(p_spidma_desc->rxbuf[0]);

    cfg.channel = p_spidma_desc->dma_rx_channel;
    cfg.prio = 0;
    cfg.u.type5.dst_loop_start_addr = &(p_spidma_desc->rxbuf[0]);
    cfg.u.type5.dst_loop_end_addr = &(p_spidma_desc->rxbuf[p_spidma_desc->rxbuf_len]);

    //cfg.half_fin_handler = spidma_rxdma_handler;
    cfg.fin_handler = p_spidma_desc->dma_rx_handler;
    
    cfg.src_module = GDMA_X_SRC_HSSPI_RX_REQ;
    cfg.dst_module = GDMA_X_DST_DTCM_WR_REQ;

    sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_TYPE5, &cfg);

    en_cfg.channel = p_spidma_desc->dma_rx_channel;
    en_cfg.param = p_spidma_desc->node_len; // dma translen
    sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_TRANS_LENGTH, &en_cfg);

    en_cfg.channel = p_spidma_desc->dma_rx_channel;
    en_cfg.param = 1;
    sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_DMA_ENABLE, &en_cfg);
}

static void spidma_exit_rxdma(void)
{
    GDMA_CFG_ST en_cfg;

    en_cfg.channel = p_spidma_desc->dma_rx_channel;
    en_cfg.param = 0;  // disable
    sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_DMA_ENABLE, &en_cfg);
}

static void spidma_config_txdma(void)
{
    GDMACFG_TPYES_ST cfg;
    GDMA_CFG_ST en_cfg;
    os_memset(&cfg, 0, sizeof(GDMACFG_TPYES_ST));
    
    cfg.dstdat_width = 8;
    cfg.srcdat_width = 32;
    cfg.dstptr_incr = 0;
    cfg.srcptr_incr = 1;
    cfg.src_start_addr = &(p_spidma_desc->txbuf[0]);
    cfg.dst_start_addr = (void*)SPI_DMA_REG4_TXFIFO;

    cfg.channel = p_spidma_desc->dma_tx_channel;
    cfg.prio = 0;
    cfg.u.type4.src_loop_start_addr = &(p_spidma_desc->txbuf[0]);
    cfg.u.type4.src_loop_end_addr = &(p_spidma_desc->txbuf[p_spidma_desc->txbuf_len]);

    //cfg.half_fin_handler = spidma_rxdma_handler;
    cfg.fin_handler = p_spidma_desc->dma_tx_handler;
    
    cfg.src_module = GDMA_X_SRC_DTCM_RD_REQ;
    cfg.dst_module = GDMA_X_DST_HSSPI_TX_REQ;

    sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_TYPE4, &cfg);

    en_cfg.channel = p_spidma_desc->dma_tx_channel;
    en_cfg.param = p_spidma_desc->txbuf_len / 2; // dma translen
    sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_TRANS_LENGTH, &en_cfg);

    en_cfg.channel = p_spidma_desc->dma_tx_channel;
    en_cfg.param = 1;
    sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_DMA_ENABLE, &en_cfg);
}

static void spidma_exit_txdma(void)
{
    GDMA_CFG_ST en_cfg;

    en_cfg.channel = p_spidma_desc->dma_tx_channel;
    en_cfg.param = 0;  // disable
    sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_DMA_ENABLE, &en_cfg);
}
#endif // CFG_GENERAL_DMA
#endif // (CFG_SOC_NAME == SOC_BK7231)

static void spidma_software_init(void)
{
    p_spidma_desc = NULL;
    ddev_register_dev(SPIDMA_DEV_NAME, &spidma_op);
}

static void spidma_hardware_init(void)
{
    UINT32 reg;
    /* register interrupt */
    intc_service_register(FIQ_SPI_DMA, PRI_FIQ_SPI_DMA, spidma_isr);

    /* config spidma register0 to 0 */
    REG_WRITE(SPI_DMA_REG0, 0);

    /* config spidma registers, rx first */
    spidma_config_rxdata_timeout_val(SPIDMA_DEF_RXDATA_TIMEOUT_VAL);

    #if (CFG_SOC_NAME == SOC_BK7231)
    spidma_config_rxbuf_start_addr(NULL);
    spidma_config_rxbuf_length(0);
    spidma_config_set_rxbuf_readptr(0);
    spidma_config_rx_threshold_int(SPIDMA_DEF_RXDATA_THRE_INT);

    /* tx */
    spidma_config_txbuf_start_addr(NULL);
    spidma_config_txbuf_length(0);
    spidma_set_txbuf_valid(0);

    /* reset int status */
    reg = REG_READ(SPI_DMA_REG10_INT_STATUS);
    REG_WRITE(SPI_DMA_REG10_INT_STATUS, reg);
    REG_WRITE(SPI_DMA_REG11, 0);
    #else
    
    /* reset int status */
    reg = REG_READ(SPI_DMA_REG5_STA);
    REG_WRITE(SPI_DMA_REG5_STA, reg);
    #endif // (CFG_SOC_NAME == SOC_BK7231)

    /* config gpio for spidam  */
    spidma_gpio_config();

}

void spidma_init(void)
{
    spidma_software_init();
    spidma_hardware_init();
}

static UINT32 spidma_open(UINT32 op_flag)
{
    UINT32 reg;

    if(!op_flag)
    {
        SPI_DMA_PRT("spidma_open op_flag is NULL\r\n");
        return SPIDMA_FAILURE;
    }

    p_spidma_desc = (SPIDMA_DESC_PTR)op_flag;

    if(!p_spidma_desc->node_len)
    {
        SPI_DMA_PRT("spidma_open node_len is 0\r\n");
        return SPIDMA_FAILURE;
    }

    spidma_config_rxdata_timeout_val(p_spidma_desc->timeout_val);
    
    #if (CFG_SOC_NAME == SOC_BK7231)
    spidma_config_rxbuf_start_addr(p_spidma_desc->rxbuf);
    spidma_config_rxbuf_length((p_spidma_desc->rxbuf_len));
    spidma_config_set_rxbuf_readptr(0);
    spidma_config_rx_threshold_int((p_spidma_desc->mode >> SPIDMA_DESC_RX_THRED_POSI)
                                   &SPIDMA_DESC_RX_THRED_MASK);

    //spidma_config_txbuf_start_addr(p_spidma_desc->txbuf);
    spidma_config_txbuf_start_addr(NULL);
    spidma_config_txbuf_length(0);
    spidma_set_txbuf_valid(0);

    spidma_config_sck_invert((p_spidma_desc->mode >> SPIDMA_DESC_SCK_MODE_POSI)
                             &SPIDMA_DESC_SCK_MODE_MASK);
    spidma_config_LSB_first_transfer((p_spidma_desc->mode >> SPIDMA_DESC_LSB_FIRST_POSI)
                                     &SPIDMA_DESC_LSB_FIRST_MASK);
    spidma_config_3wire_mode((p_spidma_desc->mode >> SPIDMA_DESC_3WIRE_MODE_POSI)
                             &SPIDMA_DESC_3WIRE_MODE_MASK);
    reg = SPIDMA_REG0_SPI_EN
          | SPIDMA_REG0_DMA_EN
          | SPIDMA_REG0_TIMEOUT_INTEN
          | SPIDMA_REG0_RXBUF_THRED_INTEN
          | SPIDMA_REG0_TX_FINISH_INTEN
          | SPIDMA_REG0_RXEN;
    #else
    #if CFG_GENERAL_DMA
    spidma_config_rxdma();
    spidma_config_txdma();
    #endif
    
    spidma_config_sck_invert((p_spidma_desc->mode >> SPIDMA_DESC_SCK_MODE_POSI)
                             &SPIDMA_DESC_SCK_MODE_MASK);
    spidma_config_LSB_first_transfer((p_spidma_desc->mode >> SPIDMA_DESC_LSB_FIRST_POSI)
                                     &SPIDMA_DESC_LSB_FIRST_MASK);
    spidma_config_3wire_mode((p_spidma_desc->mode >> SPIDMA_DESC_3WIRE_MODE_POSI)
                             &SPIDMA_DESC_3WIRE_MODE_MASK);
    reg = SPIDMA_REG0_SPI_EN
          | SPIDMA_REG0_TIMEOUT_INTEN
          | SPIDMA_REG0_TX_FINISH_INTEN
          | SPIDMA_REG0_RXEN;
    #endif // (CFG_SOC_NAME == SOC_BK7231)

    REG_WRITE(SPI_DMA_REG0, reg);
    spidma_enable_interrupt();

    return SPIDMA_SUCCESS;
}

void spidma_uninit(void)
{

}

static UINT32 spidma_close(void)
{
    UINT32 reg;

    #if (CFG_SOC_NAME != SOC_BK7231)
    #if CFG_GENERAL_DMA
    spidma_exit_rxdma();
    spidma_exit_txdma();
    #endif
    #endif // (CFG_SOC_NAME == SOC_BK7231)

    spidma_disable_interrupt();

    reg = REG_READ(SPI_DMA_REG0);
    #if (CFG_SOC_NAME == SOC_BK7231)
    reg &= ~(SPIDMA_REG0_SPI_EN
             | SPIDMA_REG0_DMA_EN
             | SPIDMA_REG0_TIMEOUT_INTEN
             | SPIDMA_REG0_RXBUF_THRED_INTEN
             | SPIDMA_REG0_TX_FINISH_INTEN
             | SPIDMA_REG0_RXEN);
    #else
    reg &= ~(SPIDMA_REG0_SPI_EN
             | SPIDMA_REG0_TIMEOUT_INTEN
             //| SPIDMA_REG0_TX_FINISH_INTEN
             | SPIDMA_REG0_RXEN);
    #endif // (CFG_SOC_NAME == SOC_BK7231)
    REG_WRITE(SPI_DMA_REG0, reg);

    return SPIDMA_SUCCESS;
}

#if (CFG_SOC_NAME == SOC_BK7231)
static void spidma_rx_handler(UINT32 timeout)
{
    UINT32 copy_len = 0, thre = 0, left_len = 0;
    INT32 valid = 0;
    UINT8 *rdaddr;
    UINT32 cur_posi = spidma_config_get_rxbuf_readptr();
    
    //REG_WRITE((0x00802800+(29*4)), 0x02);

    valid = REG_READ(SPI_DMA_REG11) & SPIDMA_REG11_RXBUF_VLDNUM_MASK;
    thre = p_spidma_desc->node_len;

    left_len = (timeout) ? 0 : thre;
    
    while(valid > left_len ) {
        copy_len = (valid > thre) ? thre : valid;
        rdaddr = p_spidma_desc->rxbuf + (cur_posi & (p_spidma_desc->rxbuf_len - 1));
        if(p_spidma_desc->rx_handler != NULL) {
            p_spidma_desc->rx_handler(rdaddr, copy_len);
        }
        valid -= copy_len;
        cur_posi += copy_len;
    }

    spidma_config_set_rxbuf_readptr(cur_posi);    

    if(timeout) {
        REG_WRITE(SPI_DMA_REG10_INT_STATUS, SPIDMA_INTSTA_RX_TIMEOUT);
        if(p_spidma_desc->rx_timeout != NULL)
        {
            p_spidma_desc->rx_timeout();
        }
    }
    else {
        REG_WRITE(SPI_DMA_REG10_INT_STATUS, SPIDMA_INTSTA_RXBUF_THRE);
    }
    
    //REG_WRITE((0x00802800+(29*4)), 0x00);
}
#else
static void spidma_rx_handler()
{
    if(p_spidma_desc->end_frame_handler != NULL)
    {
        p_spidma_desc->end_frame_handler();
    }

    REG_WRITE(SPI_DMA_REG5_STA, SPIDMA_INTSTA_RX_TIMEOUT);
}
#endif // (CFG_SOC_NAME == SOC_BK7231)

static void spidma_txfinish_handler(void)
{
    if(p_spidma_desc->tx_handler != NULL)
        p_spidma_desc->tx_handler();
    
    os_printf("txfinish\r\n");
}

static void spidma_isr(void)
{
    UINT32 spidma_sta;

    #if (CFG_SOC_NAME == SOC_BK7231)
    /*0, get isr status*/
    spidma_sta = REG_READ(SPI_DMA_REG10_INT_STATUS);

    /*1, handle isr branch*/
    if(spidma_sta & SPIDMA_INTSTA_RXBUF_THRE)
    {
        SPI_DMA_PRT("0\r\n");
        spidma_rx_handler(0);
    }
    else if(spidma_sta & SPIDMA_INTSTA_RX_TIMEOUT)
    {
        SPI_DMA_PRT("1\r\n");
        spidma_rx_handler(1);
    }
    if(spidma_sta & SPIDMA_INTSTA_TXFINISH)
    {
        SPI_DMA_PRT("2\r\n");
        spidma_txfinish_handler();
    }
    #else
    spidma_sta = REG_READ(SPI_DMA_REG5_STA);

    if(spidma_sta & SPIDMA_INTSTA_RX_TIMEOUT)
    {
        spidma_rx_handler();
    }
    
    if(spidma_sta & SPIDMA_INTSTA_TIMING_ERR)
    {
        os_printf("hspi_timing err\r\n");
    }
    
    if(spidma_sta & SPIDMA_STA_TXFIFO_EMPTY)
    {
        spidma_txfinish_handler();
    }
    #endif // (CFG_SOC_NAME == SOC_BK7231)
}

static UINT32 spidma_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret = SPIDMA_SUCCESS;
    switch(cmd)
    {
    case SPIDMA_CMD_CONF_SCK_INV:
        spidma_config_sck_invert(*((UINT32 *)param));
        break;
    case SPIDMA_CMD_CONF_LSB_FIRST:
        spidma_config_LSB_first_transfer(*((UINT32 *)param));
        break;
    case SPIDMA_CMD_CONF_3WIRE_EN:
        spidma_config_3wire_mode(*((UINT32 *)param));
        break;
    case SPIDMA_CMD_CONF_TIMEOUT_VAL:
        spidma_config_rxdata_timeout_val(*((UINT32 *)param));
        break;
    #if (CFG_SOC_NAME == SOC_BK7231)
    case SPIDMA_CMD_CONF_RXBUF_ADDR:
        spidma_config_rxbuf_start_addr((UINT8 *)param);
        break;
    case SPIDMA_CMD_CONF_RXBUF_LEN:
        spidma_config_rxbuf_length(*((UINT32 *)param));
        break;
    case SPIDMA_CMD_CONF_RXDATA_THRE:
        spidma_config_rx_threshold_int(*((UINT32 *)param));
        break;
    case SPIDMA_CMD_GET_RXDATA_THRE:
        ret = spidma_get_rx_threshold_val();
        break;
    case SPIDMA_CMD_GET_RXBUF_RDPTR:
        ret = spidma_config_get_rxbuf_readptr();
        break;
    case SPIDMA_CMD_SET_RXBUF_RDPTR:
        spidma_config_set_rxbuf_readptr(*((UINT32 *)param));
        break;
    case SPIDMA_CMD_GET_RXBUF_WRPTR:
        ret = spidma_get_rxbuf_writeptr();
        break;
    case SPIDMA_CMD_CONF_TXBUF_ADDR:
        spidma_config_txbuf_start_addr((UINT8 *)param);
        break;
    case SPIDMA_CMD_CONF_TXBUF_LEN:
        spidma_config_txbuf_length(*((UINT32 *)param));
        break;
    case SPIDMA_CMD_CONF_TXBUF_VALID:
        spidma_set_txbuf_valid(*((UINT32 *)param));
        break;
    case SPIDMA_CMD_GET_RXBUF_VALID_DATALEN:
        ret =  spidma_set_rxbuf_valid_datalen();
        break;
    #endif // (CFG_SOC_NAME == SOC_BK7231)
    default:
        break;
    }

    return ret;
}

#endif  // CFG_USE_HSLAVE_SPI

