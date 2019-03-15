/*
 ****************************************************************************************
 *
 * @file    type.h
 * @brief   define type.
 *
 * @version V1.0
 * @date    10 Jan 2017
 * @author  zwtian
 * @note
 *
 * Copyright (C) Shanghai Eastsoft Microelectronics Co. Ltd. All rights reserved.
 *
 ****************************************************************************************
 */

#ifndef __TYPE_H__
#define __TYPE_H__


typedef unsigned int    uint32_t;
typedef unsigned short  uint16_t;
typedef unsigned char   uint8_t;

typedef enum
{
    RESET = 0,
    SET   = !RESET,
} FlagStatus, ITStatus;

typedef enum
{
    BIT_RESET = 0x00,
    BIT_SET   = 0x01,
} BitStatus;

typedef enum {
    DISABLE = 0x00,
    ENABLE  = 0x01,
} TYPE_FUNCEN, FunctionalState;
#define IS_FUNCEN_STATE(x)  (((x) == DISABLE) || ((x) == ENABLE))
#define IS_FUNCTIONAL_STATE(x)  (((x) == DISABLE) || ((x) == ENABLE))

typedef enum
{
    ERROR   = 0,
    SUCCESS = !ERROR,
} ErrorStatus;

typedef enum
{
    FALSE = 0,
    TRUE  = !FALSE,
} BoolType;

typedef enum
{
    UNLOCK = 0,
    LOCK   = !UNLOCK,
} LockState;
#define IS_LOCK_STATE(STATE)    (((STATE) == UNLOCK) || ((STATE) == LOCK))


#ifdef USE_ASSERT
#define assert_param(x)         \
do {                    \
    if (!x) {           \
        __disable_irq();    \
        while (1)       \
            ;       \
    }               \
} while (0)
#else
#define assert_param(x)
#endif


#define PER_MEM_BASE         ((uint32_t) 0x40000000UL)  /* PER base address  */
#define RAM_MEM_BASE         ((uint32_t) 0x20000000UL)  /*RAM base address  */
#define BITBAND_PER_BASE     ((uint32_t) 0x42000000UL)  /* Peripheral Address Space bit-band area */
#define BITBAND_RAM_BASE     ((uint32_t) 0x22000000UL)  /* SRAM Address Space bit-band area */

__inline void BITBAND_PER(volatile uint32_t *addr, uint32_t bit, uint32_t val)
{
    uint32_t tmp = BITBAND_PER_BASE + (((uint32_t)addr - PER_MEM_BASE) << 5) + (bit << 2);
    *((volatile uint32_t *)tmp) = (uint32_t)val;
}

__inline void BITBAND_SRAM(uint32_t *addr, uint32_t bit, uint32_t val)
{
    uint32_t tmp = BITBAND_RAM_BASE + (((uint32_t)addr - RAM_MEM_BASE) << 5) + (bit << 2);
    *((volatile uint32_t *)tmp) = (uint32_t)val;
}

#endif /* end __TYPE_H__ */
