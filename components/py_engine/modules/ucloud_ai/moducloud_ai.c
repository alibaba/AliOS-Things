
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if MICROPY_PY_UCLOUD_AI

#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"
#include "ucloud_ai_common.h"
#include "utility.h"

#define LOG_TAG "MOD_UCLOUD_AI"

extern const mp_obj_type_t chatbot_type;

STATIC mp_obj_t obj_get_token_id(size_t n_args, const mp_obj_t *args)
{
    char *obj_key = (char *)mp_obj_str_get_str(args[0]);
    char *obj_secret = (char *)mp_obj_str_get_str(args[1]);
    char *obj_domain = (char *)mp_obj_str_get_str(args[2]);
    char *obj_region_id = (char *)mp_obj_str_get_str(args[3]);
    char buffer[512];
    char *response = NULL;

    LOGD(LOG_TAG, "obj_key = %s;\n", obj_key);
    LOGD(LOG_TAG, "obj_secret = %s;\n", obj_secret);
    LOGD(LOG_TAG, "obj_domain = %s;\n", obj_domain);
    LOGD(LOG_TAG, "obj_region_id = %s;\n", obj_region_id);

    ucloud_ai_set_key_secret(obj_key, obj_secret);
    response = ucloud_ai_get_token_id(obj_domain, obj_region_id);
    if (!response)
        return mp_const_none;

    if (strlen(response) > 512) {
        LOGE(LOG_TAG, "buffer is not enough\n");
        free(response);
        return mp_const_none;
    }

    strcpy(buffer, response);
    free(response);
    return mp_obj_new_strn(buffer);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_get_token_id, 4, obj_get_token_id);

STATIC mp_obj_t obj_compute_md5(mp_obj_t data, size_t size)
{
    char *response = NULL;
    char md5[100];

    if (data == NULL || size <= 0)
        return mp_const_none;

    char *obj_data = (char *)mp_obj_str_get_str(data);
    response = ucloud_ai_compute_md5(data, size);
    if (!response)
        return mp_const_none;

    strcpy(md5, response);
    free(response);
    return mp_obj_new_strn(md5);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_obj_compute_md5, obj_compute_md5);

STATIC mp_obj_t obj_generate_uuid(void)
{
    char *response = NULL;
    char uuid[36];

    response = ucloud_ai_generate_uuid();
    if (!response)
        return mp_const_none;

    strcpy(uuid, response);
    free(response);
    return mp_obj_new_strn(uuid);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_obj_generate_uuid, obj_generate_uuid);

STATIC mp_obj_t obj_url_encode(mp_obj_t src)
{
    char *response = NULL;

    char *obj_src = (char *)mp_obj_str_get_str(src);
    response = ucloud_ai_url_encode(obj_src);
    if (!response)
        return mp_const_none;

    mp_obj_t obj_encoded = mp_obj_new_strn(response);
    return obj_encoded;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_obj_url_encode, obj_url_encode);

STATIC mp_obj_t obj_url_decode(mp_obj_t src)
{
    char *response = NULL;

    char *obj_src = (char *)mp_obj_str_get_str(src);
    response = ucloud_ai_url_decode(obj_src);
    if (!response)
        return mp_const_none;

    mp_obj_t obj_decoded = mp_obj_new_strn(response);
    return obj_decoded;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_obj_url_decode, obj_url_decode);

// this is the actual C-structure for our new object
STATIC const mp_rom_map_elem_t ucloud_ai_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ucloud_ai) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getTokenId), MP_ROM_PTR(&mp_obj_get_token_id) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_computeMD5), MP_ROM_PTR(&mp_obj_compute_md5) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_generateUUID), MP_ROM_PTR(&mp_obj_generate_uuid) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_urlEncode), MP_ROM_PTR(&mp_obj_url_encode) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_urlDecode), MP_ROM_PTR(&mp_obj_url_decode) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ChatBot), MP_ROM_PTR(&chatbot_type) },
};

STATIC MP_DEFINE_CONST_DICT(ucloud_ai_locals_dict, ucloud_ai_locals_dict_table);

const mp_obj_module_t ucloud_ai_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&ucloud_ai_locals_dict,
};

MP_REGISTER_MODULE(MP_QSTR_ucloud_ai, ucloud_ai_module, MICROPY_PY_UCLOUD_AI);

#endif
