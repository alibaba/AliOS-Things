/* * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
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


#ifndef __COAP_RESOURCE_H__
#define __COAP_RESOURCE_H__

#include <stdint.h>
#include "lite-list.h"
#include "CoAPExport.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define COAP_MAX_PATH_CHECKSUM_LEN (5)

typedef struct
{
    unsigned short           permission;
    CoAPRecvMsgHandler       callback;
    unsigned int             ctype;
    unsigned int             maxage;
    struct list_head         reslist;
    char                     path[COAP_MAX_PATH_CHECKSUM_LEN];
}CoAPResource;

int CoAPResource_init(CoAPContext *context, int res_maxcount);

int CoAPPathMD5_sum (const char* path, int len, char outbuf[], int outlen);

int CoAPResource_register(CoAPContext *context, const char *path,
                    unsigned short permission, unsigned int ctype,
                    unsigned int maxage, CoAPRecvMsgHandler callback);

CoAPResource *CoAPResourceByPath_get(CoAPContext *context, const char *path);

int CoAPResource_deinit(CoAPContext *context);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
