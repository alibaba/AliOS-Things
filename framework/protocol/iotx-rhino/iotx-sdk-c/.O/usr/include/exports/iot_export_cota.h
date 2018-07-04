/*
 * iot_export_cota.h
 *
 *  Created on: 2018Äê3ÔÂ13ÈÕ
 *      Author: wb-jn347227
 */

#ifndef SRC_SDK_IMPL_EXPORTS_IOT_EXPORT_COTA_H_
#define SRC_SDK_IMPL_EXPORTS_IOT_EXPORT_COTA_H_

#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#define CONFIG_COTA_MODULE_NAME "service_cota"
#define SERVICE_COTA_CLASS get_config_ota_class()

#define config_ota_lite_free(ptr)              \
    do { \
        if(!ptr) { \
            log_err("service_ota_lite_free(%p) aborted.", ptr); \
            break; \
        } \
        \
        LITE_free_internal((void *)ptr); \
        ptr = NULL; \
    } while(0)

typedef enum {
    service_cota_callback_type_new_version_detected = 10,

    service_cota_callback_type_number,
} service_cota_callback_type_t;

typedef void (*handle_service_cota_callback_fp_t)(service_cota_callback_type_t callback_type, const char* configid,
																							  uint32_t  configsize,
																							  const char* gettype,
																							  const char* sign,
																							  const char* signmethod,
																							  const char* cota_url);
void* config_ota_lite_calloc(size_t nmemb, size_t size);
void* config_ota_lite_malloc(size_t size);
void config_ota_lite_free_func(void* ptr);
typedef struct {
    size_t size;
    char*  _class_name;
    void* (*ctor)(void* _self, va_list* params);
    void* (*dtor)(void* _self);
    int   (*get)(void* _self,const char* configScope, const char* getType, const char* attributeKeys, void* option);
    void  (*start)(void* _self);
    int   (*write)(void* _self, void* data, int data_length);
    int   (*end)(void* _self);
    int   (*perform_ota_service)(void* _self, void* _data_buf, int _data_buf_length);
    void  (*install_callback_function)(void* _self, handle_service_cota_callback_fp_t linkkit_callback_fp);
} cota_t;

typedef struct {
    const void* _;
    void*       _data_buf;
    int         _data_buf_length;
    int         _total_len;
    void*       _linkkit_callback_fp;
    char*       _current_verison;
    char*       _req_configscope;
    char*       _req_gettype;
    char*       _req_attributekeys;
    char*       _rsp_configId;
    uint32_t    _rsp_configSize;
    char*       _rsp_sign;
    char*       _rsp_signMethod;
    char*       _rsp_url;
    char*       _rsp_getType;
    int         _ota_inited;
    int         _destructing;
} config_ota_t;

extern const void* get_config_ota_class();
#endif /* SRC_SDK_IMPL_EXPORTS_IOT_EXPORT_COTA_H_ */
