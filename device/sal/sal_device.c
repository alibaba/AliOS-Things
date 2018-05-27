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
    if (ret){
        LOGE(TAG, "device init fail ret is %d\n", ret);
    }
    
    return ret;
}
