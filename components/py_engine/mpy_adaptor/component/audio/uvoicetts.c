#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HaasLog.h"
#include "k_api.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "uvoice_alios.h"
#include "uvoice_event.h"
#include "uvoice_types.h"
#include "uvoice_init.h"
#include "uvoice_tts.h"
#include "utility.h"

#define TTS_CHECK_PARAMS()                                               \
    uvocie_tts_obj_t *self = (uvocie_tts_obj_t *)MP_OBJ_TO_PTR(self_in); \
    do                                                                   \
    {                                                                    \
        if (self == NULL || self->tts_obj == NULL)                       \
        {                                                                \
            mp_raise_OSError(EINVAL);                                    \
            return mp_const_none;                                        \
        }                                                                \
    } while (0)

#define TTS_RECV_CB_URL     (0)
#define TTS_RECV_CB_DATA    (1)
#define TTS_RECV_CB_EVENT   (2)

extern const mp_obj_type_t uvoice_tts_type;
extern const char *get_str_from_dict(mp_obj_t dict, const char *key);
extern int get_int_from_dict(mp_obj_t dict, const char *key);

static mp_obj_t callback_url = mp_const_none;
static mp_obj_t callback_data = mp_const_none;
static mp_obj_t callback_event = mp_const_none;

// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t base;

    // a member created by us
    char *modName;
    uvoice_tts_t *tts_obj;
} uvocie_tts_obj_t;

void uvoice_tts_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    uvocie_tts_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "modName(%s)", self->modName);
}

STATIC mp_obj_t uvoice_tts_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    uvocie_tts_obj_t *uvocie_tts_com_obj = m_new_obj(uvocie_tts_obj_t);
    if (!uvocie_tts_com_obj) {
        mp_raise_OSError(ENOMEM);
        return mp_const_none;
    }
    memset(uvocie_tts_com_obj, 0, sizeof(uvocie_tts_obj_t));

    uvocie_tts_com_obj->base.type = &uvoice_tts_type;
    uvocie_tts_com_obj->modName = "uvoice-tts";

    return MP_OBJ_FROM_PTR(uvocie_tts_com_obj);
}

STATIC mp_int_t alicoud_tts_event(tts_event_e event, char *info)
{
    if (callback_event != mp_const_none) {
        mp_obj_t event_dict = mp_obj_new_dict(2);
        mp_obj_dict_store(event_dict, MP_OBJ_NEW_QSTR(qstr_from_str("event")), mp_obj_new_int(event));
        mp_obj_dict_store(event_dict, MP_OBJ_NEW_QSTR(qstr_from_str("info")), mp_obj_new_strn(info));

        callback_to_python(callback_event, event_dict);
    }
}

STATIC mp_int_t alicloud_tts_recv_data(uint8_t *buffer, mp_int_t nbytes, mp_int_t index)
{
    if (callback_data != mp_const_none) {
        mp_obj_t data_dict = mp_obj_new_dict(3);
        mp_obj_dict_store(data_dict, MP_OBJ_NEW_QSTR(qstr_from_str("buffer")), mp_obj_new_bytes(buffer, nbytes));
        mp_obj_dict_store(data_dict, MP_OBJ_NEW_QSTR(qstr_from_str("nbytes")), mp_obj_new_int(nbytes));
        mp_obj_dict_store(data_dict, MP_OBJ_NEW_QSTR(qstr_from_str("index")), mp_obj_new_int(index));

        callback_to_python(callback_data, data_dict);
    }
}

STATIC mp_int_t alicloud_tts_recv_url(char *tts_url)
{
    if (callback_url != mp_const_none) {
        callback_to_python(callback_url, mp_obj_new_str(tts_url, strlen(tts_url)));
    }
}

STATIC mp_obj_t uvoice_tts_init(mp_obj_t self_in, mp_obj_t aicloud_type_in, mp_obj_t config_in)
{
    TTS_CHECK_PARAMS();

    tts_aicloud_type_e aicloud_type = (tts_aicloud_type_e)mp_obj_get_int(aicloud_type_in);

    tts_config_t config = {0};
    config.app_key = (char*)get_str_from_dict(config_in, "app_key");
    config.token = (char *)get_str_from_dict(config_in, "token");
    config.format = (media_format_t)get_int_from_dict(config_in, "format");
    config.sample_rate = get_int_from_dict(config_in, "sample_rate");
    config.voice = (char *)get_str_from_dict(config_in, "voice");
    config.volume = get_int_from_dict(config_in, "volume");
    config.speech_rate = get_int_from_dict(config_in, "speech_rate");
    config.pitch_rate = get_int_from_dict(config_in, "pitch_rate");
    config.text_encode_type = (tts_encode_type_e)get_int_from_dict(config_in, "text_encode_type");

    LOG_D("app_key=%s, token=%s, format=%d, sample_rate=%d, voice=%s, volume=%d, speech_rate=%d, pitch_rate=%d, text_encode_type=%d\n",
          config.app_key, config.token, config.format, config.sample_rate, config.voice,
          config.volume, config.speech_rate, config.pitch_rate, config.text_encode_type);

    mp_int_t status = self->tts_obj->tts_init(aicloud_type, &config);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(uvoice_tts_init_obj, uvoice_tts_init);

static tts_recv_callback_t recv_cb = {
    .event = alicoud_tts_event,
    .recv_data = alicloud_tts_recv_data,
    .recv_url = alicloud_tts_recv_url,
};

STATIC mp_obj_t uvoice_tts_request(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 3) {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    mp_obj_t self_in = args[0];
    TTS_CHECK_PARAMS();

    char *text = (char *)mp_obj_str_get_str(args[1]);
    int recv_type = mp_obj_get_int(args[2]);

    LOG_D("text=%s, recv_type=%d", text, recv_type);

    mp_int_t status = self->tts_obj->tts_request(text, recv_type, &recv_cb);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(uvoice_tts_request_obj, 3, uvoice_tts_request);

STATIC mp_obj_t uvoice_tts_stop(mp_obj_t self_in)
{
    TTS_CHECK_PARAMS();

    mp_int_t status = self->tts_obj->tts_stop();
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_tts_stop_obj, uvoice_tts_stop);

STATIC mp_obj_t uvoice_create(mp_obj_t self_in)
{
    uvocie_tts_obj_t *self = (uvocie_tts_obj_t *)MP_OBJ_TO_PTR(self_in);
    if (self == NULL) {
        LOG_E("uvocie_tts_obj_t NULL");
        return mp_const_none;
    }

    self->tts_obj = uvoice_tts_create();
    if (self->tts_obj == NULL) {
        LOG_E("create tts failed !\n");
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_tts_create_obj, uvoice_create);

STATIC mp_obj_t uvoice_release(mp_obj_t self_in)
{
    TTS_CHECK_PARAMS();
    mp_int_t status = uvoice_tts_release(self->tts_obj);

    callback_url = mp_const_none;
    callback_data = mp_const_none;
    callback_event = mp_const_none;

    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_tts_release_obj, uvoice_release);

STATIC mp_obj_t uvoice_tts_set_callback(mp_obj_t self_in, mp_obj_t type_in, mp_obj_t callback_fun)
{
    TTS_CHECK_PARAMS();

    if(mp_obj_is_fun(callback_fun) == false) {
        LOG_E("Obj is not function\n");
        return mp_const_none;
    }

    int type = mp_obj_get_int(type_in);
    if (type == TTS_RECV_CB_URL) {
        callback_url = callback_fun;
    } else if (type == TTS_RECV_CB_DATA) {
        callback_data = callback_fun;
    } else if (type == TTS_RECV_CB_EVENT) {
        callback_event = callback_fun;
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(uvoice_tts_set_callback_obj, uvoice_tts_set_callback);


STATIC mp_obj_t uvoice_tts_cb_test(mp_obj_t self_in, mp_obj_t type_in)
{
    TTS_CHECK_PARAMS();

    int type = mp_obj_get_int(type_in);
    LOG_D("type=%d", type);

    if (type == TTS_RECV_CB_URL)
    {
        alicloud_tts_recv_url("test_url");
    }
    else if (type == TTS_RECV_CB_DATA)
    {
        char buf[8] = {0,1,2,3,4,5,6,7};
        alicloud_tts_recv_data(buf, 8, 10);
    }
    else if (type == TTS_RECV_CB_EVENT)
    {
        alicoud_tts_event(10, "test_info");
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_tts_cb_test_obj, uvoice_tts_cb_test);

STATIC const mp_rom_map_elem_t uvoice_module_tts_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&uvoice_tts_stop_obj)},
    {MP_ROM_QSTR(MP_QSTR_create), MP_ROM_PTR(&uvoice_tts_create_obj)},
    {MP_ROM_QSTR(MP_QSTR_release), MP_ROM_PTR(&uvoice_tts_release_obj)},
    {MP_ROM_QSTR(MP_QSTR_callback), MP_ROM_PTR(&uvoice_tts_set_callback_obj)},
    {MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&uvoice_tts_init_obj)},
    {MP_ROM_QSTR(MP_QSTR_request), MP_ROM_PTR(&uvoice_tts_request_obj)},
    {MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&uvoice_tts_stop_obj)},
    {MP_ROM_QSTR(MP_QSTR_test), MP_ROM_PTR(&uvoice_tts_cb_test_obj)},

    {MP_ROM_QSTR(MP_QSTR_CB_ENUM_URL), MP_ROM_INT(TTS_RECV_CB_URL)},
    {MP_ROM_QSTR(MP_QSTR_CB_ENUM_DATA), MP_ROM_INT(TTS_RECV_CB_DATA)},
    {MP_ROM_QSTR(MP_QSTR_CB_ENUM_EVENT), MP_ROM_INT(TTS_RECV_CB_EVENT)},
};
STATIC MP_DEFINE_CONST_DICT(uvoice_module_tts_globals, uvoice_module_tts_globals_table);

const mp_obj_type_t uvoice_tts_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_Tts,
    .print = uvoice_tts_print,
    .make_new = uvoice_tts_new,
    .locals_dict = (mp_obj_dict_t *)&uvoice_module_tts_globals,
};
