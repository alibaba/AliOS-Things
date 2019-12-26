/**
 * @file http_def_config.h
 * http default config header file.
 *
 * @version   V1.0
 * @date      2019-12-24

 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HTTP_DEF_CONFIG_H
#define HTTP_DEF_CONFIG_H

/** @defgroup aos_http_parser http_parser
  * @{
  */
#ifdef CONFIG_DEFINE_HTTP_CONFIG
#include "http_config.h"
#endif

#ifndef CONFIG_HTTPC_SESSION_NUM
#define CONFIG_HTTPC_SESSION_NUM 1  /**< the max number of the http session */
#endif

#ifndef CONFIG_HTTPC_SERVER_NAME_SIZE
#define CONFIG_HTTPC_SERVER_NAME_SIZE 64  /**< server name size */
#endif

#ifndef CONFIG_HTTPC_DEFAULT_CLIENT
#define CONFIG_HTTPC_DEFAULT_CLIENT "AliOS-HTTP-Client/2.1"  /**< http client default user agent */
#endif

#ifndef CONFIG_HTTP_STATUS_SIZE
#define CONFIG_HTTP_STATUS_SIZE 32   /**< http status size */
#endif

#ifndef CONFIG_HTTP_SECURE
#if AOS_COMP_ITLS
#define CONFIG_HTTP_SECURE 1  /**< http secure support */
#elif AOS_COMP_MBEDTLS
#define CONFIG_HTTP_SECURE 1
#else
#define CONFIG_HTTP_SECURE 0  /**< http no secure support */
#endif
#endif

#if CONFIG_HTTP_SECURE
#if AOS_COMP_ITLS
#define CONFIG_HTTP_SECURE_ITLS 1
#else
#define CONFIG_HTTP_SECURE_ITLS 0
#endif
#endif

#ifndef CONFIG_HTTP_ENABLE_MUTEX
#define CONFIG_HTTP_ENABLE_MUTEX 1
#endif

#ifndef HTTP_DELETE
#define HTTP_DELETE 0
#endif

#ifndef HTTP_GET
#define HTTP_GET 1
#endif

#ifndef HTTP_HEAD
#define HTTP_HEAD 2
#endif

#ifndef HTTP_POST
#define HTTP_POST 3
#endif

#ifndef HTTP_PUT
#define HTTP_PUT 4
#endif

#define CONFIG_HTTP_DEBUG
/** @} */
#endif
