/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 "Eric Poulsen" <eric@zyxod.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"
#include "extmod/machine_spi.h"
#include "modmachine.h"

#include "aos_hal_spi.h"
#include "ulog/ulog.h"

#if MICROPY_PY_MACHINE

#define LOG_TAG "machine_hw_spi"

typedef enum{
    SPI_NUM_0 = 0,  /*!< SPI port 0 */
    SPI_NUM_1 ,     /*!< SPI port 1 */
    SPI_NUM_MAX
} spi_port_t;

typedef struct _machine_hw_spi_obj_t {
    mp_obj_base_t base;
    uint8_t port;
    spi_dev_t spi;
} machine_hw_spi_obj_t;

// Static objects mapping to HSPI and VSPI hardware peripherals
STATIC machine_hw_spi_obj_t machine_hw_spi_obj[2] = {0};

STATIC void machine_hw_spi_deinit_internal(machine_hw_spi_obj_t *self) {
    spi_dev_t *spi = &self->spi;
    aos_hal_spi_finalize(spi);
    self->base.type = NULL;
}

STATIC int32_t machine_hw_spi_init_internal(machine_hw_spi_obj_t *self, bool first_init) {
    spi_dev_t *spi = &self->spi;
    int32_t ret = -1;

    if (!first_init) {
        ret = aos_hal_spi_finalize(spi);
        if(ret != 0) {
            LOGE(LOG_TAG, "aos_hal_spi_finalize fail, ret = %d\n", ret);
        }
        self->base.type = NULL;
        return ret;
    }

    // Initialize the SPI bus
    ret = aos_hal_spi_init(spi);
    if(ret != 0) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("invalid configuration"));
    }

    return ret;
}

STATIC void machine_hw_spi_deinit(mp_obj_base_t *self_in) {
    machine_hw_spi_obj_t *self = (machine_hw_spi_obj_t *)self_in;
    spi_dev_t *spi = &self->spi;

    int32_t ret = aos_hal_spi_finalize(spi);
    if(ret != 0) {
        LOGE(LOG_TAG, "aos_hal_spi_finalize fail, ret = %d\n", ret);
    }
    self->base.type = NULL;
}

STATIC void machine_hw_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    machine_hw_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    spi_dev_t *spi = &self->spi;
    int32_t ret = -1;

    MP_THREAD_GIL_EXIT();
    if(dest == NULL) {  // send operation
        ret = aos_hal_spi_send(spi, src, len, HAL_WAIT_FOREVER);
        if(ret != 0 ) {
            LOGE(LOG_TAG, "aos_hal_spi_send fail, ret = %d\n", ret);
        }
    } else if(src == dest) {
        ret = aos_hal_spi_recv(spi, dest, len, HAL_WAIT_FOREVER);
        if(ret != 0 ) {
            LOGE(LOG_TAG, "aos_hal_spi_recv fail, ret = %d\n", ret);
        }
    } else {
        ret = aos_hal_spi_send_recv(spi, (uint8_t *)src, dest, len, HAL_WAIT_FOREVER);
        if(ret != 0 ) {
            LOGE(LOG_TAG, "aos_hal_spi_send_recv fail, ret = %d\n", ret);
        }
    }
    MP_THREAD_GIL_ENTER();
}

/******************************************************************************/
// MicroPython bindings for hw_spi

STATIC void machine_hw_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hw_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    spi_dev_t *spi = &self->spi;
    spi_config_t *cfg = &spi->config;
    mp_printf(print, "SPI(id=%u, role=%u, polarity=%u, mode=%u, t_mode=%u, freq=%u, serial_len=%d, data_size=%d, cs=%d)",
        spi->port, cfg->role, cfg->firstbit, cfg->mode, cfg->t_mode, cfg->freq, cfg->serial_len, cfg->data_size, cfg->cs);
}

STATIC void machine_hw_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_hw_spi_obj_t *self = (machine_hw_spi_obj_t *)self_in;

    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_mode, ARG_len};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 500000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = SPI_DATA_SIZE_8BIT} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = MICROPY_PY_MACHINE_SPI_MSB} },
        { MP_QSTR_mode, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = SPI_ROLE_MASTER} },
        { MP_QSTR_len, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_SERAIL_LEN} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    spi_dev_t *spi = &self->spi;

    spi->config.role = args[ARG_mode].u_int;
    spi->config.firstbit = args[ARG_firstbit].u_int;
    spi->config.mode = (args[ARG_phase].u_int << 1) | args[ARG_polarity].u_int;
    spi->config.freq = args[ARG_baudrate].u_int;
    spi->config.serial_len = args[ARG_len].u_int;
    spi->config.data_size = args[ARG_bits].u_int;

    machine_hw_spi_init_internal(self, false);
}

mp_obj_t machine_hw_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {

    enum { ARG_id, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_mode, ARG_len, ARG_sck, ARG_mosi, ARG_miso};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_int = 0} },
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 500000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = SPI_DATA_SIZE_8BIT} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = MICROPY_PY_MACHINE_SPI_MSB} },
        { MP_QSTR_mode, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = SPI_ROLE_MASTER} },
        { MP_QSTR_len, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_SERAIL_LEN} },
        { MP_QSTR_sck, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get SPI bus
    mp_int_t port = mp_obj_get_int(args[ARG_id].u_obj);
    if (!(SPI_NUM_0 <= port && port < SPI_NUM_MAX)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SPI(%d) doesn't exist"), port);
    }

    machine_hw_spi_obj_t *self;
    if (port == 0) {
        self = &machine_hw_spi_obj[0];
    } else {
        self = &machine_hw_spi_obj[1];
    }
    self->port = port;

    bool first_init = false;
    if (self->base.type == NULL) {
        // Created for the first time, set default pins
        self->base.type = &machine_hw_spi_type;
        first_init = true;
    }

    spi_dev_t *spi = &self->spi;
    spi->port = port;
    spi->config.role = args[ARG_mode].u_int;
    spi->config.firstbit = args[ARG_firstbit].u_int;
    spi->config.mode = (args[ARG_phase].u_int << 1) | args[ARG_polarity].u_int;
    spi->config.t_mode = SPI_TRANSFER_DMA;
    spi->config.freq = args[ARG_baudrate].u_int;
    spi->config.serial_len = args[ARG_len].u_int;
    spi->config.data_size = args[ARG_bits].u_int;
    spi->config.cs = SPI_CS_EN;
    spi->priv = NULL;
    
    machine_hw_spi_init_internal(self, first_init);

    return MP_OBJ_FROM_PTR(self);
}

STATIC const mp_machine_spi_p_t machine_hw_spi_p = {
    .init = machine_hw_spi_init,
    .deinit = machine_hw_spi_deinit,
    .transfer = machine_hw_spi_transfer,
};

const mp_obj_type_t machine_hw_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = machine_hw_spi_print,
    .make_new = machine_hw_spi_make_new,
    .protocol = &machine_hw_spi_p,
    .locals_dict = (mp_obj_dict_t *)&mp_machine_spi_locals_dict,
};

#endif