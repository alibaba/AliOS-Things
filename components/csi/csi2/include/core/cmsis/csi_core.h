/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     csi_core.h
 * @brief    Header File for csi_core
 * @version  V1.0
 * @date     12. june 2019
 ******************************************************************************/
#ifndef _CSI_CORE_H_
#define _CSI_CORE_H_

#include <stddef.h>
#include <cmsis_gcc.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
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
