/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __REG_TIMER_H_
#define __REG_TIMER_H_

#include "plat_types.h"

/* ================================================================================ */
/* ================                   Timer (TIM)                  ================ */
/* ================================================================================ */
struct DUAL_TIMER_T {
    struct TIMER_T {
        __IO uint32_t Load;       /* Offset: 0x000 (R/W)  Timer X Load */
        __I  uint32_t Value;      /* Offset: 0x004 (R/ )  Timer X Counter Current Value */
        __IO uint32_t Control;    /* Offset: 0x008 (R/W)  Timer X Control */
        __O  uint32_t IntClr;     /* Offset: 0x00C ( /W)  Timer X Interrupt Clear */
        __I  uint32_t RIS;        /* Offset: 0x010 (R/ )  Timer X Raw Interrupt Status */
        __I  uint32_t MIS;        /* Offset: 0x014 (R/ )  Timer X Masked Interrupt Status */
        __IO uint32_t BGLoad;     /* Offset: 0x018 (R/W)  Background Load Register */
             uint32_t RESERVED0[1];
    } timer[2];
    struct ELAPSED_TIMER_T {
        __IO uint32_t ElapsedCtrl;
        __I  uint32_t ElapsedVal;
             uint32_t RESERVED1[6];
    } elapsed_timer[2];
};

#define TIMER_CTRL_EN                   (1 << 7)
#define TIMER_CTRL_MODE_PERIODIC        (1 << 6)
#define TIMER_CTRL_INTEN                (1 << 5)
#define TIMER_CTRL_PRESCALE_DIV_1       (0 << 2)
#define TIMER_CTRL_PRESCALE_DIV_16      (1 << 2)
#define TIMER_CTRL_PRESCALE_DIV_256     (2 << 2)
#define TIMER_CTRL_PRESCALE_MASK        (3 << 2)
#define TIMER_CTRL_SIZE_32_BIT          (1 << 1)
#define TIMER_CTRL_ONESHOT              (1 << 0)

#define TIMER_RIS_RIS                   (1 << 0)

#define TIMER_MIS_MIS                   (1 << 0)

#define TIMER_ELAPSED_CTRL_EN           (1 << 0)
#define TIMER_ELAPSED_CTRL_CLR          (1 << 1)

#endif

