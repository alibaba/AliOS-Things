/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"

#ifdef CONFIG_TIMER_MODULE

VOID
En32KCalibration(
    VOID
)
{
    u32 Rtemp;
    u32 Ttemp = 0;

    //DiagPrintf("32K clock source calibration\n");

    //set parameter
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, 0);
    //offset 1 = 0x1500
    Rtemp = 0x811500;
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, Rtemp);
    HalDelayUs(40);
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, 0);

    //offset 2 = 0x01c0
    Rtemp = 0x8201c0;
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, Rtemp);
    HalDelayUs(40);
        HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, 0);
    
    //offset 4 = 0x0100
    Rtemp = 0x840100;
        HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, Rtemp);
    HalDelayUs(40);
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, 0);

    //offset 0 = 0xf980
    Rtemp = 0x80f980;
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, Rtemp);
    HalDelayUs(40);
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, 0);

    while(1) {
        //Polling LOCK
        Rtemp = 0x110000;
        HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, Rtemp);
        //DiagPrintf("Polling lock\n");
        HalDelayUs(40);

        Rtemp = HAL_READ32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL1);
        if ((Rtemp & 0x3000) != 0x0){
            //DiagPrintf("32.768 Calibration Success\n", Ttemp);
            break;
        }
        else {
            Ttemp++;
            HalDelayUs(30);
            //DiagPrintf("Check lock: %d\n", Ttemp);
            //DiagPrintf("0x278: %x\n", Rtemp);
            if (Ttemp > 100000) { /*Delay 100ms*/            
                DiagPrintf("32K Calibration Fail!!\n", Ttemp);
                break;
            }
        }
    }
}

#if CONFIG_WDG
WDG_ADAPTER          WDGAdapter;
extern HAL_TIMER_OP HalTimerOp;

#ifdef CONFIG_WDG_NORMAL
VOID
WDGInitial(
    IN  u32 Period
)
{
    u8 CountId;
    u16 DivFactor;
    u32 CountTemp; 
    u32 CountProcess = 0;
    u32 DivFacProcess = 0;
    u32 PeriodProcess = 100*Period;
    u32 MinPeriodTemp = 0xFFFFFFFF;
    u32 PeriodTemp = 0;
    u32 *Reg = (u32*)&(WDGAdapter.Ctrl);
    
    DBG_8195A(" Period = 0x%08x\n", Period);

    for (CountId = 0; CountId < 12; CountId++) {
        CountTemp = ((0x00000001 << (CountId+1))-1);
        DivFactor = (u16)((PeriodProcess)/(CountTemp*3));
        
        if (DivFactor > 0) {
            PeriodTemp = 3*(DivFactor+1)*CountTemp;
            if (PeriodProcess < PeriodTemp) {
                if (MinPeriodTemp > PeriodTemp) {
                    MinPeriodTemp = PeriodTemp;
                    CountProcess = CountId;
                    DivFacProcess = DivFactor;
                }
            }
        }
    }

    DBG_8195A("WdgScalar = 0x%08x\n", DivFacProcess);
    DBG_8195A("WdgCunLimit = 0x%08x\n", CountProcess);

    WDGAdapter.Ctrl.WdgScalar   = DivFacProcess;
    WDGAdapter.Ctrl.WdgEnByte   = 0;
    WDGAdapter.Ctrl.WdgClear    = 1;
    WDGAdapter.Ctrl.WdgCunLimit = CountProcess;
    WDGAdapter.Ctrl.WdgMode     = RESET_MODE;
    WDGAdapter.Ctrl.WdgToISR    = 0;

    HAL_WRITE32(VENDOR_REG_BASE, 0, (*Reg));
    
}

VOID
WDGIrqHandle
(
    IN  VOID        *Data
)
{
    u32 temp;
    WDG_REG *CtrlReg;

    if (NULL != WDGAdapter.UserCallback) {
        WDGAdapter.UserCallback(WDGAdapter.callback_id);
    }

    // Clear ISR
    temp = HAL_READ32(VENDOR_REG_BASE, 0);
    CtrlReg = (WDG_REG*)&temp;
    CtrlReg->WdgToISR    = 1;    // write 1 clear
    HAL_WRITE32(VENDOR_REG_BASE, 0, (temp));
}

VOID
WDGIrqInitial(
    VOID
)
{
    u32 *Temp = (u32*)&(WDGAdapter.Ctrl);
    
    WDGAdapter.IrqHandle.Data = (u32)&WDGAdapter;
    WDGAdapter.IrqHandle.IrqFun = (IRQ_FUN)WDGIrqHandle;
    WDGAdapter.IrqHandle.IrqNum = WDG_IRQ;
    WDGAdapter.IrqHandle.Priority = 0;
         
    InterruptRegister(&(WDGAdapter.IrqHandle));
    InterruptEn(&(WDGAdapter.IrqHandle));

    WDGAdapter.Ctrl.WdgToISR    = 1;    // clear ISR first
    WDGAdapter.Ctrl.WdgMode     = INT_MODE;
    HAL_WRITE32(VENDOR_REG_BASE, 0, ((*Temp)));
    WDGAdapter.Ctrl.WdgToISR    = 0;
}

VOID
WDGStart(
    VOID
)
{
    u32 *Temp = (u32*)&(WDGAdapter.Ctrl);
    WDGAdapter.Ctrl.WdgEnByte    = 0xA5;
    HAL_WRITE32(VENDOR_REG_BASE, 0, ((*Temp)));
}

VOID
WDGStop(
    VOID
)
{
    u32 *Temp = (u32*)&(WDGAdapter.Ctrl);
    WDGAdapter.Ctrl.WdgEnByte    = 0;
    HAL_WRITE32(VENDOR_REG_BASE, 0, ((*Temp)));
}

VOID
WDGRefresh(
    VOID
)
{
    u32 *Temp = (u32*)&(WDGAdapter.Ctrl);
    WDGAdapter.Ctrl.WdgClear    = 1;
    HAL_WRITE32(VENDOR_REG_BASE, 0, ((*Temp)));
} 

VOID
WDGIrqCallBackReg(
    IN VOID *CallBack,
    IN u32   Id
)
{
    WDGAdapter.UserCallback = (VOID (*)(u32))CallBack;
    WDGAdapter.callback_id = Id;
} 

#endif

#ifdef CONFIG_WDG_TEST
VOID
WDGIrqHandle
(
    IN  VOID        *Data
)
{
}


VOID
WDGGtimerHandle
(
    IN  VOID        *Data
)
{
    u32 *Temp = (u32*)&(WDGAdapter.Ctrl);
    WDGAdapter.Ctrl.WdgClear    = 1;
    DBG_8195A("reset WDG\n");
    if (HAL_READ32(SYSTEM_CTRL_BASE,REG_SYS_DSTBY_INFO2) == 0) {
        HAL_WRITE32(VENDOR_REG_BASE, 0, ((*Temp)));
    }
}


VOID
InitWDGIRQ(VOID)
{
    u32 *Temp = (u32*)&(WDGAdapter.Ctrl);
    
    WDGAdapter.Ctrl.WdgScalar   = 0x96;
    WDGAdapter.Ctrl.WdgEnByte   = 0xA5;
    WDGAdapter.Ctrl.WdgClear    = 1;
    WDGAdapter.Ctrl.WdgCunLimit = CNTFFFH;
    WDGAdapter.Ctrl.WdgMode     = RESET_MODE;
    WDGAdapter.Ctrl.WdgToISR    = 0;

    if (WDGAdapter.Ctrl.WdgMode == INT_MODE) {
        
        WDGAdapter.IrqHandle.Data = NULL;
        WDGAdapter.IrqHandle.IrqFun = (IRQ_FUN)WDGIrqHandle;
        WDGAdapter.IrqHandle.IrqNum = WDG_IRQ;
        WDGAdapter.IrqHandle.Priority = 5;
             
        InterruptRegister(&(WDGAdapter.IrqHandle));
        InterruptEn(&(WDGAdapter.IrqHandle));
    }
    else {
        
        WDGAdapter.WdgGTimer.TimerIrqPriority   = 0;
        WDGAdapter.WdgGTimer.TimerMode          = USER_DEFINED;
        WDGAdapter.WdgGTimer.IrqDis             = OFF;
        WDGAdapter.WdgGTimer.TimerId            = 2;//
        WDGAdapter.WdgGTimer.IrqHandle.IrqFun   = (IRQ_FUN)WDGGtimerHandle;
        WDGAdapter.WdgGTimer.IrqHandle.IrqNum   = TIMER2_7_IRQ;
        WDGAdapter.WdgGTimer.IrqHandle.Priority = 5;
        WDGAdapter.WdgGTimer.IrqHandle.Data     = NULL;
        
        if ((WDGAdapter.Ctrl.WdgCunLimit == CNTFFFH)&&(WDGAdapter.Ctrl.WdgScalar >= 0x8429)){
            WDGAdapter.WdgGTimer.TimerLoadValueUs = 0xFFFFFFFF - WDGTIMERELY;
        }
        else {
            WDGAdapter.WdgGTimer.TimerLoadValueUs   = (BIT0 << (WDGAdapter.Ctrl.WdgCunLimit+1))
                *WDGAdapter.Ctrl.WdgScalar*TIMER_TICK_US - WDGTIMERELY;
        }

        HalTimerOp.HalTimerInit((VOID*) &(WDGAdapter.WdgGTimer));
    }
    //fill reg
    HAL_WRITE32(VENDOR_REG_BASE, 0, ((*Temp)));
}


//WDG
VOID HalWdgInit(
    VOID
)
{
        
}
#endif  //CONFIG_WDG_TEST
#endif  //CONFIG_WDG
#endif  //#ifdef CONFIG_TIMER_MODULE
