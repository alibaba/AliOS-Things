/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_GPIO_H
#define AOS_GPIO_H

#include <aos/device_core.h>

#define AOS_GPIO_DIR_MASK               ((uint32_t)0xF << 0)
#define AOS_GPIO_DIR_NONE               ((uint32_t)0x0 << 0)
#define AOS_GPIO_DIR_INPUT              ((uint32_t)0x1 << 0)
#define AOS_GPIO_DIR_OUTPUT             ((uint32_t)0x2 << 0)

#define AOS_GPIO_INPUT_CFG_MASK         ((uint32_t)0xF << 4)
#define AOS_GPIO_INPUT_CFG_DEFAULT      ((uint32_t)0x0 << 4)
#define AOS_GPIO_INPUT_CFG_HI           ((uint32_t)0x1 << 4)
#define AOS_GPIO_INPUT_CFG_PU           ((uint32_t)0x2 << 4)
#define AOS_GPIO_INPUT_CFG_PD           ((uint32_t)0x3 << 4)

#define AOS_GPIO_IRQ_TRIG_MASK          ((uint32_t)0xF << 8)
#define AOS_GPIO_IRQ_TRIG_NONE          ((uint32_t)0x0 << 8)
#define AOS_GPIO_IRQ_TRIG_EDGE_RISING   ((uint32_t)0x1 << 8)
#define AOS_GPIO_IRQ_TRIG_EDGE_FALLING  ((uint32_t)0x2 << 8)
#define AOS_GPIO_IRQ_TRIG_EDGE_BOTH     ((uint32_t)0x3 << 8)
#define AOS_GPIO_IRQ_TRIG_LEVEL_HIGH    ((uint32_t)0x4 << 8)
#define AOS_GPIO_IRQ_TRIG_LEVEL_LOW     ((uint32_t)0x5 << 8)

#define AOS_GPIO_IRQ_PRIO_MASK          ((uint32_t)0xFF << 16)
#define AOS_GPIO_IRQ_PRIO(x)            (((uint32_t)(x) & 0xFF) << 16)
#define AOS_GPIO_IRQ_PRIO_DEFAULT       AOS_GPIO_IRQ_PRIO(0)

#define AOS_GPIO_OUTPUT_CFG_MASK        ((uint32_t)0xF << 4)
#define AOS_GPIO_OUTPUT_CFG_DEFAULT     ((uint32_t)0x0 << 4)
#define AOS_GPIO_OUTPUT_CFG_PP          ((uint32_t)0x1 << 4)
#define AOS_GPIO_OUTPUT_CFG_ODNP        ((uint32_t)0x2 << 4)
#define AOS_GPIO_OUTPUT_CFG_ODPU        ((uint32_t)0x3 << 4)

#define AOS_GPIO_OUTPUT_INIT_MASK       ((uint32_t)0x1 << 8)
#define AOS_GPIO_OUTPUT_INIT_LOW        ((uint32_t)0x0 << 8)
#define AOS_GPIO_OUTPUT_INIT_HIGH       ((uint32_t)0x1 << 8)

typedef void (*aos_gpio_irq_handler_t)(int polarity, void *arg);

#endif /* AOS_GPIO_H */
