#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oss_app.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ucloud_ai_chatbot.h"
#include "ulog/ulog.h"

#define LOG_TAG "MOD_CHATBOT"

extern const mp_obj_type_t chatbot_type;

#define CHATBOT_CHECK_PARAMS()                                     \
    chatbot_obj_t *self = (chatbot_obj_t *)MP_OBJ_TO_PTR(self_in); \
    do {                                                           \
        if (self == NULL) {                                        \
            mp_raise_OSError(MP_EINVAL);                              \
            return mp_const_none;                                  \
        }                                                          \
    } while (0)

// this is the actual C-structure for our new object
typedef struct {
    // base represents some basic information, like type
    mp_obj_base_t base;

    // a member created by us
    char *modName;
} chatbot_obj_t;

static chatbot_obj_t *chatbot_obj = NULL;

STATIC mp_obj_t chatbot_new(const mp_obj_type_t *type, size_t n_args,
                            size_t n_kw, const mp_obj_t *args)
{
    chatbot_obj = m_new_obj(chatbot_obj_t);
    if (!chatbot_obj) {
        mp_raise_OSError(MP_ENOMEM);
        return mp_const_none;
    }

    memset(chatbot_obj, 0, sizeof(chatbot_obj));

    chatbot_obj->base.type = &chatbot_type;
    chatbot_obj->modName = "chatbot";

    return MP_OBJ_FROM_PTR(chatbot_obj);
}

void chatbot_print(const mp_print_t *print, mp_obj_t self_in,
                   mp_print_kind_t kind)
{
    chatbot_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "modName(%s)", self->modName);
}

STATIC mp_obj_t obj_input(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    char *response = NULL;
    char buffer[2048];

    if (n_args < 6) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__,
             n_args);
        return mp_const_none;
    }

    mp_obj_t self_in = args[0];
    CHATBOT_CHECK_PARAMS();

    char *key = (char *)mp_obj_str_get_str(args[1]);
    char *secret = (char *)mp_obj_str_get_str(args[2]);
    char *instanceId = (char *)mp_obj_str_get_str(args[3]);
    char *sessionId =
        mp_obj_is_str(args[4]) ? (char *)mp_obj_str_get_str(args[4]) : NULL;
    char *text = (char *)mp_obj_str_get_str(args[5]);
    LOGD(LOG_TAG, "key = %s;\n", key);
    LOGD(LOG_TAG, "secret = %s;\n", secret);
    LOGD(LOG_TAG, "instanceId = %s;\n", instanceId);
    LOGD(LOG_TAG, "sessionId = %s\n", sessionId);
    LOGD(LOG_TAG, "text = %s;\n", text);

    ucloud_ai_set_key_secret(key, secret);
    response = ucloud_ai_chatbot(instanceId, sessionId, text);
    if (!response)
        return mp_const_none;

    if (strlen(response) > 2048) {
        LOGE(LOG_TAG, "buffer is not enough\n");
        free(response);
        return mp_const_none;
    }
    strcpy(buffer, response);
    free(response);
    return mp_obj_new_strn(buffer);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(chatbot_obj_input, 6, obj_input);

STATIC const mp_rom_map_elem_t chatbot_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_input), MP_ROM_PTR(&chatbot_obj_input) },
};

STATIC MP_DEFINE_CONST_DICT(chatbot_locals_dict, chatbot_locals_dict_table);

const mp_obj_type_t chatbot_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_ChatBot,
    .print = chatbot_print,
    .make_new = chatbot_new,
    .locals_dict = (mp_obj_dict_t *)&chatbot_locals_dict,
};
