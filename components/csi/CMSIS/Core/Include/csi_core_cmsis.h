/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     csi_core_cmsis.h
 * @brief    Header File for csi_core
 * @version  V1.0
 * @date     12. June 2020
 ******************************************************************************/

#ifndef _CSI_CORE_CMSIS_H_
#define _CSI_CORE_CMSIS_H_

#include <stddef.h>
#include <cmsis_gcc.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn
{
/* -------------------  Processor Exceptions Numbers  ----------------------------- */
  NonMaskableInt_IRQn           = -14,     /*  2 Non Maskable Interrupt */
  HardFault_IRQn                = -13,     /*  3 HardFault Interrupt */



  SVCall_IRQn                   =  -5,     /* 11 SV Call Interrupt */

  PendSV_IRQn                   =  -2,     /* 14 Pend SV Interrupt */
  SysTick_IRQn                  =  -1,     /* 15 System Tick Interrupt */

/* -------------------  Processor Interrupt Numbers  ------------------------------ */
  Interrupt0_IRQn               =   0,
  Interrupt1_IRQn               =   1,
  Interrupt2_IRQn               =   2,
  Interrupt3_IRQn               =   3,
  Interrupt4_IRQn               =   4,
  Interrupt5_IRQn               =   5,
  Interrupt6_IRQn               =   6,
  Interrupt7_IRQn               =   7,
  Interrupt8_IRQn               =   8,
  Interrupt9_IRQn               =   9
  /* Interrupts 10 .. 31 are left out */
} IRQn_Type;

#if defined(__CPU_CORTEXM0__)
#define __CM0_REV                 0x0000U   /* Core revision r0p0 */
#define __MPU_PRESENT             0U        /* no MPU present */
#define __VTOR_PRESENT            0U        /* no VTOR present */
#define __NVIC_PRIO_BITS          2U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#include <../../CMSIS/Core/Include/core_cm0.h>
#elif defined(__CPU_CORTEXM0PLUS__)
#define __CM0PLUS_REV             0x0001U   /* Core revision r0p1 */
#define __MPU_PRESENT             0U        /* no MPU present */
#define __VTOR_PRESENT            0U        /* no VTOR present */
#define __NVIC_PRIO_BITS          2U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#include <core_cm0plus.h>
#elif defined(__CPU_CORTEXM1__)
#define __CM1_REV                 0x0100U   /* Core revision r1p0 */
#define __MPU_PRESENT             0U        /* no MPU present */
#define __VTOR_PRESENT            0U        /* no VTOR present */
#define __NVIC_PRIO_BITS          2U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#include <core_cm1.h>
#elif defined(__CPU_CORTEXM23__)
#define __CM23_REV                0x0100U   /* Core revision r1p0 */
#define __SAUREGION_PRESENT       0U        /* SAU regions are not present */
#define __MPU_PRESENT             1U        /* MPU is present */
#define __VTOR_PRESENT            1U        /* VTOR is present */
#define __NVIC_PRIO_BITS          2U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#include <core_cm23.h>
#elif defined(__CPU_CORTEXM33__)
#define __CM33_REV                0x0000U   /* Core revision r0p1 */
#define __SAUREGION_PRESENT       0U        /* SAU regions present */
#define __MPU_PRESENT             1U        /* MPU present */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __NVIC_PRIO_BITS          3U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT             0U        /* no FPU present */
#define __DSP_PRESENT             0U        /* no DSP extension present */
#include <core_cm33.h>
#elif defined(__CPU_CORTEXM35P__)
#define __CM35P_REV               0x0000U   /* Core revision r0p0 */
#define __SAUREGION_PRESENT       0U        /* SAU regions present */
#define __MPU_PRESENT             1U        /* MPU present */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __NVIC_PRIO_BITS          3U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT             0U        /* no FPU present */
#define __DSP_PRESENT             0U        /* no DSP extension present */
#include <core_cm35p.h>
#elif defined(__CPU_CORTEXM3__)
#define __CM3_REV                 0x0201U   /* Core revision r2p1 */
#define __MPU_PRESENT             1U        /* MPU present */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __NVIC_PRIO_BITS          3U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#include <core_cm3.h>
#elif defined(__CPU_CORTEXM4__)
#define __CM4_REV                 0x0001U   /* Core revision r0p1 */
#define __MPU_PRESENT             1U        /* MPU present */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __NVIC_PRIO_BITS          3U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT             0U        /* no FPU present */
#include <core_cm4.h>
#elif defined(__CPU_CORTEXM55__)
#define __CM55_REV                0x0001U   /* Core revision r0p1 */
#define __NVIC_PRIO_BITS          3U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __MPU_PRESENT             1U        /* MPU present */
#define __FPU_PRESENT             1U        /* FPU present */
#define __FPU_DP                  1U        /* double precision FPU */
#define __DSP_PRESENT             1U        /* DSP extension present */
#define __SAUREGION_PRESENT       1U        /* SAU regions present */
#define __PMU_PRESENT             1U        /* PMU present */
#define __PMU_NUM_EVENTCNT        8U        /* PMU Event Counters */
#define __ICACHE_PRESENT          1U        /* Instruction Cache present */
#define __DCACHE_PRESENT          1U        /* Data Cache present */
#include <core_cm55.h>
#elif defined(__CPU_CORTEXM7__)
#define __CM7_REV                 0x0000U   /* Core revision r0p0 */
#define __MPU_PRESENT             1U        /* MPU present */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __NVIC_PRIO_BITS          3U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT             0U        /* no FPU present */
#define __FPU_DP                  0U        /* unused */
#define __ICACHE_PRESENT          1U        /* Instruction Cache present */
#define __DCACHE_PRESENT          1U        /* Data Cache present */
#define __DTCM_PRESENT            1U        /* Data Tightly Coupled Memory present */
#include <core_cm7.h>
#endif


__STATIC_INLINE uint32_t csi_coret_config(uint32_t ticks, int32_t IRQn)
{
    return SysTick_Config(ticks);
}

__STATIC_INLINE uint32_t csi_coret_get_load(void)
{
    return SysTick->LOAD;
}

__STATIC_INLINE uint32_t csi_coret_get_value(void)
{
    return SysTick->VAL;
}

__STATIC_INLINE void csi_coret_clear_irq(void)
{

}

#ifdef __GNUC__

#define __ALWAYS_STATIC_INLINE __attribute__((always_inline)) static inline

__STATIC_INLINE size_t csi_irq_save(void)
{
    uint32_t result;
    result = __get_PRIMASK();
    __disable_irq();
    return (result);
}

__STATIC_INLINE void csi_irq_restore(size_t irq_state)
{
    __set_PRIMASK(irq_state);
}

#else
static inline __asm size_t csi_irq_save(void)
{
    MRS     R0, PRIMASK
    CPSID   I
    BX      LR

    return 0;
}

static inline __asm void csi_irq_restore(size_t irq_state)
{
    MSR     PRIMASK, R0
    BX      LR
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* _CSI_CORE_H_ */
