/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_DEVICE_H
#define AOS_DEVICE_H

#include <aos/kernel.h>

typedef enum {
    AOS_DEV_TYPE_MISC           = 0,
    AOS_DEV_TYPE_INTC,
    AOS_DEV_TYPE_TTY,
    AOS_DEV_TYPE_GPIOC,
    AOS_DEV_TYPE_GPIO,
    AOS_DEV_TYPE_WATCHDOG,
    AOS_DEV_TYPE_ETHERNET,
    AOS_DEV_TYPE_WLAN,
    AOS_DEV_TYPE_BLOCK,
    AOS_DEV_TYPE_FLASH,
    AOS_DEV_TYPE_MTD,
    AOS_DEV_TYPE_RTC,
    AOS_DEV_TYPE_SPI,
    AOS_DEV_TYPE_I2C,
    AOS_DEV_TYPE_ADC,
    AOS_DEV_TYPE_DAC,
    AOS_DEV_TYPE_PWM,
} aos_dev_type_t;

struct aos_dev;

typedef struct aos_dev_ref {
    struct aos_dev *dev;
    void *pdata;
} aos_dev_ref_t;

#define AOS_DEV_REF_INIT_VAL            { .dev = NULL, .pdata = NULL, }
#define aos_dev_ref_init(ref)           do { *(ref) = (aos_dev_ref_t)AOS_DEV_REF_INIT_VAL; } while (0)
#define aos_dev_ref_is_valid(ref)       (!!(ref)->dev)

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_dev_get(aos_dev_ref_t *ref, aos_dev_type_t type, uint32_t id);
void aos_dev_put(aos_dev_ref_t *ref);

#ifdef __cplusplus
}
#endif

#endif /* AOS_DEVICE_H */
