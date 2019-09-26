/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __USB_HOST_DEBUG_H
#define __USB_HOST_DEBUG_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>

/* Debug is disabled by default. */
#define USB_DEBUG_LEVEL     3 /* INFO, WARN, ERR */

#if (USB_DEBUG_LEVEL > 3)
#define usb_dbg(x, ...) printf("[USB_DBG: %s]" x "\r\n", __func__, ##__VA_ARGS__);
#else
#define usb_dbg(x, ...)
#endif

#if (USB_DEBUG_LEVEL > 2)
#define usb_info(x, ...) printf("[USB_INFO: %s]" x "\r\n", __func__, ##__VA_ARGS__);
#else
#define usb_info(x, ...)
#endif

#if (USB_DEBUG_LEVEL > 1)
#define usb_warn(x, ...) printf("[USB_WARN: %s]" x "\r\n", __func__, ##__VA_ARGS__);
#else
#define usb_warn(x, ...)
#endif

#if (USB_DEBUG_LEVEL > 0)
#define usb_err(x, ...) printf("[USB_ERR: %s]" x "\r\n", __func__, ##__VA_ARGS__);
#else
#define usb_err(x, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_DEBUG_H */
