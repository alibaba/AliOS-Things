#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "iot_import.h"
#include "class_interface.h"
#include "iot_export_dm.h"
#include "iot_export_fota.h"
#include "iot_export_cm.h"
#include "iot_export_errno.h"
#include "lite-utils.h"


static void service_ota_handler(void* pcontext, iotx_cm_fota_parameter_t* ota_parameter, void* user_data)
{
    iotx_cm_fota_parameter_t* iotx_cm_ota_parameter = ota_parameter;
    service_ota_t* service_ota = user_data;

    log_info("\n\n%s\n\n", iotx_cm_ota_parameter->purl);

    if (service_ota->_ota_version) service_ota_lite_free(service_ota->_ota_version);

    service_ota->_ota_version = service_ota_lite_calloc(1, strlen(iotx_cm_ota_parameter->version) + 1);
    if (service_ota->_ota_version == NULL) {
        log_err("ota version malloc fail.");
        return;
    }
    strcpy(service_ota->_ota_version, iotx_cm_ota_parameter->version);
    log_debug("new OTA version %s", service_ota->_ota_version);

    /* invoke callback funtions. */
    if (service_ota->_linkkit_callback_fp) {
        ((handle_service_fota_callback_fp_t)service_ota->_linkkit_callback_fp)(service_fota_callback_type_new_version_detected, service_ota->_ota_version);
    }
}

static void service_ota_cm_event_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data)
{
    service_ota_t* service_ota = user_data;
    int ret;

    log_info("\n###\n");

    if (IOTX_CM_EVENT_CLOUD_CONNECTED == msg->event_id) {
        if (service_ota->_ota_inited == 0) {
            if (service_ota->_destructing == 1) return;

            HAL_GetFirmwareVesion(service_ota->_current_verison);

            log_info("Current firmware version: %s", service_ota->_current_verison);

            ret = IOT_CM_OTA_Start(service_ota->_current_verison, NULL);
            if (ret == SUCCESS_RETURN) {
                service_ota->_ota_inited = 1;
            }

            ret = IOT_CM_OTA_Set_Callback(IOTX_CM_OTA_TYPE_FOTA, service_ota_handler, service_ota, NULL);

            log_debug("ret = IOT_CM_OTA_Set_Callback() = %d\n", ret);
        }

        log_info("event %d(%s)\n###\n", msg->event_id, "cloud connected");
    }
}

static void* service_ota_ctor(void* _self, va_list* params)
{
    service_ota_t* self = _self;
    int ret = 0;
    iotx_cm_init_param_t init_param;

    self->_data_buf = NULL;
    self->_data_buf_length = 0;
    self->_ota_inited = 0;
    self->_current_verison = service_ota_lite_malloc(FIRMWARE_VERSION_MAXLEN);
    if (self->_current_verison == NULL) return NULL;
    memset(self->_current_verison, 0x0, FIRMWARE_VERSION_MAXLEN);

    init_param.event_func = service_ota_cm_event_handler;
    init_param.user_data = self;
    init_param.domain_type = IOTX_CM_CLOUD_DOMAIN_ShangHai;
    init_param.secret_type = IOTX_CM_DEVICE_SECRET_DEVICE;

    ret = IOT_CM_Init(&init_param, NULL);

    log_debug("ret = IOT_CM_Init() = %d\n", ret);

    log_debug("service fota created@%p.", self);

    return self;
}

static void* service_ota_dtor(void* _self)
{
    service_ota_t* self = _self;

    self->_destructing = 1;
    self->_data_buf = NULL;
    self->_data_buf_length = 0;
    if (self->_ota_version) service_ota_lite_free(self->_ota_version);
    if (self->_current_verison) service_ota_lite_free(self->_current_verison);

    return self;
}

static void service_ota_start(void* _self)
{
    service_ota_t* self = _self;

    self = self;

    HAL_Firmware_Persistence_Start();
}

static int service_ota_write(void* _self, void* data, int data_length)
{
    service_ota_t* self = _self;
    int ret;

    assert(self->_data_buf_length >= data_length && data_length  && self->_data_buf == data);

    self->_data_buf = data;

    ret = HAL_Firmware_Persistence_Write(self->_data_buf, data_length);

    return ret;
}

static int service_ota_end(void* _self)
{
    service_ota_t* self = _self;
    int ret;

    self = self;

    /* this function should not return... */
    ret = HAL_Firmware_Persistence_Stop();

#if defined(_PLATFORM_IS_LINUX_) || defined(_PLATFORM_IS_WINDOWS_) || defined(_PLATFORM_IS_HOST_)
    exit(0);
#else
    log_emerg("OTA end, restarting...");
    HAL_Sys_reboot();
#endif
    return ret;
}

static int service_ota_perform_ota_service(void* _self, void* _data_buf, int _data_buf_length)
{
    service_ota_t* self = _self;
    int ret = -1;
    iotx_cm_ota_t* iotx_cm_ota;

    assert(_data_buf && _data_buf_length);

    self->_data_buf = _data_buf;
    self->_data_buf_length = _data_buf_length;

    iotx_cm_ota = service_ota_lite_calloc(1, sizeof(iotx_cm_ota_t));

    assert(iotx_cm_ota);
    if (iotx_cm_ota == NULL) return -1;

    iotx_cm_ota->buffer = self->_data_buf;
    iotx_cm_ota->buffer_length = self->_data_buf_length;
    iotx_cm_ota->ota_type = IOTX_CM_OTA_TYPE_FOTA;

    self->_total_len = 0;

    service_ota_start(self);

    while (1) {
        /* reset buffer size every time after fetch */
        iotx_cm_ota->buffer_length = self->_data_buf_length;
        ret = IOT_CM_OTA_Yield(iotx_cm_ota);
        
        assert(iotx_cm_ota->buffer && iotx_cm_ota->buffer_length);

        if (ret == 0 && iotx_cm_ota->buffer && iotx_cm_ota->buffer_length) {
            ret = service_ota_write(self, iotx_cm_ota->buffer, iotx_cm_ota->buffer_length);
            if (ret == 0) {
                self->_total_len += iotx_cm_ota->buffer_length;
            }
            log_debug("\nservice fota write flash,\tret=%d,\tbuffer_length=%d,\ttotal len:%d\n",
                      ret, iotx_cm_ota->buffer_length, self->_total_len);
            if (ret) goto err_handler;
        }

        if (ret || iotx_cm_ota->result) {
            log_debug("service fota fail, ret=%d,\tresult=%d", ret, iotx_cm_ota->result);
            goto err_handler;
        }

        if (iotx_cm_ota->is_more) continue;

        if(iotx_cm_ota->is_more == 0 && iotx_cm_ota->result == 0) {
            ret = 0;
            log_debug("\nservice fota complete\n");
            break;
        }
    }

err_handler:
    if (ret == 0 && iotx_cm_ota->result == 0) {
        ret = service_ota_end(self);
        if (ret) {
            log_err("service fota invoke end function error, ret=%d", ret);
        }
    } else {
        ret = -1;
    }

    if (iotx_cm_ota) service_ota_lite_free(iotx_cm_ota);

    return ret;
}

static void service_ota_install_callback_function(void* _self, handle_service_fota_callback_fp_t linkkit_callback_fp)
{
    service_ota_t* self = _self;

    assert(linkkit_callback_fp);

    self->_linkkit_callback_fp = linkkit_callback_fp;
}

void* service_ota_lite_calloc(size_t nmemb, size_t size)
{
#ifdef CM_SUPPORT_MEMORY_MAGIC
    return LITE_calloc(nmemb, size, MEM_MAGIC, SERVICE_FOTA_MODULE_NAME);
#else
    return LITE_calloc(nmemb, size);
#endif
}

void* service_ota_lite_malloc(size_t size)
{
#ifdef CM_SUPPORT_MEMORY_MAGIC
    return LITE_malloc(size, MEM_MAGIC, SERVICE_FOTA_MODULE_NAME);
#else
    return LITE_malloc(size);
#endif
}

void service_ota_lite_free_func(void* ptr)
{
    return LITE_free_internal(ptr);
}

static const fota_t _service_ota_class = {
    sizeof(service_ota_t),
    "fota_cls",
    service_ota_ctor,
    service_ota_dtor,
    service_ota_start,
    service_ota_write,
    service_ota_end,
    service_ota_perform_ota_service,
    service_ota_install_callback_function,
};

const void* get_service_ota_class()
{
    return &_service_ota_class;
}
