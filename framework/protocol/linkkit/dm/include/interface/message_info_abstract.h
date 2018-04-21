#ifndef MESSAGE_INFO_ABSTRACT_H
#define MESSAGE_INFO_ABSTRACT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdarg.h>
#include <stdlib.h>

typedef struct {
    size_t size;
    const char*  _class_name;
    void* (*ctor)(void* _self, va_list* params);
    void* (*dtor)(void* _self);
    int   (*set_uri)(void* _self, char* uri);  /* malloc mem and copy uri. */
    void  (*set_payload)(void* _self, char* playload_buf, unsigned int payload_len);  /* malloc mem and copy payload. */
    void  (*clear)(void* _self);
    void* (*get_uri)(void* _self);
    void* (*get_payload)(void* _self);
    void  (*set_id)(void* _self, int id);
    int   (*get_id)(void* _self);
    void  (*set_version)(void* _self, const char* version);
    char* (*get_version)(void* _self);
    void  (*add_params_data_item)(void* _self, const char* key, const char* value);
    void  (*set_method)(void* _self, const char* method);
    char* (*get_method)(void* _self);
    int   (*serialize_to_payload_request)(void* _self);
    int   (*serialize_to_payload_response)(void* _self);
    void  (*set_message_type)(void* _self, int request);
    int   (*get_message_type)(void* _self);
    char* (*get_params_data)(void* _self);
    void  (*set_params_data)(void* _self, char* params_data_buf);  /* malloc mem and copy payload. */
    int   (*set_raw_data_and_length)(void* _self, void* raw_data, int raw_data_length);  /* malloc mem and copy payload. */
    void* (*get_raw_data)(void* _self);
    int   (*get_raw_data_length)(void* _self);
    char* (*get_product_key)(void* _self);
    void  (*set_product_key)(void* _self, char* product_key);  /* malloc mem and copy payload. */
    char* (*get_device_name)(void* _self);
    void  (*set_device_name)(void* _self, char* device_name);  /* malloc mem and copy payload. */
    void  (*set_code)(void* _self, int code);
    int   (*get_code)(void* _self);
#ifdef LOCAL_CONN_ENABLE
    void* (*get_conn_ctx)(void* _self);
    void  (*set_conn_ctx)(void* _self, void* _conn_ctx);
#endif
} message_info_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MESSAGE_INFO_ABSTRACT_H */
