/*
 ****************************************************************************************
 *
 * @file    lib_scs.h
 * @brief   library of SCS Module.
 *
 * @version V1.0
 * @date    14 Feb 2017
 * @author  zwtian
 * @note
 *
 * Copyright (C) Shanghai Eastsoft Microelectronics Co. Ltd. All rights reserved.
 *
 ****************************************************************************************
 */

#ifndef __LIB_SCS_H__
#define __LIB_SCS_H__

#include "p131.h"
#include "type.h"


#define IS_PREEMPT_PRIO(x)  ((x) < 16)
#define IS_SUB_PRIO(x)      ((x) < 16)


/* EXTERN FUNCTION */
extern void NVIC_Init(IRQn_Type IRQn, uint8_t preempt_prio, uint8_t sub_prio, TYPE_FUNCEN state);
extern void NVIC_SetPriorityGrouping(uint32_t PriorityGroup);
extern uint32_t SCB_GetCpuID(void);

#endif
