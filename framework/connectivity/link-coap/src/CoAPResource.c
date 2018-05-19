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


#include <string.h>
#include "CoAPExport.h"
#include "CoAPResource.h"
#include "CoAPPlatform.h"
#include "CoAPInternal.h"
#include "lite-list.h"
#include "utils_md5.h"

#define COAP_PATH_DEFAULT_SUM_LEN (5)

int CoAPPathMD5_sum (const char* path, int len, char outbuf[], int outlen)
{
    unsigned char md5[16]={0};
    if (!path || !len || !outbuf || !outlen) {
        return -1;
    }

    utils_md5 ((unsigned char *)path, (size_t)len, md5);
    memcpy (outbuf, md5, outlen>16? 16 : outlen);
    return 0;
}


int CoAPResource_init(CoAPContext *context, int res_maxcount)
{
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    ctx->resource.list_mutex = HAL_MutexCreate();
    INIT_LIST_HEAD(&ctx->resource.list);
    ctx->resource.count = 0;
    ctx->resource.maxcount = res_maxcount;

    return COAP_SUCCESS;
}

int CoAPResource_deinit(CoAPContext *context)
{
    CoAPResource *node = NULL, *next = NULL;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    HAL_MutexLock(ctx->resource.list_mutex);
    list_for_each_entry_safe(node, next, &ctx->resource.list, reslist, CoAPResource) {
        list_del_init(&node->reslist);
        COAP_DEBUG("Release the resource %s", node->path);
        coap_free(node);
        node  = NULL;
    }
    ctx->resource.count = 0;
    ctx->resource.maxcount = 0;
    HAL_MutexUnlock(ctx->resource.list_mutex);

    HAL_MutexDestroy(ctx->resource.list_mutex);
    ctx->resource.list_mutex = NULL;
    return COAP_SUCCESS;
}

CoAPResource *CoAPResource_create(const char *path, unsigned short           permission,
                                          unsigned int ctype, unsigned int maxage,
                                          CoAPRecvMsgHandler callback)
{
    CoAPResource *resource = NULL;

    if(NULL == path){
        return NULL;
    }

    if (strlen(path) >= COAP_MSG_MAX_PATH_LEN){
        return NULL;
    }

    resource = coap_malloc(sizeof(CoAPResource));
    if(NULL == resource){
        return NULL;
    }

    memset(resource, 0x00, sizeof(CoAPResource));
    CoAPPathMD5_sum(path, strlen(path), resource->path, COAP_PATH_DEFAULT_SUM_LEN);

    resource->callback = callback;
    resource->ctype = ctype;
    resource->maxage = maxage;
    resource->permission = permission;

    return resource;
}

int CoAPResource_register(CoAPContext *context, const char *path,
                    unsigned short permission, unsigned int ctype,
                    unsigned int maxage, CoAPRecvMsgHandler callback)
{
    int exist = 0;
    char path_calc[COAP_PATH_DEFAULT_SUM_LEN] = {0};
    CoAPResource *node = NULL, *newnode = NULL;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    HAL_MutexLock(ctx->resource.list_mutex);
    if(ctx->resource.count >= ctx->resource.maxcount){
        HAL_MutexUnlock(ctx->resource.list_mutex);
        COAP_INFO("The resource count exceeds limit, cur %d, max %d",
                   ctx->resource.count,  ctx->resource.maxcount);
        return COAP_ERROR_DATA_SIZE;
    }
  

    CoAPPathMD5_sum (path, strlen(path), path_calc, COAP_PATH_DEFAULT_SUM_LEN);
    list_for_each_entry(node, &ctx->resource.list, reslist, CoAPResource) {
        if(0 == memcmp(path_calc, node->path, COAP_PATH_DEFAULT_SUM_LEN)){
            /*Alread exist, re-write it*/
            COAP_INFO("CoAPResource_register:Alread exist");
            exist = 1;
            node->callback = callback;
            node->ctype = ctype;
            node->maxage = maxage;
            node->permission = permission;
            COAP_INFO("The resource %s already exist, re-write it", path);
            break;
        }
    }

    if(0 == exist){
        newnode = CoAPResource_create(path, permission, ctype, maxage, callback);
        if(NULL != newnode){
            COAP_INFO("CoAPResource_register, context:%p, new node", ctx);
            list_add_tail(&newnode->reslist, &ctx->resource.list);
            ctx->resource.count++;
            COAP_INFO("Register new resource %s success, count: %d", path, ctx->resource.count);
        }
        else{
            COAP_ERR("New resource create failed");
        }
    }

    HAL_MutexUnlock(ctx->resource.list_mutex);

    return COAP_SUCCESS;
}

int CoAPResource_unregister(CoAPContext *context, const char *path)
{
    COAP_DEBUG("This feature isn't supported");
    return COAP_ERROR_UNSUPPORTED;
}

CoAPResource *CoAPResourceByPath_get(CoAPContext *context, const char *path)
{
    char path_calc[COAP_PATH_DEFAULT_SUM_LEN] = {0};
    CoAPResource *node = NULL;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    if(NULL == context || NULL == path){
        COAP_INFO("%s\n", "NULL == context || NULL == path");
        return NULL;
    }
    COAP_INFO("CoAPResourceByPath_get, context:%p\n", ctx);

    CoAPPathMD5_sum (path, strlen(path), path_calc, COAP_PATH_DEFAULT_SUM_LEN);

    HAL_MutexLock(ctx->resource.list_mutex);
    list_for_each_entry(node, &ctx->resource.list, reslist, CoAPResource) {
        if(0 == memcmp(path_calc, node->path, COAP_PATH_DEFAULT_SUM_LEN)){
            HAL_MutexUnlock(ctx->resource.list_mutex);
            COAP_DEBUG("Found the resource: %s", node->path);
            return node;
        }
    }
    HAL_MutexUnlock(ctx->resource.list_mutex);

    return NULL;
}
