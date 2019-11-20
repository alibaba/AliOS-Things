/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef USB_DEVICE_OS_H
#define USB_DEVICE_OS_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <k_api.h>

/* Define USB Device OS related Type */
#define usbd_malloc  krhino_mm_alloc
#define usbd_free    krhino_mm_free

#ifndef bool
#define bool unsigned char
#endif


#ifdef __cplusplus
}
#endif

#endif /* USB_DEVICE_OS_H */
