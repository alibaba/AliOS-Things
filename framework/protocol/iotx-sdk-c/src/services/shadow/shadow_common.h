/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */



#ifndef _SHADOW_IOTX_SHADOW_COMMON_H_
#define _SHADOW_IOTX_SHADOW_COMMON_H_


#include "iot_import.h"
#include "utils_timer.h"
#include "utils_list.h"
#include "shadow.h"
#include "shadow_config.h"

typedef struct iotx_update_ack_wait_list_st {
    int                 flag_busy; /* 0, free; 1, busy. */
    char                token[IOTX_DS_TOKEN_LEN];
    iotx_push_cb_fpt    callback;
    void               *pcontext;
    iotx_time_t         timer;
} iotx_update_ack_wait_list_t, *iotx_update_ack_wait_list_pt;


typedef struct iotx_inner_data_st {
    uint32_t token_num;
    uint32_t version;
    iotx_shadow_time_t time;
    iotx_update_ack_wait_list_t update_ack_wait_list[IOTX_DS_UPDATE_WAIT_ACK_LIST_NUM];
    list_t *attr_list;
    char *ptopic_update;
    char *ptopic_get;
    int32_t sync_status;
} iotx_inner_data_t, *iotx_inner_data_pt;;


typedef struct iotx_shadow_st {
    void *mqtt;
    void *mutex;
    iotx_inner_data_t inner_data;
} iotx_shadow_t, *iotx_shadow_pt;

iotx_err_t iotx_ds_common_format_init(iotx_shadow_pt pshadow,
                                      format_data_pt pformat,
                                      char *buf,
                                      uint16_t size,
                                      const char *method,
                                      const char *head_str);

iotx_err_t iotx_ds_common_format_add(iotx_shadow_pt pshadow,
                                     format_data_pt pformat,
                                     const char *name,
                                     const void *pvalue,
                                     iotx_shadow_attr_datatype_t datatype);

iotx_err_t iotx_ds_common_format_finalize(iotx_shadow_pt pshadow, format_data_pt pformat, const char *tail_str);

void iotx_ds_common_update_time(iotx_shadow_pt pshadow, uint32_t new_timestamp);

int iotx_ds_common_convert_data2string(
            char *buf,
            size_t buf_len,
            iotx_shadow_attr_datatype_t type,
            const void *pData);

iotx_err_t iotx_ds_common_convert_string2data(
            const char *buf,
            size_t buf_len,
            iotx_shadow_attr_datatype_t type,
            void *pData);

int iotx_ds_common_check_attr_existence(iotx_shadow_pt pshadow, const iotx_shadow_attr_pt pattr);

iotx_err_t iotx_ds_common_register_attr(
            iotx_shadow_pt pshadow,
            iotx_shadow_attr_pt pattr);

iotx_err_t iotx_ds_common_remove_attr(
            iotx_shadow_pt pshadow,
            iotx_shadow_attr_pt pattr);

char *iotx_ds_common_generate_topic_name(iotx_shadow_pt pshadow, const char *topic);

int iotx_ds_common_publish2update(iotx_shadow_pt pshadow, char *data, uint32_t data_len);

void iotx_ds_common_update_version(iotx_shadow_pt pshadow, uint32_t version);

uint32_t iotx_ds_common_get_version(iotx_shadow_pt pshadow);

uint32_t iotx_ds_common_get_tokennum(iotx_shadow_pt pshadow);

#endif /* _SHADOW_IOTX_SHADOW_COMMON_H_ */
