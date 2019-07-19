#include <stdbool.h>
#include <stdio.h>
#include "lega_cm4.h"
#include "lega_common.h"
#include "lega_peripheral_reg.h"
#include "lega_dma.h"
#include "lega_spi.h"



//uint32_t peri_addr[8] = {0x40011000,0x40012000,0x40013000,0x40014000,0x40015000,0x40016000,0x4001A000,0x4001B000};
uint32_t peri_addr[8] = {(uint32_t)UART0,(uint32_t)UART1,(uint32_t)UART2,SPIx_BASE(SPI0),SPIx_BASE(SPI1),SPIx_BASE(SPI2),I2C0_BASE,I2C1_BASE};

uint8_t buff_p2m = 0;

lega_dma_callback_func g_lega_dma_callback_handler[LEGA_DMA_NUM];

void DMA_CTRL_IRQHandler(void)
{
    uint8_t dma_ch = 0;
    uint32_t dma_ctl_reg = 0;
    if(REG_RD(DMA_STATUS_TFR_L_REG)&0x01)
    {
        dma_ch = 0;
        dma_ctl_reg = DMA_CTL0_L_REG;
    }
    else
    {
        dma_ch = 1;
        dma_ctl_reg = DMA_CTL1_L_REG;
    }

    if(lega_read32_bit(dma_ctl_reg,20,3) == P2M_MODE)
    {
        if(g_lega_dma_callback_handler[dma_ch])
        {
            g_lega_dma_callback_handler[dma_ch](buff_p2m);
        }
    }
    if(dma_ch == 0)
    {
        if((CHENREG_L & 0x01)!=1)
            CHENREG_L |= 0x0101;
    }
    else
    {
        if((CHENREG_L & 0x02)!=1)
            CHENREG_L |= 0x0202;
    }
    //clear TFR int,channelx
    REG_WR(DMA_CLEAR_TFR_L_REG,0x3);
    //clear block int,channelx
    REG_WR(DMA_CLEAR_BLOCK_L_REG,0x3);
}

/**
 * Initialises a dma interface
 *
 *
 * @param[in]  dma  the interface which should be initialised
 *
 * @return
 */
void lega_dma_init(lega_dma_dev_t *dma)
{
    uint32_t handshake_src = 0;
    uint32_t handshake_dest = 0;
    uint32_t temp_val = 0;
    lega_dma_config_reg_t config_reg = {0};
#ifdef LEGA_A0V2
    lega_write32_bit(DMA_CLOCK_BASE,11,1,0x1);
#endif

    if(dma->ch == 0)
    {
        config_reg.DMA_SAR_L = (uint32_t *)&DMA_SAR0_L;
        config_reg.DMA_DAR_L = (uint32_t *)&DMA_DAR0_L;
        config_reg.DMA_LLP_L = (uint32_t *)&DMA_LLP0_L;
        config_reg.DMA_LLP_H = (uint32_t *)&DMA_LLP0_H;
        config_reg.DMA_CTL_L_REG = DMA_CTL0_L_REG;
        config_reg.DMA_CTL_H = (uint32_t *)&DMA_CTL0_H;
        config_reg.DMA_CFG_L = (uint32_t *)&DMA_CFG0_L;
        config_reg.DMA_CFG_H_REG = DMA_CFG0_H_REG;
    }
    else
    {
        config_reg.DMA_SAR_L = (uint32_t *)&DMA_SAR1_L;
        config_reg.DMA_DAR_L = (uint32_t *)&DMA_DAR1_L;
        config_reg.DMA_LLP_L = (uint32_t *)&DMA_LLP1_L;
        config_reg.DMA_LLP_H = (uint32_t *)&DMA_LLP1_H;
        config_reg.DMA_CTL_L_REG = DMA_CTL1_L_REG;
        config_reg.DMA_CTL_H = (uint32_t *)&DMA_CTL1_H;
        config_reg.DMA_CFG_L = (uint32_t *)&DMA_CFG1_L;
        config_reg.DMA_CFG_H_REG = DMA_CFG1_H_REG;
    }

//        DMA_SAR1_L = (uint32_t)dma->src;
//        DMA_DAR1_L = (uint32_t)dma->dest;
    *((volatile int *)config_reg.DMA_SAR_L) = (uint32_t)dma->src;
    *((volatile int *)config_reg.DMA_DAR_L) = (uint32_t)dma->dest;
    //DMA LL
    *((volatile int *)config_reg.DMA_LLP_L) = 0x00;
    *((volatile int *)config_reg.DMA_LLP_H) = 0x00;
    if(dma->mode == M2M_MODE)
    {
        //[26:25]=01,  src AHB master 2
        lega_write32_bit(config_reg.DMA_CTL_L_REG,25,2,0x1);
        //[24:23]=00,  dst AHB master 2
        lega_write32_bit(config_reg.DMA_CTL_L_REG,23,2,0x1);
        //[22:20]=001, mem-->mem, DW_ahb_dmac flow ctrl
        lega_write32_bit(config_reg.DMA_CTL_L_REG,20,3,(uint32_t)dma->mode);
        //[10:9]=0,    src address increment
        lega_write32_bit(config_reg.DMA_CTL_L_REG,9,2,0x0);
        //[8:7]=10,    dest address increment
        lega_write32_bit(config_reg.DMA_CTL_L_REG,7,2,0x0);
        handshake_src = 0;
        handshake_dest = 0;
    }
    else if(dma->mode == M2P_MODE)
    {
        //[26:25]=01,  src AHB master 2
        lega_write32_bit(config_reg.DMA_CTL_L_REG,25,2,0x1);
        //[24:23]=00,  dst AHB master 1
        lega_write32_bit(config_reg.DMA_CTL_L_REG,23,2,0x0);
        //[22:20]=001, mem-->peri, DW_ahb_dmac flow ctrl
        lega_write32_bit(config_reg.DMA_CTL_L_REG,20,3,(uint32_t)dma->mode);
        //[10:9]=0,    src address increment
        lega_write32_bit(config_reg.DMA_CTL_L_REG,9,2,0x0);
        //[8:7]=10,    dest address no change
        lega_write32_bit(config_reg.DMA_CTL_L_REG,7,2,0x2);
        handshake_src = 0;
        for(uint8_t i = 0;i < 8;i++)
        {
            if(dma->dest == peri_addr[i])
            {
                handshake_dest = 2*i;
                break;
            }
        }
    }
    else if(dma->mode == P2M_MODE)
    {
        //[26:25]=01,  src AHB master 1
        lega_write32_bit(config_reg.DMA_CTL_L_REG,25,2,0x0);
        //[24:23]=00,  dst AHB master 2
        lega_write32_bit(config_reg.DMA_CTL_L_REG,23,2,0x1);
        //[22:20]=001, peri-->mem, DW_ahb_dmac flow ctrl
        lega_write32_bit(config_reg.DMA_CTL_L_REG,20,3,(uint32_t)dma->mode);
        //[10:9]=0,    src address no change
        lega_write32_bit(config_reg.DMA_CTL_L_REG,9,2,0x2);
        //[8:7]=10,    dest address no change
        lega_write32_bit(config_reg.DMA_CTL_L_REG,7,2,0x2);
        handshake_dest = 0;
        for(uint8_t i = 0;i < 8;i++)
        {
            if(dma->src == peri_addr[i])
            {
                handshake_src = 2*i+1;
                break;
            }
        }
        *((volatile int *)config_reg.DMA_CTL_H) = 1;
        *((volatile int *)config_reg.DMA_DAR_L) = (uint32_t)&buff_p2m;
    }
    else if(dma->mode == P2P_MODE)
    {
        //[26:25]=01,  src AHB master 1
        lega_write32_bit(config_reg.DMA_CTL_L_REG,25,2,0x0);
        //[24:23]=00,  dst AHB master 1
        lega_write32_bit(config_reg.DMA_CTL_L_REG,23,2,0x0);
        //[22:20]=001, peri-->peri, DW_ahb_dmac flow ctrl
        lega_write32_bit(config_reg.DMA_CTL_L_REG,20,3,(uint32_t)dma->mode);
        //[10:9]=0,    src address no change
        lega_write32_bit(config_reg.DMA_CTL_L_REG,9,2,0x2);
        //[8:7]=10,    dest address no change
        lega_write32_bit(config_reg.DMA_CTL_L_REG,7,2,0x2);
        for(uint8_t i = 0;i < 8;i++)
        {
            if(dma->src == peri_addr[i])
            {
                handshake_src = 2*i+1;
                break;
            }
        }
        for(uint8_t i = 0;i < 8;i++)
        {
            if(dma->dest == peri_addr[i])
            {
                handshake_dest = 2*i;
                break;
            }
        }
        *((volatile int *)config_reg.DMA_CTL_H) = 1;
    }
    //[16:14]=000, src_size=1
    lega_write32_bit(config_reg.DMA_CTL_L_REG,14,3,0x0);
    //[13:11]=001, dest_size=1
    lega_write32_bit(config_reg.DMA_CTL_L_REG,11,3,0x0);
    //[6:4]=010,   source width=8bit
    lega_write32_bit(config_reg.DMA_CTL_L_REG,4,3,0x0);
    //[3:1]=000,   destination width=8bit
    lega_write32_bit(config_reg.DMA_CTL_L_REG,1,3,0x0);
    //[0]=1,       int enable
    lega_write32_bit(config_reg.DMA_CTL_L_REG,0,1,0x1);

    //DMA channelx CFG,config handshake signal with peri chip
    //[31], reload DST, [19:18], interface polarity active high, [11]=1 src software,[10]=0 dest hardware
    *((volatile int *)config_reg.DMA_CFG_L) = 0x00000000;
    //[14:11]=2,dest chx
    lega_write32_bit(config_reg.DMA_CFG_H_REG,11,4,handshake_dest);
    //[10:7]=2,src chx
    lega_write32_bit(config_reg.DMA_CFG_H_REG,7,4,handshake_src);
    //enable fifo mode
    lega_write32_bit(config_reg.DMA_CFG_H_REG,1,1,0x1);

    if((dma->mode == P2M_MODE) || (dma->mode == P2P_MODE))
    {
        //enable dma int
        NVIC_EnableIRQ(DMA_CTRL_IRQn);
        if(dma->ch == 0)
        {
            //unmask TFR int
            temp_val = REG_RD(DMA_MASK_TFR_L_REG);
            REG_WR(DMA_MASK_TFR_L_REG,temp_val | 0X0101);
            //unmask BLOCK int
            temp_val = REG_RD(DMA_MASK_BLOCK_L_REG);
            REG_WR(DMA_MASK_BLOCK_L_REG,temp_val | 0X0101);
        }
        else
        {
            //unmask TFR int
            temp_val = REG_RD(DMA_MASK_TFR_L_REG);
            REG_WR(DMA_MASK_TFR_L_REG,temp_val | 0X0202);
            //unmask BLOCK int
            temp_val = REG_RD(DMA_MASK_BLOCK_L_REG);
            REG_WR(DMA_MASK_BLOCK_L_REG,temp_val | 0X0202);
        }
    }
    //enable dma
    DMACFGREG_L = 0x1;
    if((dma->mode == P2M_MODE)||(dma->mode == P2P_MODE))
    {
        //enable channel X
        if(dma->ch == 0)
        {
            CHENREG_L = 0x0101;
        }
        else
        {
            CHENREG_L = 0x0202;
        }
    }
    if(dma->priv)
    {
        g_lega_dma_callback_handler[dma->ch] = (lega_dma_callback_func)(dma->priv);
    }
}


/**
 * Transmit data on a dma interface
 *
 * @param[in]  dma  the dma interface
 * @param[in]  size  number of bytes to transmit
 *
 * @return
 */
void lega_dma_send(lega_dma_dev_t *dma,/*const void *data,*/ uint32_t size, uint32_t timeout)
{
    if(dma->ch == 0)
    {
        //if(CHENREG_L & 0x01)
        //{
        //    lega_write32_bit(DMA_CFG0_L_REG,8,1,0x01);
        //    while(lega_read32_bit(DMA_CFG0_L_REG,9,1) == 0);
        //    CHENREG_L = 0X0100;
        //}
        while(CHENREG_L&0x01);
        DMA_SAR0_L = (uint32_t)dma->src;
        DMA_DAR0_L = (uint32_t)dma->dest;
        DMA_CTL0_H = size;
        //enable dma
        //DMACFGREG_L = 0x1;
        //enable channel 0
        //while(CHENREG_L&0x01);
        CHENREG_L = 0x0101;
    }
    else
    {
        //if(CHENREG_L & 0x02)
        //{
        //    lega_write32_bit(DMA_CFG1_L_REG,8,1,0x01);
        //    while(lega_read32_bit(DMA_CFG1_L_REG,9,1) == 0);
        //    CHENREG_L = 0X0200;
        //}
        while(CHENREG_L&0x02);
        DMA_SAR1_L = (uint32_t)dma->src;
        DMA_DAR1_L = (uint32_t)dma->dest;
        DMA_CTL1_H = size;
        //enable dma
        //DMACFGREG_L = 0x1;
        //enable channel 1
        //while(CHENREG_L&0x02);
        CHENREG_L = 0x0202;
    }
}

/**
 * Deinitialises a dma interface
 *
 * @param[in]  DMA  the interface which should be deinitialised
 *
 * @return
 */
void lega_dma_finalize(lega_dma_dev_t *dma)
{
    uint32_t temp_val = 0;
    if(dma->ch == 0)
    {
        //unmask TFR int
        temp_val = REG_RD(DMA_MASK_TFR_L_REG);
        REG_WR(DMA_MASK_TFR_L_REG,temp_val & (~(0X0001)));
        //unmask BLOCK int
        temp_val = REG_RD(DMA_MASK_BLOCK_L_REG);
        REG_WR(DMA_MASK_BLOCK_L_REG,temp_val & (~(0X0001)));
        if(CHENREG_L & 0x01)
        {
            lega_write32_bit(DMA_CFG0_L_REG,8,1,0x01);
            while(lega_read32_bit(DMA_CFG0_L_REG,9,1) == 0);
            CHENREG_L = 0X0100;
        }
        DMA_SAR0_L = 0;
        DMA_DAR0_L = 0;
    }
    else
    {
        //unmask TFR int
        temp_val = REG_RD(DMA_MASK_TFR_L_REG);
        REG_WR(DMA_MASK_TFR_L_REG,temp_val & (~(0X0002)));
        //unmask BLOCK int
        temp_val = REG_RD(DMA_MASK_BLOCK_L_REG);
        REG_WR(DMA_MASK_BLOCK_L_REG,temp_val & (~(0X0002)));
        if(CHENREG_L & 0x02)
        {
            lega_write32_bit(DMA_CFG1_L_REG,8,1,0x01);
            while(lega_read32_bit(DMA_CFG1_L_REG,9,1) == 0);
            CHENREG_L = 0X0200;
        }
        DMA_SAR1_L = 0;
        DMA_DAR1_L = 0;

    }

    g_lega_dma_callback_handler[dma->ch] = NULL;

    //close dma
    if((DMA_SAR0_L == 0)&&(DMA_SAR1_L == 0))
    {
        NVIC_DisableIRQ(DMA_CTRL_IRQn);
        //disable dma
        DMACFGREG_L = 0x0;
        //close dma clock
        //lega_write32_bit(DMA_CLOCK_BASE,11,1,0x1);
    }
}
