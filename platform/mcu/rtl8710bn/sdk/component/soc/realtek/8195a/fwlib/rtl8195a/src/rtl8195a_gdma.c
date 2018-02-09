/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h" 
#include "rtl8195a_gdma.h"
#include "hal_gdma.h"

#ifndef CONFIG_CHIP_E_CUT
BOOL
HalGdmaChBlockSetingRtl8195a_Patch(
    IN  VOID     *Data
)
{
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    PGDMA_CH_LLI_ELE pLliEle;
    struct GDMA_CH_LLI *pGdmaChLli;
    struct BLOCK_SIZE_LIST *pGdmaChBkLi;
    u32 MultiBlockCount = pHalGdmaAdapter->MaxMuliBlock;
    u32 CtlxLow, CtlxUp, CfgxLow, CfgxUp;
    u8  GdmaIndex = pHalGdmaAdapter->GdmaIndex;
    u8  ChNum = pHalGdmaAdapter->ChNum;
    u32 ChEn = pHalGdmaAdapter->ChEn;
    u8  GdmaChIsrBitmap = (ChEn & 0xFF);
    u8  PendingIsrIndex;


    pLliEle = pHalGdmaAdapter->pLlix->pLliEle;
    pGdmaChLli = pHalGdmaAdapter->pLlix->pNextLli;
    pGdmaChBkLi = pHalGdmaAdapter->pBlockSizeList;


    //4 1) Check chanel is avaliable
    if (HAL_GDMAX_READ32(GdmaIndex, REG_GDMA_CH_EN) & ChEn) {
        //4 Disable Channel
        DBG_GDMA_WARN("Channel had used; Disable Channel!!!!\n");

        HalGdmaChDisRtl8195a(Data);

    }

    //4 2) Check if there are the pending isr; TFR, Block, Src Tran, Dst Tran,  Error
    for (PendingIsrIndex=0; PendingIsrIndex<5;PendingIsrIndex++) {

        u32 PendRaw, PendStstus;
        PendRaw = HAL_GDMAX_READ32(GdmaIndex, 
                                        (REG_GDMA_RAW_INT_BASE + PendingIsrIndex*8));
        PendStstus = HAL_GDMAX_READ32(GdmaIndex, 
                                        (REG_GDMA_STATUS_INT_BASE + PendingIsrIndex*8));

        if ((PendRaw & GdmaChIsrBitmap) || (PendStstus & GdmaChIsrBitmap)) {
            //4 Clear Pending Isr
            HAL_GDMAX_WRITE32(GdmaIndex,
                              (REG_GDMA_CLEAR_INT_BASE + PendingIsrIndex*8), 
                              (PendStstus & (GdmaChIsrBitmap))
                              );
            
        } 
    }

        //4 Fill in SARx register
    HAL_GDMAX_WRITE32(GdmaIndex,
                      (REG_GDMA_CH_SAR + ChNum*REG_GDMA_CH_OFF), 
                      (pHalGdmaAdapter->ChSar)
                      );
    
    
        //4 Fill in DARx register
    HAL_GDMAX_WRITE32(GdmaIndex,
                      (REG_GDMA_CH_DAR + ChNum*REG_GDMA_CH_OFF), 
                      (pHalGdmaAdapter->ChDar)
                      );

    

    //4 3) Process CTLx
    CtlxLow = HAL_GDMAX_READ32(GdmaIndex,
                               (REG_GDMA_CH_CTL + ChNum*REG_GDMA_CH_OFF));

        //4 Clear Config low register bits
    CtlxLow &= (BIT_INVC_CTLX_LO_INT_EN &
                BIT_INVC_CTLX_LO_DST_TR_WIDTH &
                BIT_INVC_CTLX_LO_SRC_TR_WIDTH &
                BIT_INVC_CTLX_LO_DINC &
                BIT_INVC_CTLX_LO_SINC &
                BIT_INVC_CTLX_LO_DEST_MSIZE &
                BIT_INVC_CTLX_LO_SRC_MSIZE &
                BIT_INVC_CTLX_LO_TT_FC &
                BIT_INVC_CTLX_LO_LLP_DST_EN &
                BIT_INVC_CTLX_LO_LLP_SRC_EN);

    CtlxUp = HAL_GDMAX_READ32(GdmaIndex,
                               (REG_GDMA_CH_CTL + ChNum*REG_GDMA_CH_OFF + 4));

        //4 Clear Config upper register bits
    CtlxUp &= (BIT_INVC_CTLX_UP_BLOCK_BS &
               BIT_INVC_CTLX_UP_DONE);


    CtlxLow = BIT_CTLX_LO_INT_EN(pHalGdmaAdapter->GdmaCtl.IntEn) |
              BIT_CTLX_LO_DST_TR_WIDTH(pHalGdmaAdapter->GdmaCtl.DstTrWidth) |
              BIT_CTLX_LO_SRC_TR_WIDTH(pHalGdmaAdapter->GdmaCtl.SrcTrWidth) |
              BIT_CTLX_LO_DINC(pHalGdmaAdapter->GdmaCtl.Dinc) |
              BIT_CTLX_LO_SINC(pHalGdmaAdapter->GdmaCtl.Sinc) |
              BIT_CTLX_LO_DEST_MSIZE(pHalGdmaAdapter->GdmaCtl.DestMsize) |
              BIT_CTLX_LO_SRC_MSIZE(pHalGdmaAdapter->GdmaCtl.SrcMsize) |
              BIT_CTLX_LO_TT_FC(pHalGdmaAdapter->GdmaCtl.TtFc) |
              BIT_CTLX_LO_LLP_DST_EN(pHalGdmaAdapter->GdmaCtl.LlpDstEn) |
              BIT_CTLX_LO_LLP_SRC_EN(pHalGdmaAdapter->GdmaCtl.LlpSrcEn) |
              CtlxLow;
    
    CtlxUp = BIT_CTLX_UP_BLOCK_BS(pGdmaChBkLi->BlockSize) |
             BIT_CTLX_UP_DONE(pHalGdmaAdapter->GdmaCtl.Done) |
             CtlxUp;

        //4 Fill in CTLx register
    HAL_GDMAX_WRITE32(GdmaIndex,
                      (REG_GDMA_CH_CTL + ChNum*REG_GDMA_CH_OFF), 
                      CtlxLow
                      );
    
    HAL_GDMAX_WRITE32(GdmaIndex,
                      (REG_GDMA_CH_CTL + ChNum*REG_GDMA_CH_OFF +4), 
                      CtlxUp
                      );
   
    //4 4) Program CFGx
    
    CfgxLow = HAL_GDMAX_READ32(GdmaIndex,
                              (REG_GDMA_CH_CFG + ChNum*REG_GDMA_CH_OFF));

    CfgxLow &= (BIT_INVC_CFGX_LO_CH_PRIOR &
                BIT_INVC_CFGX_LO_CH_SUSP &
                BIT_INVC_CFGX_LO_HS_SEL_DST &
                BIT_INVC_CFGX_LO_HS_SEL_SRC &
                BIT_INVC_CFGX_LO_LOCK_CH_L &
                BIT_INVC_CFGX_LO_LOCK_B_L &
                BIT_INVC_CFGX_LO_LOCK_CH &
                BIT_INVC_CFGX_LO_LOCK_B &
                BIT_INVC_CFGX_LO_RELOAD_SRC &
                BIT_INVC_CFGX_LO_RELOAD_DST);

    CfgxUp = HAL_GDMAX_READ32(GdmaIndex,
                              (REG_GDMA_CH_CFG + ChNum*REG_GDMA_CH_OFF + 4));

    CfgxUp &= (BIT_INVC_CFGX_UP_FIFO_MODE &
               BIT_INVC_CFGX_UP_DS_UPD_EN &
               BIT_INVC_CFGX_UP_SS_UPD_EN &
               BIT_INVC_CFGX_UP_SRC_PER &
               BIT_INVC_CFGX_UP_DEST_PER);

    CfgxLow = BIT_CFGX_LO_CH_PRIOR(pHalGdmaAdapter->GdmaCfg.ChPrior) |
              BIT_CFGX_LO_CH_SUSP(pHalGdmaAdapter->GdmaCfg.ChSusp) |
              BIT_CFGX_LO_HS_SEL_DST(pHalGdmaAdapter->GdmaCfg.HsSelDst) |
              BIT_CFGX_LO_HS_SEL_SRC(pHalGdmaAdapter->GdmaCfg.HsSelSrc) |
              BIT_CFGX_LO_LOCK_CH_L(pHalGdmaAdapter->GdmaCfg.LockChL) |
              BIT_CFGX_LO_LOCK_B_L(pHalGdmaAdapter->GdmaCfg.LockBL) |
              BIT_CFGX_LO_LOCK_CH(pHalGdmaAdapter->GdmaCfg.LockCh) |
              BIT_CFGX_LO_LOCK_B(pHalGdmaAdapter->GdmaCfg.LockB) |
              BIT_CFGX_LO_RELOAD_SRC(pHalGdmaAdapter->GdmaCfg.ReloadSrc) |
              BIT_CFGX_LO_RELOAD_DST(pHalGdmaAdapter->GdmaCfg.ReloadDst) |
              CfgxLow;

    CfgxUp = BIT_CFGX_UP_FIFO_MODE(pHalGdmaAdapter->GdmaCfg.FifoMode) |
             BIT_CFGX_UP_DS_UPD_EN(pHalGdmaAdapter->GdmaCfg.DsUpdEn) |
             BIT_CFGX_UP_SS_UPD_EN(pHalGdmaAdapter->GdmaCfg.SsUpdEn) |
             BIT_CFGX_UP_SRC_PER(pHalGdmaAdapter->GdmaCfg.SrcPer) |
             BIT_CFGX_UP_DEST_PER(pHalGdmaAdapter->GdmaCfg.DestPer) |
             CfgxUp;

    HAL_GDMAX_WRITE32(GdmaIndex,
                      (REG_GDMA_CH_CFG + ChNum*REG_GDMA_CH_OFF), 
                      CfgxLow
                      );
    
    HAL_GDMAX_WRITE32(GdmaIndex,
                      (REG_GDMA_CH_CFG + ChNum*REG_GDMA_CH_OFF +4), 
                      CfgxUp
                      );
    
    

    //4 Check 4 Bytes Alignment
    if ((u32)(pLliEle) & 0x3) {
        DBG_GDMA_WARN("LLi Addr: 0x%x not 4 bytes alignment!!!!\n", 
                        pHalGdmaAdapter->pLli);
        return _FALSE;
    }
    
    HAL_GDMAX_WRITE32(GdmaIndex,
                      (REG_GDMA_CH_LLP + ChNum*REG_GDMA_CH_OFF), 
                      pLliEle
                      );

    //4 Update the first llp0
    pLliEle->CtlxLow = CtlxLow;
    pLliEle->CtlxUp = CtlxUp;
    pLliEle->Llpx = (u32)pGdmaChLli->pLliEle;
    DBG_GDMA_INFO("Block Count %d\n", MultiBlockCount);

    pGdmaChBkLi = pGdmaChBkLi->pNextBlockSiz;

    while (MultiBlockCount > 1) {
        MultiBlockCount--;
        DBG_GDMA_INFO("Block Count %d\n", MultiBlockCount);
        pLliEle = pGdmaChLli->pLliEle;

        if (NULL == pLliEle) {
            DBG_GDMA_ERR("pLliEle Null Point!!!!!\n");
            return _FALSE;
        }

        //4 Clear the last element llp enable bit
        if (1 == MultiBlockCount) {
            if (((pHalGdmaAdapter->Rsvd4to7) & 0x01) == 1){
                CtlxLow &= (BIT_INVC_CTLX_LO_LLP_DST_EN &
                       BIT_INVC_CTLX_LO_LLP_SRC_EN);
            }            
        }
        //4 Update block size for transfer
        CtlxUp &= (BIT_INVC_CTLX_UP_BLOCK_BS);
        CtlxUp |= BIT_CTLX_UP_BLOCK_BS(pGdmaChBkLi->BlockSize);

        //4 Update tje Lli and Block size list point to next llp
        pGdmaChLli = pGdmaChLli->pNextLli;
        pGdmaChBkLi = pGdmaChBkLi->pNextBlockSiz;

        //4 Updatethe Llpx context
        pLliEle->CtlxLow = CtlxLow;
        pLliEle->CtlxUp = CtlxUp;        
        pLliEle->Llpx = (u32)(pGdmaChLli->pLliEle);
        
    }

    return _TRUE;
}

u32
HalGdmaQueryDArRtl8195a(
    IN  VOID     *Data
)
{
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    u8  GdmaIndex = pHalGdmaAdapter->GdmaIndex;
    u8  ChNum = pHalGdmaAdapter->ChNum;
    u32 dar;
    
    dar = HAL_GDMAX_READ32(GdmaIndex,
                               (REG_GDMA_CH_DAR + ChNum*REG_GDMA_CH_OFF));

    return dar;
}

u32
HalGdmaQuerySArRtl8195a(
    IN  VOID     *Data
)
{
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    u8  GdmaIndex = pHalGdmaAdapter->GdmaIndex;
    u8  ChNum = pHalGdmaAdapter->ChNum;
    u32 dar;
    
    dar = HAL_GDMAX_READ32(GdmaIndex,
                               (REG_GDMA_CH_SAR + ChNum*REG_GDMA_CH_OFF));

    return dar;
}

BOOL
HalGdmaQueryChEnRtl8195a (
    IN  VOID              *Data
)
{
    
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = Data;

    if (HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, REG_GDMA_CH_EN) & (pHalGdmaAdapter->ChEn)) {
        return 1;
    } else {
        return 0;
    }    
}

#endif
