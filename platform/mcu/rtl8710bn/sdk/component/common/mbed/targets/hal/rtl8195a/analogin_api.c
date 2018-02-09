/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */

#include "objects.h"
#include "PinNames.h"
#include "hal_adc.h"
#include "analogin_api.h"
#include "analogin_ex_api.h"



#if CONFIG_ADC_EN
//#include "cmsis.h"
#include "pinmap.h"


extern u32 ConfigDebugErr;
extern u32 ConfigDebuginfo;
static SAL_ADC_TRANSFER_BUF    adcrxtranbuf;

void analogin_internal_dma_callback(void *data) {
    analogin_t *obj = (analogin_t *)data;
    PSAL_ADC_MNGT_ADPT      pSalADCMngtAdpt     = NULL;
    PSAL_ADC_HND            pSalADCHND          = NULL;
    uint8_t  AnaloginIdx         = 0;
    uint8_t  AnaloginSec;
    uint32_t AnaloginCnt;
    uint32_t AnaloginUsserDatCnt;
    uint32_t AnaloginDatTmp;
    
    pSalADCMngtAdpt         = &(obj->SalADCMngtAdpt);
    pSalADCHND              = &(pSalADCMngtAdpt->pSalHndPriv->SalADCHndPriv);

    AnaloginIdx = pSalADCHND->DevNum;
    AnaloginSec = pSalADCHND->DevNum>>1;
    
    DBG_ADC_INFO("%s\n", __func__);
   
    for (AnaloginCnt = 0; AnaloginCnt < 16; AnaloginCnt+=2) {
        DBG_ADC_INFO("[%04x]: %08x  %08x  \n", AnaloginCnt, *(pSalADCHND->pRXBuf->pDataBuf + AnaloginCnt),
            *(pSalADCHND->pRXBuf->pDataBuf+AnaloginCnt+1));
    }

    for(AnaloginCnt = 0, AnaloginUsserDatCnt = 0; AnaloginCnt < pSalADCHND->pRXBuf->DataLen; AnaloginCnt++) {
        if ((AnaloginCnt & BIT0) == AnaloginSec) {
            AnaloginDatTmp = *(pSalADCHND->pRXBuf->pDataBuf + AnaloginCnt);
            AnaloginDatTmp = AnaloginDatTmp & (0xFFFF << ((AnaloginIdx & BIT0)*16));
            AnaloginDatTmp = AnaloginDatTmp >> ((AnaloginIdx & BIT0)*16);
            *(pSalADCHND->pRXBuf->pUserDataBuf + AnaloginUsserDatCnt) = (u16)AnaloginDatTmp;            
            AnaloginUsserDatCnt++;
        } 
    }
    
    RtlMfree(pSalADCHND->pRXBuf->pDataBuf, pSalADCHND->pRXBuf->DataLen*sizeof(uint32_t));
    
    if (pSalADCHND->pUserCB->pDMARXCCB->USERCB != NULL) {
        pSalADCHND->pUserCB->pDMARXCCB->USERCB((VOID*) pSalADCHND->pUserCB->pDMARXCCB->USERData);
    }
}

void analogin_init (analogin_t *obj, PinName pin){

    uint32_t adc_idx;
    PSAL_ADC_MNGT_ADPT      pSalADCMngtAdpt     = NULL;
    PSAL_ADC_USERCB_ADPT    pSalADCUserCBAdpt   = NULL;
    PSAL_ADC_HND            pSalADCHND          = NULL;

    HAL_ADC_INIT_DAT        HalADCInitDataTmp;
    PHAL_ADC_INIT_DAT       pHalADCInitDataTmp = &HalADCInitDataTmp;
    
	/* To backup user config first */
    _memcpy(pHalADCInitDataTmp, &(obj->HalADCInitData), sizeof(HAL_ADC_INIT_DAT));
	
	_memset(obj, 0x00, sizeof(analogin_t));
	
    ConfigDebugErr &= (~(_DBG_ADC_|_DBG_GDMA_));
    ConfigDebugInfo&= (~(_DBG_ADC_|_DBG_GDMA_));

    adc_idx = pin & 0x0F;
    
    /* Get I2C device handler */
    pSalADCMngtAdpt     = &(obj->SalADCMngtAdpt);
    pSalADCUserCBAdpt   = (PSAL_ADC_USERCB_ADPT)&(obj->SalADCUserCBAdpt);   

    /*To assign the rest pointers*/
    pSalADCMngtAdpt->pSalHndPriv    = &(obj->SalADCHndPriv);
    pSalADCMngtAdpt->pSalHndPriv->ppSalADCHnd = (void**)&(pSalADCMngtAdpt->pSalHndPriv);

    /* To assign the default (ROM) HAL OP initialization function */
    pSalADCMngtAdpt->pHalOpInit         = &HalADCOpInit;

    /* To assign the default (ROM) HAL GDMA OP initialization function */
    pSalADCMngtAdpt->pHalGdmaOpInit     = &HalGdmaOpInit;

    /* To assign the default (ROM) SAL interrupt function */
    pSalADCMngtAdpt->pSalIrqFunc        = &ADCISRHandle;

    /* To assign the default (ROM) SAL DMA TX interrupt function */
    pSalADCMngtAdpt->pSalDMAIrqFunc     = &ADCGDMAISRHandle;

    /* To backup user config first */
    //_memcpy(pHalADCInitDataTmp, &(obj->HalADCInitData), sizeof(HAL_ADC_INIT_DAT));

    
    pSalADCMngtAdpt->pHalInitDat        = &(obj->HalADCInitData);
    pSalADCMngtAdpt->pHalOp             = &(obj->HalADCOp);
    pSalADCMngtAdpt->pIrqHnd            = &(obj->ADCIrqHandleDat);
    pSalADCMngtAdpt->pHalGdmaAdp        = &(obj->HalADCGdmaAdpt);
    pSalADCMngtAdpt->pHalGdmaOp         = &(obj->HalADCGdmaOp);
    pSalADCMngtAdpt->pIrqGdmaHnd        = &(obj->ADCGdmaIrqHandleDat);
    pSalADCMngtAdpt->pUserCB            = &(obj->SalADCUserCB);
    
    /* Assign the private SAL handle to public SAL handle */
    pSalADCHND      = &(pSalADCMngtAdpt->pSalHndPriv->SalADCHndPriv);

    /* Assign the internal HAL initial data pointer to the SAL handle */
    pSalADCHND->pInitDat    = pSalADCMngtAdpt->pHalInitDat;

    /* Assign the internal user callback pointer to the SAL handle */
    pSalADCHND->pUserCB     = pSalADCMngtAdpt->pUserCB;

    /*To assign user callback pointers*/
    pSalADCMngtAdpt->pUserCB->pRXCB     = pSalADCUserCBAdpt;
    pSalADCMngtAdpt->pUserCB->pRXCCB    = (pSalADCUserCBAdpt+1);
    pSalADCMngtAdpt->pUserCB->pERRCB    = (pSalADCUserCBAdpt+2);
    pSalADCMngtAdpt->pUserCB->pIDMARXCCB= (pSalADCUserCBAdpt+3);
    pSalADCMngtAdpt->pUserCB->pDMARXCB  = (pSalADCUserCBAdpt+4);
    pSalADCMngtAdpt->pUserCB->pDMARXCCB = (pSalADCUserCBAdpt+5);
/*
    pSalADCMngtAdpt->pUserCB->pTXCB     = pSalADCUserCBAdpt;
    pSalADCMngtAdpt->pUserCB->pTXCCB    = (pSalADCUserCBAdpt+1);
    pSalADCMngtAdpt->pUserCB->pRXCB     = (pSalADCUserCBAdpt+2);
    pSalADCMngtAdpt->pUserCB->pRXCCB    = (pSalADCUserCBAdpt+3);
    pSalADCMngtAdpt->pUserCB->pRDREQCB  = (pSalADCUserCBAdpt+4);
    pSalADCMngtAdpt->pUserCB->pERRCB    = (pSalADCUserCBAdpt+5);
    pSalADCMngtAdpt->pUserCB->pDMATXCB  = (pSalADCUserCBAdpt+6);
    pSalADCMngtAdpt->pUserCB->pDMATXCCB = (pSalADCUserCBAdpt+7);
    pSalADCMngtAdpt->pUserCB->pDMARXCB  = (pSalADCUserCBAdpt+8);
    pSalADCMngtAdpt->pUserCB->pDMARXCCB = (pSalADCUserCBAdpt+9);
*/        
    /* Set ADC Device Number */
    pSalADCHND->DevNum = adc_idx;

    /* Load ADC default value */
    RtkADCLoadDefault(pSalADCHND);

    /* Assign ADC Pin Mux */
    pSalADCHND->PinMux        = 0;
    pSalADCHND->OpType        = ADC_RDREG_TYPE;

    /* Load user setting */
    if ((pHalADCInitDataTmp->ADCEndian == ADC_DATA_ENDIAN_LITTLE) || (pHalADCInitDataTmp->ADCEndian == ADC_DATA_ENDIAN_BIG)) {
        pSalADCHND->pInitDat->ADCEndian = pHalADCInitDataTmp->ADCEndian;
    }
    
    if ((pHalADCInitDataTmp->ADCAudioEn != ADC_FEATURE_DISABLED) && (pHalADCInitDataTmp->ADCAudioEn < 2)) {
        pSalADCHND->pInitDat->ADCAudioEn = pHalADCInitDataTmp->ADCAudioEn;
    }
    
    /* Init ADC now */
    pSalADCHND->pInitDat->ADCBurstSz       =   8;
    pSalADCHND->pInitDat->ADCOneShotTD     =   8;
    RtkADCInit(pSalADCHND); 
}

float analogin_read(analogin_t *obj){
    float value;
    uint32_t AnaloginTmp[2]      = {0,0};
    uint32_t AnaloginDatMsk      = 0xFFFF;
    uint8_t  AnaloginIdx         = 0;
    uint32_t AnalogDat           = 0;
    
    uint32_t AnalogDatFull = 0;

    PSAL_ADC_MNGT_ADPT      pSalADCMngtAdpt     = NULL;
    PSAL_ADC_HND            pSalADCHND          = NULL;
    
    pSalADCMngtAdpt         = &(obj->SalADCMngtAdpt);
    pSalADCHND              = &(pSalADCMngtAdpt->pSalHndPriv->SalADCHndPriv);
    AnaloginIdx             = pSalADCHND->DevNum;
    RtkADCReceiveBuf(pSalADCHND,&AnaloginTmp[0]);
    
    AnaloginDatMsk = (u32)(AnaloginDatMsk<<((u32)(16*(AnaloginIdx&0x01))));
    AnalogDat = AnaloginTmp[(AnaloginIdx/2)];
    AnalogDat = (AnalogDat & AnaloginDatMsk);    
    AnalogDat = (AnalogDat>>((u32)(16*(AnaloginIdx&0x01))));
    
    AnalogDatFull   = 0xCE80;

    value = (float)(AnalogDat) / (float)(AnalogDatFull);

    return (float)value;
}

uint16_t analogin_read_u16(analogin_t *obj){
    uint32_t AnaloginTmp[2]      = {0,0};
    uint32_t AnaloginDatMsk      = 0xFFFF;
    uint8_t  AnaloginIdx         = 0;
    uint32_t AnalogDat           = 0;
    
    PSAL_ADC_MNGT_ADPT      pSalADCMngtAdpt     = NULL;
    PSAL_ADC_HND            pSalADCHND          = NULL;

    pSalADCMngtAdpt         = &(obj->SalADCMngtAdpt);
    pSalADCHND              = &(pSalADCMngtAdpt->pSalHndPriv->SalADCHndPriv);
    AnaloginIdx             = pSalADCHND->DevNum;
    RtkADCRxManualRotate(pSalADCHND,&AnaloginTmp[0]);
    

    //DBG_8195A("[0]:%08x, %08x\n", AnaloginTmp[0], AnaloginTmp[1]  );
    AnaloginDatMsk = (u32)(AnaloginDatMsk<<((u32)(16*(AnaloginIdx&0x01))));
    AnalogDat = AnaloginTmp[(AnaloginIdx/2)];
    AnalogDat = (AnalogDat & AnaloginDatMsk);    
    AnalogDat = (AnalogDat>>((u32)(16*(AnaloginIdx&0x01))));

    return (uint16_t)AnalogDat;
    
}


void  analogin_deinit(analogin_t *obj){
    PSAL_ADC_MNGT_ADPT      pSalADCMngtAdpt     = NULL;
    PSAL_ADC_HND            pSalADCHND          = NULL;
    
    pSalADCMngtAdpt         = &(obj->SalADCMngtAdpt);
    pSalADCHND              = &(pSalADCMngtAdpt->pSalHndPriv->SalADCHndPriv);

    /* To deinit analogin */
    RtkADCDeInit(pSalADCHND);    
}

void analogin_set_user_callback (analogin_t * obj, AnalogInCallback analogin_cb, void(* analogin_callback)(void *)){
    
    PSAL_ADC_MNGT_ADPT      pSalADCMngtAdpt     = NULL;
    PSAL_ADC_HND            pSalADCHND          = NULL;
    pSalADCMngtAdpt         = &(obj->SalADCMngtAdpt);
    pSalADCHND              = &(pSalADCMngtAdpt->pSalHndPriv->SalADCHndPriv);
    
    switch (analogin_cb) {
        case ANALOGIN_RX_DMA_COMPLETE:
            pSalADCHND->pUserCB->pDMARXCCB->USERCB = analogin_callback;
            break;
        default:
            break;
    }

}

void analogin_clear_user_callback (analogin_t * obj, AnalogInCallback analogin_cb){

    PSAL_ADC_MNGT_ADPT      pSalADCMngtAdpt     = NULL;
    PSAL_ADC_HND            pSalADCHND          = NULL;
    pSalADCMngtAdpt         = &(obj->SalADCMngtAdpt);
    pSalADCHND              = &(pSalADCMngtAdpt->pSalHndPriv->SalADCHndPriv);
    
    switch (analogin_cb) {
        case ANALOGIN_RX_DMA_COMPLETE:
            pSalADCHND->pUserCB->pDMARXCB = NULL;
            break;
        default:
            break;
    }

}

uint8_t analogin_read_u16_dma (analogin_t * obj, uint16_t *buf, uint16_t length) {
    PSAL_ADC_MNGT_ADPT      pSalADCMngtAdpt     = NULL;
    PSAL_ADC_HND            pSalADCHND          = NULL;
    uint8_t  AnaloginIdx         = 0;

    pSalADCMngtAdpt         = &(obj->SalADCMngtAdpt);
    pSalADCHND              = &(pSalADCMngtAdpt->pSalHndPriv->SalADCHndPriv);
    
    if (length > (uint16_t)(MAX_DMA_BLOCK_SIZE/2)) {
        DBG_ADC_ERR("Data length is more than supported size.\n");
        return 1;
    }
    
    _memset(&adcrxtranbuf, 0x0000, sizeof(adcrxtranbuf));
    AnaloginIdx             = pSalADCHND->DevNum;
    pSalADCHND->pRXBuf      = &adcrxtranbuf;
    pSalADCHND->OpType = ADC_DMA_TYPE;
    DBG_ADC_INFO("ch%d, DMA len: %d, ptr: %x\n", AnaloginIdx, length, buf);

    pSalADCHND->pUserCB->pIDMARXCCB->USERCB = analogin_internal_dma_callback;
    pSalADCHND->pUserCB->pIDMARXCCB->USERData = (u32)obj;
    pSalADCHND->pRXBuf->pDataBuf = (u32 *)RtlZmalloc(sizeof(uint32_t)*length*2); 
    pSalADCHND->pRXBuf->pUserDataBuf = (u16 *)buf;
    pSalADCHND->pRXBuf->DataLen = length*2;
    
    RtkADCReceiveDMA(pSalADCHND, 0);

    return 0;
}
#endif

