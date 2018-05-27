#ifndef CM_ABSTRACT_H
#define CM_ABSTRACT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdlib.h>
#include <stdarg.h>

#include "iot_import.h"
#include "iot_export_cm.h"
#include "iot_export_dm.h"

#include "message_info_abstract.h"

typedef struct {
    size_t size;
    const char*  _class_name;
    void* (*ctor)(void* _self, va_list* params);
    void* (*dtor)(void* _self);
    int   (*init)(void* _self, const char* product_key, const char* device_name, const char* device_secret,
                  const char* device_id, iotx_cm_event_handle_fp_t event_cb, void* pcontext, dm_cloud_domain_type_t domain_type);
    int   (*deinit)(void* _self);
    int   (*regist)(void* _self, char* uri, iotx_cm_register_fp_t register_cb, void* pcontext);
    int   (*unregist)(void* _self, char* uri);
    int   (*send)(void* _self, message_info_t** msg);
#ifdef LOCAL_CONN_ENABLE
    int   (*add_service)(void* _self, char* uri, iotx_cm_register_fp_t register_cb, void* pcontext, int auth);
#endif
#ifndef CM_SUPPORT_MULTI_THREAD
    int   (*yield)(void* _self, int timeout_ms);
#endif /* CM_SUPPORT_MULTI_THREAD */
} cm_abstract_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CM_ABSTRACT_H */
