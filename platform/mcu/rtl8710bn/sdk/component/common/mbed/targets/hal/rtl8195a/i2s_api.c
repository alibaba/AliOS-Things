/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2015, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */
#include "objects.h"
#include "i2s_api.h"
#include "pinmap.h"

#if CONFIG_I2S_EN
static const PinMap PinMap_I2S_TX[] = {
    {PE_2,  RTL_PIN_PERI(I2S0, 0, S0), RTL_PIN_FUNC(I2S0, S0)},
    {PH_2,  RTL_PIN_PERI(I2S0, 0, S1), RTL_PIN_FUNC(I2S0, S1)},
    {PD_2,  RTL_PIN_PERI(I2S0, 0, S2), RTL_PIN_FUNC(I2S0, S2)},
    {PC_7,  RTL_PIN_PERI(I2S0, 0, S3), RTL_PIN_FUNC(I2S0, S3)},
    {PC_2,  RTL_PIN_PERI(I2S1, 1, S0), RTL_PIN_FUNC(I2S1, S0)},
    {PD_6,  RTL_PIN_PERI(I2S1, 1, S1), RTL_PIN_FUNC(I2S1, S1)},
    {PE_6,  RTL_PIN_PERI(I2S1, 1, S2), RTL_PIN_FUNC(I2S1, S2)},
    {NC,    NC,     0}
};

static const PinMap PinMap_I2S_RX[] = {
    {PH_5,  RTL_PIN_PERI(I2S0, 0, S1), RTL_PIN_FUNC(I2S0, S1)},
    {PC_5,  RTL_PIN_PERI(I2S0, 0, S3), RTL_PIN_FUNC(I2S0, S3)},
    {PC_4,  RTL_PIN_PERI(I2S1, 1, S0), RTL_PIN_FUNC(I2S1, S0)},
    {PD_3,  RTL_PIN_PERI(I2S1, 1, S1), RTL_PIN_FUNC(I2S1, S1)},
    {PE_8,  RTL_PIN_PERI(I2S1, 1, S2), RTL_PIN_FUNC(I2S1, S1)},
    {NC,    NC,     0}
};

static const PinMap PinMap_I2S_CLK[] = {
    {PE_1,  RTL_PIN_PERI(I2S0, 0, S0), RTL_PIN_FUNC(I2S0, S0)},
    {PH_1,  RTL_PIN_PERI(I2S0, 0, S1), RTL_PIN_FUNC(I2S0, S1)},
    {PD_1,  RTL_PIN_PERI(I2S0, 0, S2), RTL_PIN_FUNC(I2S0, S2)},
    {PC_8,  RTL_PIN_PERI(I2S0, 0, S3), RTL_PIN_FUNC(I2S0, S3)},
    {PC_1,  RTL_PIN_PERI(I2S1, 1, S0), RTL_PIN_FUNC(I2S1, S0)},
    {PD_5,  RTL_PIN_PERI(I2S1, 1, S1), RTL_PIN_FUNC(I2S1, S1)},
    {PE_5,  RTL_PIN_PERI(I2S1, 1, S2), RTL_PIN_FUNC(I2S1, S2)},
    {NC,    NC,     0}
};

static const PinMap PinMap_I2S_WS[] = {
    {PE_0,  RTL_PIN_PERI(I2S0, 0, S0), RTL_PIN_FUNC(I2S0, S0)},
    {PH_0,  RTL_PIN_PERI(I2S0, 0, S1), RTL_PIN_FUNC(I2S0, S1)},
    {PD_0,  RTL_PIN_PERI(I2S0, 0, S2), RTL_PIN_FUNC(I2S0, S2)},
    {PC_9,  RTL_PIN_PERI(I2S0, 0, S3), RTL_PIN_FUNC(I2S0, S3)},
    {PC_0,  RTL_PIN_PERI(I2S1, 1, S0), RTL_PIN_FUNC(I2S1, S0)},
    {PD_4,  RTL_PIN_PERI(I2S1, 1, S1), RTL_PIN_FUNC(I2S1, S1)},
    {PE_4,  RTL_PIN_PERI(I2S1, 1, S2), RTL_PIN_FUNC(I2S1, S2)},
    {NC,    NC,     0}
};

static const HAL_I2S_DEF_SETTING I2SDefaultSetting = {
    .I2SMaster = I2S_MASTER_MODE,       // I2S Function Mode
    .DevSts = I2S_STS_UNINITIAL,        //I2S device status
    .I2SChNum = I2S_CH_STEREO,          //I2S Channel number mono or stereo
    .I2SPageNum = I2S_4PAGE,            //I2S Page number 2~4
    .I2STRxAct = I2S_TXRX,              //I2S tx rx act, tx only or rx only or tx+rx
    .I2SWordLen = I2S_WL_16,            //I2S Word length 16bit or 24bit
    .I2SPageSize = (768/4)-1,           //I2S Page size 1~4096 word
    .I2SRate = I2S_SR_48KHZ,           //I2S sample rate 8k ~ 96khz
    
    .I2STxIntrMSK = I2S_TX_INT_PAGE0_OK|I2S_TX_INT_PAGE1_OK| \
                    I2S_TX_INT_PAGE2_OK|I2S_TX_INT_PAGE3_OK,   /*I2S Tx Interrupt Mask*/
    .I2SRxIntrMSK = I2S_RX_INT_PAGE0_OK|I2S_RX_INT_PAGE1_OK| \
                    I2S_RX_INT_PAGE2_OK|I2S_RX_INT_PAGE3_OK   /*I2S Rx Interrupt Mask*/
};

void i2s_init(i2s_t *obj, PinName sck, PinName ws, PinName sd)
{
    uint32_t i2s_sck, i2s_ws, i2s_tx, i2s_rx;
    uint32_t i2s_sel;;
    uint8_t i2s_idx;
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) &obj->I2SAdapter;
    HAL_Status ret;

    // Determine the UART to use (UART0, UART1, or UART3)
    i2s_sck = pinmap_peripheral(sck, PinMap_I2S_CLK);
    i2s_ws = pinmap_peripheral(ws, PinMap_I2S_WS);
    i2s_tx = pinmap_find_peripheral(sd, PinMap_I2S_TX);
    i2s_rx = pinmap_find_peripheral(sd, PinMap_I2S_RX);

    i2s_sel = pinmap_merge(i2s_sck, i2s_ws);
    if (unlikely(i2s_sel == NC)) {
        DBG_I2S_ERR("%s: Cannot find matched I2S for given pin\n", __FUNCTION__);
        return;
    }

    if( (i2s_sel != i2s_tx) && (i2s_sel != i2s_rx)){
        DBG_I2S_ERR("%s: Cannot find matched I2S for given pin\n", __FUNCTION__);
        return;
    }    
   
    i2s_idx = RTL_GET_PERI_IDX(i2s_sel);
	
	pI2SAdapter->DevNum =  i2s_idx;
	pI2SAdapter->PinMux =  RTL_GET_PERI_SEL(i2s_sel);;
    DBG_I2S_INFO("%s: Use I2S%d Sel%d\r\n", __FUNCTION__, pI2SAdapter->DevNum, pI2SAdapter->PinMux);

	pI2SAdapter->pInitDat = &obj->InitDat;
	RtkI2SLoadDefault(pI2SAdapter, (VOID*)&I2SDefaultSetting);

    // Load user defined parameters
	pI2SAdapter->pInitDat->I2SChNum = obj->channel_num;
	pI2SAdapter->pInitDat->I2SRate = obj->sampling_rate;
	pI2SAdapter->pInitDat->I2SWordLen = obj->word_length;	
	pI2SAdapter->pInitDat->I2STRxAct = obj->direction;	

    //RtkI2SInit(pI2SAdapter);
    ret = HalI2SInit(pI2SAdapter);

    if(ret != HAL_OK){
        DBG_I2S_ERR("%s: HalI2SInit is failure\n", __FUNCTION__);
    }  

}

void i2s_set_dma_buffer(i2s_t *obj, char *tx_buf, char *rx_buf, 
    uint32_t page_num, uint32_t page_size)
{
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) &obj->I2SAdapter;
    u32 i;

    if ((page_num < 2) || (page_num > 4) || (page_size < 8)) {
        DBG_I2S_INFO("%s: PageNum(%d) valid value is 2~4; PageSize(%d must > 8)\r\n", \
            __FUNCTION__, page_num, page_size);
        return;
    }
    
    pI2SAdapter->pInitDat->I2SPageNum = page_num - 1; 
    pI2SAdapter->pInitDat->I2SPageSize = page_size/4 - 1;    // unit is 4-bytes
    pI2SAdapter->pInitDat->I2STxData = (u8*)tx_buf;
    pI2SAdapter->pInitDat->I2SRxData = (u8*)rx_buf;
    HalI2SSetDMABuf(pI2SAdapter->pInitDat);

    for (i=0;i<page_num;i++) {
        pI2SAdapter->TxPageList[i] = (uint32_t*)(tx_buf + ((page_size) * i)); 
        pI2SAdapter->RxPageList[i] = (uint32_t*)(rx_buf + ((page_size) * i)); 
    }
}

void i2s_tx_irq_handler(i2s_t *obj, i2s_irq_handler handler, uint32_t id)
{
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) &obj->I2SAdapter;
    
    pI2SAdapter->UserCB.TxCCB = handler;
    pI2SAdapter->UserCB.TxCBId = id;
}

void i2s_rx_irq_handler(i2s_t *obj, i2s_irq_handler handler, uint32_t id)
{
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) &obj->I2SAdapter;
    
    pI2SAdapter->UserCB.RxCCB = handler;
    pI2SAdapter->UserCB.RxCBId = id;
}

void i2s_set_direction(i2s_t *obj, int trx_type)
{
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) &obj->I2SAdapter;

    obj->direction = trx_type;
	pI2SAdapter->pInitDat->I2STRxAct = trx_type;
    HalI2SSetDirection(pI2SAdapter->pInitDat);
}

void i2s_set_param(i2s_t *obj, int channel_num, int rate, int word_len)
{
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) &obj->I2SAdapter;
	
    obj->channel_num = channel_num;
    obj->sampling_rate = rate;
    obj->word_length = word_len;
	pI2SAdapter->pInitDat->I2SChNum = channel_num;
	pI2SAdapter->pInitDat->I2SRate = rate;
	pI2SAdapter->pInitDat->I2SWordLen = word_len;
	HalI2SSetChNum(pI2SAdapter->pInitDat);
    HalI2SSetRate(pI2SAdapter->pInitDat);
    HalI2SSetWordLen(pI2SAdapter->pInitDat);
}

void i2s_deinit(i2s_t *obj)
{
	//RtkI2SDeInit((VOID*)&obj->I2SAdapter);
    HalI2SDeInit((VOID*)&obj->I2SAdapter);   
}

int* i2s_get_tx_page(i2s_t *obj)
{
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) &obj->I2SAdapter;
    u8 page_idx;

    page_idx = HalI2SGetTxPage((VOID*)pI2SAdapter->pInitDat);
    if (page_idx <= pI2SAdapter->pInitDat->I2SPageNum) {
        return ((int*)pI2SAdapter->TxPageList[page_idx]);
    } else {
        return NULL;
    }
}

void i2s_send_page(i2s_t *obj, uint32_t *pbuf)
{
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) &obj->I2SAdapter;
    u32 page_num, i;

    page_num = pI2SAdapter->pInitDat->I2SPageNum + 1;
    for (i=0;i<page_num;i++) {
        if (pI2SAdapter->TxPageList[i] == pbuf) {
            HalI2SPageSend(pI2SAdapter->pInitDat, i);
            break;  // break the for loop
        }
    }

    if (i == page_num) {
        DBG_I2S_ERR("i2s_send_page: the pbuf(0x%x) is not a DMA buffer\r\n", pbuf);
    }
}

void i2s_recv_page(i2s_t *obj)
{
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) &obj->I2SAdapter;

    HalI2SPageRecv(pI2SAdapter->pInitDat);
}

void i2s_enable(i2s_t *obj)
{
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) &obj->I2SAdapter;
    
    //RtkI2SEnable(pI2SAdapter);
    HalI2SEnable(pI2SAdapter);
}

void i2s_disable(i2s_t *obj)
{
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) &obj->I2SAdapter;
    
    //RtkI2SDisable(pI2SAdapter);
    HalI2SDisable(pI2SAdapter);
}

#endif  // end of "#if CONFIG_I2S_EN"
