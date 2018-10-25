/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _MDAL_MAL_EXPORT_H_
#define _MDAL_MAL_EXPORT_H_

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

int iotx_mdal_get_auth_username_passwd(char* username, char* password);

#ifdef __cplusplus
}
#endif

#endif

