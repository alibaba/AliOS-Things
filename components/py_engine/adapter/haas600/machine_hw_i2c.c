/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/machine_i2c.h"
#include "modmachine.h"

#include "aos_hal_i2c.h"
#include "ulog/ulog.h"

#if MICROPY_PY_MACHINE

#define LOG_TAG "machine_hw_i2c"

typedef enum{
    I2C_NUM_0 = 0,  /*!< I2C port 0 */
    I2C_NUM_1 ,     /*!< I2C port 1 */
    I2C_NUM_MAX
} i2c_port_t;

#define I2C_0_DEFAULT_SCL (18)
#define I2C_0_DEFAULT_SDA (19)

#define I2C_1_DEFAULT_SCL (20)
#define I2C_1_DEFAULT_SDA (21)

#define I2C_DEFAULT_TIMEOUT_US (1000 * 10) // 10ms

typedef struct _machine_hw_i2c_obj_t {
    mp_obj_base_t base;
    uint16_t port : 8;
    uint16_t scl : 8;
    uint16_t sda : 8;
    i2c_dev_t dev;
    uint32_t timeout;
} machine_hw_i2c_obj_t;

STATIC machine_hw_i2c_obj_t machine_hw_i2c_obj[I2C_NUM_MAX];

int machine_hw_i2c_transfer(mp_obj_base_t *self_in, uint16_t addr, size_t n, mp_machine_i2c_buf_t *bufs, unsigned int flags) {
    machine_hw_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    i2c_dev_t *dev = &self->dev;
    int32_t ret = -1;
    uint32_t timeout = self->timeout;
    uint8_t mode = dev->config.mode;
    
    int data_len = 0;
    for (; n--; ++bufs) {
        if (flags & MP_MACHINE_I2C_FLAG_READ) {
            if(mode == I2C_MODE_MASTER) {
                ret = aos_hal_i2c_master_recv(dev, addr, bufs->buf, bufs->len, timeout);
                for(int k=0; k<bufs->len; k++) {
                    LOGD(LOG_TAG, "data = 0x%2x\n",  bufs->buf[k]);
                }
            } else {
                ret = aos_hal_i2c_slave_recv(dev, bufs->buf, bufs->len, timeout);
            }
        } else {
            if (bufs->len != 0) {
                for(int k=0; k<bufs->len; k++) {
                    LOGD(LOG_TAG, "data = 0x%2x\n", bufs->buf[k]);
                }
                if(mode == I2C_MODE_MASTER) {
                    ret = aos_hal_i2c_master_send(dev, addr, bufs->buf, bufs->len, timeout);
                } else {
                    ret = aos_hal_i2c_slave_send(dev, bufs->buf, bufs->len, timeout);
                }
            }
        }

        if (ret != 0) {
            LOGE(LOG_TAG, "machine_hw_i2c_transfer fail, ret = %d\n", ret);
            goto fail;
        }

        data_len += bufs->len;
    }
    return data_len;

fail:
    return ret;
}

/******************************************************************************/
// MicroPython bindings for machine API

STATIC void machine_hw_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hw_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    i2c_dev_t *dev = &self->dev;

    mp_printf(print, "I2C(port=%u, scl=%u, sda=%u, freq=%u, addr_width=%u, dev_addr=%u, mode=%u)",
        self->port, self->scl, self->sda, dev->config.freq, dev->config.address_width, 
        dev->config.dev_addr, dev->config.mode);
}

STATIC int32_t machine_hw_i2c_init(machine_hw_i2c_obj_t *self, bool first_init) {
    i2c_dev_t *dev = &self->dev;
    int32_t ret = -1;
    if (!first_init) {
        ret = aos_hal_i2c_finalize(self->port);
        if(ret != 0) {
            LOGE(LOG_TAG, "aos_hal_i2c_finalize fail, ret = %d\n", ret);
            return ret;
        }
    }

    ret = aos_hal_i2c_init(dev);
    if(ret != 0) {
        LOGE(LOG_TAG, "aos_hal_i2c_finalize fail, ret = %d\n", ret);
    }
    return ret;
}

mp_obj_t machine_hw_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // MP_MACHINE_I2C_CHECK_FOR_LEGACY_SOFTI2C_CONSTRUCTION(n_args, n_kw, all_args);

    // Parse args
    enum { ARG_id, ARG_scl, ARG_sda, ARG_freq, ARG_timeout, ARG_mode, ARG_addr, ARG_addrsize};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = I2C_BUS_BIT_RATES_400K} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = I2C_DEFAULT_TIMEOUT_US} },
        { MP_QSTR_mode, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = I2C_MODE_MASTER} },
        { MP_QSTR_addr, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = I2C_NUM_0} },
        { MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = I2C_HAL_ADDRESS_WIDTH_7BIT} },
        
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get I2C bus
    mp_int_t port = mp_obj_get_int(args[ARG_id].u_obj);
    if (!(I2C_NUM_0 <= port && port < I2C_NUM_MAX)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C(%d) doesn't exist"), port);
    }

    // Get static peripheral object
    machine_hw_i2c_obj_t *self = (machine_hw_i2c_obj_t *)&machine_hw_i2c_obj[port];

    bool first_init = false;
    if (self->base.type == NULL) {
        // Created for the first time, set default pins
        self->base.type = &machine_hw_i2c_type;
        self->port = port;
        if (self->port == I2C_NUM_0) {
            self->scl = I2C_0_DEFAULT_SCL;
            self->sda = I2C_0_DEFAULT_SDA;
        } else {
            self->scl = I2C_1_DEFAULT_SCL;
            self->sda = I2C_1_DEFAULT_SDA;
        }
        first_init = true;
    }

    // Set SCL/SDA pins if given
    if (args[ARG_scl].u_obj != MP_OBJ_NULL) {
        self->scl = mp_hal_get_pin_obj(args[ARG_scl].u_obj);
    }
    if (args[ARG_sda].u_obj != MP_OBJ_NULL) {
        self->sda = mp_hal_get_pin_obj(args[ARG_sda].u_obj);
    }

    // Set Address width if given
    mp_int_t addr_width = args[ARG_addrsize].u_int;

    // Set freq if given
    mp_int_t freq = addr_width = args[ARG_freq].u_int;

    // Set timeout if given
    mp_int_t timeout = args[ARG_timeout].u_int;

    // Set mode if given
    mp_int_t mode = args[ARG_mode].u_int;

    // Set addr if given
    mp_int_t addr = args[ARG_addr].u_int;

    i2c_dev_t *dev = &self->dev;
    dev->port = port;
    dev->config.address_width = addr_width;
    dev->config.freq = freq;
    dev->config.mode = mode;
    dev->config.dev_addr = addr;
    dev->priv = NULL;

    self->timeout = timeout;

    // Initialise the I2C peripheral
    machine_hw_i2c_init(self, first_init);

    return MP_OBJ_FROM_PTR(self);
}

STATIC const mp_machine_i2c_p_t machine_hw_i2c_p = {
    .transfer = machine_hw_i2c_transfer,
};

const mp_obj_type_t machine_hw_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = machine_hw_i2c_print,
    .make_new = machine_hw_i2c_make_new,
    .protocol = &machine_hw_i2c_p,
    .locals_dict = (mp_obj_dict_t *)&mp_machine_i2c_locals_dict,
};

#endif