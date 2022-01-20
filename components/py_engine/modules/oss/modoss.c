#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if MICROPY_PY_OSS

#include "oss_app.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"
#include "utility.h"

#define LOG_TAG "MOD_OSS"

STATIC mp_obj_t obj_uploadFile(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    char *url = NULL;
    if (n_args < 5) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    char *key = (char *)mp_obj_str_get_str(args[0]);
    char *secret = (char *)mp_obj_str_get_str(args[1]);
    char *endPoint = (char *)mp_obj_str_get_str(args[2]);
    char *bucketName = (char *)mp_obj_str_get_str(args[3]);
    char *filePath = (char *)mp_obj_str_get_str(args[4]);
    LOGD(LOG_TAG, "key = %s;\n", key);
    LOGD(LOG_TAG, "secret = %s;\n", secret);
    LOGD(LOG_TAG, "endPoint = %s;\n", endPoint);
    LOGD(LOG_TAG, "bucketName = %s;\n", bucketName);
    LOGD(LOG_TAG, "filePath = %s;\n", filePath);
    MP_THREAD_GIL_EXIT();
    url = oss_upload_local_file(key, secret, endPoint, bucketName, filePath);
    MP_THREAD_GIL_ENTER();
    if (url)
        return mp_obj_new_strn(url);
    else
        return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(oss_obj_uploadFile, 5, obj_uploadFile);

STATIC mp_obj_t obj_uploadContent(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    char *url = NULL;
    if (n_args < 6) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    char *key = (char *)mp_obj_str_get_str(args[0]);
    char *secret = (char *)mp_obj_str_get_str(args[1]);
    char *endPoint = (char *)mp_obj_str_get_str(args[2]);
    char *bucketName = (char *)mp_obj_str_get_str(args[3]);
    char *fileContent = NULL;
    int32_t contentLen = 0;
    if ((mp_obj_get_type(args[4]) == &mp_type_bytes) || mp_obj_get_type(args[4]) == &mp_type_bytearray) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[4], &bufinfo, MP_BUFFER_READ);
        fileContent = bufinfo.buf;
        contentLen = bufinfo.len;
    } else if (mp_obj_get_type(args[4]) == &mp_type_str) {
        fileContent = (char *)mp_obj_str_get_str(args[4]);
        contentLen = strlen(fileContent);
    }
    char *ossFilePath = (char *)mp_obj_str_get_str(args[5]);
    LOGD(LOG_TAG, "key = %s;\n", key);
    LOGD(LOG_TAG, "secret = %s;\n", secret);
    LOGD(LOG_TAG, "endPoint = %s;\n", endPoint);
    LOGD(LOG_TAG, "bucketName = %s;\n", bucketName);
    LOGD(LOG_TAG, "fileContent = %s;\n", fileContent);
    LOGD(LOG_TAG, "ossFilePath = %s;\n", ossFilePath);
    MP_THREAD_GIL_EXIT();
    url = oss_upload_local_content(key, secret, endPoint, bucketName, fileContent, contentLen, ossFilePath);
    MP_THREAD_GIL_ENTER();
    if (url)
        return mp_obj_new_strn(url);
    else
        return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(oss_obj_uploadContent, 6, obj_uploadContent);

STATIC const mp_rom_map_elem_t oss_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_oss) },
    { MP_ROM_QSTR(MP_QSTR_uploadFile), MP_ROM_PTR(&oss_obj_uploadFile) },
    { MP_ROM_QSTR(MP_QSTR_uploadContent), MP_ROM_PTR(&oss_obj_uploadContent) },
};

STATIC MP_DEFINE_CONST_DICT(oss_locals_dict, oss_locals_dict_table);

const mp_obj_module_t oss_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&oss_locals_dict,
};

MP_REGISTER_MODULE(MP_QSTR_oss, oss_module, MICROPY_PY_OSS);

#endif
