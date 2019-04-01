/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __OS_PORT_H__
#define __OS_PORT_H__

#ifdef __cplusplus
extern "C" {
#endif

// MT7687
#include "osal.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
//#include "syslog.h"

// HTTPD
// Feature Option
//#ifndef MTK_DEBUG_LEVEL_NONE
//#define HTTPD_DEBUG 1
//#else
//#define HTTPD_DEBUG 0
//#endif
#define HTTPD_DEBUG 0
//#define CONFIG_HTTP_DIRECTORIES
//#define CONFIG_HTTP_HAS_IPV6

// Macro define
#define CONFIG_HTTP_PORT 80
#define CONFIG_HTTP_HOME_HTML "index.html"

//typedef char int8_t;

/* Plaform specific diagnostic output */
#define HTTPD_DIAG(x)   do { LOG_I x; } while(0)

#if HTTPD_DEBUG
#define HTTPD_DEBUGF(message) do { HTTPD_DIAG(message);} while(0)
#else  /* HTTPD_DEBUG */
#define HTTPD_DEBUGF(message)
#endif /* HTTPD_DEBUG */

#define SOCKET_READ(A,B,C)      read(A,B,C)
#define SOCKET_WRITE(A,B,C)     write(A,B,C)
#define SOCKET_CLOSE(A)         if (A >= 0) close(A)
#define TTY_FLUSH()

void *ax_malloc(size_t s);
void *ax_realloc(void *y, size_t s);
void *ax_calloc(size_t s);
void ax_free(void *y);

#ifdef __cplusplus
}
#endif

#endif /* __OS_PORT_H__ */
