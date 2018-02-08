/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "rtl8195a.h" 
#include "rtl8195a_timer.h"

extern u32 gTimerRecord;
extern IRQ_FUN Timer2To7VectorTable[MAX_TIMER_VECTOR_TABLE_NUM];

#ifdef CONFIG_CHIP_A_CUT
HAL_RAM_BSS_SECTION u32 gTimerRecord;
#endif

#if defined(CONFIG_CHIP_C_CUT) || defined(CONFIG_CHIP_E_CUT)
extern u32 Timer2To7HandlerData[MAX_TIMER_VECTOR_TABLE_NUM];
#else
u32 Timer2To7HandlerData[MAX_TIMER_VECTOR_TABLE_NUM];
#endif

VOID
HalTimerIrq2To7Handle_Patch(
    IN  VOID    *Data
)
{
    u32 TimerIrqStatus = 0, CheckIndex;
    IRQ_FUN pHandler;

    TimerIrqStatus = HAL_TIMER_READ32(TIMERS_INT_STATUS_OFF);

    DBG_TIMER_INFO("%s:TimerIrqStatus: 0x%x\n",__FUNCTION__, TimerIrqStatus);
    
    for (CheckIndex = 2; CheckIndex<8; CheckIndex++) {

        //3 Check IRQ status bit and Timer X IRQ enable bit
        if ((TimerIrqStatus & BIT_(CheckIndex)) && 
            (HAL_TIMER_READ32(TIMER_INTERVAL*CheckIndex + TIMER_CTL_REG_OFF) & BIT0)) {
            //3 Execute Timer callback function
            pHandler = Timer2To7VectorTable[CheckIndex-2];
            if (pHandler != NULL) {
                pHandler((void*)Timer2To7HandlerData[CheckIndex-2]);
            }
            //3 Clear Timer ISR
            HAL_TIMER_READ32(TIMER_INTERVAL*CheckIndex + TIMER_EOI_OFF);
        }
    }
}

HAL_Status
HalTimerIrqRegisterRtl8195a_Patch(
    IN  VOID    *Data
)
{
    PTIMER_ADAPTER      pHalTimerAdap = (PTIMER_ADAPTER) Data;
    IRQ_HANDLE          TimerIrqHandle;
    //IRQ_FUN             BackUpIrqFun = NULL;
    
    if (pHalTimerAdap->TimerId > 7) {
        DBG_TIMER_ERR("%s: No Support Timer ID %d!\r\n", __FUNCTION__, pHalTimerAdap->TimerId);
        return HAL_ERR_PARA;
    }
    else {
        if (pHalTimerAdap->TimerId > 1) {

            TimerIrqHandle.IrqNum = TIMER2_7_IRQ;
            TimerIrqHandle.IrqFun = (IRQ_FUN) HalTimerIrq2To7Handle_Patch;

            Timer2To7VectorTable[pHalTimerAdap->TimerId-2] = 
                (IRQ_FUN) pHalTimerAdap->IrqHandle.IrqFun;
            Timer2To7HandlerData[pHalTimerAdap->TimerId-2] = 
                (uint32_t) pHalTimerAdap->IrqHandle.Data;
        }
        else {
            TimerIrqHandle.IrqNum = (pHalTimerAdap->TimerId ? TIMER1_IRQ : TIMER0_IRQ);
            TimerIrqHandle.IrqFun = (IRQ_FUN) pHalTimerAdap->IrqHandle.IrqFun;
        }
        TimerIrqHandle.Data = (u32)pHalTimerAdap;
        InterruptRegister(&TimerIrqHandle);
    }
    
    return HAL_OK;
}

#if defined(CONFIG_CHIP_A_CUT) || defined(CONFIG_CHIP_B_CUT)
// Patch for A/B Cut
HAL_Status
HalTimerInitRtl8195a_Patch(
    IN  VOID    *Data
)
{
    PTIMER_ADAPTER pHalTimerAdap = (PTIMER_ADAPTER) Data;
    HAL_Status ret=HAL_OK;
    u32 ControlReg;

    if ((gTimerRecord & (1<<pHalTimerAdap->TimerId)) != 0) {
        DBG_TIMER_ERR ("%s:Error! Timer %d is occupied!\r\n", __FUNCTION__, pHalTimerAdap->TimerId);
        return HAL_BUSY;
    }

    //4 1) Config Timer Setting
    ControlReg = ((u32)pHalTimerAdap->TimerMode<<1)|((u32)pHalTimerAdap->IrqDis<<2);
    /*
        set TimerControlReg
        0: Timer enable (0,disable; 1,enable)
        1: Timer Mode (0, free-running mode; 1, user-defined count mode)
        2: Timer Interrupt Mask (0, not masked; 1,masked)
        */
    HAL_TIMER_WRITE32((TIMER_INTERVAL*pHalTimerAdap->TimerId + TIMER_CTL_REG_OFF), 
                        ControlReg);

    if (pHalTimerAdap->TimerMode) {
        //User-defined Mode
        HalTimerReLoadRtl8195a_Patch(pHalTimerAdap->TimerId ,pHalTimerAdap->TimerLoadValueUs);
    } 
    else {
        // set TimerLoadCount Register
        HAL_TIMER_WRITE32((TIMER_INTERVAL*pHalTimerAdap->TimerId + TIMER_LOAD_COUNT_OFF),
                            0xFFFFFFFF);
    }

    //4  2) Setting Timer IRQ
    if (!pHalTimerAdap->IrqDis) {
        if (pHalTimerAdap->IrqHandle.IrqFun != NULL) {
            //4 2.1) Initial TimerIRQHandle
            ret = HalTimerIrqRegisterRtl8195a_Patch(pHalTimerAdap);
            if (HAL_OK != ret) {
                DBG_TIMER_ERR ("%s: Timer %d Register IRQ Err!\r\n", __FUNCTION__, pHalTimerAdap->TimerId);
                return ret;
            }
            //4 2.2) Enable TimerIRQ for Platform
            InterruptEn((PIRQ_HANDLE)&pHalTimerAdap->IrqHandle);
        }
        else {
            DBG_TIMER_ERR ("%s: Timer %d ISR Handler is NULL!\r\n", __FUNCTION__, pHalTimerAdap->TimerId);
            return HAL_ERR_PARA;
        }
    }
    
    //4 4) Enable Timer
//    HAL_TIMER_WRITE32((TIMER_INTERVAL*pHalTimerAdap->TimerId + TIMER_CTL_REG_OFF), 
//                        (ControlReg|0x1));

    gTimerRecord |= (1<<pHalTimerAdap->TimerId);

    return ret;
}

#elif defined(CONFIG_CHIP_C_CUT)
// Patch for C Cut
HAL_Status
HalTimerInitRtl8195a_Patch(
    IN  VOID    *Data
)
{
    PTIMER_ADAPTER pHalTimerAdap = (PTIMER_ADAPTER) Data;
    HAL_Status ret=HAL_OK;

    ret = HalTimerInitRtl8195aV02(Data);

    // Patch the Rom code to load the correct count value
    if (pHalTimerAdap->TimerMode) {
        //User-defined Mode
        HalTimerReLoadRtl8195a_Patch(pHalTimerAdap->TimerId ,pHalTimerAdap->TimerLoadValueUs);
    } 

    return ret;
}
#endif

#if defined(CONFIG_CHIP_A_CUT) || defined(CONFIG_CHIP_B_CUT)

HAL_Status
HalTimerIrqUnRegisterRtl8195a_Patch(
    IN  VOID    *Data
)
{
    PTIMER_ADAPTER      pHalTimerAdap = (PTIMER_ADAPTER) Data;
    PIRQ_HANDLE         pTimerIrqHandle;
    u32 i;
    
    pTimerIrqHandle = &pHalTimerAdap->IrqHandle;

    if (pHalTimerAdap->TimerId > 7) {
        DBG_TIMER_ERR("%s:Error: No Support Timer ID!\n", __FUNCTION__);
        return HAL_ERR_PARA;
    }
    else {
        if (pHalTimerAdap->TimerId > 1) {
            pTimerIrqHandle->IrqNum = TIMER2_7_IRQ;
            Timer2To7VectorTable[pHalTimerAdap->TimerId-2] = NULL;
            for (i=0;i<MAX_TIMER_VECTOR_TABLE_NUM;i++) {
                if (Timer2To7VectorTable[i] != NULL) {
                    break;
                }
            }
            
            if (i == MAX_TIMER_VECTOR_TABLE_NUM) {
                // All timer UnRegister Interrupt
                InterruptDis((PIRQ_HANDLE)&pHalTimerAdap->IrqHandle);
                InterruptUnRegister(pTimerIrqHandle);
            }
        }
        else {
            pTimerIrqHandle->IrqNum = (pHalTimerAdap->TimerId ? TIMER1_IRQ : TIMER0_IRQ);
            InterruptUnRegister(pTimerIrqHandle);
        }

    }
    
    return HAL_OK;
}


VOID
HalTimerDeInitRtl8195a_Patch(
    IN  VOID    *Data
)
{
    PTIMER_ADAPTER pHalTimerAdap = (PTIMER_ADAPTER) Data;
    u32 timer_id;

    timer_id = pHalTimerAdap->TimerId;
    HalTimerDisRtl8195a (timer_id);
    if (!pHalTimerAdap->IrqDis) {
        if (pHalTimerAdap->IrqHandle.IrqFun != NULL) {
            HalTimerIrqUnRegisterRtl8195a_Patch(pHalTimerAdap);
        }
    }

    gTimerRecord &= ~(1<<pHalTimerAdap->TimerId);
}

u32
HalTimerReadCountRtl8195a_Patch(
    IN  u32 TimerId
)
{
    u32 TimerCountOld;
    u32 TimerCountNew;
    u32 TimerRDCnt;
    
    TimerRDCnt = 0;
    TimerCountOld = HAL_TIMER_READ32(TimerId*TIMER_INTERVAL + TIMER_CURRENT_VAL_OFF);
    while(1) {        
        TimerCountNew = HAL_TIMER_READ32(TimerId*TIMER_INTERVAL + TIMER_CURRENT_VAL_OFF);

        if (TimerCountOld == TimerCountNew) {
            return (u32)TimerCountOld;
        }
        else {
            TimerRDCnt++;
            TimerCountOld = TimerCountNew;

            if (TimerRDCnt >= 2){
                return (u32)TimerCountOld;
            }
        }
    }    
}


#endif  // #if defined(CONFIG_CHIP_A_CUT) || defined(CONFIG_CHIP_B_CUT)

#if defined(CONFIG_CHIP_A_CUT) || defined(CONFIG_CHIP_B_CUT) || defined(CONFIG_CHIP_C_CUT)

VOID
HalTimerReLoadRtl8195a_Patch(
    IN  u32 TimerId,
    IN  u32 LoadUs
)
{
    u32 LoadCount = 0;
    u32 ms125;  // how many 125ms
    u32 remain_us;

    ms125 = LoadUs/125000;
    remain_us = LoadUs - (ms125*125000);
    LoadCount = ms125 * (GTIMER_CLK_HZ/8);
    LoadCount += (remain_us*GTIMER_CLK_HZ)/1000000;
    if (LoadCount == 0) {
        LoadCount = 1;
    }
    
//    DBG_TIMER_INFO("%s: Load Count=0x%x\r\n", __FUNCTION__, LoadCount);
    // set TimerLoadCount Register
    HAL_TIMER_WRITE32((TIMER_INTERVAL*TimerId + TIMER_LOAD_COUNT_OFF),
                        LoadCount);
}

#endif  // #if defined(CONFIG_CHIP_A_CUT) || defined(CONFIG_CHIP_B_CUT) || defined(CONFIG_CHIP_C_CUT)

VOID
HalTimerIrqEnRtl8195a(
    IN  u32 TimerId
)
{
    HAL_TIMER_WRITE32((TIMER_INTERVAL*TimerId + TIMER_CTL_REG_OFF), 
                        HAL_TIMER_READ32(TIMER_INTERVAL*TimerId  + TIMER_CTL_REG_OFF) & (~(BIT2)));
}

VOID
HalTimerIrqDisRtl8195a(
    IN  u32 TimerId
)
{
    HAL_TIMER_WRITE32((TIMER_INTERVAL*TimerId + TIMER_CTL_REG_OFF), 
                        HAL_TIMER_READ32(TIMER_INTERVAL*TimerId  + TIMER_CTL_REG_OFF) | (BIT2));
}

VOID
HalTimerClearIsrRtl8195a(
    IN  u32 TimerId
)
{
    HAL_TIMER_READ32(TIMER_INTERVAL*TimerId + TIMER_EOI_OFF);
}

VOID
HalTimerEnRtl8195a_Patch(
    IN  u32 TimerId
)
{
    HAL_TIMER_WRITE32((TIMER_INTERVAL*TimerId + TIMER_CTL_REG_OFF), 
                        HAL_TIMER_READ32(TIMER_INTERVAL*TimerId  + TIMER_CTL_REG_OFF) | (BIT0));
}

VOID
HalTimerDisRtl8195a_Patch(
    IN  u32 TimerId
)
{
    // Disable Timer will alos disable the IRQ, so need to re-enable the IRQ when re-enable the timer
    HAL_TIMER_WRITE32((TIMER_INTERVAL*TimerId + TIMER_CTL_REG_OFF), 
                        HAL_TIMER_READ32(TIMER_INTERVAL*TimerId  + TIMER_CTL_REG_OFF) & (~BIT0));    
}

