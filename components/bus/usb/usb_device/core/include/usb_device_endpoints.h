/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef USB_DEVICE_ENDPOINTS_H
#define USB_DEVICE_ENDPOINTS_H

#ifdef __cplusplus
 extern "C" {
#endif


/* SETUP packet size */
#define SETUP_PACKET_SIZE (8)

/* Options flags for configuring endpoints */
#define DEFAULT_OPTIONS     (0)
#define SINGLE_BUFFERED     (1U << 0)
#define ISOCHRONOUS         (1U << 1)
#define RATE_FEEDBACK_MODE  (1U << 2) /* Interrupt endpoints only */

/* Include configuration for specific target */
#if defined(TARGET_STM32L4)
#include "usbendpoints_stm32.h"
#else
#error "Unknown target type"
#endif

#ifdef __cplusplus
}
#endif

#endif /* USB_DEVICE_ENDPOINTS_H */