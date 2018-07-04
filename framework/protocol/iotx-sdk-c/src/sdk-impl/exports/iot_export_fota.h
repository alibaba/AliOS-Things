#ifndef FOTA_EXPORT_H
#define FOTA_EXPORT_H

#include <stdarg.h>
#include <stdlib.h>

#define SERVICE_FOTA_MODULE_NAME "service_fota"

#define SERVICE_FOTA_CLASS get_service_ota_class()

typedef enum {
    service_fota_callback_type_new_version_detected = 10,

    service_fota_callback_type_number,
} service_fota_callback_type_t;

typedef void (*handle_service_fota_callback_fp_t)(service_fota_callback_type_t callback_type, const char* version);

typedef struct {
    size_t size;
    char*  _class_name;
    void* (*ctor)(void* _self, va_list* params);
    void* (*dtor)(void* _self);
    void  (*start)(void* _self);
    int   (*write)(void* _self, void* data, int data_length);
    int   (*end)(void* _self);
    int   (*perform_ota_service)(void* _self, void* _data_buf, int _data_buf_length);
    void  (*install_callback_function)(void* _self, handle_service_fota_callback_fp_t linkkit_callback_fp);
} fota_t;

void* service_ota_lite_malloc(size_t size);
void* service_ota_lite_calloc(size_t nmemb, size_t size);
void  service_ota_lite_free_func(void* ptr);

#define service_ota_lite_free(ptr)              \
    do { \
        if(!ptr) { \
            log_err("service_ota_lite_free(%p) aborted.", ptr); \
            break; \
        } \
        \
        LITE_free_internal((void *)ptr); \
        ptr = NULL; \
    } while(0)

typedef struct {
    const void* _;
    void*       _data_buf;
    char*       _version;
    int         _data_buf_length;
    int         _total_len;
    char*       _ota_version;
    void*       _linkkit_callback_fp;
    char*       _current_verison;
    int         _ota_inited;
    int         _destructing;
} service_ota_t;

extern const void* get_service_ota_class();


#endif /* FOTA_EXPORT_H */
