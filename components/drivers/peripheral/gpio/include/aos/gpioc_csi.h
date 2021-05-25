/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_GPIOC_CSI_H
#define AOS_GPIOC_CSI_H

#include <aos/gpioc.h>
#include <drv/gpio.h>

#define AOS_GPIOC_CSI_NUM_PINS  32

typedef struct {
    aos_gpioc_t gpioc;
    aos_gpioc_pin_t pins[AOS_GPIOC_CSI_NUM_PINS];
    uint32_t modes[AOS_GPIOC_CSI_NUM_PINS];
    uint32_t default_input_cfg;
    uint32_t default_output_cfg;
    csi_gpio_t csi_gpio;
} aos_gpioc_csi_t;

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_gpioc_csi_register(aos_gpioc_csi_t *gpioc_csi);
aos_status_t aos_gpioc_csi_unregister(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* AOS_GPIOC_CSI_H */
