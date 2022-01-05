#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ucamera_service.h"
#include "ulog/ulog.h"
#include "wifi_camera.h"
#include "stdatomic.h"

#define LOG_TAG "MOD_UCAMERA"

#if MICROPY_PY_UCAMERA

// this is the actual C-structure for our new object
typedef struct
{
    cam_dev_type_t cam_dev_type;
} mp_ml_obj_t;

mp_ml_obj_t *ucamera_obj = NULL;

STATIC mp_obj_t obj_init(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 1) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__,
             n_args);
        return mp_obj_new_int(ret);
    }
    ucamera_obj = m_new_obj(mp_ml_obj_t);
    char *camera_type = (char *)mp_obj_str_get_str(args[0]);

    if (!strcmp(camera_type, "wifi")) {
        ucamera_obj->cam_dev_type = CAM_WIFI_TYPE;
        ret = ucamera_service_init(WIFI_CAMERA_NAME);
        if (ret < 0) {
            LOGD(LOG_TAG, "ucamera_service_init failed  %s;\n", __func__);
        }
    } else if (!strcmp(camera_type,  "uart")) {
        uart_cam_params_t params;
        uint32_t rx = (uint32_t)mp_obj_get_int(args[1]);
        uint32_t tx = (uint32_t)mp_obj_get_int(args[2]);

        ucamera_obj->cam_dev_type = CAM_UART_TYPE;
        ret = ucamera_service_init(UART_CAMERA_NAME);
        if (ret < 0) {
            LOGD(LOG_TAG, "ucamera_service_init failed  %s;\n", __func__);
        }
        // set bandrate
        params.bandrate = 1500000;
        params.rx = rx;
        params.tx = tx;
        ret = ucamera_service_config(UCAMERA_CMD_SET_UART_PARAMS, (void *)&params);
        if (ret < 0) {
            LOGD(LOG_TAG, "init failed  %s;\n", __func__);
        }
    } else {
        ret = -1;
        LOGE(LOG_TAG, "unsupport camera type %s yet\n", __func__);
    }

    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ucamera_obj_init, 1, obj_init);

STATIC mp_obj_t obj_connect(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 1) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__,
             n_args);
        return mp_obj_new_int(ret);
    }

    if (ucamera_obj->cam_dev_type != CAM_WIFI_TYPE)
        return mp_obj_new_int(ret);

    char *camera_url = (char *)mp_obj_str_get_str(args[0]);
    if (camera_url != NULL) {
        ret = ucamera_service_connect(camera_url);
    } else {
        ret = -1;
        LOGE(LOG_TAG, "camera_url can not be null %s;\n", __func__);
    }

    if (ret < 0) {
        LOGD(LOG_TAG, "ucamera connect failed  %s;\n", __func__);
    }
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ucamera_obj_connect, 1, obj_connect);

STATIC mp_obj_t obj_disconnect(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;

    if (ucamera_obj->cam_dev_type != CAM_WIFI_TYPE)
        return mp_obj_new_int(ret);

    ret = ucamera_service_disconnect();
    if (ret < 0) {
        LOGD(LOG_TAG, "ucamera config failed  %s;\n", __func__);
    }
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ucamera_obj_disconnect, 0, obj_disconnect);

STATIC mp_obj_t obj_config(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 1) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__,
             n_args);
        return mp_obj_new_int(ret);
    }

    if (ucamera_obj->cam_dev_type != CAM_WIFI_TYPE)
        return mp_obj_new_int(ret);

    char *camera_control_url = (char *)mp_obj_str_get_str(args[0]);
    if (camera_control_url != NULL) {
        ret = ucamera_service_config(UCAMERA_CMD_SET_CONTROL_URL, (void *)camera_control_url);
    } else {
        ret = -1;
        LOGE(LOG_TAG, "camera_control_url can not be null %s;\n", __func__);
    }

    if (ret < 0) {
        LOGD(LOG_TAG, "ucamera config failed  %s;\n", __func__);
    }
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ucamera_obj_config, 1, obj_config);


STATIC mp_obj_t obj_setProp(size_t n_args, const mp_obj_t *args)
{
    int32_t ret = -1;
    frame_size_t frame_size;
    uart_cam_params_t params;
    uint32_t time = 0;

    if (n_args < 2) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__,
             n_args);
        return mp_obj_new_int(ret);
    }

    uint32_t cmd_id = mp_obj_get_int(args[0]);
    switch (cmd_id) {
        case UCAMERA_CMD_SET_FRAME_SIZE:
            frame_size.id = mp_obj_get_int(args[1]);
            ret = ucamera_service_config(UCAMERA_CMD_SET_FRAME_SIZE, (void *)&frame_size);
            break;
        case UCAMERA_CMD_SET_UPLOAD_TIME:
            time = mp_obj_get_int(args[1]);
            ret = ucamera_service_config(UCAMERA_CMD_SET_UPLOAD_TIME, (void *)&time);
            break;
        case UCAMERA_CMD_SET_MODE:
            params.mode = mp_obj_get_int(args[1]);
            ret = ucamera_service_config(UCAMERA_CMD_SET_MODE, (void *)&params);
            break;
        case UCAMERA_CMD_SET_WIFI_SSID_PWD:
            params.wifi_ssid = mp_obj_str_get_str(args[1]);
            params.wifi_pwd = mp_obj_str_get_str(args[1]);
            ret = ucamera_service_config(UCAMERA_CMD_SET_WIFI_SSID_PWD, (void *)&params);
            break;
        case UCAMERA_CMD_SET_LED_BRIGHTNESS:
            params.led_brightness = mp_obj_get_int(args[1]);
            ret = ucamera_service_config(UCAMERA_CMD_SET_LED_BRIGHTNESS, (void *)&params);
            break;
        default:
            break;
    }
    return mp_obj_new_int(ret);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ucamera_obj_setProp, 2, 3, obj_setProp);

STATIC mp_obj_t obj_getProp(size_t n_args, const mp_obj_t *args)
{
    int32_t ret = -1;

    if (n_args < 1) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__,
             n_args);
        return mp_obj_new_int(ret);
    }

    uint32_t cmd_id = mp_obj_get_int(args[0]);
    switch (cmd_id) {
        case UCAMERA_CMD_GET_TOKEN:
            /* code */
            printf("not support yet\n");
            break;

        default:
            break;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ucamera_obj_getProp, 1, obj_getProp);

STATIC mp_obj_t obj_capture(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;

    frame_buffer_t *frame = NULL;
    mp_obj_t *mp_frame = NULL;

    MP_THREAD_GIL_EXIT();
    /*get one camera frame*/
    frame = ucamera_service_get_frame();
    MP_THREAD_GIL_ENTER();
    if (!frame) {
        LOGE(LOG_TAG, "ucamera get frame fail\n");
        mp_printf(MP_PYTHON_PRINTER, "ucamera get frame fail\n");
        return mp_const_none;
    } else {
        LOGD(LOG_TAG, "ucamera get frame OK!\n");
    }

    if (frame) {
        if (frame->len > 0) {
            mp_frame = mp_obj_new_bytes(frame->buf, frame->len);
            if (frame->buf) {
                free(frame->buf);
                frame->buf = NULL;
            }
            return mp_frame;
        } else {
            return mp_const_none;
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ucamera_obj_capture, 0,
                                   obj_capture);

STATIC mp_obj_t obj_save(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 2) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__,
             n_args);
        return mp_obj_new_int(ret);
    }

    frame_buffer_t frame;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_READ);
    frame.buf = bufinfo.buf;
    frame.len = bufinfo.len;
    printf("frame.len: %d\n", frame.len);
    char *path = (char *)mp_obj_str_get_str(args[1]);

    /*save frame to jpeg file*/
    ret = ucamera_service_save_frame(&frame, path);
    if (ret < 0) {
        LOGE(LOG_TAG, "save image fail\n");
    } else {
        LOGD(LOG_TAG, "save image to %s successfully!\n", path);
    }

    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ucamera_obj_save, 1,
                                   obj_save);

STATIC mp_obj_t obj_uninit(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;

    ret = ucamera_service_uninit();
    if (ret < 0) {
        LOGD(LOG_TAG, "init failed  %s;\n", __func__);
    }
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ucamera_obj_uninit, 0, obj_uninit);

STATIC const mp_rom_map_elem_t ucamera_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ucamera) },
    { MP_ROM_QSTR(MP_QSTR_init),    MP_ROM_PTR(&ucamera_obj_init) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&ucamera_obj_connect) },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&ucamera_obj_disconnect) },
    { MP_ROM_QSTR(MP_QSTR_config),  MP_ROM_PTR(&ucamera_obj_config) },
    { MP_ROM_QSTR(MP_QSTR_uninit),  MP_ROM_PTR(&ucamera_obj_uninit) },
    { MP_ROM_QSTR(MP_QSTR_capture), MP_ROM_PTR(&ucamera_obj_capture) },
    { MP_ROM_QSTR(MP_QSTR_save),    MP_ROM_PTR(&ucamera_obj_save) },
    { MP_ROM_QSTR(MP_QSTR_setProp), MP_ROM_PTR(&ucamera_obj_setProp) },
    { MP_ROM_QSTR(MP_QSTR_getProp), MP_ROM_PTR(&ucamera_obj_getProp) },

    { MP_ROM_QSTR(MP_QSTR_SIZE_800X600),                MP_ROM_INT(9)},
    { MP_ROM_QSTR(MP_QSTR_SIZE_640X480),                MP_ROM_INT(8)},
    { MP_ROM_QSTR(MP_QSTR_SIZE_320X240),                MP_ROM_INT(5)},
    { MP_ROM_QSTR(MP_QSTR_SIZE_240X240),                MP_ROM_INT(4)},
    { MP_ROM_QSTR(MP_QSTR_SIZE_160X120),                MP_ROM_INT(1)},

    { MP_ROM_QSTR(MP_QSTR_UART_MODE),                   MP_ROM_INT(CAM_UART_MODE)},
    { MP_ROM_QSTR(MP_QSTR_CLOUD_MODE),                  MP_ROM_INT(CAM_TIMING_MODE)},
    { MP_ROM_QSTR(MP_QSTR_WIFI_STA_MODE),               MP_ROM_INT(CAM_WIFI_STA_MODE)},

    { MP_ROM_QSTR(MP_QSTR_SET_FRAME_SIZE),              MP_ROM_INT(UCAMERA_CMD_SET_FRAME_SIZE)},
//  { MP_ROM_QSTR(MP_QSTR_SET_UPLOAD_TIME),             MP_ROM_INT(UCAMERA_CMD_SET_UPLOAD_TIME)},
    { MP_ROM_QSTR(MP_QSTR_SET_MODE),                    MP_ROM_INT(UCAMERA_CMD_SET_MODE)},
    { MP_ROM_QSTR(MP_QSTR_SET_WIFI_SSID_PWD),           MP_ROM_INT(UCAMERA_CMD_SET_WIFI_SSID_PWD)},
    { MP_ROM_QSTR(MP_QSTR_SET_LED_BRIGHTNESS),          MP_ROM_INT(UCAMERA_CMD_SET_LED_BRIGHTNESS)},
};

STATIC MP_DEFINE_CONST_DICT(ucamera_locals_dict, ucamera_locals_dict_table);

const mp_obj_module_t ucamera_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&ucamera_locals_dict,
};

MP_REGISTER_MODULE(MP_QSTR_ucamera, ucamera_module, MICROPY_PY_UCAMERA);

#endif
