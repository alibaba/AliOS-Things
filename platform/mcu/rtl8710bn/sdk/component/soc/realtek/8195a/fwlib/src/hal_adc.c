/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
 
#include "basic_types.h"
#include "diag.h"
#include "rand.h"
#include "section_config.h"
#include "rtl_utility.h"
#include "osdep_api.h"
#include "hal_adc.h"
#include "hal_gdma.h"
#include "hal_timer.h"

#define ADC_STATIC_ALLOC      0

static volatile u32 ADCDatBuf[2];
static volatile u8  ADCFullStsFlag;
static RTK_STATUS
RtkADCPinMuxDeInit(
    IN  PSAL_ADC_HND    pSalADCHND
);

static RTK_STATUS
RtkADCIrqDeInit(
    IN  PSAL_ADC_HND    pSalADCHND
);

static RTK_STATUS
RtkADCDMADeInit(
    IN  PSAL_ADC_HND    pSalADCHND
);
/* DAC SAL global variables declaration when kernel disabled */
#ifndef CONFIG_KERNEL
    SRAM_BF_DATA_SECTION
    HAL_ADC_OP HalADCOpSAL;
#endif

#if ADC0_USED   /*#if ADC0_USED*/    
#if ADC_STATIC_ALLOC
    SRAM_BF_DATA_SECTION
    SAL_ADC_MNGT_ADPT   SalADC0MngtAdpt;
    
    SRAM_BF_DATA_SECTION
    SAL_ADC_HND_PRIV    SalADC0HndPriv;

    SRAM_BF_DATA_SECTION
    HAL_ADC_INIT_DAT    HalADC0InitData;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE          ADC0IrqHandleDat;

    SRAM_BF_DATA_SECTION
    HAL_GDMA_ADAPTER    HalADC0GdmaAdpt;

    SRAM_BF_DATA_SECTION
    HAL_GDMA_OP         HalADC0GdmaOp;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE          ADC0GDMAIrqHandleDat;
    
    SRAM_BF_DATA_SECTION
    SAL_ADC_USER_CB     SalADC0UserCB;
    
    SRAM_BF_DATA_SECTION
    SAL_ADC_USERCB_ADPT SalADC0UserCBAdpt[SAL_ADC_USER_CB_NUM];
#endif    
#endif          /*#if ADC0_USED*/

#if ADC1_USED   /*#if ADC1_USED*/    
#if ADC_STATIC_ALLOC
    SRAM_BF_DATA_SECTION
    SAL_ADC_MNGT_ADPT   SalADC1MngtAdpt;
    
    SRAM_BF_DATA_SECTION
    SAL_ADC_HND_PRIV    SalADC1HndPriv;

    SRAM_BF_DATA_SECTION
    HAL_ADC_INIT_DAT    HalADC1InitData;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE          ADC1IrqHandleDat;

    SRAM_BF_DATA_SECTION
    HAL_GDMA_ADAPTER    HalADC1GdmaAdpt;

    SRAM_BF_DATA_SECTION
    HAL_GDMA_OP         HalADC1GdmaOp;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE          ADC1GDMAIrqHandleDat;
    
    SRAM_BF_DATA_SECTION
    SAL_ADC_USER_CB     SalADC1UserCB;
    
    SRAM_BF_DATA_SECTION
    SAL_ADC_USERCB_ADPT SalADC1UserCBAdpt[SAL_ADC_USER_CB_NUM];
#endif    
#endif          /*#if ADC1_USED*/

#if ADC2_USED   /*#if ADC2_USED*/    
#if ADC_STATIC_ALLOC
    SRAM_BF_DATA_SECTION
    SAL_ADC_MNGT_ADPT   SalADC2MngtAdpt;
    
    SRAM_BF_DATA_SECTION
    SAL_ADC_HND_PRIV    SalADC2HndPriv;

    SRAM_BF_DATA_SECTION
    HAL_ADC_INIT_DAT    HalADC2InitData;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE          ADC2IrqHandleDat;

    SRAM_BF_DATA_SECTION
    HAL_GDMA_ADAPTER    HalADC2GdmaAdpt;

    SRAM_BF_DATA_SECTION
    HAL_GDMA_OP         HalADC2GdmaOp;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE          ADC2GDMAIrqHandleDat;
    
    SRAM_BF_DATA_SECTION
    SAL_ADC_USER_CB     SalADC2UserCB;
    
    SRAM_BF_DATA_SECTION
    SAL_ADC_USERCB_ADPT SalADC2UserCBAdpt[SAL_ADC_USER_CB_NUM];
#endif    
#endif          /*#if ADC2_USED*/

#if ADC3_USED   /*#if ADC3_USED*/    
#if ADC_STATIC_ALLOC
    SRAM_BF_DATA_SECTION
    SAL_ADC_MNGT_ADPT   SalADC3MngtAdpt;
    
    SRAM_BF_DATA_SECTION
    SAL_ADC_HND_PRIV    SalADC3HndPriv;

    SRAM_BF_DATA_SECTION
    HAL_ADC_INIT_DAT    HalADC3InitData;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE          ADC3IrqHandleDat;

    SRAM_BF_DATA_SECTION
    HAL_GDMA_ADAPTER    HalADC3GdmaAdpt;

    SRAM_BF_DATA_SECTION
    HAL_GDMA_OP         HalADC3GdmaOp;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE          ADC3GDMAIrqHandleDat;
    
    SRAM_BF_DATA_SECTION
    SAL_ADC_USER_CB     SalADC3UserCB;
    
    SRAM_BF_DATA_SECTION
    SAL_ADC_USERCB_ADPT SalADC3UserCBAdpt[SAL_ADC_USER_CB_NUM];
#endif    
#endif          /*#if ADC3_USED*/

/* Global variables */
u8  SalAdcInitialFlag = 0;
#ifdef CONFIG_SOC_PS_MODULE
u8  SalAdcEnableState = 0;
#endif

HAL_ADC_INIT_DAT        SalAdcInitialDatKeep = {.ADCIdx         = 0,
                                                .ADCEn          = 0,
                                                .ADCEndian      = 0,
                                                .ADCBurstSz     = 0,
                                                .ADCCompOnly    = 0,
                                                .ADCOneShotEn   = 0,
                                                .ADCOverWREn    = 0,
                                                .ADCOneShotTD   = 0,
                                                .ADCCompCtrl    = 0,
                                                .ADCCompTD      = 0,
                                                .ADCDataRate    = 0,
                                                .ADCAudioEn     = 0,
                                                .ADCEnManul     = 0,
                                                .ADCDbgSel      = 0,
                                                .RSVD0          = 0,
                                                .ADCData        = (u32 *)NULL,
                                                .ADCPWCtrl      = 0,
                                                .ADCIntrMSK     = 0};

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
VOID HalADCOpInit(
    IN  VOID *Data
)
{
    PHAL_ADC_OP pHalAdcOp = (PHAL_ADC_OP) Data;

    pHalAdcOp->HalADCInit       =   HalADCInit8195a;
    pHalAdcOp->HalADCDeInit     =   HalADCDeInit8195a;
    pHalAdcOp->HalADCEnable     =   HalADCEnableRtl8195a;
    pHalAdcOp->HalADCReceive    =   HalADCReceiveRtl8195a;
    pHalAdcOp->HalADCIntrCtrl   =   HalADCIntrCtrl8195a;
    pHalAdcOp->HalADCReadReg    =   HalADCReadRegRtl8195a;
}

#ifndef CONFIG_MBED_ENABLED
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
PSAL_ADC_MNGT_ADPT
RtkADCGetMngtAdpt(
    IN  u8  ADCIdx
){
    PSAL_ADC_MNGT_ADPT      pSalADCMngtAdpt     = NULL;
    PSAL_ADC_USERCB_ADPT    pSalADCUserCBAdpt   = NULL;

    /* If the kernel is available, Memory-allocation is used. */
#if (!ADC_STATIC_ALLOC)
    pSalADCMngtAdpt = (PSAL_ADC_MNGT_ADPT)RtlZmalloc(sizeof(SAL_ADC_MNGT_ADPT));
    pSalADCMngtAdpt->pSalHndPriv    = (PSAL_ADC_HND_PRIV)RtlZmalloc(sizeof(SAL_ADC_HND_PRIV));
    pSalADCMngtAdpt->pHalInitDat    = (PHAL_ADC_INIT_DAT)RtlZmalloc(sizeof(HAL_ADC_INIT_DAT));
    pSalADCMngtAdpt->pHalOp         = (PHAL_ADC_OP)RtlZmalloc(sizeof(HAL_ADC_OP));
    pSalADCMngtAdpt->pIrqHnd        = (PIRQ_HANDLE)RtlZmalloc(sizeof(IRQ_HANDLE));
    pSalADCMngtAdpt->pUserCB        = (PSAL_ADC_USER_CB)RtlZmalloc(sizeof(SAL_ADC_USER_CB));
    pSalADCMngtAdpt->pHalGdmaAdp    = (PHAL_GDMA_ADAPTER)RtlZmalloc(sizeof(HAL_GDMA_ADAPTER));
    pSalADCMngtAdpt->pHalGdmaOp     = (PHAL_GDMA_OP)RtlZmalloc(sizeof(HAL_GDMA_OP));
    pSalADCMngtAdpt->pIrqGdmaHnd    = (PIRQ_HANDLE)RtlZmalloc(sizeof(IRQ_HANDLE));
    pSalADCUserCBAdpt               = (PSAL_ADC_USERCB_ADPT)RtlZmalloc((sizeof(SAL_ADC_USERCB_ADPT)*SAL_ADC_USER_CB_NUM));
#else
    switch (ADCIdx){
        case ADC0_SEL:
        {
            pSalADCMngtAdpt = &SalADC0MngtAdpt;
            pSalADCMngtAdpt->pSalHndPriv    = &SalADC0HndPriv;
            pSalADCMngtAdpt->pHalInitDat    = &HalADC0InitData;
            pSalADCMngtAdpt->pHalOp         = &HalADCOpSAL;
            pSalADCMngtAdpt->pIrqHnd        = &ADC0IrqHandleDat;
            pSalADCMngtAdpt->pHalGdmaAdp    = &HalADC0GdmaAdpt;
            pSalADCMngtAdpt->pHalGdmaOp     = &HalADC0GdmaOp;
            pSalADCMngtAdpt->pIrqGdmaHnd    = &ADC0GDMAIrqHandleDat;
            pSalADCMngtAdpt->pUserCB        = &SalADC0UserCB;
            pSalADCUserCBAdpt               = &SalADC0UserCBAdpt;
            break;
        }
        
        case ADC1_SEL:
        {
            pSalADCMngtAdpt = &SalADC1MngtAdpt;
            pSalADCMngtAdpt->pSalHndPriv    = &SalADC1HndPriv;
            pSalADCMngtAdpt->pHalInitDat    = &HalADC1InitData;
            pSalADCMngtAdpt->pHalOp         = &HalADCOpSAL;
            pSalADCMngtAdpt->pIrqHnd        = &ADC1IrqHandleDat;
            pSalADCMngtAdpt->pHalGdmaAdp    = &HalADC1GdmaAdpt;
            pSalADCMngtAdpt->pHalGdmaOp     = &HalADC1GdmaOp;
            pSalADCMngtAdpt->pIrqGdmaHnd    = &ADC1GDMAIrqHandleDat;
            pSalADCMngtAdpt->pUserCB        = &SalADC1UserCB;
            pSalADCUserCBAdpt               = &SalADC1UserCBAdpt;
            break;
        }    

        case ADC2_SEL:
        {
            pSalADCMngtAdpt = &SalADC2MngtAdpt;
            pSalADCMngtAdpt->pSalHndPriv    = &SalADC2HndPriv;
            pSalADCMngtAdpt->pHalInitDat    = &HalADC2InitData;
            pSalADCMngtAdpt->pHalOp         = &HalADCOpSAL;
            pSalADCMngtAdpt->pIrqHnd        = &ADC2IrqHandleDat;
            pSalADCMngtAdpt->pHalGdmaAdp    = &HalADC2GdmaAdpt;
            pSalADCMngtAdpt->pHalGdmaOp     = &HalADC2GdmaOp;
            pSalADCMngtAdpt->pIrqGdmaHnd    = &ADC2GDMAIrqHandleDat;
            pSalADCMngtAdpt->pUserCB        = &SalADC2UserCB;
            pSalADCUserCBAdpt               = &SalADC2UserCBAdpt;
            break;
        }

        case ADC3_SEL:
        {
            pSalADCMngtAdpt = &SalADC3MngtAdpt;
            pSalADCMngtAdpt->pSalHndPriv    = &SalADC3HndPriv;
            pSalADCMngtAdpt->pHalInitDat    = &HalADC3InitData;
            pSalADCMngtAdpt->pHalOp         = &HalADCOpSAL;
            pSalADCMngtAdpt->pIrqHnd        = &ADC3IrqHandleDat;
            pSalADCMngtAdpt->pHalGdmaAdp    = &HalADC3GdmaAdpt;
            pSalADCMngtAdpt->pHalGdmaOp     = &HalADC3GdmaOp;
            pSalADCMngtAdpt->pIrqGdmaHnd    = &ADC3GDMAIrqHandleDat;
            pSalADCMngtAdpt->pUserCB        = &SalADC3UserCB;
            pSalADCUserCBAdpt               = &SalADC3UserCBAdpt;
            break;
        }
        default
            break;
    }
#endif

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
    /*To assign the rest pointers*/
    pSalADCMngtAdpt->pSalHndPriv->ppSalADCHnd = (void**)&(pSalADCMngtAdpt->pSalHndPriv);

    /* To assign the default (ROM) HAL OP initialization function */
    pSalADCMngtAdpt->pHalOpInit     = &HalADCOpInit;
    
    /* To assign the default (ROM) HAL GDMA OP initialization function */
    pSalADCMngtAdpt->pHalGdmaOpInit = &HalGdmaOpInit;

    /* To assign the default (ROM) SAL interrupt function */
    pSalADCMngtAdpt->pSalIrqFunc    = &ADCISRHandle;

    /* To assign the default (ROM) SAL DMA TX interrupt function */
    pSalADCMngtAdpt->pSalDMAIrqFunc   = &ADCGDMAISRHandle;

    return pSalADCMngtAdpt;
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
RtkADCFreeMngtAdpt(
    IN  PSAL_ADC_MNGT_ADPT  pSalADCMngtAdpt
){
#ifdef CONFIG_KERNEL
    RtlMfree((u8 *)pSalADCMngtAdpt->pUserCB->pRXCB, (sizeof(SAL_ADC_USERCB_ADPT)*SAL_ADC_USER_CB_NUM));
    RtlMfree((u8 *)pSalADCMngtAdpt->pIrqGdmaHnd, sizeof(IRQ_HANDLE));
    RtlMfree((u8 *)pSalADCMngtAdpt->pHalGdmaOp, sizeof(HAL_GDMA_OP));
    RtlMfree((u8 *)pSalADCMngtAdpt->pHalGdmaAdp, sizeof(HAL_GDMA_ADAPTER));
    RtlMfree((u8 *)pSalADCMngtAdpt->pUserCB, sizeof(SAL_ADC_USER_CB));
    RtlMfree((u8 *)pSalADCMngtAdpt->pIrqHnd, sizeof(IRQ_HANDLE));
    RtlMfree((u8 *)pSalADCMngtAdpt->pHalOp, sizeof(HAL_ADC_OP));
    RtlMfree((u8 *)pSalADCMngtAdpt->pHalInitDat, sizeof(HAL_ADC_INIT_DAT));
    RtlMfree((u8 *)pSalADCMngtAdpt->pSalHndPriv, sizeof(SAL_ADC_HND_PRIV));    
    RtlMfree((u8 *)pSalADCMngtAdpt, sizeof(SAL_ADC_MNGT_ADPT));
#else
    ;
#endif

    return _EXIT_SUCCESS;
}
#endif

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
ADCISRHandle(
    IN  VOID *Data
){
#ifdef CONFIG_DEBUG_LOG_ADC_HAL
    PSAL_ADC_HND        pSalADCHND          = (PSAL_ADC_HND) Data;
    PSAL_ADC_HND_PRIV   pSalADCHNDPriv      = NULL;
    PSAL_ADC_MNGT_ADPT  pSalADCMngtAdpt     = NULL;
    PHAL_ADC_INIT_DAT   pHalADCInitDat      = NULL;
    PHAL_ADC_OP         pHalADCOP           = NULL;    
    PSAL_ADC_USER_CB    pSalADCUserCB       = NULL;
    u8  ADCIrqIdx;
    
    /* To get the SAL_I2C_MNGT_ADPT pointer, and parse the rest pointers */
    pSalADCHNDPriv  = CONTAINER_OF(pSalADCHND, SAL_ADC_HND_PRIV, SalADCHndPriv);
    pSalADCMngtAdpt = CONTAINER_OF(pSalADCHNDPriv->ppSalADCHnd, SAL_ADC_MNGT_ADPT, pSalHndPriv);
    pHalADCInitDat  = pSalADCMngtAdpt->pHalInitDat;
    pHalADCOP       = pSalADCMngtAdpt->pHalOp;
    ADCIrqIdx       = pHalADCInitDat->ADCIdx;
    pSalADCUserCB   = pSalADCHND->pUserCB;

    DBG_8195A_ADC_LVL(HAL_ADC_LVL,"ADC INTR STS:%x\n\n",pHalADCOP->HalADCReadReg(pHalADCInitDat, REG_ADC_INTR_STS));
#else
    PSAL_ADC_HND        pSalADCHND          = (PSAL_ADC_HND) Data;
    PSAL_ADC_HND_PRIV   pSalADCHNDPriv      = NULL;
    PSAL_ADC_MNGT_ADPT  pSalADCMngtAdpt     = NULL;
    PHAL_ADC_INIT_DAT   pHalADCInitDat      = NULL;
    PHAL_ADC_OP         pHalADCOP           = NULL;    

    //u8  ADCIrqIdx;

    
    /* To get the SAL_I2C_MNGT_ADPT pointer, and parse the rest pointers */
    pSalADCHNDPriv  = CONTAINER_OF(pSalADCHND, SAL_ADC_HND_PRIV, SalADCHndPriv);
    pSalADCMngtAdpt = CONTAINER_OF(pSalADCHNDPriv->ppSalADCHnd, SAL_ADC_MNGT_ADPT, pSalHndPriv);
    pHalADCInitDat  = pSalADCMngtAdpt->pHalInitDat;
    pHalADCOP       = pSalADCMngtAdpt->pHalOp;
    //ADCIrqIdx       = pHalADCInitDat->ADCIdx;

	DBG_ADC_INFO("ADC INTR STS:%x\n",pHalADCOP->HalADCReadReg(pHalADCInitDat, REG_ADC_INTR_STS));
    if (pSalADCHND->OpType == ADC_RDREG_TYPE){
        ADCFullStsFlag = 1;
        ADCDatBuf[0]    = (u32)HAL_ADC_READ32(REG_ADC_FIFO_READ);
        ADCDatBuf[1]    = (u32)HAL_ADC_READ32(REG_ADC_FIFO_READ);
        pSalADCHND->pInitDat->ADCIntrMSK  =  0;
        pHalADCOP->HalADCIntrCtrl(pSalADCHND->pInitDat); 
    }
    else
        pHalADCOP->HalADCReadReg(pHalADCInitDat, REG_ADC_INTR_STS);
    
#endif
}

VOID
ADCGDMAISRHandle(
    IN  VOID *Data
){

    /* DBG_ENTRANCE; */
    PSAL_ADC_HND        pSalADCHND          = (PSAL_ADC_HND) Data;
    PSAL_ADC_HND_PRIV   pSalADCHNDPriv      = NULL;
    PSAL_ADC_MNGT_ADPT  pSalADCMngtAdpt     = NULL;

    PHAL_ADC_OP         pHalADCOP           = NULL;    
    PSAL_ADC_USER_CB    pSalADCUserCB       = NULL;

    PHAL_GDMA_ADAPTER   pHalADCGdmaAdapter;
    PHAL_GDMA_OP        pHalADCGdmaOp;
    PIRQ_HANDLE         pIrqHandleADCGdma   = NULL;
    
    u8  IsrTypeMap = 0;
    u32 AdcTempDat;
    
    /* To get the SAL_I2C_MNGT_ADPT pointer, and parse the rest pointers */
    pSalADCHNDPriv  = CONTAINER_OF(pSalADCHND, SAL_ADC_HND_PRIV, SalADCHndPriv);
    pSalADCMngtAdpt = CONTAINER_OF(pSalADCHNDPriv->ppSalADCHnd, SAL_ADC_MNGT_ADPT, pSalHndPriv);

    pHalADCOP       = pSalADCMngtAdpt->pHalOp;
    
    pSalADCUserCB   = pSalADCHND->pUserCB;
    
    pHalADCGdmaAdapter  = pSalADCMngtAdpt->pHalGdmaAdp;
    pHalADCGdmaOp       = pSalADCMngtAdpt->pHalGdmaOp;

    pIrqHandleADCGdma   = pSalADCMngtAdpt->pIrqGdmaHnd;
    DBG_8195A_ADC_LVL(HAL_ADC_LVL,"%s\n",__func__);
    
    if ((pHalADCGdmaAdapter->MaxMuliBlock) == pHalADCGdmaAdapter->MuliBlockCunt+1) {
		pSalADCHND->pInitDat->ADCIntrMSK    =   0;
        pHalADCOP->HalADCIntrCtrl(pSalADCHND->pInitDat);
		
        AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);
    	AdcTempDat &= (~0x01);
    	HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD0, AdcTempDat);
    	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);


    	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
    	AdcTempDat &= (~0x01);
    	HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD1, AdcTempDat);
    	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);

        /* Clear ADC Status */
		HAL_ADC_READ32(REG_ADC_INTR_STS);
        
        pHalADCGdmaOp->HalGdmaChCleanAutoSrc(pHalADCGdmaAdapter);
        //pHalADCGdmaOp->HalGdmaChDis(pHalADCGdmaAdapter);
        RtkADCDMADeInit(pSalADCHND);
        pSalADCHND->DevSts  = ADC_STS_IDLE;

        /* user callback */
        if (pSalADCUserCB->pIDMARXCCB->USERCB != NULL) {
            pSalADCUserCB->pIDMARXCCB->USERCB((VOID*)pSalADCUserCB->pIDMARXCCB->USERData);
        } else if (pSalADCUserCB->pDMARXCCB->USERCB != NULL) {
            pSalADCUserCB->pDMARXCCB->USERCB((VOID*)pSalADCUserCB->pDMARXCCB->USERData);
        }

        //InterruptDis(pIrqHandleADCGdma);
        //InterruptUnRegister(pIrqHandleADCGdma);        
    }

    //3 Clear Pending ISR
    IsrTypeMap = pHalADCGdmaOp->HalGdmaChIsrClean((VOID*)pHalADCGdmaAdapter);

    //3 Maintain Block Count
    if (IsrTypeMap & BlockType) {
        pHalADCGdmaAdapter->MuliBlockCunt++;
    }
}

RTK_STATUS
RtkADCPinMuxInit(
    IN  PSAL_ADC_HND    pSalADCHND
){

    u32 ADCLocalTemp;

    /* Check the I2C index first */
    if (RtkADCIdxChk(pSalADCHND->DevNum))
        return _EXIT_FAILURE;

    ADCLocalTemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL2);
    ADCLocalTemp |= BIT25;

    /* To release DAC delta sigma clock gating */
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_SYS_SYSPLL_CTRL2,ADCLocalTemp);

    /* Turn on DAC active clock */
    ACTCK_ADC_CCTRL(ON);

    /* Enable DAC0 module */
    ADC0_FCTRL(ON);  
    
    return _EXIT_SUCCESS;

}

static RTK_STATUS
RtkADCPinMuxDeInit(
    IN  PSAL_ADC_HND    pSalADCHND
){

    u32 ADCLocalTemp;
    
    /* Check the I2C index first */
    if (RtkADCIdxChk(pSalADCHND->DevNum))
        return _EXIT_FAILURE;

    /* Turn on DAC active clock */
    ACTCK_ADC_CCTRL(OFF);

    /* Enable DAC1 module */
    ADC0_FCTRL(OFF);  

    ADCLocalTemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL2);
    ADCLocalTemp &= (~BIT25);

    /* To release DAC delta sigma clock gating */
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_SYS_SYSPLL_CTRL2,ADCLocalTemp);
    return _EXIT_SUCCESS;
}


#if ADC_INTR_OP_TYPE
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
RtkADCIrqInit(
    IN  PSAL_ADC_HND    pSalADCHND
){

    PSAL_ADC_HND_PRIV   pSalADCHNDPriv      = NULL;
    PSAL_ADC_MNGT_ADPT  pSalADCMngtAdpt     = NULL;
    PIRQ_HANDLE         pIrqHandle          = NULL;
    
    /* To Get the SAL_I2C_MNGT_ADPT Pointer */
    pSalADCHNDPriv  = CONTAINER_OF(pSalADCHND, SAL_ADC_HND_PRIV, SalADCHndPriv);
    pSalADCMngtAdpt = CONTAINER_OF(pSalADCHNDPriv->ppSalADCHnd, SAL_ADC_MNGT_ADPT, pSalHndPriv);
    pIrqHandle      = pSalADCMngtAdpt->pIrqHnd;
/*    
    if (RtkADCIdxChk(pSalADCHND->DevNum))
        return _EXIT_FAILURE;
*/

    pIrqHandle->Data        =   (u32)(pSalADCHND);
    pIrqHandle->IrqNum      =   ADC_IRQ;
    pIrqHandle->IrqFun      =   (IRQ_FUN)pSalADCMngtAdpt->pSalIrqFunc;
    pIrqHandle->Priority    =   5;
    InterruptRegister(pIrqHandle);
    InterruptEn(pIrqHandle);
  
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
RtkADCIrqDeInit(
    IN  PSAL_ADC_HND    pSalADCHND
){

    PSAL_ADC_HND_PRIV   pSalADCHNDPriv      = NULL;
    PSAL_ADC_MNGT_ADPT  pSalADCMngtAdpt     = NULL;
    PIRQ_HANDLE         pIrqHandle          = NULL;
    
    /* To Get the SAL_I2C_MNGT_ADPT Pointer */
    pSalADCHNDPriv  = CONTAINER_OF(pSalADCHND, SAL_ADC_HND_PRIV, SalADCHndPriv);
    pSalADCMngtAdpt = CONTAINER_OF(pSalADCHNDPriv->ppSalADCHnd, SAL_ADC_MNGT_ADPT, pSalHndPriv);
    pIrqHandle      = pSalADCMngtAdpt->pIrqHnd;
/*
    if (RtkADCIdxChk(pSalADCHND->DevNum))
        return _EXIT_FAILURE;
*/
    InterruptUnRegister(pIrqHandle);
    return _EXIT_SUCCESS;

}

#endif


#if ADC_DMA_OP_TYPE
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
RtkADCDMAInit(
    IN  PSAL_ADC_HND    pSalADCHND
){
    PSAL_ADC_HND_PRIV   pSalADCHNDPriv      = NULL;
    PSAL_ADC_MNGT_ADPT  pSalADCMngtAdpt     = NULL;
    PHAL_GDMA_ADAPTER   pHALADCGdmaAdpt     = NULL;
    PHAL_GDMA_OP        pHALADCGdmaOp       = NULL;
    PIRQ_HANDLE         pIrqHandleADCGdma   = NULL;
    
    /* To Get the SAL_I2C_MNGT_ADPT Pointer */
    pSalADCHNDPriv  = CONTAINER_OF(pSalADCHND, SAL_ADC_HND_PRIV, SalADCHndPriv);
    pSalADCMngtAdpt = CONTAINER_OF(pSalADCHNDPriv->ppSalADCHnd, SAL_ADC_MNGT_ADPT, pSalHndPriv);
    
    pHALADCGdmaAdpt     = pSalADCMngtAdpt->pHalGdmaAdp;
    pHALADCGdmaOp       = pSalADCMngtAdpt->pHalGdmaOp;
    pIrqHandleADCGdma   = pSalADCMngtAdpt->pIrqGdmaHnd;
/*
    if (RtkADCIdxChk(pSalADCHND->DevNum))
        return _EXIT_FAILURE;
*/
    //HalGdmaOpInit(pHALADCGdmaOp);
    pSalADCMngtAdpt->pHalGdmaOpInit(pHALADCGdmaOp);
    _memset((void *)pHALADCGdmaAdpt, 0, sizeof(HAL_GDMA_ADAPTER));
    
    pHALADCGdmaAdpt->GdmaCtl.IntEn = 1;

    //ADC RX DMA
    pHALADCGdmaAdpt->GdmaCtl.SrcTrWidth = TrWidthFourBytes;
    pHALADCGdmaAdpt->GdmaCtl.DstTrWidth = TrWidthFourBytes;
    pHALADCGdmaAdpt->GdmaCtl.SrcMsize   = MsizeEight;
    pHALADCGdmaAdpt->GdmaCtl.DestMsize  = MsizeEight;

    pHALADCGdmaAdpt->GdmaCtl.Sinc       = NoChange;
    pHALADCGdmaAdpt->GdmaCtl.Dinc       = IncType;	

    pHALADCGdmaAdpt->GdmaCtl.Done       = 1;
    pHALADCGdmaAdpt->GdmaCtl.TtFc       = (GDMA_CTL_TT_FC_TYPE)0x2;
    
    pHALADCGdmaAdpt->GdmaCfg.SrcPer     = 12;
    pHALADCGdmaAdpt->GdmaCfg.ReloadSrc  = 1;

    pHALADCGdmaAdpt->MuliBlockCunt      = 0;
    pHALADCGdmaAdpt->MaxMuliBlock       = 1;//MaxLlp;
    
    pHALADCGdmaAdpt->GdmaIsrType        = (BlockType|TransferType|ErrType);
    pHALADCGdmaAdpt->IsrCtrl            = ENABLE;
    pHALADCGdmaAdpt->GdmaOnOff          = ON;

    pHALADCGdmaAdpt->ChNum              = 4;
    pHALADCGdmaAdpt->ChEn               = GdmaCh4;
    
    pHALADCGdmaAdpt->TestItem = 3;
    DBG_ADC_INFO("pSalADCHND->DevNum:%x\n",pSalADCHND->DevNum);

    pHALADCGdmaAdpt->GdmaIndex          =   1;
    pIrqHandleADCGdma->IrqNum           =   GDMA1_CHANNEL4_IRQ;

    /* GDMA interrupt register */
    pIrqHandleADCGdma->Data     = (u32) (pSalADCHND);  
    pIrqHandleADCGdma->IrqFun   = (IRQ_FUN) pSalADCMngtAdpt->pSalDMAIrqFunc;
    pIrqHandleADCGdma->Priority = 6;
    InterruptRegister(pIrqHandleADCGdma);
    InterruptEn(pIrqHandleADCGdma);

    /* GDMA initialization */
    /* Enable the whole GDMA module first */
    if (pHALADCGdmaAdpt->GdmaIndex == 0) {        
        ACTCK_GDMA0_CCTRL(ON);
        SLPCK_GDMA0_CCTRL(ON);
        GDMA0_FCTRL(ON);
    }
    else {        
        ACTCK_GDMA1_CCTRL(ON);
        SLPCK_GDMA1_CCTRL(ON);
        GDMA1_FCTRL(ON);
    }

    pHALADCGdmaOp->HalGdmaOnOff((VOID*)pHALADCGdmaAdpt);
    pHALADCGdmaOp->HalGdmaChIsrEnAndDis((VOID*)pHALADCGdmaAdpt);
    //pHALADCGdmaOp->HalGdmaChSeting((VOID*)pHALADCGdmaAdpt);

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
RtkADCDMADeInit(
    IN  PSAL_ADC_HND    pSalADCHND
){
    PSAL_ADC_HND_PRIV   pSalADCHNDPriv      = NULL;
    PSAL_ADC_MNGT_ADPT  pSalADCMngtAdpt     = NULL;

    PHAL_GDMA_ADAPTER   pHALADCGdmaAdpt     = NULL;
    PHAL_GDMA_OP        pHALADCGdmaOp       = NULL;
    PIRQ_HANDLE         pIrqHandleADCGdma   = NULL;
    
    /*To Get the SAL_I2C_MNGT_ADPT Pointer*/
    pSalADCHNDPriv  = CONTAINER_OF(pSalADCHND, SAL_ADC_HND_PRIV, SalADCHndPriv);
    pSalADCMngtAdpt = CONTAINER_OF(pSalADCHNDPriv->ppSalADCHnd, SAL_ADC_MNGT_ADPT, pSalHndPriv);
    
    pHALADCGdmaAdpt     = pSalADCMngtAdpt->pHalGdmaAdp;
    pHALADCGdmaOp       = pSalADCMngtAdpt->pHalGdmaOp;
    pIrqHandleADCGdma   = pSalADCMngtAdpt->pIrqGdmaHnd;

    if (RtkADCIdxChk(pSalADCHND->DevNum))
        return _EXIT_FAILURE;

    //HalGdmaOpInit(pHALADCGdmaOp);
    pSalADCMngtAdpt->pHalGdmaOpInit(pHALADCGdmaOp);
    
    pHALADCGdmaAdpt->IsrCtrl    = DISABLE;
    pHALADCGdmaOp->HalGdmaChIsrEnAndDis((VOID*)pHALADCGdmaAdpt);
    pHALADCGdmaOp->HalGdmaChIsrClean((VOID*)pHALADCGdmaAdpt);
    pHALADCGdmaOp->HalGdmaChDis((VOID*)pHALADCGdmaAdpt);
     
    InterruptUnRegister(pIrqHandleADCGdma);
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
RtkADCInit(
    IN  VOID *Data
){
    PSAL_ADC_HND        pSalADCHND          = (PSAL_ADC_HND) Data;
    PSAL_ADC_HND_PRIV   pSalADCHNDPriv      = NULL;
    PSAL_ADC_MNGT_ADPT  pSalADCMngtAdpt     = NULL;
    PHAL_ADC_INIT_DAT   pHalADCInitDat      = NULL;
    PHAL_ADC_OP         pHalADCOP           = NULL;    
    
#ifdef CONFIG_SOC_PS_MODULE
    REG_POWER_STATE adcPwrState;
#endif

    DBG_ADC_INFO("%s\n",__func__);
    /* To Get the SAL_ADC_MNGT_ADPT Pointer */
    pSalADCHNDPriv  = CONTAINER_OF(pSalADCHND, SAL_ADC_HND_PRIV, SalADCHndPriv);
    pSalADCMngtAdpt = CONTAINER_OF(pSalADCHNDPriv->ppSalADCHnd, SAL_ADC_MNGT_ADPT, pSalHndPriv);
    
    pHalADCInitDat  = pSalADCMngtAdpt->pHalInitDat;
    pHalADCOP       = pSalADCMngtAdpt->pHalOp;

    /* Check the input I2C index first */
    if (RtkADCIdxChk(pSalADCHND->DevNum))
        return _EXIT_FAILURE;

#if 0
    /* Check the input I2C operation type */
    if (RtkI2COpTypeChk(pSalI2CHND))
        return _EXIT_FAILURE;
#endif
    
    /* ADC Initial data check, if the setting is different from the previous initial data,
          an warning is shown on Log-Uart and directly return from this function */
    if (SalAdcInitialFlag != 0) {
        if (_memcmp(pHalADCInitDat, &SalAdcInitialDatKeep, sizeof(HAL_ADC_INIT_DAT))) {
            pSalADCMngtAdpt->pHalOpInit(pHalADCOP);
            /* DAC Device Status Update */
            pSalADCHND->DevSts  = ADC_STS_IDLE;
            DBG_ADC_WARN("The ADC initial value is different from the previous value.\n");
        }
    }
    else {        
        /* ADC Initialize HAL Operations */
        //HalADCOpInit(pHalADCOP);
        pSalADCMngtAdpt->pHalOpInit(pHalADCOP);
        
        /* ADC Interrupt Initialization */
#if ADC_INTR_OP_TYPE
        RtkADCIrqInit(pSalADCHND);
#endif

        /* ADC DMA Initialization */
#if ADC_DMA_OP_TYPE
        RtkADCDMAInit(pSalADCHND);
#endif

        /* ADC Function and Clock Enable*/
        RtkADCPinMuxInit(pSalADCHND);
        pHalADCOP->HalADCInit(pSalADCHND->pInitDat);

        if (pSalADCHND->OpType == ADC_DMA_TYPE){
            pSalADCHND->pInitDat->ADCIntrMSK  =  (BIT_ADC_FIFO_RD_REQ_EN |
                                                 BIT_ADC_FIFO_RD_ERROR_EN);
        }
        else if (pSalADCHND->OpType == ADC_INTR_TYPE){
            pSalADCHND->pInitDat->ADCIntrMSK  =  (BIT_ADC_FIFO_FULL_EN |
                                                  BIT_ADC_FIFO_RD_REQ_EN |
                                                 BIT_ADC_FIFO_RD_ERROR_EN);
        }
        else{
            pSalADCHND->pInitDat->ADCIntrMSK  =  0;
        }
        
                                                 
        if (pHalADCInitDat->ADCOneShotEn == ADC_FEATURE_ENABLED) {
            pSalADCHND->pInitDat->ADCIntrMSK |= BIT_ADC_AWAKE_CPU_EN;
        }
        pHalADCOP->HalADCIntrCtrl(pSalADCHND->pInitDat);
        
        //pSalADCHND->pInitDat->ADCEn = ADC_ENABLE;
        //pHalADCOP->HalADCEnable(pSalADCHND->pInitDat);

        if (pHalADCInitDat->ADCOneShotEn == ADC_FEATURE_ENABLED) {
            HAL_TIMER_WRITE32(TIMER_INTERVAL, 30);
            HAL_TIMER_WRITE32(0x1C, 3);
        }
        
        SalAdcInitialFlag |= (0x01 << pSalADCHND->DevNum);
        _memcpy(&SalAdcInitialDatKeep, pSalADCHND->pInitDat, sizeof(HAL_ADC_INIT_DAT));
    }

    /* DAC Device Status Update */
    pSalADCHND->DevSts  = ADC_STS_IDLE;

#ifdef CONFIG_SOC_PS_MODULE
    // To register a new peripheral device power state
    adcPwrState.FuncIdx = ADC0;
    adcPwrState.PwrState = ACT;
    RegPowerState(adcPwrState);

    SalAdcEnableState |= (0x01 << pSalADCHND->DevNum);
#endif

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
RtkADCDeInit(
    IN  VOID *Data
){
    PSAL_ADC_HND        pSalADCHND          = (PSAL_ADC_HND) Data;
    PSAL_ADC_HND_PRIV   pSalADCHNDPriv      = NULL;
    PSAL_ADC_MNGT_ADPT  pSalADCMngtAdpt     = NULL;
    PHAL_ADC_INIT_DAT   pHalADCInitDat      = NULL;
    PHAL_ADC_OP         pHalADCOP           = NULL;    
#ifdef CONFIG_SOC_PS_MODULE
    REG_POWER_STATE adcPwrState;
    u8 HwState;
#endif

    /* To Get the SAL_ADC_MNGT_ADPT Pointer */
    pSalADCHNDPriv  = CONTAINER_OF(pSalADCHND, SAL_ADC_HND_PRIV, SalADCHndPriv);
    pSalADCMngtAdpt = CONTAINER_OF(pSalADCHNDPriv->ppSalADCHnd, SAL_ADC_MNGT_ADPT, pSalHndPriv);
    
    pHalADCInitDat  = pSalADCMngtAdpt->pHalInitDat;
    pHalADCOP       = pSalADCMngtAdpt->pHalOp;

    /* Check the input ADC index first */
    if (RtkADCIdxChk(pSalADCHND->DevNum))
        return _EXIT_FAILURE;

    SalAdcInitialFlag   &= (~(0x01 << pSalADCHND->DevNum));

    if (SalAdcInitialFlag == 0) {
#ifdef CONFIG_SOC_PS_MODULE   
        adcPwrState.FuncIdx = ADC0;        
        QueryRegPwrState(adcPwrState.FuncIdx, &(adcPwrState.PwrState), &HwState);
        
        // if the power state isn't ACT, then switch the power state back to ACT first
        if ((adcPwrState.PwrState != ACT) && (adcPwrState.PwrState != INACT)) {
            RtkADCEnablePS(Data);
            QueryRegPwrState(adcPwrState.FuncIdx, &(adcPwrState.PwrState), &HwState);
        }
    
        if (adcPwrState.PwrState == ACT) {
            adcPwrState.PwrState = INACT;
            RegPowerState(adcPwrState);
        } 
#endif

        /* ADC Initialize HAL Operations */
        HalADCOpInit(pHalADCOP);

        RtkADCPinMuxDeInit(pSalADCHND);  

                /* ADC Interrupt Initialization */
#if ADC_INTR_OP_TYPE
        RtkADCIrqDeInit(pSalADCHND);
#endif

        /* ADC DMA Initialization */
#if ADC_DMA_OP_TYPE
        RtkADCDMADeInit(pSalADCHND);
#endif

        pHalADCInitDat->ADCEn   =   ADC_DISABLE;
        pHalADCOP->HalADCEnable(pHalADCInitDat);
        pHalADCOP->HalADCDeInit(pHalADCInitDat);

        /* ADC Function and Clock Enable*/
        RtkADCPinMuxDeInit(pSalADCHND); 
    }
    
    return _EXIT_SUCCESS;
}

u32
RtkADCReceive(
    IN  VOID *Data
){
    PSAL_ADC_HND        pSalADCHND          = (PSAL_ADC_HND) Data;
    PSAL_ADC_HND_PRIV   pSalADCHNDPriv      = NULL;
    PSAL_ADC_MNGT_ADPT  pSalADCMngtAdpt     = NULL;
    PHAL_GDMA_ADAPTER   pHALADCGdmaAdpt     = NULL;
    PHAL_GDMA_OP        pHALADCGdmaOp       = NULL;



    //PIRQ_HANDLE         pIrqHandleADCGdma   = NULL;
    u32 AdcTempDat;
    
    /* To Get the SAL_I2C_MNGT_ADPT Pointer */
    pSalADCHNDPriv  = CONTAINER_OF(pSalADCHND, SAL_ADC_HND_PRIV, SalADCHndPriv);
    pSalADCMngtAdpt = CONTAINER_OF(pSalADCHNDPriv->ppSalADCHnd, SAL_ADC_MNGT_ADPT, pSalHndPriv);
    
    pHALADCGdmaAdpt     = pSalADCMngtAdpt->pHalGdmaAdp;
    pHALADCGdmaOp       = pSalADCMngtAdpt->pHalGdmaOp;


		
    if (pSalADCHND->OpType == ADC_DMA_TYPE) {
		/* Clear ADC Status */
		HAL_ADC_READ32(REG_ADC_INTR_STS);
		
        HalGdmaOpInit(pHALADCGdmaOp);
        pHALADCGdmaAdpt->GdmaCtl.BlockSize  =   pSalADCHND->pRXBuf->DataLen;
        pHALADCGdmaAdpt->ChSar              =   (u32)(ADC_REG_BASE);
        pHALADCGdmaAdpt->ChDar              =   (u32)pSalADCHND->pRXBuf->pDataBuf;
        pHALADCGdmaAdpt->MuliBlockCunt      = 0;
        
        pHALADCGdmaOp->HalGdmaChSeting(pHALADCGdmaAdpt);
        pHALADCGdmaOp->HalGdmaChEn(pHALADCGdmaAdpt);

        pSalADCHND->DevSts  = ADC_STS_RX_ING;
        AdcTempDat  =   HAL_ADC_READ32(REG_ADC_POWER);
        AdcTempDat  |=  BIT_ADC_PWR_AUTO;
        HAL_ADC_WRITE32(REG_ADC_POWER, AdcTempDat);
        return _EXIT_SUCCESS;
    }
    return _EXIT_FAILURE;
}

extern u32 
HalDelayUs(
    IN  u32 us
);

u32
RtkADCReceiveBuf(
    IN  VOID *Data,
    IN  u32  *pBuf
){
    PSAL_ADC_HND        pSalADCHND          = (PSAL_ADC_HND) Data;
    PSAL_ADC_HND_PRIV   pSalADCHNDPriv      = NULL;
    PSAL_ADC_MNGT_ADPT  pSalADCMngtAdpt     = NULL;



    PHAL_ADC_OP         pHalADCOP           = NULL;    

    //PIRQ_HANDLE         pIrqHandleADCGdma   = NULL;
    u32 AdcTempDat;
    
    /* To Get the SAL_I2C_MNGT_ADPT Pointer */
    pSalADCHNDPriv  = CONTAINER_OF(pSalADCHND, SAL_ADC_HND_PRIV, SalADCHndPriv);
    pSalADCMngtAdpt = CONTAINER_OF(pSalADCHNDPriv->ppSalADCHnd, SAL_ADC_MNGT_ADPT, pSalHndPriv);
    



	pHalADCOP           = pSalADCMngtAdpt->pHalOp; 

    
    
    /* Clear ADC Status */
	//HAL_ADC_READ32(REG_ADC_INTR_STS);
	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_INTR_STS);
	DBG_ADC_INFO("RtkADCReceiveBuf, INTR:%x\n", AdcTempDat);
	//AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_INTR_STS);
	//DBG_8195A(">>INTR:%x\n",AdcTempDat);

    ADCFullStsFlag = 0;
    HalDelayUs(2000);

	DBG_ADC_INFO("RtkADCReceiveBuf, Check to enable ADC manully or not\n");
	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_POWER);
	if (unlikely((AdcTempDat & 0x00000008) == 0)) {
		;
	}
	else {

		pSalADCHND->pInitDat->ADCEn = ADC_ENABLE;
    	pHalADCOP->HalADCEnable(pSalADCHND->pInitDat);  
		//AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_POWER);
	}


    pSalADCHND->pInitDat->ADCIntrMSK  =  (BIT_ADC_FIFO_FULL_EN);
    pHalADCOP->HalADCIntrCtrl(pSalADCHND->pInitDat);
	pSalADCHND->DevSts = ADC_STS_IDLE;
	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);

	if ((AdcTempDat & 0x00000001) == 0){
        AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
		DBG_ADC_INFO("RtkADCReceiveBuf, Before set, Reg AD1:%x\n", AdcTempDat);
		AdcTempDat |= (0x01);
		HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD1, AdcTempDat);
		AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
		DBG_ADC_INFO("RtkADCReceiveBuf, After set, Reg AD1:%x\n", AdcTempDat);

		AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);
		DBG_ADC_INFO("RtkADCReceiveBuf, Before set, Reg AD0:%x\n", AdcTempDat);
		AdcTempDat |= (0x01);
		HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD0, AdcTempDat);
		AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);
		DBG_ADC_INFO("RtkADCReceiveBuf, After set, Reg AD0:%x\n", AdcTempDat);
	}
	else{
		;
	}
     
    while (ADCFullStsFlag == 0){
    }

	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);
	DBG_ADC_INFO("RtkADCReceiveBuf, End of ADC, Before set, AD0:%x\n", AdcTempDat);
	AdcTempDat &= (~0x01);
	HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD0, AdcTempDat);
	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);
	DBG_ADC_INFO("RtkADCReceiveBuf, End of ADC, After set, AD0:%x\n", AdcTempDat);

	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
	DBG_ADC_INFO("RtkADCReceiveBuf, End of ADC, Before set, AD1:%x\n", AdcTempDat);
	AdcTempDat &= (~0x01);
	HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD1, AdcTempDat);
	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
	DBG_ADC_INFO("RtkADCReceiveBuf, End of ADC, After set, AD1:%x\n", AdcTempDat);
	
    /* Clear ADC Status */
	HAL_ADC_READ32(REG_ADC_INTR_STS);
	ADCFullStsFlag = 0;
    
    *pBuf       = (u32)ADCDatBuf[0];
    *(pBuf+1)   = (u32)ADCDatBuf[1];
    ADCDatBuf[0] = 0;
    ADCDatBuf[1] = 0;
    
    return _EXIT_SUCCESS;    
}


u32
RtkADCRxManualRotate(
    IN  VOID *Data,
    IN  u32  *pBuf
){
    PSAL_ADC_HND        pSalADCHND          = (PSAL_ADC_HND) Data;
    PSAL_ADC_HND_PRIV   pSalADCHNDPriv      = NULL;
    PSAL_ADC_MNGT_ADPT  pSalADCMngtAdpt     = NULL;

    PHAL_ADC_OP         pHalADCOP           = NULL;    

    u32 AdcTempDat;
    u16 tempcnt;
    
    /* To Get the SAL_I2C_MNGT_ADPT Pointer */
    pSalADCHNDPriv  = CONTAINER_OF(pSalADCHND, SAL_ADC_HND_PRIV, SalADCHndPriv);
    pSalADCMngtAdpt = CONTAINER_OF(pSalADCHNDPriv->ppSalADCHnd, SAL_ADC_MNGT_ADPT, pSalHndPriv);
	pHalADCOP           = pSalADCMngtAdpt->pHalOp; 


    /* Clear ADC Status */
	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_INTR_STS);
	DBG_ADC_INFO("RtkADCRxManualRotate, INTR:%x\n", AdcTempDat);
    
	DBG_ADC_INFO("RtkADCRxManualRotate, Check to enable ADC manully or not\n");
	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_POWER);
	if (unlikely((AdcTempDat & 0x00000008) == 0)) {
		;
	}
	else {
		pSalADCHND->pInitDat->ADCEn = ADC_ENABLE;
    	pHalADCOP->HalADCEnable(pSalADCHND->pInitDat);  
	}

    
	/* Comment when manual rotation 
    pSalADCHND->pInitDat->ADCIntrMSK  =  (BIT_ADC_FIFO_FULL_EN);
    pHalADCOP->HalADCIntrCtrl(pSalADCHND->pInitDat);
      */
	pSalADCHND->DevSts = ADC_STS_IDLE;
	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);

	if ((AdcTempDat & 0x00000001) == 0){
        AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
		DBG_ADC_INFO("RtkADCRxManualRotate, Before set, Reg AD1:%x\n", AdcTempDat);
		/* Clear for manual rotrate first*/
		AdcTempDat &= ~(BIT7|BIT5|BIT4|BIT3|BIT2|BIT1);
		/* Enable filter */
		AdcTempDat |= (BIT0);

        /* Enable manual mode, this is to turn cali. off */
        AdcTempDat &= ~(BIT11);
		AdcTempDat |= (BIT11);
		
		/* Set rotation to default state
                ad1[7][5][4][3][2][1]=100010=>000001=>000110=>001010=>010010
		*/
		AdcTempDat |= (BIT7|BIT2);
		//AdcTempDat |= (BIT4|BIT2);
		
		HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD1, AdcTempDat);
		AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
		DBG_ADC_INFO("RtkADCRxManualRotate, After set, Reg AD1:%x\n", AdcTempDat);
		//DBG_8195A("1. Reg AD1:%x\n", AdcTempDat);
		
		AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);
		DBG_ADC_INFO("RtkADCRxManualRotate, Before set, Reg AD0:%x\n", AdcTempDat);
		AdcTempDat |= (0x01);
		HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD0, AdcTempDat);
		AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);
		DBG_ADC_INFO("RtkADCRxManualRotate, After set, Reg AD0:%x\n", AdcTempDat);
	}
	else{
		;
	}

    if (pSalADCHND->DevNum < 2) {
        /* Start rotation sequence, ad1[7][5][4][3][2][1]=000001 */
        AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
        AdcTempDat &= ~(BIT7|BIT5|BIT4|BIT3|BIT2|BIT1);
        AdcTempDat |= (BIT1);
    	HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD1, AdcTempDat);
        HalDelayUs(500);
        
    	/* Start rotation sequence, ad1[7][5][4][3][2][1]=000110 */
        AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
        AdcTempDat &= ~(BIT7|BIT5|BIT4|BIT3|BIT2|BIT1);
        AdcTempDat |= (BIT3|BIT2);
    	HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD1, AdcTempDat);
        HalDelayUs(500);

        /* Read Content */
        for (tempcnt=0; tempcnt<16; tempcnt++){
            ADCDatBuf[0]    = (u32)HAL_ADC_READ32(REG_ADC_FIFO_READ);            
        }
    }else {
        /* Open ADC */
        AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
    	DBG_ADC_INFO("RtkADCRxManualRotate, Before set, Reg AD1:%x\n", AdcTempDat);
    	AdcTempDat |= (BIT0);
    	HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD1, AdcTempDat);
    	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
    	DBG_ADC_INFO("RtkADCRxManualRotate, After set, Reg AD1:%x\n", AdcTempDat);
    	
    	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);
    	DBG_ADC_INFO("RtkADCRxManualRotate, Before set, Reg AD0:%x\n", AdcTempDat);
    	AdcTempDat |= (0x01);
    	HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD0, AdcTempDat);
    	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);
    	DBG_ADC_INFO("RtkADCRxManualRotate, After set, Reg AD0:%x\n", AdcTempDat);
        
    	/* Start rotation sequence, ad1[7][5][4][3][2][1]=001010 */
        AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
        AdcTempDat &= ~(BIT7|BIT5|BIT4|BIT3|BIT2|BIT1);
        AdcTempDat |= (BIT4|BIT2);
    	HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD1, AdcTempDat);
        HalDelayUs(500);
        
    	/* Start rotation sequence, ad1[7][5][4][3][2][1]=010010 */
        AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
        AdcTempDat &= ~(BIT7|BIT5|BIT4|BIT3|BIT2|BIT1);
        AdcTempDat |= (BIT5|BIT2);
    	HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD1, AdcTempDat);
        HalDelayUs(500);

        //DBG_8195A("INT STAT:%08x\n", HAL_ADC_READ32(REG_ADC_INTR_STS));
    	/* Read ADC FIFO */
    	/* Read Content */
        for (tempcnt=0; tempcnt<16; tempcnt++){
            ADCDatBuf[1]    = (u32)HAL_ADC_READ32(REG_ADC_FIFO_READ);            
        }
    }

    /* Close ADC */	
	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);
	DBG_ADC_INFO("RtkADCRxManualRotate, End of ADC, Before set, AD0:%x\n", AdcTempDat);
	AdcTempDat &= (~0x01);
	HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD0, AdcTempDat);
	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);
	DBG_ADC_INFO("RtkADCRxManualRotate, End of ADC, After set, AD0:%x\n", AdcTempDat);

	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
	DBG_ADC_INFO("RtkADCRxManualRotate, End of ADC, Before set, AD1:%x\n", AdcTempDat);
	AdcTempDat &= (~0x01);

    /* Disable manual mode */
    AdcTempDat &= (~BIT11);
    /* Set roration to default state */
	AdcTempDat &= ~(BIT7|BIT5|BIT4|BIT3|BIT2|BIT1);
	
	HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD1, AdcTempDat);
	AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
	DBG_ADC_INFO("RtkADCRxManualRotate, End of ADC, After set, AD1:%x\n", AdcTempDat);
	
    /* Clear ADC Status */
	HAL_ADC_READ32(REG_ADC_INTR_STS);
	ADCFullStsFlag = 0;

    *pBuf       = (u32)ADCDatBuf[0];
    *(pBuf+1)   = (u32)ADCDatBuf[1];
    ADCDatBuf[0] = 0;
    ADCDatBuf[1] = 0;
    
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
PSAL_ADC_HND
RtkADCGetSalHnd(
    IN  u8  ADCIdx
){
    PSAL_ADC_MNGT_ADPT  pSalADCMngtAdpt = NULL;
    PSAL_ADC_HND        pSalADCHND      = NULL;

    /* Check the user define setting and the given index */
    if (RtkADCIdxChk(ADCIdx)) {
        return (PSAL_ADC_HND)NULL;
    }

    /* Invoke RtkI2CGetMngtAdpt to get the I2C SAL management adapter pointer */
    pSalADCMngtAdpt = RtkADCGetMngtAdpt(ADCIdx);

    /* Assign the private SAL handle to public SAL handle */
    pSalADCHND      = &(pSalADCMngtAdpt->pSalHndPriv->SalADCHndPriv);

    /* Assign the internal HAL initial data pointer to the SAL handle */
    pSalADCHND->pInitDat    = pSalADCMngtAdpt->pHalInitDat;

    /* Assign the internal user callback pointer to the SAL handle */
    pSalADCHND->pUserCB     = pSalADCMngtAdpt->pUserCB;

    return &(pSalADCMngtAdpt->pSalHndPriv->SalADCHndPriv);

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
RtkADCFreeSalHnd(
    IN  PSAL_ADC_HND    pSalADCHND
){
    PSAL_ADC_MNGT_ADPT  pSalADCMngtAdpt     = NULL;
    PSAL_ADC_HND_PRIV   pSalADCHNDPriv      = NULL;

    /* To get the SAL_DAC_MNGT_ADPT pointer */
    pSalADCHNDPriv  = CONTAINER_OF(pSalADCHND, SAL_ADC_HND_PRIV, SalADCHndPriv);
    pSalADCMngtAdpt = CONTAINER_OF(pSalADCHNDPriv->ppSalADCHnd, SAL_ADC_MNGT_ADPT, pSalHndPriv);

    /* Invoke RtkDACFreeMngtAdpt to free all the lower layer memory space */
    return (RtkADCFreeMngtAdpt(pSalADCMngtAdpt));
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
RtkADCLoadDefault(
    IN  VOID *Data
){
    PSAL_ADC_HND    pSalADCHND = (PSAL_ADC_HND) Data;

    /* Check the input ADC index first */
    if (RtkADCIdxChk(pSalADCHND->DevNum))
        return _EXIT_FAILURE;
        
    /* Load SAL handle default value */
    pSalADCHND->PinMux          =   0;
    pSalADCHND->OpType          =   ADC_RDREG_TYPE;
    pSalADCHND->DevSts          =   ADC_STS_UNINITIAL;
    pSalADCHND->ADCExd          =   0;
    pSalADCHND->ErrType         =   (u32)NULL;

    /* Load HAL initial data structure default value */
    pSalADCHND->pInitDat->ADCIdx        =   pSalADCHND->DevNum;
    pSalADCHND->pInitDat->ADCEn         =   ADC_DISABLE;
    pSalADCHND->pInitDat->ADCEndian     =   ADC_DATA_ENDIAN_LITTLE;
    pSalADCHND->pInitDat->ADCBurstSz    =   8;
    pSalADCHND->pInitDat->ADCCompOnly   =   ADC_FEATURE_DISABLED;
    pSalADCHND->pInitDat->ADCOneShotEn  =   ADC_FEATURE_DISABLED;
    pSalADCHND->pInitDat->ADCOverWREn   =   ADC_FEATURE_DISABLED;
    pSalADCHND->pInitDat->ADCOneShotTD  =   8;
    pSalADCHND->pInitDat->ADCCompCtrl   =   ADC_COMP_SMALLER_THAN;
    pSalADCHND->pInitDat->ADCCompTD     =   8;
    pSalADCHND->pInitDat->ADCDataRate   =   0;
    pSalADCHND->pInitDat->ADCAudioEn    =   ADC_FEATURE_DISABLED;
    pSalADCHND->pInitDat->ADCEnManul    =   ADC_FEATURE_DISABLED;
    pSalADCHND->pInitDat->ADCDbgSel     =   ADC_DBG_SEL_DISABLE;
    pSalADCHND->pInitDat->ADCPWCtrl     =   0;
    pSalADCHND->pInitDat->ADCIntrMSK    =   ADC_FEATURE_DISABLED;
    pSalADCHND->pInitDat->ADCAnaParAd3  =   0;
    return _EXIT_SUCCESS;
}


//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkADCDisablePS
//
// Description:
//         ADC disable opertion by setting clock disable.
//		
// Arguments:
//		[in] VOID *Data -
//			ADC SAL handle
//
// Return:
//		The status of the ADC disable process.
//          HAL_OK if the RtkADCDisablePS succeeded.
//          HAL_ERR_PARA if the RtkADCDisablePS failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2015-06-15.
//
//----------------------------------------------------------------------------------------------------
HAL_Status
RtkADCDisablePS(
    IN  VOID *Data
){

    PSAL_ADC_HND    pSalADCHND  =   (PSAL_ADC_HND) Data;
    u8 adcIdx = pSalADCHND->DevNum;
#ifdef CONFIG_SOC_PS_MODULE
    REG_POWER_STATE adcPwrState;
#endif

    if (RtkADCIdxChk(adcIdx))
        return HAL_ERR_UNKNOWN;
        

#ifdef CONFIG_SOC_PS_MODULE
    SalAdcEnableState &= (~(0x01 << pSalADCHND->DevNum));

    if (SalAdcEnableState == 0) {
        // To register a new peripheral device power state
        adcPwrState.FuncIdx = ADC0;
        adcPwrState.PwrState = SLPCG;   
        RegPowerState(adcPwrState);
    }   
#endif

    return HAL_OK;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkADCEnablePS
//
// Description:
//         ADC enable opertion by setting clock enable.
//		
// Arguments:
//		[in] VOID *Data -
//			ADC SAL handle
//
// Return:
//		The status of the ADC enable process.
//          HAL_OK if the RtkADCEnablePS succeeded.
//          HAL_ERR_PARA if the RtkADCEnablePS failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2015-06-15.
//
//----------------------------------------------------------------------------------------------------
HAL_Status
RtkADCEnablePS(
    IN  VOID *Data
){

    PSAL_ADC_HND    pSalADCHND  =   (PSAL_ADC_HND) Data;
    u8 adcIdx = pSalADCHND->DevNum;
#ifdef CONFIG_SOC_PS_MODULE
    REG_POWER_STATE adcPwrState;
#endif

    if (RtkADCIdxChk(adcIdx))
        return HAL_ERR_UNKNOWN;

#ifdef CONFIG_SOC_PS_MODULE
    SalAdcEnableState |= (0x01 << pSalADCHND->DevNum);
    
    // To register a new peripheral device power state
    adcPwrState.FuncIdx = ADC0;
    adcPwrState.PwrState = ACT;   
    RegPowerState(adcPwrState);
#endif

    return HAL_OK;
}


u32
RtkADCReceiveDMA(
    IN  VOID *Data,
    IN  u8 multiblock
){
    PSAL_ADC_HND        pSalADCHND          = (PSAL_ADC_HND) Data;
    PSAL_ADC_HND_PRIV   pSalADCHNDPriv      = NULL;
    PSAL_ADC_MNGT_ADPT  pSalADCMngtAdpt     = NULL;
    PHAL_GDMA_ADAPTER   pHALADCGdmaAdpt     = NULL;
    PHAL_GDMA_OP        pHALADCGdmaOp       = NULL;
    PHAL_ADC_OP         pHalADCOP           = NULL; 
    u32 AdcTempDat;
    
    /* To Get the SAL_I2C_MNGT_ADPT Pointer */
    pSalADCHNDPriv  = CONTAINER_OF(pSalADCHND, SAL_ADC_HND_PRIV, SalADCHndPriv);
    pSalADCMngtAdpt = CONTAINER_OF(pSalADCHNDPriv->ppSalADCHnd, SAL_ADC_MNGT_ADPT, pSalHndPriv);
    
    pHALADCGdmaAdpt     = pSalADCMngtAdpt->pHalGdmaAdp;
    pHALADCGdmaOp       = pSalADCMngtAdpt->pHalGdmaOp;
    pHalADCOP           = pSalADCMngtAdpt->pHalOp;
	
    if (pSalADCHND->OpType == ADC_DMA_TYPE) {
        RtkADCDMAInit(pSalADCHND);
        
		/* Clear ADC Status */
		HAL_ADC_READ32(REG_ADC_INTR_STS);
		if (multiblock == 0) {
            HalGdmaOpInit(pHALADCGdmaOp);
            pHALADCGdmaAdpt->GdmaCtl.BlockSize  =   pSalADCHND->pRXBuf->DataLen;
            pHALADCGdmaAdpt->ChSar              =   (u32)(ADC_REG_BASE);
            pHALADCGdmaAdpt->ChDar              =   (u32)pSalADCHND->pRXBuf->pDataBuf;
            pHALADCGdmaAdpt->MuliBlockCunt      = 0;
            
            pHALADCGdmaOp->HalGdmaChSeting(pHALADCGdmaAdpt);
            pHALADCGdmaOp->HalGdmaChEn(pHALADCGdmaAdpt);

            DBG_ADC_INFO("RtkADCReceiveDMA, Check to enable ADC manully or not\n");
            pSalADCHND->DevSts  = ADC_STS_RX_ING;
            AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_POWER);
        	if (unlikely((AdcTempDat & 0x00000008) == 0)) {
        		;
        	}
        	else {
        		pSalADCHND->pInitDat->ADCEn = ADC_ENABLE;
            	pHalADCOP->HalADCEnable(pSalADCHND->pInitDat);  
        	}

            AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);

        	if ((AdcTempDat & 0x00000001) == 0){
                pSalADCHND->pInitDat->ADCIntrMSK  =  (BIT_ADC_FIFO_FULL_EN);
                pHalADCOP->HalADCIntrCtrl(pSalADCHND->pInitDat);

                AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
        		DBG_ADC_INFO("RtkADCReceiveDMA, Before set, Reg AD1:%x\n", AdcTempDat);

        		AdcTempDat |= (0x01);
        		HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD1, AdcTempDat);
        		AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD1);
        		DBG_ADC_INFO("RtkADCReceiveDMA, After set, Reg AD1:%x\n", AdcTempDat);

        		AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);
        		DBG_ADC_INFO("RtkADCReceiveDMA, Before set, Reg AD0:%x\n", AdcTempDat);

        		AdcTempDat |= (0x01);
        		HAL_ADC_WRITE32(REG_ADC_ANAPAR_AD0, AdcTempDat);
        		AdcTempDat  = (u32)HAL_ADC_READ32(REG_ADC_ANAPAR_AD0);
        		DBG_ADC_INFO("RtkADCReceiveDMA, After set, Reg AD0:%x\n", AdcTempDat);

        	}
        	else{
        		;
        	}
           
            return _EXIT_SUCCESS;
        } else {
            /* multi block is NOT support. */
            return _EXIT_FAILURE;
        }
    }
    
    return _EXIT_FAILURE;
}

