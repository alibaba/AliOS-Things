/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/
#include "lib_scs.h"


void NVIC_Init(IRQn_Type IRQn, uint8_t preempt_prio, uint8_t sub_prio, TYPE_FUNCEN state)
{
    uint32_t pri;
    uint8_t sub_bw, pre_bw;
    uint8_t sub_mask = 0xF;

    assert_param(IS_FUNCEN_STATE(state));
    assert_param(IS_PREEMPT_PRIO(preempt_prio));
    assert_param(IS_SUB_PRIO(sub_prio));

    if (state == ENABLE) {
        pre_bw     = 7 - (((SCB->AIRCR) >> 8) & 7);
        sub_bw     = 3 - pre_bw;
        sub_mask >>= pre_bw;

        pri  = preempt_prio << sub_bw;
        pri |= sub_prio & sub_mask;

        NVIC_SetPriority(IRQn, pri);
        NVIC_EnableIRQ(IRQn);
    }
    else {
        NVIC_DisableIRQ(IRQn);
    }

    return;
}

uint32_t SCB_GetCpuID(void)
{
    return SCB->CPUID;
}
