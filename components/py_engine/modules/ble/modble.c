#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if MICROPY_PY_BLE

#include "modble.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "ulog/ulog.h"

#define LOG_TAG "ble"

STATIC mp_obj_t cb_ble_event[BLE_EVENT_MAX] = { 0 };

void native_bt_host_gatts_handle_write(uint8_t *data, size_t len)
{
    mp_obj_t cb_chara_written = cb_ble_event[BLE_EVENT_CHARA_WRITTEN];
    if (cb_chara_written != mp_const_none && mp_obj_is_callable(cb_chara_written)) {
        mp_obj_t data_bytes = mp_obj_new_bytes(data, len);
        callback_to_python(cb_chara_written, data_bytes);
    }
}

void py_ble_event_notify(int16_t handle, int32_t state)
{
    mp_obj_t cb_net_status = cb_ble_event[BLE_EVENT_NET_STATUS];
    if (cb_net_status != mp_const_none && mp_obj_is_callable(cb_net_status)) {
        mp_obj_t data_dict = mp_obj_new_dict(2);
        mp_obj_dict_store(data_dict, MP_ROM_QSTR(qstr_from_str("handle")), mp_obj_new_int(handle));
        mp_obj_dict_store(data_dict, MP_ROM_QSTR(qstr_from_str("state")), mp_obj_new_int(state));

        callback_to_python(cb_net_status, data_dict);
    }
}

/**************************************************/
/************* For ble host operation *************/
/**************************************************/
STATIC mp_obj_t ble_open(mp_obj_t device_name_in, mp_obj_t conn_num_max_in)
{
    amp_bt_host_adapter_init_t init = { 0 };
    init.dev_name = mp_obj_str_get_str(device_name_in);
    init.conn_num_max = mp_obj_get_int(conn_num_max_in);

    mp_int_t ret = py_bt_host_adapter_init(&init);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
MP_DEFINE_CONST_FUN_OBJ_2(ble_open_obj, ble_open);

STATIC mp_obj_t ble_startAdv(mp_obj_t *data)
{
    if (!mp_obj_is_dict_or_ordereddict(data)) {
        LOGE(LOG_TAG, "%s param type must be dict \r\n", __func__);
        return mp_obj_new_int(MP_EINVAL);
    }
    amp_bt_host_adapter_adv_start_t adv_param = { 0 };

    mp_obj_t index = mp_obj_new_str_via_qstr("type", strlen("type"));
    mp_obj_t value = mp_obj_dict_get(data, index);
    if (mp_obj_is_int(value)) {
        adv_param.type = mp_obj_get_int(value);
    } else {
        LOGE(LOG_TAG, "%s   key type value must be int \r\n", __func__);
        return mp_obj_new_int(MP_EINVAL);
    }

    index = mp_obj_new_str_via_qstr("adv_data", strlen("adv_data"));
    value = mp_obj_dict_get(data, index);
    if (mp_obj_is_str(value)) {
        adv_param.adv_data = mp_obj_str_get_str(value);
    } else {
        LOGE(LOG_TAG, "%s   key adv_data value must be str \r\n", __func__);
        return mp_obj_new_int(MP_EINVAL);
    }

    index = mp_obj_new_str_via_qstr("scan_rsp_data", strlen("scan_rsp_data"));
    value = mp_obj_dict_get(data, index);
    if (mp_obj_is_str(value)) {
        adv_param.scan_rsp_data = mp_obj_str_get_str(value);
    } else {
        LOGE(LOG_TAG, "%s   key scan_rsp_data value must be str \r\n", __func__);
        return mp_obj_new_int(MP_EINVAL);
    }

    index = mp_obj_new_str_via_qstr("interval_min", strlen("interval_min"));
    value = mp_obj_dict_get(data, index);
    if (mp_obj_is_int(value)) {
        adv_param.interval_min = mp_obj_get_int(value);
    } else {
        LOGE(LOG_TAG, "%s   key interval_min value must be int \r\n", __func__);
        return mp_obj_new_int(MP_EINVAL);
    }

    index = mp_obj_new_str_via_qstr("interval_max", strlen("interval_max"));
    value = mp_obj_dict_get(data, index);
    if (mp_obj_is_int(value)) {
        adv_param.interval_max = mp_obj_get_int(value);
    } else {
        LOGE(LOG_TAG, "%s   key interval_max value must be int \r\n", __func__);
        return mp_obj_new_int(MP_EINVAL);
    }

    index = mp_obj_new_str_via_qstr("channel_map", strlen("channel_map"));
    value = mp_obj_dict_get(data, index);
    if (mp_obj_is_int(value)) {
        adv_param.channel_map = mp_obj_get_int(value);
    } else {
        LOGE(LOG_TAG, "%s   key channel_map value must be int \r\n", __func__);
        return mp_obj_new_int(MP_EINVAL);
    }

    mp_int_t ret = bt_host_adapter_start_adv(&adv_param);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ble_startAdv_obj, ble_startAdv);

STATIC mp_obj_t ble_stopAdv(void)
{
    mp_int_t ret = ble_stack_adv_stop();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ble_stopAdv_obj, ble_stopAdv);

STATIC mp_obj_t ble_addService(mp_obj_t srv_in)
{
    mp_int_t ret = -1;
    amp_bt_host_adapter_gatts_srvc_t service = { 0 };

    mp_obj_t s_uuid_obj = mp_obj_dict_get(srv_in, mp_obj_new_strn("uuid"));
    service.s_uuid = mp_obj_str_get_str(s_uuid_obj);
    service.attr_cnt += 1;

    mp_obj_t chara_list_obj = mp_obj_dict_get(srv_in, mp_obj_new_strn("charas"));

    size_t chara_num;
    mp_obj_t *chara_items = NULL;
    mp_obj_list_get(chara_list_obj, &chara_num, &chara_items);

    /* if we have service to add */
    if (chara_num != 0) {
        amp_bt_host_adapter_gatt_chars_t *chara_list = m_new0(amp_bt_host_adapter_gatt_chars_t, chara_num);
        if (chara_list == NULL) {
            mp_raise_OSError(MP_ENOMEM);
            return mp_const_none;
        } else {
            service.chars = chara_list;
        }

        /* parser chara_list */
        for (size_t i = 0; i < chara_num; i++) {
            /* parser each chara item */
            size_t chara_items_num = 0;
            mp_obj_t *chara_items_eles = NULL;
            mp_obj_list_get(chara_items[i], &chara_items_num, &chara_items_eles);

            bool has_descr = false;
            if (chara_items_num != 0) {
                chara_list->char_uuid = mp_obj_str_get_str(chara_items_eles[0]);
                chara_list->permission = mp_obj_str_get_str(chara_items_eles[1]);

                if (mp_obj_is_str(chara_items_eles[2])) {
                    chara_list->descr_uuid = mp_obj_str_get_str(chara_items_eles[2]);
                    has_descr = true;
                } else {
                    chara_list->descr_uuid = NULL;
                }

                if (mp_obj_is_str(chara_items_eles[3])) {
                    chara_list->descr_type = mp_obj_str_get_str(chara_items_eles[3]);
                    has_descr = true;
                } else {
                    chara_list->descr_type = NULL;
                }

                if (has_descr == true) {
                    service.attr_cnt += 3;
                } else {
                    service.attr_cnt += 2;
                }

                chara_list += 1;
            } else {
                mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Charas Null"));
            }
        }

        ret = bt_gatts_adapter_add_service(&service);
        m_del(amp_bt_host_adapter_gatt_chars_t, service.chars, chara_num);
    }

    return MP_OBJ_NEW_SMALL_INT(ret);
}
MP_DEFINE_CONST_FUN_OBJ_1(ble_addService_obj, ble_addService);

STATIC mp_obj_t ble_updateCharacter(mp_obj_t uuid_in, mp_obj_t data_in)
{
    const char *uuid = mp_obj_str_get_str(uuid_in);

    mp_buffer_info_t data;
    mp_get_buffer_raise(data_in, &data, MP_BUFFER_READ);

    mp_int_t ret = bt_gatts_adapter_update_chars(uuid, data.buf, data.len);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ble_updateCharacter_obj, ble_updateCharacter);

STATIC mp_obj_t ble_on(mp_obj_t event_in, mp_obj_t cb)
{
    mp_int_t event = mp_obj_get_int(event_in);
    if (event >= BLE_EVENT_MAX) {
        return mp_obj_new_int(MP_EINVAL);
    }
    cb_ble_event[event] = cb;
    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ble_on_obj, ble_on);

STATIC const mp_rom_map_elem_t ble_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ble) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&ble_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_startAdv), MP_ROM_PTR(&ble_startAdv_obj) },
    { MP_ROM_QSTR(MP_QSTR_stopAdv), MP_ROM_PTR(&ble_stopAdv_obj) },
    { MP_ROM_QSTR(MP_QSTR_addService), MP_ROM_PTR(&ble_addService_obj) },
    { MP_ROM_QSTR(MP_QSTR_updateCharacter), MP_ROM_PTR(&ble_updateCharacter_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&ble_on_obj) },

    { MP_ROM_QSTR(MP_QSTR_STATUS), MP_ROM_INT(BLE_EVENT_NET_STATUS) },
    { MP_ROM_QSTR(MP_QSTR_CHARAWRITE), MP_ROM_INT(BLE_EVENT_CHARA_WRITTEN) },
};
STATIC MP_DEFINE_CONST_DICT(ble_module_globals, ble_module_globals_table);

const mp_obj_module_t mp_module_ble = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&ble_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ble, mp_module_ble, MICROPY_PY_BLE);

#endif
