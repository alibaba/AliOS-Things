/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_GPIO_H
#define AOS_GPIO_H

#ifdef AOS_KERNEL_BUILD
#include <aos/device.h>
#else
#include <stdint.h>
#endif

/**
 * @defgroup gpio_api GPIO Pin
 * @ingroup driver_api
 * @brief AOS API for GPIO pin.
 * @{
 */

#define AOS_GPIO_DIR_MASK               ((uint32_t)0x7 << 0)
#define AOS_GPIO_DIR_NONE               ((uint32_t)0x0 << 0)
#define AOS_GPIO_DIR_INPUT              ((uint32_t)0x1 << 0)
#define AOS_GPIO_DIR_OUTPUT             ((uint32_t)0x2 << 0)
#define AOS_GPIO_DIR_BOTH               ((uint32_t)0x3 << 0)

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

#define AOS_GPIO_OUTPUT_INIT_MASK       ((uint32_t)0x1 << 3)
#define AOS_GPIO_OUTPUT_INIT_LOW        ((uint32_t)0x0 << 3)
#define AOS_GPIO_OUTPUT_INIT_HIGH       ((uint32_t)0x1 << 3)

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

#define AOS_GPIO_IRQ_TRIG_MASK          ((uint32_t)0x7 << 12)
#define AOS_GPIO_IRQ_TRIG_NONE          ((uint32_t)0x0 << 12)
#define AOS_GPIO_IRQ_TRIG_EDGE_RISING   ((uint32_t)0x1 << 12)
#define AOS_GPIO_IRQ_TRIG_EDGE_FALLING  ((uint32_t)0x2 << 12)
#define AOS_GPIO_IRQ_TRIG_EDGE_BOTH     ((uint32_t)0x3 << 12)
#define AOS_GPIO_IRQ_TRIG_LEVEL_HIGH    ((uint32_t)0x4 << 12)
#define AOS_GPIO_IRQ_TRIG_LEVEL_LOW     ((uint32_t)0x5 << 12)

#define AOS_GPIO_IRQT_MASK              AOS_GPIO_IRQ_TRIG_MASK
#define AOS_GPIO_IRQT_NONE              AOS_GPIO_IRQ_TRIG_NONE
#define AOS_GPIO_IRQT_EDGE_RISING       AOS_GPIO_IRQ_TRIG_EDGE_RISING
#define AOS_GPIO_IRQT_EDGE_FALLING      AOS_GPIO_IRQ_TRIG_EDGE_FALLING
#define AOS_GPIO_IRQT_EDGE_BOTH         AOS_GPIO_IRQ_TRIG_EDGE_BOTH
#define AOS_GPIO_IRQT_LEVEL_HIGH        AOS_GPIO_IRQ_TRIG_LEVEL_HIGH
#define AOS_GPIO_IRQT_LEVEL_LOW         AOS_GPIO_IRQ_TRIG_LEVEL_LOW

#ifdef AOS_KERNEL_BUILD

#define AOS_GPIO_IRQ_ALTERNATE          ((uint32_t)0x1 << 15)

#define AOS_GPIO_IRQ_PRIO_MASK          ((uint32_t)0xFF << 16)
#define AOS_GPIO_IRQ_PRIO(x)            (((uint32_t)(x) & 0xFF) << 16)
#define AOS_GPIO_IRQ_PRIO_DEFAULT       AOS_GPIO_IRQ_PRIO(0)

#endif /* AOS_KERNEL_BUILD */

#if (defined(AOS_KERNEL_BUILD) && defined(AOS_COMP_DEVFS)) || !defined(AOS_KERNEL_BUILD)

#define AOS_GPIO_IOC_GET_MODE           0x4701
#define AOS_GPIO_IOC_SET_MODE           0x4702
#define AOS_GPIO_IOC_TOGGLE             0x4703
#define AOS_GPIO_IOC_ENTER_IRQ          0x4704
#define AOS_GPIO_IOC_EXIT_IRQ           0x4705

#endif /* (defined(AOS_KERNEL_BUILD) && defined(AOS_COMP_DEVFS)) || !defined(AOS_KERNEL_BUILD) */

#ifdef AOS_KERNEL_BUILD

typedef aos_dev_ref_t aos_gpio_ref_t;
typedef void (*aos_gpio_irq_handler_t)(int polarity, void *arg);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Get a GPIO pin.
 * @param   ref GPIO pin ref to operate.
 * @param   id  GPIO pin device ID.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_gpio_get(aos_gpio_ref_t *ref, uint32_t id);
/**
 * @brief   Release a GPIO pin.
 * @param   ref GPIO pin ref to operate.
 * @return  None.
 */
void aos_gpio_put(aos_gpio_ref_t *ref);
/**
 * @brief   Get the parameters and IRQ settings associated with a GPIO pin.
 * @param   ref         GPIO pin ref to operate.
 * @param   mode        Pin mode.
 * @param   irq_handler IRQ handler.
 * @param   irq_arg     Argument passed to IRQ handler.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t
aos_gpio_get_mode_irq(aos_gpio_ref_t *ref, uint32_t *mode, aos_gpio_irq_handler_t *irq_handler, void **irq_arg);
/**
 * @brief   Set the parameters and IRQ settings associated with a GPIO pin.
 * @param   ref         GPIO pin ref to operate.
 * @param   mode        Pin mode.
 * @param   irq_handler IRQ handler.
 * @param   irq_arg     Argument passed to IRQ handler.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t
aos_gpio_set_mode_irq(aos_gpio_ref_t *ref, uint32_t mode, aos_gpio_irq_handler_t irq_handler, void *irq_arg);
/**
 * @brief   Get the parameters associated with a GPIO pin.
 * @param   ref     GPIO pin ref to operate.
 * @param   mode    Pin mode.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_gpio_get_mode(aos_gpio_ref_t *ref, uint32_t *mode);
/**
 * @brief   Set the parameters associated with a GPIO pin.
 * @param   ref     GPIO pin ref to operate.
 * @param   mode    Pin mode.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_gpio_set_mode(aos_gpio_ref_t *ref, uint32_t mode);
/**
 * @brief   Get the IRQ trigger type of a GPIO pin.
 * @param   ref     GPIO pin ref to operate.
 * @param   trig    Trigger type.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_gpio_get_irq_trigger(aos_gpio_ref_t *ref, uint32_t *trig);
/**
 * @brief   Set the IRQ trigger type of a GPIO pin.
 * @param   ref     GPIO pin ref to operate.
 * @param   trig    Trigger type.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_gpio_set_irq_trigger(aos_gpio_ref_t *ref, uint32_t trig);
/**
 * @brief   Get the input or output level of a GPIO pin.
 * @param   ref GPIO pin ref to operate.
 * @return  0: low level; > 0: high level; < 0: on failure.
 */
aos_status_t aos_gpio_get_value(aos_gpio_ref_t *ref);
/**
 * @brief   Set the output level of a GPIO pin.
 * @param   ref GPIO pin ref to operate.
 * @param   val Output level.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_gpio_set_value(aos_gpio_ref_t *ref, int val);
/**
 * @brief   Toggle the output level of a GPIO pin.
 * @param   ref GPIO pin ref to operate.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_gpio_toggle(aos_gpio_ref_t *ref);
/**
 * @brief   Wait for an IRQ to become ready.
 * @param   ref     GPIO pin ref to operate.
 * @param   timeout Timeout in milliseconds.
 * @return  0: on low level or falling edge; 1: on high level or rising edge; < 0: on failure.
 */
aos_status_t aos_gpio_poll_irq(aos_gpio_ref_t *ref, uint32_t timeout);
/**
 * @brief   Wait for an IRQ and then handle it.
 * @param   ref     GPIO pin ref to operate.
 * @param   timeout Timeout in milliseconds.
 * @return  0: on low level or falling edge; 1: on high level or rising edge; < 0: on failure.
 */
aos_status_t aos_gpio_enter_irq(aos_gpio_ref_t *ref, uint32_t timeout);
/**
 * @brief   End an IRQ.
 * @param   ref GPIO pin ref to operate.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_gpio_exit_irq(aos_gpio_ref_t *ref);

#ifdef __cplusplus
}
#endif

#endif /* AOS_KERNEL_BUILD */

/** @} */

#endif /* AOS_GPIO_H */
