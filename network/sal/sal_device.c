/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <aos/aos.h>

#define  TAG  "SAL_DEVICE"

#ifdef DEV_SAL_MK3060
extern int mk3060_sal_init(void);
#endif

int sal_device_init()
{
    int ret = 0;

#ifdef DEV_SAL_MK3060
    ret = mk3060_sal_init();
#endif

#ifdef DEV_SAL_GT202
    ret = gt202_sal_init();
#endif

#ifdef DEV_SAL_SIM800
    extern int sim800_sal_init(void);
    ret = sim800_sal_init();
#endif

#ifdef DEV_SAL_ESP8266
    extern int esp8266_sal_init(void);
    ret = esp8266_sal_init();
#endif

    /*
    * Add extern wifi module BK7231 support for starterkit/developerkit.
    * Currently it follow mk3060 module implementation only removing
    * unsupported AT checkings in sal_wifi_init()(device/sal/wifi/bk7231/bk7231.c)
    */
#ifdef DEV_SAL_BK7231
    extern int bk7231_sal_init(void);
    ret = bk7231_sal_init();
#endif

#ifdef DEV_SAL_ATHOST
    extern int athost_sal_init(void);
    ret = athost_sal_init();
#endif

    if (ret) {
        LOGE(TAG, "device init fail ret is %d\n", ret);
    }

    return ret;
}
