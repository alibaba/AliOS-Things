/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"
#include "hal_i2s.h"
#include "rand.h"
#include "rtl_utility.h"


//1 need to be modified 


/*======================================================
    Local used variables
*/
SRAM_BF_DATA_SECTION
HAL_I2S_OP HalI2SOpSAL={0};


VOID
I2SISRHandle(
    IN  VOID *Data
)
{
    PHAL_I2S_ADAPTER pI2SAdp = (PHAL_I2S_ADAPTER) Data;
    PHAL_I2S_OP     pHalI2SOP = &HalI2SOpSAL;
    PHAL_I2S_INIT_DAT pI2SCfg = pI2SAdp->pInitDat;
    u32 I2STxIsr, I2SRxIsr;
    u8  I2SPageNum = pI2SCfg->I2SPageNum+1;
//    u32 I2SPageSize = (pI2SAdp->I2SPageSize+1)<<2;
    u32 i;
    u32 pbuf;

    I2STxIsr = pHalI2SOP->HalI2SReadReg(pI2SCfg, REG_I2S_TX_STATUS_INT);
    I2SRxIsr = pHalI2SOP->HalI2SReadReg(pI2SCfg, REG_I2S_RX_STATUS_INT);

    pI2SCfg->I2STxIntrClr = I2STxIsr;
    pI2SCfg->I2SRxIntrClr = I2SRxIsr;
    pHalI2SOP->HalI2SClrIntr(pI2SCfg);

    for (i=0 ; i<I2SPageNum ; i++) { // page 0, 1, 2, 3
        if (I2STxIsr & (1<<pI2SCfg->I2SHWTxIdx)) {
//            pbuf = ((u32)(pI2SCfg->I2STxData)) + (I2SPageSize*pI2SCfg->I2SHWTxIdx);
            pbuf = (u32)pI2SAdp->TxPageList[pI2SCfg->I2SHWTxIdx];
            pI2SAdp->UserCB.TxCCB(pI2SAdp->UserCB.TxCBId, (char*)pbuf);
            I2STxIsr &= ~(1<<pI2SCfg->I2SHWTxIdx);
            pI2SCfg->I2SHWTxIdx += 1;
            if (pI2SCfg->I2SHWTxIdx == I2SPageNum) {
                pI2SCfg->I2SHWTxIdx = 0;
            }
        }
        
        if (I2SRxIsr & (1<<pI2SCfg->I2SHWRxIdx)) {
//            pbuf = ((u32)(pI2SCfg->I2SRxData)) + (I2SPageSize*pI2SCfg->I2SHWRxIdx);
            pbuf = (u32)pI2SAdp->RxPageList[pI2SCfg->I2SHWRxIdx];
            pI2SAdp->UserCB.RxCCB(pI2SAdp->UserCB.RxCBId, (char*)pbuf);
            I2SRxIsr &= ~(1<<pI2SCfg->I2SHWRxIdx);
            pI2SCfg->I2SHWRxIdx += 1;
            if (pI2SCfg->I2SHWRxIdx == I2SPageNum) {
                pI2SCfg->I2SHWRxIdx = 0;
            }
        }
    }
}


static HAL_Status
RtkI2SIrqInit(
    IN  PHAL_I2S_ADAPTER pI2SAdapter
)
{
    PIRQ_HANDLE pIrqHandle;

    if (pI2SAdapter->DevNum > I2S_MAX_ID) {
        DBG_I2S_ERR("RtkI2SIrqInit: Invalid I2S Index(&d)\r\n", pI2SAdapter->DevNum);
        return HAL_ERR_PARA;
    }
    
    pIrqHandle = &pI2SAdapter->IrqHandle;
    
    switch (pI2SAdapter->DevNum){
        case I2S0_SEL:
            pIrqHandle->IrqNum = I2S0_PCM0_IRQ;
            break;
            
        case I2S1_SEL:
            pIrqHandle->IrqNum = I2S1_PCM1_IRQ;
            break;
            
        default:
            return HAL_ERR_PARA;
    }

    pIrqHandle->Data = (u32) (pI2SAdapter);
    pIrqHandle->IrqFun = (IRQ_FUN) I2SISRHandle;
    pIrqHandle->Priority = 6;
    InterruptRegister(pIrqHandle);
    InterruptEn(pIrqHandle);
    
    return HAL_OK;
}

static HAL_Status
RtkI2SIrqDeInit(
    IN  PHAL_I2S_ADAPTER pI2SAdapter
)
{
    if (pI2SAdapter->DevNum > I2S_MAX_ID) {
        DBG_I2S_ERR("RtkI2SIrqDeInit: Invalid I2S Index(&d)\r\n", pI2SAdapter->DevNum);
        return HAL_ERR_PARA;
    }

    InterruptDis(&pI2SAdapter->IrqHandle);
    InterruptUnRegister(&pI2SAdapter->IrqHandle);  

    return HAL_OK;
}

static HAL_Status
RtkI2SPinMuxInit(
    IN  PHAL_I2S_ADAPTER pI2SAdapter
)
{
    u32 I2Stemp;

    if (pI2SAdapter->DevNum > I2S_MAX_ID) {
        DBG_I2S_ERR("RtkI2SPinMuxInit: Invalid I2S Index(&d)\r\n", pI2SAdapter->DevNum);
        return HAL_ERR_PARA;
    }

    // enable system pll
    I2Stemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL1) | (1<<9) | (1<<10);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL1, I2Stemp);

    switch (pI2SAdapter->DevNum){
        case I2S0_SEL:
            ACTCK_I2S_CCTRL(ON);
            SLPCK_I2S_CCTRL(ON);
            LXBUS_FCTRL(ON); // enable lx bus for i2s
            
            /*I2S0 Pin Mux Setting*/
            PinCtrl(I2S0, pI2SAdapter->PinMux, ON);
            if (pI2SAdapter->PinMux == I2S_S0) {
                DBG_I2S_WARN(ANSI_COLOR_MAGENTA"I2S0 Pin may conflict with JTAG\r\n"ANSI_COLOR_RESET);
            }
            I2S0_MCK_CTRL(ON);
            I2S0_PIN_CTRL(ON);
            I2S0_FCTRL(ON);
            
            break;
        case I2S1_SEL:
            ACTCK_I2S_CCTRL(ON);
            SLPCK_I2S_CCTRL(ON);
            LXBUS_FCTRL(ON); // enable lx bus for i2s
            
            /*I2S1 Pin Mux Setting*/
            PinCtrl(I2S1, pI2SAdapter->PinMux, ON);
            if (pI2SAdapter->PinMux == I2S_S2) {
                DBG_I2S_WARN(ANSI_COLOR_MAGENTA"I2S1 Pin may conflict with JTAG\r\n"ANSI_COLOR_RESET);
            }
            I2S1_MCK_CTRL(ON);
            I2S1_PIN_CTRL(ON);
            I2S0_FCTRL(ON); //i2s 1 is control by bit 24 BIT_PERI_I2S0_EN
            I2S1_FCTRL(ON);
            break;
        default:
            return HAL_ERR_PARA;
    }
    
    return HAL_OK;
}


static HAL_Status
RtkI2SPinMuxDeInit(
    IN  PHAL_I2S_ADAPTER pI2SAdapter
)
{
    if (pI2SAdapter->DevNum > I2S_MAX_ID) {
        DBG_I2S_ERR("RtkI2SPinMuxDeInit: Invalid I2S Index(&d)\r\n", pI2SAdapter->DevNum);
        return HAL_ERR_PARA;
    }

    switch (pI2SAdapter->DevNum){
        case I2S0_SEL:
            /*I2S0 Pin Mux Setting*/
            //ACTCK_I2C0_CCTRL(OFF);
            PinCtrl(I2S0, pI2SAdapter->PinMux, OFF);
            I2S0_MCK_CTRL(OFF);
            I2S0_PIN_CTRL(OFF);
            //I2S0_FCTRL(OFF);
            
            break;
        case I2S1_SEL:
            /*I2S1 Pin Mux Setting*/
            //ACTCK_I2C1_CCTRL(OFF);
            PinCtrl(I2S1, pI2SAdapter->PinMux, OFF);
            I2S1_MCK_CTRL(OFF);
            I2S1_PIN_CTRL(OFF);
            //I2S1_FCTRL(OFF);
            break;
        default:
            return HAL_ERR_PARA;
    }
    
    return HAL_OK;
}


HAL_Status
RtkI2SInit(
    IN  VOID *Data
)
{
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) Data;
    PHAL_I2S_OP     pHalI2SOP  = &HalI2SOpSAL;
    PHAL_I2S_INIT_DAT pI2SCfg;

    if (pI2SAdapter == 0) {
        DBG_I2S_ERR("RtkI2SInit: Null Pointer\r\n");
        return HAL_ERR_PARA;
    }
    
    if (pI2SAdapter->DevNum > I2S_MAX_ID) {
        DBG_I2S_ERR("RtkI2SInit: Invalid I2S Index(&d)\r\n", pI2SAdapter->DevNum);
        return HAL_ERR_PARA;
    }

    pI2SCfg = pI2SAdapter->pInitDat;

    /*I2S Initialize HAL Operations*/
    HalI2SOpInit(pHalI2SOP);
    
    /*I2S Interrupt Initialization*/
    RtkI2SIrqInit(pI2SAdapter);

    /*I2S Pin Mux Initialization*/
    RtkI2SPinMuxInit(pI2SAdapter);

    /*I2S Load User Setting*/
    pI2SCfg->I2SIdx = pI2SAdapter->DevNum;

    /*I2S HAL Initialization*/
    pHalI2SOP->HalI2SInit(pI2SCfg);

    /*I2S Device Status Update*/
    pI2SAdapter->DevSts  = I2S_STS_INITIALIZED;
    
    /*I2S Enable Module*/
    pI2SCfg->I2SEn = I2S_ENABLE;
    pHalI2SOP->HalI2SEnable(pI2SCfg);

    /*I2S Device Status Update*/
    pI2SAdapter->DevSts  = I2S_STS_IDLE;    
    
    return HAL_OK;
}

HAL_Status
RtkI2SDeInit(
    IN  VOID *Data
)
{
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) Data;
    PHAL_I2S_OP     pHalI2SOP  = &HalI2SOpSAL;
    PHAL_I2S_INIT_DAT pI2SCfg;
    u32 I2Stemp;

    if (pI2SAdapter == 0) {
        DBG_I2S_ERR("RtkI2SDeInit: Null Pointer\r\n");
        return HAL_ERR_PARA;
    }

    pI2SCfg = pI2SAdapter->pInitDat;

    /*I2S Disable Module*/
    pI2SCfg->I2SEn = I2S_DISABLE;
    pHalI2SOP->HalI2SEnable(pI2SCfg);
    HalI2SClearAllOwnBit((VOID*)pI2SCfg);

    /*I2C HAL DeInitialization*/
    //pHalI2SOP->HalI2SDeInit(pI2SCfg);
    
    /*I2S Interrupt DeInitialization*/
    RtkI2SIrqDeInit(pI2SAdapter);

    /*I2S Pin Mux DeInitialization*/
    RtkI2SPinMuxDeInit(pI2SAdapter);
    
    /*I2S HAL DeInitialization*/
    pHalI2SOP->HalI2SDeInit(pI2SCfg);

    /*I2S CLK Source Close*/
    I2Stemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL1) & (~((1<<9) | (1<<10)));
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL1, I2Stemp);     

    /*I2S Device Status Update*/
    pI2SAdapter->DevSts  = I2S_STS_UNINITIAL;
    
    return HAL_OK;
}

HAL_Status
RtkI2SEnable(
    IN  VOID *Data
)
{
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) Data;
    PHAL_I2S_OP     pHalI2SOP  = &HalI2SOpSAL;
    PHAL_I2S_INIT_DAT pI2SCfg;
    u32 I2Stemp;

    // Enable IP Clock
    I2Stemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL1) | (1<<9) | (1<<10);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL1, I2Stemp);  
    ACTCK_I2S_CCTRL(ON);
    SLPCK_I2S_CCTRL(ON);

    pI2SCfg = pI2SAdapter->pInitDat;
    pI2SCfg->I2SEn = I2S_ENABLE;
    pHalI2SOP->HalI2SEnable(pI2SCfg);

    return HAL_OK;
}

HAL_Status
RtkI2SDisable(
    IN  VOID *Data
)
{
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) Data;
    PHAL_I2S_OP     pHalI2SOP  = &HalI2SOpSAL;
    PHAL_I2S_INIT_DAT pI2SCfg;
    u32 I2Stemp;

    pI2SCfg = pI2SAdapter->pInitDat;
    pI2SCfg->I2SEn = I2S_DISABLE;
    pHalI2SOP->HalI2SEnable(pI2SCfg);

    // Gate IP Clock
    ACTCK_I2S_CCTRL(OFF);
    SLPCK_I2S_CCTRL(OFF);

    // Close I2S bus clock(WS,SCLK,MCLK). If needs that clock, mark this. 
    I2Stemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL1) & (~((1<<9) | (1<<10)));
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL1, I2Stemp);    
   
    return HAL_OK;
}

RTK_STATUS
RtkI2SIoCtrl(
    IN  VOID *Data
)
{
    return _EXIT_SUCCESS;
}

RTK_STATUS
RtkI2SPowerCtrl(
    IN  VOID *Data
)
{
    return _EXIT_SUCCESS;
}

HAL_Status
RtkI2SLoadDefault( 
    IN  VOID *Adapter,
    IN  VOID *Setting
)
{
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) Adapter;
    PHAL_I2S_INIT_DAT pI2SCfg = pI2SAdapter->pInitDat;
    PHAL_I2S_DEF_SETTING pLoadSetting = (PHAL_I2S_DEF_SETTING)Setting;

    if (pI2SAdapter == 0) {
        DBG_I2S_ERR("RtkI2SLoadDefault: Null Pointer\r\n");
        return HAL_ERR_PARA;
    }

    if (pI2SAdapter->pInitDat == NULL) {
        DBG_I2S_ERR("RtkI2SLoadDefault: pInitDat is NULL!\r\n", pI2SAdapter->DevNum);
        return HAL_ERR_PARA;
    }

    pI2SAdapter->DevSts          = pLoadSetting->DevSts;
    pI2SAdapter->ErrType         = 0;
    pI2SAdapter->TimeOut         = 0;
    
    pI2SCfg->I2SIdx        = pI2SAdapter->DevNum;
    pI2SCfg->I2SEn         = I2S_DISABLE;
    pI2SCfg->I2SMaster     = pLoadSetting->I2SMaster;
    pI2SCfg->I2SWordLen    = pLoadSetting->I2SWordLen;
    pI2SCfg->I2SChNum      = pLoadSetting->I2SChNum;
    pI2SCfg->I2SPageNum    = pLoadSetting->I2SPageNum;
    pI2SCfg->I2SPageSize   = pLoadSetting->I2SPageSize;
    pI2SCfg->I2SRate       = pLoadSetting->I2SRate;
    pI2SCfg->I2STRxAct     = pLoadSetting->I2STRxAct;
    pI2SCfg->I2STxIntrMSK  = pLoadSetting->I2STxIntrMSK;
    pI2SCfg->I2SRxIntrMSK  = pLoadSetting->I2SRxIntrMSK;

    return HAL_OK;
}

VOID HalI2SOpInit(
    IN  VOID *Data
)
{
    PHAL_I2S_OP pHalI2SOp = (PHAL_I2S_OP) Data;

    pHalI2SOp->HalI2SDeInit      = HalI2SDeInitRtl8195a;
    pHalI2SOp->HalI2STx          = HalI2STxRtl8195a;
    pHalI2SOp->HalI2SRx          = HalI2SRxRtl8195a;
    pHalI2SOp->HalI2SEnable      = HalI2SEnableRtl8195a;
    pHalI2SOp->HalI2SIntrCtrl    = HalI2SIntrCtrlRtl8195a;
    pHalI2SOp->HalI2SReadReg     = HalI2SReadRegRtl8195a;
    pHalI2SOp->HalI2SClrIntr     = HalI2SClrIntrRtl8195a;
    pHalI2SOp->HalI2SClrAllIntr  = HalI2SClrAllIntrRtl8195a;
    pHalI2SOp->HalI2SDMACtrl     = HalI2SDMACtrlRtl8195a;

#ifndef CONFIG_CHIP_E_CUT
    pHalI2SOp->HalI2SInit        = HalI2SInitRtl8195a_Patch;
    pHalI2SOp->HalI2SSetRate     = HalI2SSetRateRtl8195a;
    pHalI2SOp->HalI2SSetWordLen  = HalI2SSetWordLenRtl8195a;
    pHalI2SOp->HalI2SSetChNum    = HalI2SSetChNumRtl8195a;
    pHalI2SOp->HalI2SSetPageNum  = HalI2SSetPageNumRtl8195a;
    pHalI2SOp->HalI2SSetPageSize = HalI2SSetPageSizeRtl8195a;
#else
    pHalI2SOp->HalI2SInit        = HalI2SInitRtl8195a_V04;
    pHalI2SOp->HalI2SSetRate     = HalI2SSetRateRtl8195a_V04;
    pHalI2SOp->HalI2SSetWordLen  = HalI2SSetWordLenRtl8195a_V04;
    pHalI2SOp->HalI2SSetChNum    = HalI2SSetChNumRtl8195a_V04;
    pHalI2SOp->HalI2SSetPageNum  = HalI2SSetPageNumRtl8195a_V04;
    pHalI2SOp->HalI2SSetPageSize = HalI2SSetPageSizeRtl8195a_V04;
#endif  // #ifndef CONFIG_CHIP_E_CUT
}

HAL_Status 
HalI2SInit(
    IN VOID *Data
)
{
    HAL_Status ret;
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) Data;
    u32 Function;
    u8 funret;    
    
#ifdef CONFIG_SOC_PS_MODULE
    REG_POWER_STATE I2sPwrState;
#endif

    if(pI2SAdapter->DevNum == 0){
        Function = I2S0;
    }
    else {
        Function = I2S1;
    }
        
    funret = FunctionChk(Function, (u32)pI2SAdapter->PinMux);

    if (funret == _FALSE){
        return HAL_ERR_HW;
    }

    ret = RtkI2SInit(Data);
#ifdef CONFIG_SOC_PS_MODULE
    if(ret == HAL_OK) {
        // To register a new peripheral device power state
        I2sPwrState.FuncIdx = I2S0 + pI2SAdapter->DevNum;
        I2sPwrState.PwrState = ACT;
        RegPowerState(I2sPwrState);
    }
#endif

    return ret;

}

VOID 
HalI2SDeInit(
    IN VOID *Data
)
{
#ifdef CONFIG_SOC_PS_MODULE
    REG_POWER_STATE I2sPwrState;
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) Data;
    u8 HwState;

    I2sPwrState.FuncIdx = I2S0 + pI2SAdapter->DevNum;
    QueryRegPwrState(I2sPwrState.FuncIdx, &(I2sPwrState.PwrState), &HwState);

    // if the power state isn't ACT, then switch the power state back to ACT first
    if ((I2sPwrState.PwrState != ACT) && (I2sPwrState.PwrState != INACT)) {
        HalI2SEnable(Data);
        QueryRegPwrState(I2sPwrState.FuncIdx, &(I2sPwrState.PwrState), &HwState);
    }

    if (I2sPwrState.PwrState == ACT) {
        I2sPwrState.PwrState = INACT;
        RegPowerState(I2sPwrState);
    }   
#endif

    RtkI2SDeInit(Data);  

}


HAL_Status 
HalI2SDisable(
    IN VOID *Data
)
{
    HAL_Status ret;
#ifdef CONFIG_SOC_PS_MODULE
    REG_POWER_STATE I2sPwrState;
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) Data;
#endif

    ret = RtkI2SDisable(Data);
#ifdef CONFIG_SOC_PS_MODULE
    if (ret == HAL_OK) {
        I2sPwrState.FuncIdx = I2S0 + pI2SAdapter->DevNum;
        I2sPwrState.PwrState = SLPCG;
        RegPowerState(I2sPwrState);
    }
#endif
    return ret;
}

HAL_Status 
HalI2SEnable(
    IN VOID *Data
)
{
    HAL_Status ret;
#ifdef CONFIG_SOC_PS_MODULE
    REG_POWER_STATE I2sPwrState;
    PHAL_I2S_ADAPTER pI2SAdapter = (PHAL_I2S_ADAPTER) Data;
#endif

    ret = RtkI2SEnable(Data);
#ifdef CONFIG_SOC_PS_MODULE
    if (ret == HAL_OK) {
        I2sPwrState.FuncIdx = I2S0 + pI2SAdapter->DevNum;
        I2sPwrState.PwrState = ACT;
        RegPowerState(I2sPwrState);
    }
#endif
    return ret;
}

