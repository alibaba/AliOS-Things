/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _USB_UTIL_H
#define _USB_UTIL_H

#define USB_DEBUG 0

#if USB_DEBUG
#define USB_DBG(fmt,arg...)    printf("[USB %s %d] "fmt" \n",__func__,__LINE__,##arg)
#define USB_WARN(fmt,arg...)   printf("[USB %s %d] "fmt" \n",__func__,__LINE__,##arg)
#define USB_ERR(fmt,arg...)    printf("[USB %s %d] "fmt" \n",__func__,__LINE__,##arg)
#define USB_INFO(fmt,arg...)   printf("[USB %s %d] "fmt" \n",__func__,__LINE__,##arg)
#else
#define USB_DBG(fmt,arg...)
#define USB_WARN(fmt,arg...)
#define USB_ERR(fmt,arg...)    printf("[USB %s %d] "fmt" \n",__func__,__LINE__,##arg)
#define USB_INFO(fmt,arg...)   printf("[USB %s %d] "fmt" \n",__func__,__LINE__,##arg)
#endif

#endif /* _USB_UTIL_H */
