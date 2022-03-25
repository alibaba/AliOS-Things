#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos_hal_spi.h"
#include "board_mgr.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

#define LOG_TAG "driver_spi"

extern const mp_obj_type_t driver_spi_type;

#define SPI_CHECK_PARAMS(cnt)                                                       \
    if (n_args < cnt) {                                                             \
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args); \
        return MP_OBJ_NEW_SMALL_INT(-MP_E2BIG);                                     \
    }                                                                               \
    mp_spi_obj_t *driver_obj = (mp_spi_obj_t *)MP_OBJ_TO_PTR(args[0]);              \
    do {                                                                            \
        if (driver_obj == NULL) {                                                   \
            LOGE(LOG_TAG, "driver_obj[SPI]] is NULL\n");                            \
            return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);                                \
        }                                                                           \
    } while (0)

#define SPI_NODE_GET()                                                                          \
    spi_dev_t *spi_device = py_board_get_node_by_handle(MODULE_SPI, &(driver_obj->spi_handle)); \
    do {                                                                                        \
        if (NULL == spi_device) {                                                               \
            LOGE(LOG_TAG, "%s: py_board_get_node_by_handle failed;\n", __func__);               \
            py_board_disattach_item(MODULE_SPI, &(driver_obj->spi_handle));                     \
            return MP_OBJ_NEW_SMALL_INT(-MP_ENXIO);                                             \
        }                                                                                       \
    } while (0)

#define SPI_TIMEOUT (1000)

// this is the actual C-structure for our new object
typedef struct {
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    item_handle_t spi_handle;
} mp_spi_obj_t;

void spi_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t spi_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_spi_obj_t *driver_obj = m_new_obj(mp_spi_obj_t);
    if (!driver_obj) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Memory malloc failed"));
    }
    memset(driver_obj, 0, sizeof(mp_spi_obj_t));

    driver_obj->Base.type = &driver_spi_type;
    driver_obj->ModuleName = "spi";

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t spi_open(size_t n_args, const mp_obj_t *args)
{
    mp_int_t ret = -1;

    SPI_CHECK_PARAMS(2);

    char *id = NULL;
    if (mp_obj_is_str(args[1])) {
        id = (char *)mp_obj_str_get_str(args[1]);
    } else {
        LOGE(LOG_TAG, "%s: 2nd parameter should be string\n", __func__);
        return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
    }

    ret = py_board_mgr_init();
    if (ret != 0) {
        LOGE(LOG_TAG, "%s:py_board_mgr_init failed\n", __func__);
        return MP_OBJ_NEW_SMALL_INT(-MP_ENOENT);
    }

    ret = py_board_attach_item(MODULE_SPI, id, &(driver_obj->spi_handle));
    if (ret != 0) {
        LOGE(LOG_TAG, "%s: py_board_attach_item failed ret = %d\n", __func__, ret);
        goto fail;
    }

    SPI_NODE_GET();

    LOGD(LOG_TAG, "%s: port = %d;\n", __func__, spi_device->port);
    LOGD(LOG_TAG, "%s: role = %d;\n", __func__, spi_device->config.role);
    LOGD(LOG_TAG, "%s: firstbit = %d;\n", __func__, spi_device->config.firstbit);
    LOGD(LOG_TAG, "%s: mode = %d;\n", __func__, spi_device->config.mode);
    LOGD(LOG_TAG, "%s: t_mode = %d;\n", __func__, spi_device->config.t_mode);
    LOGD(LOG_TAG, "%s: freq = %d;\n", __func__, spi_device->config.freq);
    LOGD(LOG_TAG, "%s: serial_len = %d;\n", __func__, spi_device->config.serial_len);
    LOGD(LOG_TAG, "%s: data_size = %d;\n", __func__, spi_device->config.data_size);
    LOGD(LOG_TAG, "%s: cs = %d;\n", __func__, spi_device->config.cs);

    ret = aos_hal_spi_init(spi_device);

fail:
    if (ret < 0) {
        LOGE(LOG_TAG, "%s: spi_open failed ret = %d\n", __func__, ret);
        py_board_disattach_item(MODULE_SPI, &(driver_obj->spi_handle));
    }

    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(spi_open_obj, 2, spi_open);

STATIC mp_obj_t spi_close(size_t n_args, const mp_obj_t *args)
{
    mp_int_t ret = -1;

    SPI_CHECK_PARAMS(1);
    SPI_NODE_GET();

    ret = aos_hal_spi_finalize(spi_device);
    if (ret != 0) {
        LOGE(LOG_TAG, "%s: hal_spi_finalize failed\n", __func__);
    }

    py_board_disattach_item(MODULE_SPI, &(driver_obj->spi_handle));
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(spi_close_obj, 1, spi_close);

STATIC mp_obj_t spi_read(size_t n_args, const mp_obj_t *args)
{
    mp_int_t ret = -1;

    SPI_CHECK_PARAMS(2);
    SPI_NODE_GET();

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
    MP_THREAD_GIL_EXIT();
    ret = aos_hal_spi_recv(spi_device, bufinfo.buf, bufinfo.len, SPI_TIMEOUT);
    MP_THREAD_GIL_ENTER();
    if (ret < 0) {
        LOGE(LOG_TAG, "%s: aos_hal_spi_recv failed;\n", __func__);
        return MP_OBJ_NEW_SMALL_INT(ret);
    } else {
        return MP_OBJ_NEW_SMALL_INT(bufinfo.len);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(spi_read_obj, 2, spi_read);

STATIC mp_obj_t spi_write(size_t n_args, const mp_obj_t *args)
{
    SPI_CHECK_PARAMS(2);
    SPI_NODE_GET();

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);
    MP_THREAD_GIL_EXIT();
    mp_int_t ret = aos_hal_spi_send(spi_device, bufinfo.buf, bufinfo.len, SPI_TIMEOUT);
    MP_THREAD_GIL_ENTER();
    if (ret < 0) {
        LOGE(LOG_TAG, "%s: hal_spi_send failed;\n", __func__);
        return MP_OBJ_NEW_SMALL_INT(ret);
    } else {
        return MP_OBJ_NEW_SMALL_INT(bufinfo.len);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(spi_write_obj, 2, spi_write);

extern int32_t aos_hal_spi_sends_recvs(spi_dev_t *spi, uint8_t *tx_data, uint32_t tx_size,
                            uint8_t *rx_data, uint32_t rx_size, uint32_t timeout);
STATIC mp_obj_t spi_readAfterWrite(size_t n_args, const mp_obj_t *args)
{
    SPI_CHECK_PARAMS(3);
    SPI_NODE_GET();

    mp_buffer_info_t bufinfo_rx;
    mp_get_buffer_raise(args[1], &bufinfo_rx, MP_BUFFER_WRITE);

    mp_buffer_info_t bufinfo_tx;
    mp_get_buffer_raise(args[2], &bufinfo_tx, MP_BUFFER_READ);

    MP_THREAD_GIL_EXIT();
    mp_int_t ret = -1;
    if (bufinfo_tx.len == 1) {
        ret = aos_hal_spi_send_recv(spi_device, bufinfo_tx.buf, bufinfo_rx.buf, bufinfo_rx.len, SPI_TIMEOUT);
    } else {
        if (bufinfo_tx.len != bufinfo_rx.len) {
            mp_raise_ValueError(MP_ERROR_TEXT("TX and RX buffer should have same length"));
        }
        ret = aos_hal_spi_sends_recvs(spi_device, bufinfo_tx.buf, bufinfo_tx.len, bufinfo_rx.buf, bufinfo_rx.len,
                                      SPI_TIMEOUT);
    }
    MP_THREAD_GIL_ENTER();

    if (ret < 0) {
        LOGE(LOG_TAG, "aos_hal_spi_send_recv failed\n");
        return MP_OBJ_NEW_SMALL_INT(ret);
    } else {
        return MP_OBJ_NEW_SMALL_INT(bufinfo_rx.len);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(spi_readAfterWrite_obj, 3, spi_readAfterWrite);

STATIC mp_obj_t spi_readAndWrite(size_t n_args, const mp_obj_t *args)
{
    SPI_CHECK_PARAMS(3);
    SPI_NODE_GET();

    mp_buffer_info_t read_bufinfo;
    mp_get_buffer_raise(args[1], &read_bufinfo, MP_BUFFER_WRITE);

    mp_buffer_info_t write_bufinfo;
    mp_get_buffer_raise(args[2], &write_bufinfo, MP_BUFFER_READ);
    MP_THREAD_GIL_EXIT();
    mp_int_t ret = aos_hal_spi_send_and_recv(spi_device, write_bufinfo.buf, write_bufinfo.len, read_bufinfo.buf,
                                             read_bufinfo.len, SPI_TIMEOUT);
    MP_THREAD_GIL_ENTER();
    if (ret < 0) {
        LOGE(LOG_TAG, "%s: hal_spi_recv failed;\n", __func__);
        return MP_OBJ_NEW_SMALL_INT(ret);
    } else {
        return MP_OBJ_NEW_SMALL_INT(read_bufinfo.len);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(spi_readAndWrite_obj, 3, spi_readAndWrite);

STATIC const mp_rom_map_elem_t spi_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_SPI) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&spi_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&spi_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&spi_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&spi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_readAfterWrite), MP_ROM_PTR(&spi_readAfterWrite_obj) },
    { MP_ROM_QSTR(MP_QSTR_readAndWrite), MP_ROM_PTR(&spi_readAndWrite_obj) },
};
STATIC MP_DEFINE_CONST_DICT(spi_locals_dict, spi_locals_dict_table);

const mp_obj_type_t driver_spi_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = spi_obj_print,
    .make_new = spi_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&spi_locals_dict,
};
