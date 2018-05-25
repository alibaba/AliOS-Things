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

#ifndef __JOYLINK_PORTING_LAYER__
#define __JOYLINK_PORTING_LAYER__

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "nvdm.h"

//#include "joylink.h"
#include "httpclient.h"
#include "fota_config.h"
#include "fota.h"
#include "hal_wdt.h"

#define FOTA_BUF_SIZE   (1024 * 1)
#define FOTA_MAX_UAL_LEN    (100)

/*
 * fota upgrade context structure
 */
typedef struct _fota_ctx{
    char download_url[FOTA_MAX_UAL_LEN];
    uint32_t upgrade_status;
    uint32_t progress;
    uint32_t crc32;
    uint32_t data_len;
    uint32_t error_code;
}joylink_fota_ctx_t;

/*
 * fota upgrade status definition
 */
typedef enum _fota_status{
    FOTA_STATUS_DOWNLOAD = 0,
    FOTA_STATUS_INSTALL,
    FOTA_STATUS_SUCCESS,
    FOTA_STATUS_FAILURE
}joylink_fota_status_t;

typedef enum _fota_error_code{
    FOTA_ERROR_CODE_NONE = 0,
    FOTA_ERROR_CODE_CRC32_ERROR,
    FOTA_ERROR_CODE_DOWNLOAD_ERROR,
    FOTA_ERROR_CODE_CONNECT_SERVER_ERROR
}joylink_fota_error_code_t;

extern joylink_fota_ctx_t _g_fota_ctx;

extern void joylink_v2_init();
extern void joylink_v2_task(void *args);

extern void* joylink_calloc(size_t size, size_t num);
extern void* joylink_malloc(size_t sz);
extern void joylink_free(void *ptr);

extern int joylink_get_feedid(char *buf, unsigned int buf_sz);
extern int joylink_set_feedid(const char *json_in);
extern int joylink_get_accesskey(char *buf, unsigned int buf_sz);
extern int joylink_set_accesskey(const char *json_in);
extern int joylink_get_localkey(char *buf, unsigned int buf_sz);
extern int joylink_set_localkey(const char *json_in);
extern int joylink_get_server_info(char *buf, unsigned int buf_sz);
extern int joylink_set_server_info(const char *json_in);
extern int joylink_get_version(char *buf, unsigned int buf_sz);
extern int joylink_set_version(const char *json_in);

extern void joylink_fota_download_package();
extern void joylink_fota_set_upgrade_status(joylink_fota_status_t status, uint32_t progress, joylink_fota_error_code_t error_code);
extern void joylink_fota_download_task(void *parameter);
extern const char* joylink_fota_get_status_desc(joylink_fota_error_code_t error_code);

#endif /* __JOYLINK_PORTING_LAYER__ */
