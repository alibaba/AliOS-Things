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

#define AOS_GPIO_ICFG_MASK              AOS_GPIO_INPUT_CFG_MASK
#define AOS_GPIO_ICFG_DEFAULT           AOS_GPIO_INPUT_CFG_DEFAULT
#define AOS_GPIO_ICFG_HI                AOS_GPIO_INPUT_CFG_HI
#define AOS_GPIO_ICFG_PU                AOS_GPIO_INPUT_CFG_PU
#define AOS_GPIO_ICFG_PD                AOS_GPIO_INPUT_CFG_PD

#define AOS_GPIO_IRQ_TRIG_MASK          ((uint32_t)0xF << 8)
#define AOS_GPIO_IRQ_TRIG_NONE          ((uint32_t)0x0 << 8)
#define AOS_GPIO_IRQ_TRIG_EDGE_RISING   ((uint32_t)0x1 << 8)
#define AOS_GPIO_IRQ_TRIG_EDGE_FALLING  ((uint32_t)0x2 << 8)
#define AOS_GPIO_IRQ_TRIG_EDGE_BOTH     ((uint32_t)0x3 << 8)
#define AOS_GPIO_IRQ_TRIG_LEVEL_HIGH    ((uint32_t)0x4 << 8)
#define AOS_GPIO_IRQ_TRIG_LEVEL_LOW     ((uint32_t)0x5 << 8)

#define AOS_GPIO_IRQT_MASK              AOS_GPIO_IRQ_TRIG_MASK
#define AOS_GPIO_IRQT_NONE              AOS_GPIO_IRQ_TRIG_NONE
#define AOS_GPIO_IRQT_EDGE_RISING       AOS_GPIO_IRQ_TRIG_EDGE_RISING
#define AOS_GPIO_IRQT_EDGE_FALLING      AOS_GPIO_IRQ_TRIG_EDGE_FALLING
#define AOS_GPIO_IRQT_EDGE_BOTH         AOS_GPIO_IRQ_TRIG_EDGE_BOTH
#define AOS_GPIO_IRQT_LEVEL_HIGH        AOS_GPIO_IRQ_TRIG_LEVEL_HIGH
#define AOS_GPIO_IRQT_LEVEL_LOW         AOS_GPIO_IRQ_TRIG_LEVEL_LOW

#define AOS_GPIO_IRQ_PRIO_MASK          ((uint32_t)0xFF << 16)
#define AOS_GPIO_IRQ_PRIO(x)            (((uint32_t)(x) & 0xFF) << 16)
#define AOS_GPIO_IRQ_PRIO_DEFAULT       AOS_GPIO_IRQ_PRIO(0)

#define AOS_GPIO_OUTPUT_CFG_MASK        ((uint32_t)0xF << 4)
#define AOS_GPIO_OUTPUT_CFG_DEFAULT     ((uint32_t)0x0 << 4)
#define AOS_GPIO_OUTPUT_CFG_PP          ((uint32_t)0x1 << 4)
#define AOS_GPIO_OUTPUT_CFG_ODNP        ((uint32_t)0x2 << 4)
#define AOS_GPIO_OUTPUT_CFG_ODPU        ((uint32_t)0x3 << 4)

#define AOS_GPIO_OCFG_MASK              AOS_GPIO_OUTPUT_CFG_MASK
#define AOS_GPIO_OCFG_DEFAULT           AOS_GPIO_OUTPUT_CFG_DEFAULT
#define AOS_GPIO_OCFG_PP                AOS_GPIO_OUTPUT_CFG_PP
#define AOS_GPIO_OCFG_ODNP              AOS_GPIO_OUTPUT_CFG_ODNP
#define AOS_GPIO_OCFG_ODPU              AOS_GPIO_OUTPUT_CFG_ODPU

#define AOS_GPIO_OUTPUT_INIT_MASK       ((uint32_t)0x1 << 8)
#define AOS_GPIO_OUTPUT_INIT_LOW        ((uint32_t)0x0 << 8)
#define AOS_GPIO_OUTPUT_INIT_HIGH       ((uint32_t)0x1 << 8)

typedef void (*aos_gpio_irq_handler_t)(int polarity, void *arg);

#endif /* AOS_GPIO_H */
