/**
 * Copyright (C) 2015-2016 The YunOS Project. All rights reserved.
 */

#ifndef _CK_PMU_H_
#define _CK_PMU_H_

#define  CK_INTC_POWM            18

#define CK_POWM_CPU_WAKECOUNT    0x70000
#define CK_POWN_PLL_WAITCOUNT    0x20000
#define CK_POWM_CLEARINT         0x00
#define	CK_POWM_LPCR_ENABLE      0x04
#define	CK_POWM_LPCR_STANDBY     0x34
#define CK_POWM_LPCR_STOP        0x2c
#define CK_POWM_MCLK_PLL         0x08
#define CK_POWM_MCLK_IHS         0x00
#define CK_POWM_MCLK_ILS         0x04
#define CK_POWM_MCLK_EHS         0x02
#define CK_POWM_MCLK_ELS         0x05

#define CK_POWM_IRQID            CK_INTC_POWM
#define GPIO_MIN_IRQ             0
#define GPIO_MAX_IRQ             7

#define LPM_RESERVE_STACK_SIZE          (0x200)

#endif /* _CK_PMU_H_ */
