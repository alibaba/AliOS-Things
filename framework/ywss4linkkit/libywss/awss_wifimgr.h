/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 *
 * Alibaba Group retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Alibaba Group., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Alibaba Group all copies of this computer program.  If you
 * are licensed by Alibaba Group, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Alibaba Group.
 *
 * This computer program contains trade secrets owned by Alibaba Group.
 * and, unless unauthorized by Alibaba Group in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
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
