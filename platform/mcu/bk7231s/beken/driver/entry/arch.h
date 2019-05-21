/**
 ****************************************************************************************
 *
 * @file arch.h
 *
 * @brief This file contains the definitions of the macros and functions that are
 * architecture dependent.  The implementation of those is implemented in the
 * appropriate architecture directory.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */


#ifndef _ARCH_H_
#define _ARCH_H_

/**
 ****************************************************************************************
 * @defgroup PLATFORM_DRIVERS PLATFORM_DRIVERS
 * @ingroup MACSW
 * @brief Declaration of the ATMEL AT91SAM261 architecture API.
 * @{
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup ARCH_AT91SAM9261 ARCH_AT91SAM9261
 * @ingroup PLATFORM_DRIVERS
 * @brief Declaration of the ATMEL AT91SAM261 architecture API.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "generic.h"
#include "co_int.h"
#include "compiler.h"
#include "portmacro.h"
#if CFG_SUPPORT_RTT
#include <rthw.h>
#include <rtthread.h>
#endif


#define GLOBAL_INT_START               portENABLE_INTERRUPTS
#define GLOBAL_INT_STOP                portDISABLE_INTERRUPTS

#if CFG_SUPPORT_RTT

#define GLOBAL_INT_DECLARATION()   rt_base_t irq_level
#define GLOBAL_INT_DISABLE()       do{\
										irq_level = rt_hw_interrupt_disable();\
									}while(0)


#define GLOBAL_INT_RESTORE()       do{                         \
                                        rt_hw_interrupt_enable(irq_level);\
                                   }while(0)

#else

#define GLOBAL_INT_DECLARATION()   uint32_t fiq_tmp, irq_tmp
#define GLOBAL_INT_DISABLE()       do{\
										fiq_tmp = portDISABLE_FIQ();\
										irq_tmp = portDISABLE_IRQ();\
									}while(0)


#define GLOBAL_INT_RESTORE()       do{                         \
                                        if(!fiq_tmp)           \
                                        {                      \
                                            portENABLE_FIQ();  \
                                        }                      \
                                        if(!irq_tmp)           \
                                        {                      \
                                            portENABLE_IRQ();  \
                                        }                      \
                                   }while(0)
#endif

/*
 * CPU WORD SIZE
 ****************************************************************************************
 */
/// ARM is a 32-bit CPU
#define CPU_WORD_SIZE                  4

/*
 * CPU Endianness
 ****************************************************************************************
 */
/// ARM is little endian
#define CPU_LE                          1

#define ASSERT_REC(cond)
#define ASSERT_REC_VAL(cond, ret)
#define ASSERT_REC_NO_RET(cond)

#define ASSERT_ERR(cond)                  ASSERT(cond)
#define ASSERT_ERR2(cond, param0, param1)
#define ASSERT_WARN(cond)

/// @}
/// @}
#endif // _ARCH_H_
