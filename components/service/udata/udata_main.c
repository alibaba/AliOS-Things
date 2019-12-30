/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "abs_data_model.h"
#include "service_mgr.h"
#include "udata_queue.h"
#include "service_data_to_cloud.h"

int udata_init(void)
{
    int ret = 0;

    /* NOTE:
     * please run the abs data init firstly, then run udata service init */
    ret = udata_start();
    if (unlikely(ret)) {
        return -1;
    }

    ret = abs_data_model_init();
    if (unlikely(ret)) {
        return -1;
    }

    ret = udata_service_mgr_init();
    if (unlikely(ret)) {
        return -1;
    }

    ret = udata_service_init();
    if (unlikely(ret)) {
        return -1;
    }
#ifdef DATA_TO_CLOUD
    ret = service_dtc_init();
    if (unlikely(ret)) {
        return -1;
    }
#endif
    ret = abs_data_cali_init();
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}
