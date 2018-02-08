/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h"
#include "rtl_utility.h"
#include "osdep_api.h"
#include "hal_dac.h"
#include "hal_gdma.h"

#define DAC_STATIC_ALLOC      0

/* DAC SAL global variables declaration when kernel disabled */

#if DAC_STATIC_ALLOC
    SRAM_BF_DATA_SECTION
    HAL_DAC_OP HalDACOpSAL;
#endif


#if DAC0_USED   /*#if DAC0_USED*/    
#if DAC_STATIC_ALLOC
    SRAM_BF_DATA_SECTION
    SAL_DAC_MNGT_ADPT   SalDAC0MngtAdpt;
    
    SRAM_BF_DATA_SECTION
    SAL_DAC_HND_PRIV    SalDAC0HndPriv;

    SRAM_BF_DATA_SECTION
    HAL_DAC_INIT_DAT    HalDAC0InitData;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE          DAC0IrqHandleDat;

    SRAM_BF_DATA_SECTION
    HAL_GDMA_ADAPTER    HalDAC0GdmaAdpt;

    SRAM_BF_DATA_SECTION
    HAL_GDMA_OP         HalDAC0GdmaOp;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE          DAC0GDMAIrqHandleDat;
    
    SRAM_BF_DATA_SECTION
    SAL_DAC_USER_CB     SalDAC0UserCB;

    SRAM_BF_DATA_SECTION
    SAL_DAC_DMA_USER_DEF SalDAC0DmaUserDef;
    
    SRAM_BF_DATA_SECTION
    SAL_DAC_USERCB_ADPT SalDAC0UserCBAdpt[SAL_DAC_USER_CB_NUM];
#endif    
#endif          /*#if DAC0_USED*/

#if DAC1_USED   /*#if DAC1_USED*/    
#if DAC_STATIC_ALLOC

    SRAM_BF_DATA_SECTION
    SAL_DAC_MNGT_ADPT   SalDAC1MngtAdpt;
    
    SRAM_BF_DATA_SECTION
    SAL_DAC_HND_PRIV    SalDAC1HndPriv;

    SRAM_BF_DATA_SECTION
    HAL_DAC_INIT_DAT    HalDAC1InitData;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE          DAC1IrqHandleDat;

    SRAM_BF_DATA_SECTION
    HAL_GDMA_ADAPTER    HalDAC1GdmaAdpt;

    SRAM_BF_DATA_SECTION
    HAL_GDMA_OP         HalDAC1GdmaOp;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE          DAC1GDMAIrqHandleDat;
    
    SRAM_BF_DATA_SECTION
    SAL_DAC_USER_CB     SalDAC1UserCB;

    SRAM_BF_DATA_SECTION
    SAL_DAC_DMA_USER_DEF SalDAC1DmaUserDef;
    
    SRAM_BF_DATA_SECTION
    SAL_DAC_USERCB_ADPT SalDAC1UserCBAdpt[SAL_DAC_USER_CB_NUM];
#endif    
#endif          /*#if DAC1_USED*/

/* Function prototype */
VOID DACISRHandle(IN  VOID *Data);
VOID DACGDMAISRHandle(IN VOID * Data);
VOID DACGDMALLPISRHandle(IN  VOID *Data);

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CGetMngtAdpt
//
// Description:
//		According to the input index, all the memory space are allocated and all the 
//          related pointers are assigned. The management adapter pointer will be 
//          returned.
//
// Arguments:
//		[in] u8  I2CIdx -
//			I2C module index
//
// Return:
//		PSAL_I2C_MNGT_ADPT
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-02.
//
//--------------------------------------------------------------------------------------------------- 
VOID HalDACOpInit(
    IN  VOID *Data
)
{
    PHAL_DAC_OP pHalDacOp = (PHAL_DAC_OP) Data;

    pHalDacOp->HalDACInit       =   HalDACInit8195a;
    pHalDacOp->HalDACDeInit     =   HalDACDeInit8195a;
    pHalDacOp->HalDACEnable     =   HalDACEnableRtl8195a;
    pHalDacOp->HalDACSend       =   HalDACSendRtl8195a;
    pHalDacOp->HalDACIntrCtrl   =   HalDACIntrCtrl8195a;
    pHalDacOp->HalDACReadReg    =   HalDACReadRegRtl8195a;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CGetMngtAdpt
//
// Description:
//		According to the input index, all the memory space are allocated and all the 
//          related pointers are assigned. The management adapter pointer will be 
//          returned.
//
// Arguments:
//		[in] u8  I2CIdx -
//			I2C module index
//
// Return:
//		PSAL_I2C_MNGT_ADPT
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-02.
//
//--------------------------------------------------------------------------------------------------- 
PSAL_DAC_MNGT_ADPT
RtkDACGetMngtAdpt(
    IN  u8  DACIdx
){
    PSAL_DAC_MNGT_ADPT      pSalDACMngtAdpt     = NULL;
    PSAL_DAC_USERCB_ADPT    pSalDACUserCBAdpt   = NULL;
	DACIdx = DACIdx;
    /* If the kernel is available, Memory-allocation is used. */
#if !DAC_STATIC_ALLOC

    pSalDACMngtAdpt = (PSAL_DAC_MNGT_ADPT)RtlZmalloc(sizeof(SAL_DAC_MNGT_ADPT));
    pSalDACMngtAdpt->pSalHndPriv    = (PSAL_DAC_HND_PRIV)RtlZmalloc(sizeof(SAL_DAC_HND_PRIV));
    pSalDACMngtAdpt->pHalInitDat    = (PHAL_DAC_INIT_DAT)RtlZmalloc(sizeof(HAL_DAC_INIT_DAT));
    pSalDACMngtAdpt->pHalOp         = (PHAL_DAC_OP)RtlZmalloc(sizeof(HAL_DAC_OP));
    pSalDACMngtAdpt->pIrqHnd        = (PIRQ_HANDLE)RtlZmalloc(sizeof(IRQ_HANDLE));
    pSalDACMngtAdpt->pUserCB        = (PSAL_DAC_USER_CB)RtlZmalloc(sizeof(SAL_DAC_USER_CB));
    pSalDACMngtAdpt->pDMAConf       = (PSAL_DAC_DMA_USER_DEF)RtlZmalloc(sizeof(SAL_DAC_DMA_USER_DEF));
    pSalDACMngtAdpt->pHalGdmaAdp    = (PHAL_GDMA_ADAPTER)RtlZmalloc(sizeof(HAL_GDMA_ADAPTER));
    pSalDACMngtAdpt->pHalGdmaOp     = (PHAL_GDMA_OP)RtlZmalloc(sizeof(HAL_GDMA_OP));
    pSalDACMngtAdpt->pIrqGdmaHnd    = (PIRQ_HANDLE)RtlZmalloc(sizeof(IRQ_HANDLE));
    pSalDACUserCBAdpt               = (PSAL_DAC_USERCB_ADPT)RtlZmalloc((sizeof(SAL_DAC_USERCB_ADPT)*SAL_DAC_USER_CB_NUM));
#else
    switch (DACIdx){
        case DAC0_SEL:
        {
            pSalDACMngtAdpt = &SalDAC0MngtAdpt;
            pSalDACMngtAdpt->pSalHndPriv    = &SalDAC0HndPriv;
            pSalDACMngtAdpt->pHalInitDat    = &HalDAC0InitData;
            pSalDACMngtAdpt->pHalOp         = &HalDACOpSAL;
            pSalDACMngtAdpt->pIrqHnd        = &DAC0IrqHandleDat;
            pSalDACMngtAdpt->pUserCB        = &SalDAC0UserCB;
            pSalDACMngtAdpt->pDMAConf       = &SalDAC0DmaUserDef;
            pSalDACMngtAdpt->pHalGdmaAdp    = &HalDAC0GdmaAdpt;
            pSalDACMngtAdpt->pHalGdmaOp     = &HalDAC0GdmaOp;
            pSalDACMngtAdpt->pIrqGdmaHnd    = &DAC0IrqHandleDat;            
            pSalDACUserCBAdpt               = &SalDAC0UserCBAdpt;
            break;
        }
        
        case DAC1_SEL:
        {
            pSalDACMngtAdpt = &SalDAC1MngtAdpt;
            pSalDACMngtAdpt->pSalHndPriv    = &SalDAC1HndPriv;
            pSalDACMngtAdpt->pHalInitDat    = &HalDAC1InitData;
            pSalDACMngtAdpt->pHalOp         = &HalDACOpSAL;
            pSalDACMngtAdpt->pIrqHnd        = &DAC1IrqHandleDat;
            pSalDACMngtAdpt->pUserCB        = &SalDAC1UserCB;
            pSalDACMngtAdpt->pDMAConf       = &SalDAC1DmaUserDef;
            pSalDACMngtAdpt->pHalGdmaAdp    = &HalDAC1GdmaAdpt;
            pSalDACMngtAdpt->pHalGdmaOp     = &HalDAC1GdmaOp;
            pSalDACMngtAdpt->pIrqGdmaHnd    = &DAC1IrqHandleDat;            
            pSalDACUserCBAdpt               = &SalDAC1UserCBAdpt;
            break;
        }
        
        default:
            break;
    }
#endif

    /*To assign user callback pointers*/
    pSalDACMngtAdpt->pUserCB->pTXCB     = pSalDACUserCBAdpt;
    pSalDACMngtAdpt->pUserCB->pTXCCB    = (pSalDACUserCBAdpt+1);
    pSalDACMngtAdpt->pUserCB->pRXCB     = (pSalDACUserCBAdpt+2);
    pSalDACMngtAdpt->pUserCB->pRXCCB    = (pSalDACUserCBAdpt+3);
    pSalDACMngtAdpt->pUserCB->pRDREQCB  = (pSalDACUserCBAdpt+4);
    pSalDACMngtAdpt->pUserCB->pERRCB    = (pSalDACUserCBAdpt+5);
    pSalDACMngtAdpt->pUserCB->pDMATXCB  = (pSalDACUserCBAdpt+6);
    pSalDACMngtAdpt->pUserCB->pDMATXCCB = (pSalDACUserCBAdpt+7);
    pSalDACMngtAdpt->pUserCB->pDMARXCB  = (pSalDACUserCBAdpt+8);
    pSalDACMngtAdpt->pUserCB->pDMARXCCB = (pSalDACUserCBAdpt+9);
    
    /*To assign the rest pointers*/
    pSalDACMngtAdpt->pSalHndPriv->ppSalDACHnd = (void**)&(pSalDACMngtAdpt->pSalHndPriv);

    /* To assign the default HAL OP initialization function */
    pSalDACMngtAdpt->pHalOpInit         = &HalDACOpInit;

    /* To assign the default HAL GDMA OP initialization function */
    pSalDACMngtAdpt->pHalGdmaOpInit     = &HalGdmaOpInit;

    /* To assign the default SAL interrupt function */
    pSalDACMngtAdpt->pSalIrqFunc        = &DACISRHandle;

    /* To assign the default SAL DMA interrupt function */
    pSalDACMngtAdpt->pSalDMAIrqFunc     = &DACGDMAISRHandle;
    
    return pSalDACMngtAdpt;
}


//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CFreeMngtAdpt
//
// Description:
//		Free all the previous allocated memory space.
//
// Arguments:
//		[in] PSAL_I2C_MNGT_ADPT  pSalI2CMngtAdpt -
//			I2C SAL management adapter pointer
//         
//
// Return:
//		The status of the enable process.
//          _EXIT_SUCCESS if the RtkI2CFreeMngtAdpt succeeded.
//          _EXIT_FAILURE if the RtkI2CFreeMngtAdpt failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-02.
//
//---------------------------------------------------------------------------------------------------
RTK_STATUS
RtkDACFreeMngtAdpt(
    IN  PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt
){
#if !DAC_STATIC_ALLOC
    RtlMfree((u8 *)pSalDACMngtAdpt->pUserCB->pTXCB, (sizeof(SAL_DAC_USERCB_ADPT)*SAL_DAC_USER_CB_NUM));
    RtlMfree((u8 *)pSalDACMngtAdpt->pIrqGdmaHnd, sizeof(IRQ_HANDLE));
    RtlMfree((u8 *)pSalDACMngtAdpt->pHalGdmaOp, sizeof(HAL_GDMA_OP));
    RtlMfree((u8 *)pSalDACMngtAdpt->pHalGdmaAdp, sizeof(HAL_GDMA_ADAPTER));
    RtlMfree((u8 *)pSalDACMngtAdpt->pDMAConf, sizeof(SAL_DAC_DMA_USER_DEF));
    RtlMfree((u8 *)pSalDACMngtAdpt->pUserCB, sizeof(SAL_DAC_USER_CB));
    RtlMfree((u8 *)pSalDACMngtAdpt->pIrqHnd, sizeof(IRQ_HANDLE));
    RtlMfree((u8 *)pSalDACMngtAdpt->pHalOp, sizeof(HAL_DAC_OP));
    RtlMfree((u8 *)pSalDACMngtAdpt->pHalInitDat, sizeof(HAL_DAC_INIT_DAT));
    RtlMfree((u8 *)pSalDACMngtAdpt->pSalHndPriv, sizeof(SAL_DAC_HND_PRIV));    
    RtlMfree((u8 *)pSalDACMngtAdpt, sizeof(SAL_DAC_MNGT_ADPT));
#else
    ;
#endif

    return _EXIT_SUCCESS;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		I2CISRHandle
//
// Description:
//         I2C Interrupt Service Routine.
//		According to the input pointer to SAL_I2C_HND, all the rest pointers will be 
//         found and be used to the rest part of this servie routine.
//         The following types of interrupt will be taken care:
//              - General Call  (providing General Call Callback). Slave receives a general call.
//              - STOP Bit       (NOT providing General Call Callback)
//              - START Bit     (NOTproviding General Call Callback)
//              - I2C Activity   (NOTproviding General Call Callback)
//              - RX Done       (providing Error Callback). The slave transmitter does NOT
//                                     receive a proper NACK for the end of whole transfer.
//              - TX Abort       (providing Error Call Callback). The Master/Slave 
//                                    transmitting is terminated.
//              - RD Req         (providing TX and TXC Callback). Slave gets a Read Request
//                                    and starts a slave-transmitter operation. The slave transmit
//                                    data will be written into slave TX FIFO from user data buffer.
//              - TX Empty      (providing TX and TXC Callback). Master TX FIFO is empty.
//                                    The user transmit data will be written into master TX FIFO
//                                    from user data buffer.
//              - TX Over        (providing Error Callback). Master TX FIFO is Overflow.
//              - RX Full         (providing RX and RXC Callback). Master/Slave RX FIFO contains
//                                    data. And the received data will be put into Master/Slave user
//                                    receive data buffer.
//              - RX Over        (providing Error Callback). Master/Slave RX FIFO is Overflow.
//              - RX Under      (providing Error Callback). Master/Slave RX FIFO is Underflow.
//
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		NA
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-02.
//
//----------------------------------------------------------------------------------------------------
VOID
DACISRHandle(
    IN  VOID *Data
){
#ifdef CONFIG_DEBUG_LOG_DAC_HAL
    PSAL_DAC_HND        pSalDACHND          = (PSAL_DAC_HND) Data;
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    PHAL_DAC_INIT_DAT   pHalDACInitDat      = NULL;
    PHAL_DAC_OP         pHalDACOP           = NULL;    
    PSAL_DAC_USER_CB    pSalDACUserCB       = NULL;
    u8  DACIrqIdx;
    
    /* To get the SAL_I2C_MNGT_ADPT pointer, and parse the rest pointers */
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);
    pHalDACInitDat  = pSalDACMngtAdpt->pHalInitDat;
    pHalDACOP       = pSalDACMngtAdpt->pHalOp;
    DACIrqIdx       = pHalDACInitDat->DACIdx;
    pSalDACUserCB   = pSalDACHND->pUserCB;

    DBG_DAC_INFO("DAC INTR STS:%x\n",pHalDACOP->HalDACReadReg(pHalDACInitDat, REG_DAC_INTR_STS));
    if ((pHalDACOP->HalDACReadReg(pHalDACInitDat, REG_DAC_INTR_STS)) & BIT_DAC_FIFO_STOP_ST){
        pHalDACInitDat->DACEn = DAC_DISABLE;
        pHalDACOP->HalDACEnable((void *)pHalDACInitDat);
    }
#else
    /* To reduce warning */
    PSAL_DAC_HND        pSalDACHND          = (PSAL_DAC_HND) Data;
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    PHAL_DAC_INIT_DAT   pHalDACInitDat      = NULL;
    PHAL_DAC_OP         pHalDACOP           = NULL;    


    
    /* To get the SAL_I2C_MNGT_ADPT pointer, and parse the rest pointers */
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);
    pHalDACInitDat  = pSalDACMngtAdpt->pHalInitDat;
    pHalDACOP       = pSalDACMngtAdpt->pHalOp;


    pHalDACOP->HalDACReadReg(pHalDACInitDat, REG_DAC_INTR_STS);
#endif
}

VOID
DACGDMAISRHandle(
    IN  VOID *Data
){
    
    PSAL_DAC_HND        pSalDACHND          = (PSAL_DAC_HND) Data;
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    
    PHAL_GDMA_ADAPTER   pHalDACGdmaAdapter  = NULL;
    PHAL_GDMA_OP        pHalDACGdmaOp       = NULL;
    PSAL_DAC_USER_CB    pSalDACUserCB       = NULL;

    u8  IsrTypeMap = 0;
    DBG_8195A_DAC_LVL(HAL_DAC_LVL,"%s\n",__func__);


    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);

    pHalDACGdmaAdapter  = pSalDACMngtAdpt->pHalGdmaAdp;
    pHalDACGdmaOp       = pSalDACMngtAdpt->pHalGdmaOp;
    pSalDACUserCB       = pSalDACMngtAdpt->pUserCB;

    pSalDACMngtAdpt->pHalGdmaOpInit(pHalDACGdmaOp);
    
    if ((pHalDACGdmaAdapter->MaxMuliBlock) == pHalDACGdmaAdapter->MuliBlockCunt+1) {
        pHalDACGdmaOp->HalGdmaChCleanAutoSrc(pHalDACGdmaAdapter);
        pHalDACGdmaOp->HalGdmaChDis(pHalDACGdmaAdapter);
        pSalDACHND->DevSts = DAC_STS_IDLE;
        if (pSalDACUserCB->pDMATXCCB->USERCB != NULL)
        {
            pSalDACUserCB->pDMATXCCB->USERCB((void*)pSalDACUserCB->pDMATXCCB->USERData);
        }
    }
    else {
        //pHalDACGdmaOp->HalGdmaChCleanAutoSrc(pHalDACGdmaAdapter);
        pSalDACHND->DevSts = DAC_STS_TX_ING;

        if (pSalDACUserCB->pDMATXCB->USERCB != NULL){
            pSalDACUserCB->pDMATXCB->USERCB((void*)pSalDACUserCB->pDMATXCB->USERData);}
        }

    //3 Clear Pending ISR
    IsrTypeMap = pHalDACGdmaOp->HalGdmaChIsrClean((VOID*)pHalDACGdmaAdapter);

    //3 Maintain Block Count
    if (IsrTypeMap & BlockType) {
        pHalDACGdmaAdapter->MuliBlockCunt++;
    }

}

VOID
DACGDMALLPISRHandle(
    IN  VOID *Data
){
    PSAL_DAC_HND        pSalDACHND          = (PSAL_DAC_HND) Data;
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    
    PHAL_GDMA_ADAPTER   pHalDACGdmaAdapter  = NULL;
    PHAL_GDMA_OP        pHalDACGdmaOp       = NULL;

    u8  IsrTypeMap;

    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);

    pHalDACGdmaAdapter  = pSalDACMngtAdpt->pHalGdmaAdp;
    pHalDACGdmaOp       = pSalDACMngtAdpt->pHalGdmaOp;


    pSalDACMngtAdpt->pHalGdmaOpInit(pHalDACGdmaOp);
#if 0    
    PGDMA_ADAPTER pGdmaAdapte = (PGDMA_ADAPTER) Data;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = pGdmaAdapte->pHalGdmaAdapter;
    PGDMA_CH_LLI_ELE pGdmaChLliEle;
    struct GDMA_CH_LLI *pGdmaChLli = pHalGdmaAdapter->pLlix;
    struct BLOCK_SIZE_LIST *pBlockSizeList = pHalGdmaAdapter->pBlockSizeList;
    u32 TotalBlockSize = 0;
    u8  IsrTypeMap, BlockIndex;
    u8 *pSrc = NULL, *pDst = NULL;
    DBG_8195A_DMA("Enter Gdma0 Channel 5 ISr =====>\n");
#endif

    

    if ((pHalDACGdmaAdapter->MaxMuliBlock) == pHalDACGdmaAdapter->MuliBlockCunt) {
        //HalGdmaOp.HalGdmaChCleanAutoSrc(pHalGdmaAdapter);
        //DAC0_FCTRL(OFF);  
        
        //HalGdmaOp.HalGdmaChCleanAutoDst(pHalGdmaAdapter);
        pHalDACGdmaOp->HalGdmaChDis(pHalDACGdmaAdapter);
        
        DBG_8195A("dma done\n");
    }
                

    IsrTypeMap = pHalDACGdmaOp->HalGdmaChIsrClean((VOID*)pHalDACGdmaAdapter);
    
    if (IsrTypeMap & BlockType) {   
        pHalDACGdmaAdapter->MuliBlockCunt++;       
    }
}

static RTK_STATUS
RtkDACPinMuxInit(
    IN  PSAL_DAC_HND    pSalDACHND
){
    u32 DACLocalTemp;

    /* Check the I2C index first */
    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;

    DACLocalTemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL2);
    DACLocalTemp |= BIT26;

    /* To release DAC delta sigma clock gating */
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_SYS_SYSPLL_CTRL2,DACLocalTemp);
    
    switch (pSalDACHND->DevNum){
#if DAC0_USED
        case DAC0_SEL:
        {
            /* Turn on DAC active clock */
            ACTCK_DAC_CCTRL(ON);

            /* Enable DAC0 module */
            DAC0_FCTRL(ON);            
            break;
        }
#endif
#if DAC1_USED
        case DAC1_SEL:
        {
            /* Turn on DAC active clock */
            ACTCK_DAC_CCTRL(ON);

            /* Enable DAC1 module */
            DAC1_FCTRL(ON);            
            break;
        }
#endif
        default:
            return _EXIT_FAILURE;
    }
    
    return _EXIT_SUCCESS;
}

/** \brief HalDACPinMuxInit:\n
 *		  to set DAC clock control and enable control
 *  
 *   This function is mainly to set DAC clock control and enable control.
 *   \para VOID *: Data, It's a pointer to HAL_DAC_INIT_DAT
 */
void
HalDACPinMuxInit(
    IN  VOID *Data
){
    u32 DACLocalTemp;
    PHAL_DAC_INIT_DAT   pHalInitPara = (PHAL_DAC_INIT_DAT)Data;
    
    /* Check the I2C index first */
    if (RtkDACIdxChk(pHalInitPara->DACIdx))
        return;

    DACLocalTemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL2);
    DACLocalTemp |= BIT26;

    /* To release DAC delta sigma clock gating */
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_SYS_SYSPLL_CTRL2,DACLocalTemp);
    
    switch (pHalInitPara->DACIdx){
#if DAC0_USED
        case DAC0_SEL:
        {
            /* Turn on DAC active clock */
            ACTCK_DAC_CCTRL(ON);

            /* Enable DAC0 module */
            DAC0_FCTRL(ON);            
            break;
        }
#endif
#if DAC1_USED
        case DAC1_SEL:
        {
            /* Turn on DAC active clock */
            ACTCK_DAC_CCTRL(ON);

            /* Enable DAC1 module */
            DAC1_FCTRL(ON);            
            break;
        }
#endif
        default:
            return;
    }
    
}


static RTK_STATUS
RtkDACPinMuxDeInit(
    IN  PSAL_DAC_HND    pSalDACHND
){

    u32 DACLocalTemp;

    /* Check the I2C index first */
    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;

    switch (pSalDACHND->DevNum){
#if DAC0_USED
        case DAC0_SEL:
        {
            /* Turn on DAC active clock */
            ACTCK_DAC_CCTRL(OFF);

            /* Enable DAC0 module */
            DAC0_FCTRL(OFF);            
            break;
        }
#endif
#if DAC1_USED
        case DAC1_SEL:
        {
            /* Turn on DAC active clock */
            ACTCK_DAC_CCTRL(OFF);

            /* Enable DAC1 module */
            DAC1_FCTRL(OFF);            
            break;
        }
#endif
        default:
            return _EXIT_FAILURE;
    }


    DACLocalTemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL2);
    DACLocalTemp &= (~BIT26);

    /* To release DAC delta sigma clock gating */
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_SYS_SYSPLL_CTRL2,DACLocalTemp);
    
    return _EXIT_SUCCESS;
}

/** \brief HalDACPinMuxDeInit:\n
 *		  to disable DAC clock control and enable control
 *  
 *   This function is mainly to disable DAC clock control and enable control.
 *   \para VOID *: Data, a pointer to HAL_DAC_INIT_DAT.
 */
void
HalDACPinMuxDeInit(
    IN  VOID *Data
){

    u32 DACLocalTemp;
    PHAL_DAC_INIT_DAT   pHalInitPara = (PHAL_DAC_INIT_DAT)Data;
    
    /* Check the I2C index first */
    if (RtkDACIdxChk(pHalInitPara->DACIdx))
        return;

    switch (pHalInitPara->DACIdx){
#if DAC0_USED
        case DAC0_SEL:
        {
            /* Turn on DAC active clock */
            ACTCK_DAC_CCTRL(OFF);

            /* Enable DAC0 module */
            DAC0_FCTRL(OFF);            
            break;
        }
#endif
#if DAC1_USED
        case DAC1_SEL:
        {
            /* Turn on DAC active clock */
            ACTCK_DAC_CCTRL(OFF);

            /* Enable DAC1 module */
            DAC1_FCTRL(OFF);            
            break;
        }
#endif
        default:
            return;
    }


    DACLocalTemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL2);
    DACLocalTemp &= (~BIT26);

    /* To release DAC delta sigma clock gating */
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_SYS_SYSPLL_CTRL2,DACLocalTemp);
    
    return;
}


#if DAC_INTR_OP_TYPE
//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CIrqInit
//
// Description:
//         I2C interrupt initialization function.
//         For I2C interrupt operation mode, I2C module MUST register itself to the platform
//         by providing the interrupt handler which contains interrupt input data (arguments),
//         interrupt service routine, interrupt number, interrupt priority. And then the interrupt
//         should be enabled.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C interrupt initialization process.
//          _EXIT_SUCCESS if the RtkI2CIrqInit succeeded.
//          _EXIT_FAILURE if the RtkI2CIrqInit failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
static RTK_STATUS
RtkDACIrqInit(
    IN  PSAL_DAC_HND    pSalDACHND
){
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    PIRQ_HANDLE         pIrqHandle          = NULL;
    
    /* To Get the SAL_I2C_MNGT_ADPT Pointer */
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);
    pIrqHandle      = pSalDACMngtAdpt->pIrqHnd;
    
    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;

    switch (pSalDACHND->DevNum){
#if DAC0_USED
        case DAC0_SEL:
        {
            pIrqHandle->Data = (u32) (pSalDACHND);
            pIrqHandle->IrqNum = DAC0_IRQ;
            pIrqHandle->IrqFun = (IRQ_FUN) pSalDACMngtAdpt->pSalIrqFunc;
            pIrqHandle->Priority = 5;
            InterruptRegister(pIrqHandle);
            InterruptEn(pIrqHandle);
            break;
        }
#endif
#if DAC1_USED
        case DAC1_SEL:
        {
            pIrqHandle->Data = (u32) (pSalDACHND);
            pIrqHandle->IrqNum = DAC1_IRQ;
            pIrqHandle->IrqFun = (IRQ_FUN) pSalDACMngtAdpt->pSalIrqFunc;;
            pIrqHandle->Priority = 5;
            InterruptRegister(pIrqHandle);
            InterruptEn(pIrqHandle);
            break;
        }
#endif
        default:
            return _EXIT_FAILURE;
    }

    
    
    return _EXIT_SUCCESS;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CIrqDeInit
//
// Description:
//         I2C interrupt de-initialization function.
//         According to the given I2C device number, the I2C interrupt will be unreigster 
//         from the platform and the relative interrupt handler will be cleared.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C interrupt de-initialization process.
//          _EXIT_SUCCESS if the RtkI2CIrqDeInit succeeded.
//          _EXIT_FAILURE if the RtkI2CIrqDeInit failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
static RTK_STATUS
RtkDACIrqDeInit(
    IN  PSAL_DAC_HND    pSalDACHND
){
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    PIRQ_HANDLE         pIrqHandle          = NULL;
    
    /*To Get the SAL_I2C_MNGT_ADPT Pointer*/
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);
    pIrqHandle      = pSalDACMngtAdpt->pIrqHnd;
    
    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;

    InterruptUnRegister(pIrqHandle);
    return _EXIT_SUCCESS;
}

#endif


#if DAC_DMA_OP_TYPE
const u16 DACDmaChNo[10] = {GdmaNoCh   ,GdmaCh0,
                             GdmaCh1    ,GdmaCh2,
                             GdmaCh3    ,GdmaCh4,
                             GdmaCh5    ,GdmaCh6,
                             GdmaCh7    ,GdmaAllCh};

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CIrqInit
//
// Description:
//         I2C interrupt initialization function.
//         For I2C interrupt operation mode, I2C module MUST register itself to the platform
//         by providing the interrupt handler which contains interrupt input data (arguments),
//         interrupt service routine, interrupt number, interrupt priority. And then the interrupt
//         should be enabled.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C interrupt initialization process.
//          _EXIT_SUCCESS if the RtkI2CIrqInit succeeded.
//          _EXIT_FAILURE if the RtkI2CIrqInit failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
static RTK_STATUS
RtkDACDMAInit(
    IN  PSAL_DAC_HND    pSalDACHND
){
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    PHAL_GDMA_ADAPTER   pHALDACGdmaAdpt     = NULL;
    PHAL_GDMA_OP        pHALDACGdmaOp       = NULL;
    PIRQ_HANDLE         pIrqHandleDACGdma   = NULL;
    PSAL_DAC_DMA_USER_DEF pSalDACDmaUserDef = NULL;


    
    /* To Get the SAL_I2C_MNGT_ADPT Pointer */
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);
    
    pHALDACGdmaAdpt     = pSalDACMngtAdpt->pHalGdmaAdp;
    pHALDACGdmaOp       = pSalDACMngtAdpt->pHalGdmaOp;
    pIrqHandleDACGdma   = pSalDACMngtAdpt->pIrqGdmaHnd;
    pSalDACDmaUserDef   = pSalDACHND->pDMAConf;
    
    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;

    if (pSalDACHND->DACInType == DAC_INPUT_SINGLE_WR)
        return _EXIT_SUCCESS;

    /* GDMA operation initialization */
    //HalGdmaOpInit(pHalI2CGdmaOp);
    _memset((void *)pHALDACGdmaAdpt, 0, sizeof(HAL_GDMA_ADAPTER));
    pSalDACMngtAdpt->pHalGdmaOpInit(pHALDACGdmaOp);    
    pHALDACGdmaOp->HalGdamChInit((VOID*)(pHALDACGdmaAdpt));

    

    pHALDACGdmaAdpt->GdmaIndex          = pSalDACHND->DevNum;
    pHALDACGdmaAdpt->GdmaCtl.IntEn      = 1;
    pHALDACGdmaAdpt->ChNum              = pSalDACDmaUserDef->TxChNo;
    
    pHALDACGdmaAdpt->ChEn               = DACDmaChNo[pHALDACGdmaAdpt->ChNum+1];
    pHALDACGdmaAdpt->IsrCtrl            = ENABLE;
    pHALDACGdmaAdpt->GdmaOnOff          = ON;

    
    /* GDMA initialization */
    /* Enable the whole GDMA module first */
    if (pHALDACGdmaAdpt->GdmaIndex == 0) {        
        ACTCK_GDMA0_CCTRL(ON);
        SLPCK_GDMA0_CCTRL(ON);
        GDMA0_FCTRL(ON);
    }
    else {        
        ACTCK_GDMA1_CCTRL(ON);
        SLPCK_GDMA1_CCTRL(ON);
        GDMA1_FCTRL(ON);
    }
    
    if (pSalDACHND->DACInType == DAC_INPUT_DMA_ONEBLK) {
        //DAC TX DMA
        pHALDACGdmaAdpt->GdmaCtl.SrcTrWidth = pSalDACDmaUserDef->TxDatSrcWdth;
        pHALDACGdmaAdpt->GdmaCtl.DstTrWidth = pSalDACDmaUserDef->TxDatDstWdth;
        pHALDACGdmaAdpt->GdmaCtl.SrcMsize   = pSalDACDmaUserDef->TxDatSrcBstSz;
        pHALDACGdmaAdpt->GdmaCtl.DestMsize  = pSalDACDmaUserDef->TxDatDstBstSz;

        pHALDACGdmaAdpt->GdmaCtl.Sinc       = IncType;
        pHALDACGdmaAdpt->GdmaCtl.Dinc       = NoChange;	

        pHALDACGdmaAdpt->GdmaCtl.Done       = 1;
        pHALDACGdmaAdpt->GdmaCtl.TtFc       = 0x01;
        
        pHALDACGdmaAdpt->GdmaCfg.DestPer    = 13;
        pHALDACGdmaAdpt->GdmaCfg.ReloadSrc  = 1;

        pHALDACGdmaAdpt->MuliBlockCunt      = 1;
        pHALDACGdmaAdpt->MaxMuliBlock       = pSalDACHND->pDMAConf->MaxMultiBlk;
        
        pHALDACGdmaAdpt->GdmaIsrType        = (BlockType|TransferType|ErrType);    

        
        pHALDACGdmaAdpt->TestItem           = 3;
        
        
        //pSalDACMngtAdpt->pSalDMAIrqFunc     = &DACGDMAISRHandle;
    }
    else if (pSalDACHND->DACInType == DAC_INPUT_DMA_LLP) {
        //DAC TX DMA
        pHALDACGdmaAdpt->GdmaCtl.SrcTrWidth = pSalDACDmaUserDef->TxDatSrcWdth;
        pHALDACGdmaAdpt->GdmaCtl.DstTrWidth = pSalDACDmaUserDef->TxDatDstWdth;
        pHALDACGdmaAdpt->GdmaCtl.SrcMsize   = pSalDACDmaUserDef->TxDatSrcBstSz;
        pHALDACGdmaAdpt->GdmaCtl.DestMsize  = pSalDACDmaUserDef->TxDatDstBstSz;

        pHALDACGdmaAdpt->GdmaCtl.Dinc       = NoChange;

        pHALDACGdmaAdpt->GdmaCtl.Done       = 1;
        pHALDACGdmaAdpt->GdmaCtl.TtFc       = 0x01;
        pHALDACGdmaAdpt->GdmaCtl.LlpSrcEn   = 1;
        
        pHALDACGdmaAdpt->GdmaCfg.DestPer    = 13;

        pHALDACGdmaAdpt->GdmaIsrType        = (BlockType|ErrType);

        /* Enable LLP control */
        pHALDACGdmaAdpt->Llpctrl            = pSalDACDmaUserDef->LlpCtrl;

        pHALDACGdmaAdpt->MuliBlockCunt      = 1;
        pHALDACGdmaAdpt->MaxMuliBlock       = pSalDACDmaUserDef->MaxMultiBlk;
        
        pHALDACGdmaAdpt->TestItem           = 9;

        //pSalDACMngtAdpt->pSalDMAIrqFunc     = &DACGDMALLPISRHandle;
    }
    

    /* GDMA interrupt register */
    pIrqHandleDACGdma->Data     = (u32) (pSalDACHND);  
    pIrqHandleDACGdma->IrqNum   = GDMA0_CHANNEL0_IRQ + pHALDACGdmaAdpt->ChNum + 
                                             ((pHALDACGdmaAdpt->GdmaIndex)*6);
    pIrqHandleDACGdma->IrqFun   = (IRQ_FUN) pSalDACMngtAdpt->pSalDMAIrqFunc;
    pIrqHandleDACGdma->Priority = 6;
    InterruptRegister(pIrqHandleDACGdma);
    InterruptEn(pIrqHandleDACGdma);

    
    pHALDACGdmaOp->HalGdmaOnOff((VOID*)pHALDACGdmaAdpt);
    pHALDACGdmaOp->HalGdmaChIsrEnAndDis((VOID*)pHALDACGdmaAdpt);

#if 0
    /* Enable GDMA according to the DMA type */
    if (pSalDACHND->DACInType == DAC_INPUT_DMA_ONEBLK) {
        pHALDACGdmaOp->HalGdmaChSeting((VOID*)pHALDACGdmaAdpt);
    }
    else if (pSalDACHND->DACInType == DAC_INPUT_DMA_LLP){
        //pHALDACGdmaOp->HalGdmaChBlockSeting((VOID*)(pHALDACGdmaAdpt));
    }
#endif
    return _EXIT_SUCCESS;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CIrqDeInit
//
// Description:
//         I2C interrupt de-initialization function.
//         According to the given I2C device number, the I2C interrupt will be unreigster 
//         from the platform and the relative interrupt handler will be cleared.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C interrupt de-initialization process.
//          _EXIT_SUCCESS if the RtkI2CIrqDeInit succeeded.
//          _EXIT_FAILURE if the RtkI2CIrqDeInit failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
static RTK_STATUS
RtkDACDMADeInit(
    IN  PSAL_DAC_HND    pSalDACHND
){
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;

    PHAL_GDMA_ADAPTER   pHALDACGdmaAdpt     = NULL;
    PHAL_GDMA_OP        pHALDACGdmaOp       = NULL;
    PIRQ_HANDLE         pIrqHandleDACGdma   = NULL;
    
    /*To Get the SAL_I2C_MNGT_ADPT Pointer*/
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);
    
    pHALDACGdmaAdpt     = pSalDACMngtAdpt->pHalGdmaAdp;
    pHALDACGdmaOp       = pSalDACMngtAdpt->pHalGdmaOp;
    pIrqHandleDACGdma   = pSalDACMngtAdpt->pIrqGdmaHnd;

    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;

    HalGdmaOpInit(pHALDACGdmaOp);
        
    pHALDACGdmaAdpt->IsrCtrl    = DISABLE;
    pHALDACGdmaOp->HalGdmaChIsrEnAndDis((VOID*)pHALDACGdmaAdpt);
    pHALDACGdmaOp->HalGdmaChIsrClean((VOID*)pHALDACGdmaAdpt);
    pHALDACGdmaOp->HalGdmaChDis((VOID*)pHALDACGdmaAdpt);
     
    InterruptUnRegister(pIrqHandleDACGdma);
#if 0
    _memset((void *)pIrqHandleDACGdma , 0, sizeof(IRQ_HANDLE));
    _memset((void *)pHALDACGdmaOp     , 0, sizeof(HAL_GDMA_OP));
    _memset((void *)pHALDACGdmaAdpt   , 0, sizeof(HAL_GDMA_ADAPTER));
#endif    
    return _EXIT_SUCCESS;
}

#endif

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CInit
//
// Description:
//         According to the given I2C index, the related SAL_I2C_MNGT_ADPT pointer used 
//         for retrieving each I2C data sturcture pointer will be reversely parsed first.
//         Then, initializing I2C HAL operation, initializing I2C interrupt (if needed), 
//         initializing I2C DMA (if needed) and initializing I2C pinmux will be done.
//         User specified I2C configuration will be assigned to I2C initial data structure
//         (PHAL_I2C_INIT_DAT   pHalI2CInitDat). I2C HAL initialization is executed after
//         all the configuration data taken.
//         In the end, I2C module is enabled as a final step of the whole initialization.
//         For a slave ack General Call support, an additional step may be followed after
//         the above steps.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C initialization process.
//          _EXIT_SUCCESS if the RtkI2CInit succeeded.
//          _EXIT_FAILURE if the RtkI2CInit failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
RTK_STATUS
RtkDACInit(
    IN  VOID *Data
){
    PSAL_DAC_HND        pSalDACHND          = (PSAL_DAC_HND) Data;
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    //PHAL_DAC_INIT_DAT   pHalDACInitDat      = NULL;
    PHAL_DAC_OP         pHalDACOP           = NULL;    

    u32 DacTemp;
    
    /* To Get the SAL_I2C_MNGT_ADPT Pointer */
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);
    
    //pHalDACInitDat  = pSalDACMngtAdpt->pHalInitDat;
    pHalDACOP       = pSalDACMngtAdpt->pHalOp;

    /* Check the input I2C index first */
    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;

#if 0
    /* Check the input I2C operation type */
    if (RtkI2COpTypeChk(pSalI2CHND))
        return _EXIT_FAILURE;
#endif

    /* DAC Initialize HAL Operations */
    HalDACOpInit(pHalDACOP);
    
    /* DAC Interrupt Initialization */
#if DAC_INTR_OP_TYPE
    RtkDACIrqInit(pSalDACHND);
#endif

    /* DAC DMA Initialization */
#if DAC_DMA_OP_TYPE
    RtkDACDMAInit(pSalDACHND);
#endif

    
    /* DAC Function and Clock Enable*/
    RtkDACPinMuxInit(pSalDACHND);

    pHalDACOP->HalDACInit(pSalDACHND->pInitDat);
    
   #if 1 
        HAL_DAC_WRITE32(pSalDACHND->DevNum, REG_DAC_INTR_CTRL, 
                                        (BIT_DAC_FIFO_FULL_EN       |
                                         BIT_DAC_FIFO_OVERFLOW_EN   |
                                         BIT_DAC_FIFO_STOP_EN       |
                                         BIT_DAC__WRITE_ERROR_EN    |
                                         BIT_DAC_DSC_OVERFLOW0_EN   |
                                         BIT_DAC_DSC_OVERFLOW1_EN));
    #else
        HAL_DAC_WRITE32(pSalDACHND->DevNum, REG_DAC_INTR_CTRL,                                      
                                         (BIT_DAC_FIFO_FULL_EN|
                                          BIT_DAC_FIFO_OVERFLOW_EN|                                          
                                          BIT_DAC_FIFO_STOP_EN|
                                          BIT_DAC__WRITE_ERROR_EN|
                                          BIT_DAC_DSC_OVERFLOW0_EN|
                                          BIT_DAC_DSC_OVERFLOW1_EN));
    #endif
    DBG_DAC_INFO("INTR MSK:%x\n", HAL_DAC_READ32(pSalDACHND->DevNum,REG_DAC_INTR_CTRL));



    DacTemp = HAL_DAC_READ32(pSalDACHND->DevNum, REG_DAC_ANAPAR_DA1);
    DacTemp |= (BIT31);
    HAL_DAC_WRITE32(pSalDACHND->DevNum, REG_DAC_ANAPAR_DA1, DacTemp);
    //DBG_DAC_INFO("REG_DAC_ANAPAR_DA1:%08x\n",DacTemp);
    DBG_DAC_INFO("REG_DAC_ANAPAR_DA1:%08x\n",HAL_DAC_READ32(pSalDACHND->DevNum, REG_DAC_ANAPAR_DA1));
    DacTemp = HAL_DAC_READ32(pSalDACHND->DevNum, REG_DAC_CTRL);
    DacTemp |= BIT3;
    HAL_DAC_WRITE32(pSalDACHND->DevNum, REG_DAC_CTRL, DacTemp);
    DBG_DAC_INFO("REG_DAC_CTRL:%08x\n",DacTemp);
    
    pSalDACHND->pInitDat->DACEn = DAC_ENABLE;
    pHalDACOP->HalDACEnable(pSalDACHND->pInitDat);
    
    /* DAC Device Status Update */
    pSalDACHND->DevSts  = DAC_STS_IDLE;    
    
    return _EXIT_SUCCESS;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CInit
//
// Description:
//         According to the given I2C index, the related SAL_I2C_MNGT_ADPT pointer used 
//         for retrieving each I2C data sturcture pointer will be reversely parsed first.
//         Then, initializing I2C HAL operation, initializing I2C interrupt (if needed), 
//         initializing I2C DMA (if needed) and initializing I2C pinmux will be done.
//         User specified I2C configuration will be assigned to I2C initial data structure
//         (PHAL_I2C_INIT_DAT   pHalI2CInitDat). I2C HAL initialization is executed after
//         all the configuration data taken.
//         In the end, I2C module is enabled as a final step of the whole initialization.
//         For a slave ack General Call support, an additional step may be followed after
//         the above steps.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C initialization process.
//          _EXIT_SUCCESS if the RtkI2CInit succeeded.
//          _EXIT_FAILURE if the RtkI2CInit failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
RTK_STATUS
RtkDACDeInit(
    IN  VOID *Data
){
    PSAL_DAC_HND        pSalDACHND          = (PSAL_DAC_HND) Data;
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    PHAL_DAC_INIT_DAT   pHalDACInitDat      = NULL;
    PHAL_DAC_OP         pHalDACOP           = NULL;    

    /* To Get the SAL_DAC_MNGT_ADPT Pointer */
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);
    
    pHalDACInitDat  = pSalDACMngtAdpt->pHalInitDat;
    pHalDACOP       = pSalDACMngtAdpt->pHalOp;

    /* Check the input DAC index first */
    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;
        
    {
        /* DAC Initialize HAL Operations */
        HalDACOpInit(pHalDACOP);

        /* DAC Interrupt Initialization */
#if DAC_INTR_OP_TYPE
        RtkDACIrqDeInit(pSalDACHND);
#endif

        /* DAC DMA Initialization */
#if DAC_DMA_OP_TYPE
        RtkDACDMADeInit(pSalDACHND);
#endif

        pHalDACInitDat->DACEn   =   DAC_DISABLE;
        pHalDACOP->HalDACEnable(pHalDACInitDat);
        pHalDACOP->HalDACDeInit(pHalDACInitDat);
        
        /* DAC Function and Clock Enable*/
        RtkDACPinMuxDeInit(pSalDACHND);       
    }
    
    return _EXIT_SUCCESS;
}


//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CInit
//
// Description:
//         According to the given I2C index, the related SAL_I2C_MNGT_ADPT pointer used 
//         for retrieving each I2C data sturcture pointer will be reversely parsed first.
//         Then, initializing I2C HAL operation, initializing I2C interrupt (if needed), 
//         initializing I2C DMA (if needed) and initializing I2C pinmux will be done.
//         User specified I2C configuration will be assigned to I2C initial data structure
//         (PHAL_I2C_INIT_DAT   pHalI2CInitDat). I2C HAL initialization is executed after
//         all the configuration data taken.
//         In the end, I2C module is enabled as a final step of the whole initialization.
//         For a slave ack General Call support, an additional step may be followed after
//         the above steps.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C initialization process.
//          _EXIT_SUCCESS if the RtkI2CInit succeeded.
//          _EXIT_FAILURE if the RtkI2CInit failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
RTK_STATUS
RtkDACSend(
    IN  VOID *Data
){
    PSAL_DAC_HND        pSalDACHND          = (PSAL_DAC_HND) Data;
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    PHAL_GDMA_ADAPTER   pHALDACGdmaAdpt     = NULL;
    PHAL_GDMA_OP        pHALDACGdmaOp       = NULL;
    PSAL_DAC_DMA_USER_DEF pSalDACDmaUserDef = NULL;
    //PIRQ_HANDLE         pIrqHandleDACGdma   = NULL;
    
    /* To Get the SAL_I2C_MNGT_ADPT Pointer */
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);
    
    pHALDACGdmaAdpt     = pSalDACMngtAdpt->pHalGdmaAdp;
    pHALDACGdmaOp       = pSalDACMngtAdpt->pHalGdmaOp;
    pSalDACDmaUserDef   = pSalDACMngtAdpt->pDMAConf;
    
    switch (pSalDACHND->DACInType) {
        case DAC_INPUT_SINGLE_WR:
        {
            break;
        }
        case DAC_INPUT_DMA_ONEBLK:
        {
            HalGdmaOpInit(pHALDACGdmaOp);
            
            pHALDACGdmaAdpt->GdmaCtl.BlockSize  = pSalDACHND->pTXBuf->DataLen;
            pHALDACGdmaAdpt->ChSar              =   (u32)pSalDACHND->pTXBuf->pDataBuf;
            pHALDACGdmaAdpt->ChDar              =   (u32)(DAC_REG_BASE+(pSalDACHND->DevNum*0x800));  
            
            DBG_DAC_INFO("src addr:%x\n", pHALDACGdmaAdpt->ChSar);
            DBG_DAC_INFO("dst addr:%x\n", pHALDACGdmaAdpt->ChDar);
            pHALDACGdmaOp->HalGdmaChSeting(pHALDACGdmaAdpt);
            
            pHALDACGdmaOp->HalGdmaChEn(pHALDACGdmaAdpt);
            break;
        }
        case DAC_INPUT_DMA_LLP:
        {
            pHALDACGdmaAdpt->Rsvd4to7           = 1;
            pHALDACGdmaAdpt->pLlix              = (struct GDMA_CH_LLI *)pSalDACDmaUserDef->pLlix;
            pHALDACGdmaAdpt->pBlockSizeList     = (struct BLOCK_SIZE_LIST *)pSalDACDmaUserDef->pBlockSizeList;
            pHALDACGdmaAdpt->ChDar              =   (u32)(DAC_REG_BASE+(pSalDACHND->DevNum*0x800)); 
            pHALDACGdmaOp->HalGdmaChBlockSeting(pHALDACGdmaAdpt);
            
            pHALDACGdmaOp->HalGdmaChEn(pHALDACGdmaAdpt);
            break;
        }

        default:
            return _EXIT_FAILURE;
    } 


    return _EXIT_SUCCESS;
}


//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CGetSalHnd
//
// Description:
//         Allocation of lower layer memory spaces will be done by invoking RtkI2CGetMngtAdpt
//         in this function and return a SAL_I2C_HND pointer to upper layer.
//         According to the given I2C index, RtkI2CGetMngtAdpt will allocate all the memory
//         space such as SAL_I2C_HND, HAL_I2C_INIT_DAT, SAL_I2C_USER_CB etc.
//         
//
// Arguments:
//		[in] u8 I2CIdx -
//			I2C Index
//
// Return:
//		PSAL_I2C_HND
//               A pointer to SAL_I2C_HND which is allocated in the lower layer.
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
PSAL_DAC_HND
RtkDACGetSalHnd(
    IN  u8  DACIdx
){
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt = NULL;
    PSAL_DAC_HND        pSalDACHND      = NULL;

    /* Check the user define setting and the given index */
    if (RtkDACIdxChk(DACIdx)) {
        return (PSAL_DAC_HND)NULL;
    }

    /* Invoke RtkI2CGetMngtAdpt to get the I2C SAL management adapter pointer */
    pSalDACMngtAdpt = RtkDACGetMngtAdpt(DACIdx);

    /* Assign the private SAL handle to public SAL handle */
    pSalDACHND      = &(pSalDACMngtAdpt->pSalHndPriv->SalDACHndPriv);

    /* Assign the internal HAL initial data pointer to the SAL handle */
    pSalDACHND->pInitDat    = pSalDACMngtAdpt->pHalInitDat;

    /* Assign the internal user callback pointer to the SAL handle */
    pSalDACHND->pUserCB     = pSalDACMngtAdpt->pUserCB;

    /* Assign the internal user DMA config to the SAL handle */
    pSalDACHND->pDMAConf    = pSalDACMngtAdpt->pDMAConf;
    
    return &(pSalDACMngtAdpt->pSalHndPriv->SalDACHndPriv);
}


//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CGetSalHnd
//
// Description:
//         Allocation of lower layer memory spaces will be done by invoking RtkI2CGetMngtAdpt
//         in this function and return a SAL_I2C_HND pointer to upper layer.
//         According to the given I2C index, RtkI2CGetMngtAdpt will allocate all the memory
//         space such as SAL_I2C_HND, HAL_I2C_INIT_DAT, SAL_I2C_USER_CB etc.
//         
//
// Arguments:
//		[in] u8 I2CIdx -
//			I2C Index
//
// Return:
//		PSAL_I2C_HND
//               A pointer to SAL_I2C_HND which is allocated in the lower layer.
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
RTK_STATUS
RtkDACFreeSalHnd(
    IN  PSAL_DAC_HND    pSalDACHND
){
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;

    /* To get the SAL_DAC_MNGT_ADPT pointer */
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);

    /* Invoke RtkDACFreeMngtAdpt to free all the lower layer memory space */
    return (RtkDACFreeMngtAdpt(pSalDACMngtAdpt));

}


//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CLoadDefault
//
// Description:
//         Accrording the given I2C index, the default I2C configuration is done.
//         
//
// Arguments:
//		[in] PSAL_I2C_HND    pSalI2CHND -
//			SAL I2C handle
//
// Return:
//		The status of the loading I2C default configuration.
//          _EXIT_SUCCESS if the RtkI2CLoadDefault succeeded.
//          _EXIT_FAILURE if the RtkI2CLoadDefault failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
RTK_STATUS
RtkDACLoadDefault(
    IN  VOID *Data
){
    PSAL_DAC_HND    pSalDACHND = (PSAL_DAC_HND) Data;

    /* Check the input DAC index first */
    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;

    /* Load SAL handle default value */
    pSalDACHND->PinMux          =   0;
    pSalDACHND->OpType          =   DAC_POLL_TYPE;    
    pSalDACHND->DevSts          =   DAC_STS_UNINITIAL;
    pSalDACHND->DACExd          =   0;
    pSalDACHND->ErrType         =   (u32)NULL;    

    /* Load HAL initial data structure default value */
    pSalDACHND->pInitDat->DACIdx        =   pSalDACHND->DevNum;
    pSalDACHND->pInitDat->DACEn         =   DAC_DISABLE;
    pSalDACHND->pInitDat->DACDataRate   =   DAC_DATA_RATE_250K;
    pSalDACHND->pInitDat->DACEndian     =   DAC_DATA_ENDIAN_LITTLE;
    pSalDACHND->pInitDat->DACBurstSz    =   7;
    pSalDACHND->pInitDat->DACDbgSel     =   DAC_DBG_SEL_DISABLE;
    pSalDACHND->pInitDat->DACDscDbgSel  =   DAC_DSC_DBG_SEL_DISABLE;
    pSalDACHND->pInitDat->DACBPDsc      =   DAC_BYPASS_DSC_SEL_DISABLE;
    pSalDACHND->pInitDat->DACDeltaSig   =   0;
    pSalDACHND->pInitDat->DACAnaCtrl0   =   0;
    pSalDACHND->pInitDat->DACAnaCtrl1   =   0;
    pSalDACHND->pInitDat->DACIntrMSK    =   DAC_FEATURE_DISABLED;

    /* Load DAC DMA user configuration default value */
    pSalDACHND->pDMAConf->MaxMultiBlk   =   5000;
    pSalDACHND->pDMAConf->TxDatSrcWdth  =   TrWidthFourBytes;
    pSalDACHND->pDMAConf->TxDatSrcBstSz =   MsizeFour;
    pSalDACHND->pDMAConf->TxDatDstWdth  =   TrWidthFourBytes;
    pSalDACHND->pDMAConf->TxDatDstBstSz =   MsizeFour;
    pSalDACHND->pDMAConf->TxChNo        =   4;
    
    return _EXIT_SUCCESS;
}
