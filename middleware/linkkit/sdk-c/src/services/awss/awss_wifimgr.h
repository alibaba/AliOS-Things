/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifndef WIFIMGR_H
#define WIFIMGR_H

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

enum {
    SHUB_ERR,
    SHUB_OK
};

enum {
    SEC_LVL_OPEN = 0,       // open
    SEC_LVL_AES256,         // AES256
    SEC_LVL_AES128_DEFAULT, // AES128 with default key
    SEC_LVL_AES128_PRODUCT, // AES128 with key from product_sec
    SEC_LVL_AES128_DEVICE,  // AES128 with key from device_sec
    SEC_LVL_AES128_MANU,    // AES128 with key from manufacturer_sec
};

int wifimgr_process_get_wifilist_request(void *ctx, void *resource, void *remote, void *request);
int wifimgr_process_mcast_get_device_info(void *ctx, void *resource, void *remote, void *request);
int wifimgr_process_ucast_get_device_info(void *ctx, void *resource, void *remote, void *request);
int wifimgr_process_switch_ap_request(void *ctx, void *resource, void *remote, void *request);

void *awss_build_dev_info(int type, void *dev_info, int info_len);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
