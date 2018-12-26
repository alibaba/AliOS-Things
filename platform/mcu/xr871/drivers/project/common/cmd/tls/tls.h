/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __TLS_H_H
#define __TLS_H_H

#define MBEDTLS_CLIENT_SERVER

#if defined (MBEDTLS_CLIENT)
#include "net/mbedtls/configs/config-xr-mini-cli.h"
#endif
#if defined (MBEDTLS_CLIENT_SERVER)
#include "net/mbedtls/configs/config-xr-mini-cliserv.h"
#endif
#if defined (MBEDTLS_SERVER)
#include "net/mbedtls/configs/config-xr-mini-serv.h"
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#include <stdlib.h>
#define mbedtls_fprintf         fprintf
#define mbedtls_printf          printf
#define mbedtls_calloc          calloc
#define mbedtls_strlen          strlen
#define mbedtls_strncmp         strncmp
#endif

#include "kernel/os/os_time.h"
#include "kernel/os/os_thread.h"
#include "kernel/os/FreeRTOS/os_common.h"

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL                      0
#endif

#ifndef DEFALT_TEST_TIME
#define DEFALT_TEST_TIME                 0 //ms
#endif

#ifndef TLS_TEST_BUF_SIZE
#define TLS_TEST_BUF_SIZE                4096
#endif

#ifndef TLS_GET_TIME
#define TLS_GET_TIME                     OS_GetTicks
#endif

#ifndef TLS_THREAD_STACK_SIZE
#define TLS_THREAD_STACK_SIZE            (6 * 1024)
#endif

#define SERVER_PORT          "10000"
#define CLINET_PORT          "10001"
#define CLIENT_DATA          "1234567890QWERTYUIOPASDFGHJKLZXCVBNMfjafjakfjlak"\
                             "jlakfjlakjflkajflkajflkjflkajflakfjlak"
typedef struct
{
	char          server_port[10];
	char          server_name[30];
	unsigned int  continue_ms;
	unsigned int  flags;
} mbedtls_test_param;

typedef enum
{
	MBEDTLS_SSL_FLAG_SERVER_PORT = 0x00000001,
	MBEDTLS_SSL_FLAG_SERVER_NAME = 0x00000002,
	MBEDTLS_SSL_FLAG_CONTINUE    = 0x00000004,
	MBEDTLS_SSL_FLAG_CLINET      = 0x00000008,
	MBEDTLS_SSL_FLAG_SERVER      = 0x00000010,
	MBEDTLS_SSL_FLAG_WEBSERVER   = 0x00000020,
	MBEDTLS_SSL_FLAG_WEBCLIENT   = 0x00000040,
} mbedtls_test_flags;

#define TLS_THREAD_EXIT                 OS_ThreadDelete
#define FREE_BUF(P)                     \
                           if (P != NULL) { \
                           free(P);\
                           P = NULL;}

extern const char mbedtls_custom_cas_pem[];
extern const size_t mbedtls_custom_cas_pem_len;
int tls_start(mbedtls_test_param *param);
int tls_thread_stop();

#endif /* __TLS_H_H */

