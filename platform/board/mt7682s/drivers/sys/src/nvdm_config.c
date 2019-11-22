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

#include <stdio.h>
#include <string.h>
//#include "FreeRTOS.h"
#include "nvdm.h"
#include "syslog.h"
#include "type_def.h"
#include "nvdm_config.h"


void check_default_value(const char *group_name, const group_data_item_t *group_data_array, uint32_t count)
{
    uint8_t buffer[1024] = {0};

    for (uint32_t index = 0; index < count; index++) {
        uint32_t buffer_size = 1024;
        nvdm_status_t status = nvdm_read_data_item(group_name,
                               group_data_array[index].item_name,
                               buffer,
                               &buffer_size);
        if (NVDM_STATUS_OK != status) {
            {
                status = nvdm_write_data_item(group_name,
                                              group_data_array[index].item_name,
                                              group_data_array[index].data_type,
                                              (uint8_t *)group_data_array[index].item_default_value,
                                              group_data_array[index].item_size);
                if (NVDM_STATUS_OK != status) {
                    LOG_I(common, "write to [%s]%s error", group_name, group_data_array[index].item_name);
                }
            }
        }
    }
}

void reset_to_default(const char *group_name, const group_data_item_t *group_data_array, uint32_t count)
{
    for (uint32_t index = 0; index < count; index++) {
        nvdm_status_t status;
        {
            status = nvdm_write_data_item(group_name,
                                          group_data_array[index].item_name,
                                          group_data_array[index].data_type,
                                          (uint8_t *)group_data_array[index].item_default_value,
                                          group_data_array[index].item_size);
            if (NVDM_STATUS_OK != status) {
                LOG_I(common, "write to [%s]%s error", group_name, group_data_array[index].item_name);
            }
        }
    }
}

void show_group_value(const char *group_name, const group_data_item_t *group_data_array, uint32_t count)
{
    uint8_t buffer[1024] = {0};
    for (uint32_t index = 0; index < count; index++) {
        uint32_t buffer_size = 1024;
        nvdm_status_t status = nvdm_read_data_item(group_name,
                               group_data_array[index].item_name,
                               buffer,
                               &buffer_size);
        if (NVDM_STATUS_OK == status) {
            printf("[%s]%s: %s\r\n", group_name, group_data_array[index].item_name, (char *)buffer);
        } else {
            printf("read from [%s]%s error.\r\n", group_name, group_data_array[index].item_name);
        }
    }
}

static user_data_item_operate_t user_data_item_operate_array[USER_DATA_MAX_ITEM] = {{0}};

extern user_data_item_operate_t wifi_data_item_operate_array[];
extern user_data_item_operate_t network_data_item_operate_array[];

int32_t nvdm_register_module(user_data_item_operate_t *data_item_array,uint8_t group_num)
{
    uint32_t index;
    for (index = 0; index < USER_DATA_MAX_ITEM; index++) {
        if(user_data_item_operate_array[index].group_name == NULL)
            break;
    }
    if((USER_DATA_MAX_ITEM - index)>= group_num) {
        memcpy(user_data_item_operate_array + index,data_item_array, group_num*sizeof(user_data_item_operate_t));
        return 0;
    }else {
        LOG_E(common,"user_data_item_operate_array is not enough.");
        return -1;
    }
}

/* This function is used to check whether data is exist in NVDM region,
 * and write default value to NVDM region if no value can be found in NVDM region.
 */
void user_check_default_value(void)
{
    uint32_t index;
    nvdm_register_module(wifi_data_item_operate_array,3);
    nvdm_register_module(network_data_item_operate_array,1);

    for (index = 0; index < USER_DATA_MAX_ITEM; index++) {
        if(user_data_item_operate_array[index].group_name != NULL)
            user_data_item_operate_array[index].check_default_value();
    }
}

void user_data_item_reset_to_default(char *group_name)
{
    uint32_t index;
    uint32_t max = sizeof(user_data_item_operate_array) / sizeof(user_data_item_operate_t);

    if (group_name == NULL) {
        for (index = 0; index < max; index++) {
            if(user_data_item_operate_array[index].group_name != NULL)
              user_data_item_operate_array[index].reset_default_value();
        }
    } else {
        for (index = 0; index < max; index++) {
            if (memcmp(user_data_item_operate_array[index].group_name, group_name,
                       strlen(user_data_item_operate_array[index].group_name)) == 0) {
                user_data_item_operate_array[index].reset_default_value();
                break;
            }
        }
    }
}

void user_data_item_show_value(char *group_name)
{
    uint32_t index;
    uint32_t max = sizeof(user_data_item_operate_array) / sizeof(user_data_item_operate_t);

    if (group_name == NULL) {
        for (index = 0; index < max; index++) {
            if(user_data_item_operate_array[index].group_name != NULL)
                user_data_item_operate_array[index].show_value();
        }
    } else {
        for (index = 0; index < max; index++) {
            if (memcmp(user_data_item_operate_array[index].group_name, group_name,
                       strlen(user_data_item_operate_array[index].group_name)) == 0) {
                user_data_item_operate_array[index].show_value();
                break;
            }
        }
    }
}


