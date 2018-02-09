/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"
#include "rtl8195a_pcm.h"
#include "hal_pcm.h"

extern void *
_memset( void *s, int c, SIZE_T n );

VOID
HalPcmOnOffRtl8195a (
    IN  VOID              *Data
)
{
    PHAL_PCM_ADAPTER pHalPcmAdapter = (PHAL_PCM_ADAPTER) Data;
    //todo on off pcm

}

//default sampling rate 8khz, linear,  10ms frame size, time slot 0 , tx+rx
//          master mode, enable endian swap
// Question: need local tx/rx page?
BOOL
HalPcmInitRtl8195a(
    IN  VOID     *Data
)
{

    PHAL_PCM_ADAPTER pHalPcmAdapter = (PHAL_PCM_ADAPTER) Data;
    _memset((void *)pHalPcmAdapter, 0, sizeof(HAL_PCM_ADAPTER));

    //4 1) Initial PcmChCNR03 Register
    pHalPcmAdapter->PcmChCNR03.CH0MuA      = 0;
    pHalPcmAdapter->PcmChCNR03.CH0Band     = 0;


    //4 1) Initial PcmTSR03 Register
    pHalPcmAdapter->PcmTSR03.CH0TSA      = 0; 

    //4 1) Initial PcmBSize03 Register
    pHalPcmAdapter->PcmBSize03.CH0BSize    = 39; // 40word= 8khz*0.01s*1ch*2byte/4byte


    //4 2) Initial Ctl Register

    pHalPcmAdapter->PcmCtl.Pcm_En      = 1;
    pHalPcmAdapter->PcmCtl.SlaveMode   = 0;
    pHalPcmAdapter->PcmCtl.FsInv       = 0;
    pHalPcmAdapter->PcmCtl.LinearMode  = 0;
    pHalPcmAdapter->PcmCtl.LoopBack    = 0;
    pHalPcmAdapter->PcmCtl.EndianSwap  = 1;

    return _TRUE;        
}


BOOL
HalPcmSettingRtl8195a(
    IN  VOID     *Data
)
{

    PHAL_PCM_ADAPTER pHalPcmAdapter = (PHAL_PCM_ADAPTER) Data;
    u8  PcmIndex = pHalPcmAdapter->PcmIndex;
    u8  PcmCh = pHalPcmAdapter->PcmCh;
    u32 RegCtl, RegChCNR03, RegTSR03, RegBSize03;
    u32 Isr03;
    
    PcmCh=0;
    //4 1) Check Pcm index is avaliable
    if (HAL_PCMX_READ32(PcmIndex, REG_PCM_CHCNR03) & (BIT24|BIT25)) {
        //4 Pcm index is running, stop first
        DBG_8195A_DMA("Error, PCM %d ch%d is running; stop first!\n", PcmIndex, PcmCh);

        return _FALSE;
    }

    //4 2) Check if there are the pending isr


        Isr03 = HAL_PCMX_READ32(PcmIndex, REG_PCM_ISR03);
	Isr03 &= 0xff000000;
            //4 Clear Pending Isr
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_ISR03, Isr03);
        //} 


    //4 3) Process RegCtl 
    RegCtl = HAL_PCMX_READ32(PcmIndex, REG_PCM_CTL);

        //4 Clear Ctl register bits
    RegCtl &= ( BIT_INV_CTLX_SLAVE_SEL &
                BIT_INV_CTLX_FSINV &
                BIT_INV_CTLX_PCM_EN &
                BIT_INV_CTLX_LINEARMODE &
                BIT_INV_CTLX_LOOP_BACK &
                BIT_INV_CTLX_ENDIAN_SWAP);

    RegCtl =  BIT_CTLX_SLAVE_SEL(pHalPcmAdapter->PcmCtl.SlaveMode) |
              BIT_CTLX_FSINV(pHalPcmAdapter->PcmCtl.FsInv) |
              BIT_CTLX_PCM_EN(pHalPcmAdapter->PcmCtl.Pcm_En) |
              BIT_CTLX_LINEARMODE(pHalPcmAdapter->PcmCtl.LinearMode) |
              BIT_CTLX_LOOP_BACK(pHalPcmAdapter->PcmCtl.LoopBack) |
              BIT_CTLX_ENDIAN_SWAP(pHalPcmAdapter->PcmCtl.EndianSwap) |
              RegCtl; 

    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CTL, RegCtl);
        //4 4) Program ChCNR03 Register
        
        RegChCNR03 = HAL_PCMX_READ32(PcmIndex, REG_PCM_CHCNR03);

        RegChCNR03 &= (BIT_INV_CHCNR03_CH0RE &
                    BIT_INV_CHCNR03_CH0TE &
                    BIT_INV_CHCNR03_CH0MUA &
                    BIT_INV_CHCNR03_CH0BAND);

        RegChCNR03 = BIT_CHCNR03_CH0RE(pHalPcmAdapter->PcmChCNR03.CH0RE) |
                     BIT_CHCNR03_CH0TE(pHalPcmAdapter->PcmChCNR03.CH0TE) |
                     BIT_CHCNR03_CH0MUA(pHalPcmAdapter->PcmChCNR03.CH0MuA) |
                     BIT_CHCNR03_CH0BAND(pHalPcmAdapter->PcmChCNR03.CH0Band) |
                     RegChCNR03;
        
        DBG_8195A_DMA("RegChCNR03 data:0x%x\n", RegChCNR03);

        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CHCNR03, RegChCNR03);
        // time slot
        RegTSR03 = HAL_PCMX_READ32(PcmIndex, REG_PCM_TSR03);

        RegTSR03 &= (BIT_INV_TSR03_CH0TSA);
        RegTSR03 = BIT_TSR03_CH0TSA(pHalPcmAdapter->PcmTSR03.CH0TSA) |
                     RegTSR03;
        
        DBG_8195A_DMA("RegTSR03 data:0x%x\n", RegTSR03);

        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_TSR03, RegTSR03);

        // buffer size
        RegBSize03 = HAL_PCMX_READ32(PcmIndex, REG_PCM_BSIZE03);

        RegBSize03 &= (BIT_INV_BSIZE03_CH0BSIZE);
        RegBSize03 = BIT_BSIZE03_CH0BSIZE(pHalPcmAdapter->PcmBSize03.CH0BSize) |
                     RegBSize03;
        
        DBG_8195A_DMA("RegBSize03 data:0x%x\n", RegBSize03);

        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_BSIZE03, RegBSize03);




    return _TRUE;
}

BOOL
HalPcmEnRtl8195a(
    IN  VOID     *Data
)
{

    PHAL_PCM_ADAPTER pHalPcmAdapter = (PHAL_PCM_ADAPTER) Data;
    u8  PcmIndex = pHalPcmAdapter->PcmIndex;
    u8  PcmCh = pHalPcmAdapter->PcmCh;
    u32 RegChCNR03;

    PcmCh=0;
    pHalPcmAdapter->Enable = 1;


    //4 1) Check Pcm index is avaliable
    RegChCNR03 = HAL_PCMX_READ32(PcmIndex, REG_PCM_CHCNR03);
    if (RegChCNR03 & (BIT24|BIT25)) {
        //4 Pcm index is running, stop first
        DBG_8195A_DMA("Error, PCM %d ch%d is running; stop first!\n", PcmIndex, PcmCh);

        return _FALSE;
    }

    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CHCNR03, RegChCNR03|BIT24|BIT25);
    pHalPcmAdapter->PcmChCNR03.CH0RE = 1;
    pHalPcmAdapter->PcmChCNR03.CH0TE = 1;

    return _TRUE;
}

BOOL
HalPcmDisRtl8195a(
    IN  VOID     *Data
)
{

    PHAL_PCM_ADAPTER pHalPcmAdapter = (PHAL_PCM_ADAPTER) Data;
    u8  PcmIndex = pHalPcmAdapter->PcmIndex;
    u8  PcmCh = pHalPcmAdapter->PcmCh;
    u32 RegChCNR03;

    PcmCh=0;
    pHalPcmAdapter->Enable = 0;


    RegChCNR03 = HAL_PCMX_READ32(PcmIndex, REG_PCM_CHCNR03);

    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CHCNR03, RegChCNR03&(~(BIT24|BIT25)));
    pHalPcmAdapter->PcmChCNR03.CH0RE = 0;
    pHalPcmAdapter->PcmChCNR03.CH0TE = 0;

    return _TRUE;
}



BOOL
HalPcmIsrEnAndDisRtl8195a (
    IN  VOID              *Data
)
{
/*
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    u32 IsrMask, Addr, IsrCtrl;
    u8  IsrTypeIndex = 0;

    for (IsrTypeIndex=0; IsrTypeIndex<5; IsrTypeIndex++) {
        
        if (BIT_(IsrTypeIndex) & pHalGdmaAdapter->GdmaIsrType) {
            Addr =  (REG_GDMA_MASK_INT_BASE + IsrTypeIndex*8);

            IsrMask = HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, Addr);

            IsrCtrl = ((pHalGdmaAdapter->IsrCtrl)?(pHalGdmaAdapter->ChEn | IsrMask):
                                                 ((~pHalGdmaAdapter->ChEn) & IsrMask));

            HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex,
                              Addr, 
                              IsrCtrl
                              );
                    
        }   
    }
*/
    return _TRUE;
}



BOOL
HalPcmDumpRegRtl8195a (
    IN  VOID              *Data
)
{
/*    
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = Data;
    HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex, 
                      REG_GDMA_CH_EN, 
                      (HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, REG_GDMA_CH_EN)| 
                        (pHalGdmaAdapter->ChEn))
                      );    
*/
    return _TRUE;    
}

BOOL
HalPcmRtl8195a (
    IN  VOID              *Data
)
{
/*    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex, 
                      REG_GDMA_CH_EN, 
                      (HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, REG_GDMA_CH_EN)& 
                        ~(pHalGdmaAdapter->ChEn))
                      );    
*/		      
    return _TRUE;		      
}
/*
u8
HalGdmaChIsrCleanRtl8195a (
    IN  VOID              *Data
)
{
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    u32 IsrStatus;
    u8  IsrTypeIndex = 0, IsrActBitMap = 0;

    for (IsrTypeIndex=0; IsrTypeIndex<5; IsrTypeIndex++) {

        IsrStatus = HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, 
                                    (REG_GDMA_RAW_INT_BASE + IsrTypeIndex*8));

//        DBG_8195A_DMA("Isr Type %d: Isr Status 0x%x\n", IsrTypeIndex, IsrStatus);

        IsrStatus = (IsrStatus & (pHalGdmaAdapter->ChEn & 0xFF));
        
        if (BIT_(IsrTypeIndex) & pHalGdmaAdapter->GdmaIsrType) {
            HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex,
                              (REG_GDMA_CLEAR_INT_BASE+ (IsrTypeIndex*8)), 
                              (IsrStatus)// & (pHalGdmaAdapter->ChEn & 0xFF))
                              );
            IsrActBitMap |= BIT_(IsrTypeIndex);
            
        }
        
    }
    return IsrActBitMap;
     
}


VOID
HalGdmaChCleanAutoSrcRtl8195a (
    IN  VOID              *Data
) 
{
    u32 CfgxLow;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    CfgxLow = HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex,
                              (REG_GDMA_CH_CFG + pHalGdmaAdapter->ChNum*REG_GDMA_CH_OFF));

    CfgxLow &= BIT_INVC_CFGX_LO_RELOAD_SRC;

    HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex,
                  (REG_GDMA_CH_CFG + pHalGdmaAdapter->ChNum*REG_GDMA_CH_OFF), 
                  CfgxLow
                  );
    
    DBG_8195A_DMA("CFG Low data:0x%x\n",
    HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, (REG_GDMA_CH_CFG + pHalGdmaAdapter->ChNum*REG_GDMA_CH_OFF)));
}

VOID
HalGdmaChCleanAutoDstRtl8195a (
    IN  VOID              *Data
) 
{
    u32 CfgxLow;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    CfgxLow = HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex,
                              (REG_GDMA_CH_CFG + pHalGdmaAdapter->ChNum*REG_GDMA_CH_OFF));

    CfgxLow &= BIT_INVC_CFGX_LO_RELOAD_DST;

    HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex,
                  (REG_GDMA_CH_CFG + pHalGdmaAdapter->ChNum*REG_GDMA_CH_OFF), 
                  CfgxLow
                  );
    DBG_8195A_DMA("CFG Low data:0x%x\n",
    HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, (REG_GDMA_CH_CFG + pHalGdmaAdapter->ChNum*REG_GDMA_CH_OFF)));
        
}
*/


