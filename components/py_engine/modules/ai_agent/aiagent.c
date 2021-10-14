#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aiagent_engine.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

#define LOG_TAG "MOD_AIAGENT"

extern const mp_obj_type_t aiagent_type;

#define AIAGENT_CHECK_PARAMS()                                     \
    aiagent_obj_t *self = (aiagent_obj_t *)MP_OBJ_TO_PTR(self_in); \
    do {                                                           \
        if (self == NULL || self->engine_obj == NULL) {            \
            mp_raise_OSError(EINVAL);                              \
            return mp_const_none;                                  \
        }                                                          \
    } while (0)

// this is the actual C-structure for our new object
typedef struct {
    // base represents some basic information, like type
    mp_obj_base_t base;

    // a member created by us
    char *modName;
    char *engine_name;
    aiagent_engine_t *engine_obj;
    mp_obj_t callback;
} aiagent_obj_t;

static aiagent_obj_t *aiagent_obj = NULL;

STATIC mp_obj_t aiagent_new(const mp_obj_type_t *type, size_t n_args,
                            size_t n_kw, const mp_obj_t *args)
{
    aiagent_obj = m_new_obj(aiagent_obj_t);
    if (!aiagent_obj) {
        mp_raise_OSError(ENOMEM);
        return mp_const_none;
    }

    memset(aiagent_obj, 0, sizeof(aiagent_obj));

    aiagent_obj->base.type = &aiagent_type;
    aiagent_obj->modName = "aiagent";
    aiagent_obj->engine_name = (char *)mp_obj_str_get_str(args[0]);
    printf("aiagent_obj->engine_name: %s\n", aiagent_obj->engine_name);
    aiagent_obj->engine_obj = NULL;

    return MP_OBJ_FROM_PTR(aiagent_obj);
}

void aiagent_print(const mp_print_t *print, mp_obj_t self_in,
                   mp_print_kind_t kind)
{
    aiagent_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "modName(%s)", self->modName);
}

STATIC mp_obj_t aiagent_init(mp_obj_t self_in)
{
    aiagent_obj_t *self = (aiagent_obj_t *)MP_OBJ_TO_PTR(self_in);
    if (self == NULL) {
        LOGE(LOG_TAG, "aiagent_obj_t NULL");
        return mp_const_none;
    }

    if (self->engine_obj == NULL) {
        self->engine_obj = aiagent_engine_init(self->engine_name);
        if (self->engine_obj == NULL) {
            LOGE(LOG_TAG, "create aiagent engine failed !\n");
        }
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(aiagent_obj_init, aiagent_init);

STATIC mp_obj_t aiagent_uninit(mp_obj_t self_in)
{
    AIAGENT_CHECK_PARAMS();

    int status = -1;
    if (self->engine_obj != NULL) {
        aiagent_engine_uninit();
        self->engine_obj = NULL;
    }
    self->callback = NULL;
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(aiagent_obj_uninit, aiagent_uninit);

STATIC mp_obj_t aiagent_config(mp_obj_t self_in, const mp_obj_t channel_in)
{
    int32_t ret;

    AIAGENT_CHECK_PARAMS();

    ai_config_t config;
    config.channel = mp_obj_get_int(channel_in);
    self->engine_obj->config = (ai_config_t *)&config;
    self->engine_obj->ai_engine_config(self->engine_obj);

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(aiagent_obj_config, aiagent_config);

STATIC int aiagent_python_cb(ai_result_t *result)
{
    /*TODO, need to optimize for callback*/
    if (aiagent_obj && (aiagent_obj->callback != mp_const_none)) {
        callback_to_python(aiagent_obj->callback, mp_const_none);
    }
    return 0;
}

STATIC mp_obj_t aiagent_infer(size_t n_args, const mp_obj_t *args)
{
    int32_t ret;

    if (n_args < 4) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__,
             n_args);
        return mp_const_none;
    }

    mp_obj_t self_in = args[0];
    AIAGENT_CHECK_PARAMS();
    self->callback =
        (mp_obj_get_type(args[3]) == &mp_type_NoneType) ? NULL : args[3];
    self->engine_obj->src1 =
        mp_obj_is_str(args[1]) ? (char *)mp_obj_str_get_str(args[1]) : NULL;
    self->engine_obj->src1 =
        mp_obj_is_str(args[2]) ? (char *)mp_obj_str_get_str(args[2]) : NULL;
    self->engine_obj->callback = aiagent_python_cb;

    ret = self->engine_obj->ai_engine_model_infer(self->engine_obj);

    return mp_obj_new_int(ret);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(aiagent_obj_model_infer, 4, aiagent_infer);

STATIC const mp_rom_map_elem_t aiagent_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&aiagent_obj_init) },
    { MP_ROM_QSTR(MP_QSTR_uninit), MP_ROM_PTR(&aiagent_obj_uninit) },
    { MP_ROM_QSTR(MP_QSTR_infer), MP_ROM_PTR(&aiagent_obj_model_infer) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&aiagent_obj_config) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&aiagent_obj_uninit) },
};

STATIC MP_DEFINE_CONST_DICT(aiagent_locals_dict, aiagent_locals_dict_table);

const mp_obj_type_t aiagent_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_AIAgent,
    .print = aiagent_print,
    .make_new = aiagent_new,
    .locals_dict = (mp_obj_dict_t *)&aiagent_locals_dict,
};
