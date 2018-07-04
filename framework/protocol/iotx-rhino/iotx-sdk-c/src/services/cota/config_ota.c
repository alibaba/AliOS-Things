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


#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "iot_import.h"
#include "class_interface.h"
#include "iot_export_dm.h"
#include "iot_export_cota.h"
#include "iot_export_cm.h"
#include "iot_export_errno.h"
#include "lite-utils.h"

static void config_ota_handler(void* pcontext, iotx_cm_cota_parameter_t* ota_parameter, void* user_data)
{
	iotx_cm_cota_parameter_t* iotx_cm_ota_parameter = ota_parameter;
    config_ota_t* config_ota = user_data;

    log_info("\n\n%s\n\n", iotx_cm_ota_parameter->url);
    /**< save config*/
    if (config_ota->_rsp_configId) config_ota_lite_free(config_ota->_rsp_configId);
    if (config_ota->_rsp_getType) config_ota_lite_free(config_ota->_rsp_getType);
    if (config_ota->_rsp_sign) config_ota_lite_free(config_ota->_rsp_sign);
    if (config_ota->_rsp_signMethod) config_ota_lite_free(config_ota->_rsp_signMethod);
    if (config_ota->_rsp_url) config_ota_lite_free(config_ota->_rsp_url);

    config_ota->_rsp_configId = config_ota_lite_malloc(strlen(iotx_cm_ota_parameter->configId) + 1);
    if (config_ota->_rsp_configId == NULL) {
        log_err("rsp_configid malloc fail.");
        return;
    }
    config_ota->_rsp_getType = config_ota_lite_malloc(strlen(iotx_cm_ota_parameter->getType) + 1);
        if (config_ota->_rsp_getType == NULL) {
            log_err("_rsp_getType malloc fail.");
            return;
        }
	config_ota->_rsp_sign = config_ota_lite_malloc(strlen(iotx_cm_ota_parameter->sign) + 1);
		if (config_ota->_rsp_sign == NULL) {
			log_err("_rsp_sign malloc fail.");
			return;
		}
	config_ota->_rsp_signMethod = config_ota_lite_malloc(strlen(iotx_cm_ota_parameter->signMethod) + 1);
		if (config_ota->_rsp_signMethod == NULL) {
			log_err("_rsp_signMethod malloc fail.");
			return;
		}
	config_ota->_rsp_url = config_ota_lite_malloc(strlen(iotx_cm_ota_parameter->url) + 1);
		if (config_ota->_rsp_url == NULL) {
			log_err("_rsp_url malloc fail.");
			return;
		}
    strcpy(config_ota->_rsp_configId, iotx_cm_ota_parameter->configId);
    strcpy(config_ota->_rsp_getType, iotx_cm_ota_parameter->getType);
    strcpy(config_ota->_rsp_sign, iotx_cm_ota_parameter->sign);
    strcpy(config_ota->_rsp_signMethod, iotx_cm_ota_parameter->signMethod);
    strcpy(config_ota->_rsp_url, iotx_cm_ota_parameter->url);
    config_ota->_rsp_configSize = ota_parameter->configSize;
    /**< end*/


    log_debug("_rsp_configId %s", config_ota->_rsp_configId);
    log_debug("_rsp_configSize %u", config_ota->_rsp_configSize);
    log_debug("_rsp_getType %s", config_ota->_rsp_getType);
    log_debug("_rsp_sign %s", config_ota->_rsp_sign);
    log_debug("_rsp_signMethod %s", config_ota->_rsp_signMethod);
    log_debug("_rsp_url %s", config_ota->_rsp_url);

    /* invoke callback funtions. */
    if (config_ota->_linkkit_callback_fp) {
        ((handle_service_cota_callback_fp_t)config_ota->_linkkit_callback_fp)(service_cota_callback_type_new_version_detected,
        																	  config_ota->_rsp_configId,
																			  config_ota->_rsp_configSize,
																			  config_ota->_rsp_getType,
																			  config_ota->_rsp_sign,
																			  config_ota->_rsp_signMethod,
																			  config_ota->_rsp_url);
    }
}

static void config_ota_cm_event_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data)
{
    config_ota_t* config_ota = user_data;
    int ret;

    log_info("\n###\n");

    if (IOTX_CM_EVENT_CLOUD_CONNECTED == msg->event_id) {
        if (config_ota->_ota_inited == 0) {
            if (config_ota->_destructing == 1) return;

            HAL_GetFirmwareVesion(config_ota->_current_verison);

            log_info("Current firmware version: %s", config_ota->_current_verison);

            ret = IOT_CM_OTA_Start(config_ota->_current_verison, NULL);
            if (ret == SUCCESS_RETURN) {
            	config_ota->_ota_inited = 1;
            }

            ret = IOT_CM_OTA_Set_Callback(IOTX_CM_OTA_TYPE_COTA, config_ota_handler, config_ota, NULL);

            log_debug("ret = IOT_CM_OTA_Set_Callback() = %d\n", ret);
        }

        log_info("event %d(%s)\n###\n", msg->event_id, "cloud connected");
    }
}
static void* config_ota_ctor(void* _self, va_list* params)
{
	config_ota_t* self = _self;
	int ret = 0;
	iotx_cm_init_param_t init_param;



	self->_data_buf = NULL;
	self->_data_buf_length = 0;
	self->_ota_inited = 0;
	self->_current_verison = config_ota_lite_malloc(FIRMWARE_VERSION_MAXLEN);
	if (self->_current_verison == NULL) return NULL;
	memset(self->_current_verison, 0x0, FIRMWARE_VERSION_MAXLEN);

	init_param.secret_type = IOTX_CM_DEVICE_SECRET_DEVICE;

	init_param.event_func = config_ota_cm_event_handler;
	init_param.user_data = self;
	init_param.domain_type = IOTX_CM_CLOUD_DOMAIN_ShangHai;

	ret = IOT_CM_Init(&init_param, NULL);

	log_debug("ret = IOT_CM_Init() = %d\n", ret);

	log_debug("service cota created@%p.", self);

	return self;
}
static void* config_ota_dtor(void* _self)
{
    config_ota_t* self = _self;

    self->_destructing = 1;
    self->_data_buf = NULL;
    self->_data_buf_length = 0;
    if (self->_rsp_configId) config_ota_lite_free(self->_rsp_configId);
    if (self->_rsp_getType) config_ota_lite_free(self->_rsp_getType);
    if (self->_rsp_sign) config_ota_lite_free(self->_rsp_sign);
    if (self->_rsp_signMethod) config_ota_lite_free(self->_rsp_signMethod);
    if (self->_rsp_url) config_ota_lite_free(self->_rsp_url);
    if (self->_current_verison) config_ota_lite_free(self->_current_verison);

    return self;
}
/**< get config from server;used after ota inited*/
static int config_ota_get(void* _self,const char* configScope, const char* getType, const char* attributeKeys, void* option)
{
	config_ota_t* self = _self;
	self = self;
	return IOT_CM_OTA_Get_Config(configScope,getType,attributeKeys,option);
}
static void config_ota_start(void* _self)
{
    config_ota_t* self = _self;

    self = self;

    HAL_Firmware_Persistence_Start();
}

static int config_ota_write(void* _self, void* data, int data_length)
{
    config_ota_t* self = _self;
    int ret;

    assert(self->_data_buf_length >= data_length && data_length  && self->_data_buf == data);

    self->_data_buf = data;

    ret = HAL_Firmware_Persistence_Write(self->_data_buf, data_length);

    return ret;
}

static int config_ota_end(void* _self)
{
    config_ota_t* self = _self;
    int ret;

    self = self;

    /* this function should not return... */
    ret = HAL_Firmware_Persistence_Stop();

    log_emerg("OTA end");
    /* update config*/
    return ret;
}

static int config_ota_perform_ota_service(void* _self, void* _data_buf, int _data_buf_length)
{
    config_ota_t* self = _self;
    int ret = -1;
    iotx_cm_ota_t* iotx_cm_ota;

    assert(_data_buf && _data_buf_length);

    self->_data_buf = _data_buf;
    self->_data_buf_length = _data_buf_length;

    iotx_cm_ota = config_ota_lite_calloc(1, sizeof(iotx_cm_ota_t));

    assert(iotx_cm_ota);
    if (iotx_cm_ota == NULL) return -1;

    iotx_cm_ota->buffer = self->_data_buf;
    iotx_cm_ota->buffer_length = self->_data_buf_length;
	iotx_cm_ota->ota_type = IOTX_CM_OTA_TYPE_COTA;

    self->_total_len = 0;

    config_ota_start(self);

    while (1) {
        /* reset buffer size every time after fetch */
        iotx_cm_ota->buffer_length = self->_data_buf_length;
        ret = IOT_CM_OTA_Yield(iotx_cm_ota);

        assert(iotx_cm_ota->buffer && iotx_cm_ota->buffer_length);

        if (ret == 0 && iotx_cm_ota->buffer && iotx_cm_ota->buffer_length) {
            ret = config_ota_write(self, iotx_cm_ota->buffer, iotx_cm_ota->buffer_length);
            if (ret == 0) {
                self->_total_len += iotx_cm_ota->buffer_length;
            }
            log_debug("\nservice cota write flash,\tret=%d,\tbuffer_length=%d,\ttotal len:%d\n",
                      ret, iotx_cm_ota->buffer_length, self->_total_len);
            if (ret) goto err_handler;
        }

        if (ret || iotx_cm_ota->result) {
            log_debug("service cota fail, ret=%d,\tresult=%d", ret, iotx_cm_ota->result);
            goto err_handler;
        }

        if (iotx_cm_ota->is_more) continue;

        if(iotx_cm_ota->is_more == 0 && iotx_cm_ota->result == 0) {
            ret = 0;
            log_debug("\nservice cota complete\n");
            break;
        }
    }

err_handler:
    if (ret == 0 && iotx_cm_ota->result == 0) {
        ret = config_ota_end(self);
        if (ret) {
            log_err("service cota invoke end function error, ret=%d", ret);
        }
    } else {
        ret = -1;
    }

    if (iotx_cm_ota) service_ota_lite_free(iotx_cm_ota);

    return ret;
}

static void config_ota_install_callback_function(void* _self, handle_service_cota_callback_fp_t linkkit_callback_fp)
{
    config_ota_t* self = _self;

    assert(linkkit_callback_fp);

    self->_linkkit_callback_fp = linkkit_callback_fp;
}

void* config_ota_lite_calloc(size_t nmemb, size_t size)
{
#ifdef CM_SUPPORT_MEMORY_MAGIC
    return LITE_calloc(nmemb, size, MEM_MAGIC, SERVICE_COTA_MODULE_NAME);
#else
    return LITE_calloc(nmemb, size);
#endif
}

void* config_ota_lite_malloc(size_t size)
{
#ifdef CM135gg_SUPPORT_MEMORY_MAGIC
    return LITE_malloc(size, MEM_MAGIC, SERVICE_COTA_MODULE_NAME);
#else
    return LITE_malloc(size);
#endif
}

void config_ota_lite_free_func(void* ptr)
{
    return LITE_free_internal(ptr);
}

static const cota_t _service_cota_class = {
		sizeof(config_ota_t),
		"cota_cls",
		config_ota_ctor,
		config_ota_dtor,
		config_ota_get,
		config_ota_start,
		config_ota_write,
		config_ota_end,
		config_ota_perform_ota_service,
		config_ota_install_callback_function
};
const void* get_config_ota_class()
{
	return &_service_cota_class;
}
