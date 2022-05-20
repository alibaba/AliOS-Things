#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if MICROPY_PY_OSS_HTTP

#include "aliot_httpapi_oss.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

#define LOG_TAG "OSS_HTTP"

STATIC mp_obj_t oss_http_uploadFile(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;
    char buf[2048] = { 0 };

    char *key = (char *)mp_obj_str_get_str(args[0]);
    char *secret = (char *)mp_obj_str_get_str(args[1]);
    char *endPoint = (char *)mp_obj_str_get_str(args[2]);
    char *bucketName = (char *)mp_obj_str_get_str(args[3]);
    char *objname = (char *)mp_obj_str_get_str(args[4]);
    char *filePath = (char *)mp_obj_str_get_str(args[5]);

    ret = oss_http_put_object(key, secret, endPoint, bucketName, objname, filePath, buf, sizeof(buf));
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(oss_http_uploadFile_obj, 6, 6, oss_http_uploadFile);

STATIC mp_obj_t oss_http_uploadContent(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;
    char buf[2048] = { 0 };

    char *key = (char *)mp_obj_str_get_str(args[0]);
    char *secret = (char *)mp_obj_str_get_str(args[1]);
    char *endPoint = (char *)mp_obj_str_get_str(args[2]);
    char *bucketName = (char *)mp_obj_str_get_str(args[3]);
    char *objname = (char *)mp_obj_str_get_str(args[4]);
    char *contentData = (char *)mp_obj_str_get_str(args[5]);

    ret = oss_http_put_content(key, secret, endPoint, bucketName, objname, contentData, buf, sizeof(buf));
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(oss_http_uploadContent_obj, 6, 8, oss_http_uploadContent);

STATIC mp_obj_t oss_http_downloadFile(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;
    char *range = NULL;
    char buf[2048] = { 0 };

    char *key = (char *)mp_obj_str_get_str(args[0]);
    char *secret = (char *)mp_obj_str_get_str(args[1]);
    char *endPoint = (char *)mp_obj_str_get_str(args[2]);
    char *bucketName = (char *)mp_obj_str_get_str(args[3]);
    char *objname = (char *)mp_obj_str_get_str(args[4]);
    char *filePath = (char *)mp_obj_str_get_str(args[5]);
    if (n_args == 6) {
        range = NULL;
    } else if (n_args == 7) {
        range = (char *)mp_obj_str_get_str(args[6]);
    }

    ret = oss_http_get_object(key, secret, endPoint, bucketName, objname, filePath, range, buf, sizeof(buf));
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(oss_http_downloadFile_obj, 6, 7, oss_http_downloadFile);

STATIC const mp_rom_map_elem_t oss_http_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_OSS) },
    { MP_ROM_QSTR(MP_QSTR_uploadFile), MP_ROM_PTR(&oss_http_uploadFile_obj) },
    { MP_ROM_QSTR(MP_QSTR_uploadContent), MP_ROM_PTR(&oss_http_uploadContent_obj) },
    { MP_ROM_QSTR(MP_QSTR_downloadFile), MP_ROM_PTR(&oss_http_downloadFile_obj) },
};

STATIC MP_DEFINE_CONST_DICT(oss_http_locals_dict, oss_http_locals_dict_table);

const mp_obj_module_t oss_http_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&oss_http_locals_dict,
};

MP_REGISTER_MODULE(MP_QSTR_oss, oss_http_module, MICROPY_PY_OSS_HTTP);

#endif
