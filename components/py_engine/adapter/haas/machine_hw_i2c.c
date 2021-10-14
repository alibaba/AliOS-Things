/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "aos_hal_i2c.h"
#include "extmod/machine_i2c.h"
#include "modmachine.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

#if MICROPY_PY_MACHINE_I2C

#define LOG_TAG "machine_hw_i2c"

#define MI2C_CHECK_PARAMS()                                                      \
    machine_hw_i2c_obj_t *self = (machine_hw_i2c_obj_t *)MP_OBJ_TO_PTR(self_in); \
    do {                                                                         \
        if (self == NULL) {                                                      \
            mp_raise_OSError(MP_EINVAL);                                         \
            return mp_const_none;                                                \
        }                                                                        \
    } while (0)

#define I2C_0_DEFAULT_SCL      (18)
#define I2C_0_DEFAULT_SDA      (19)

#define I2C_1_DEFAULT_SCL      (20)
#define I2C_1_DEFAULT_SDA      (21)

#define I2C_DEFAULT_TIMEOUT_US (10000)

typedef enum {
    I2C_NUM_0 = 0, /*!< I2C port 0 */
    I2C_NUM_1,     /*!< I2C port 1 */
    I2C_NUM_MAX
} i2c_port_t;

typedef struct _machine_hw_i2c_obj_t {
    mp_obj_base_t base;
    uint16_t port : 8;
    uint16_t scl : 8;
    uint16_t sda : 8;
    i2c_dev_t dev;
    mp_uint_t timeout;
} machine_hw_i2c_obj_t;

STATIC machine_hw_i2c_obj_t machine_hw_i2c_obj[I2C_NUM_MAX];

mp_int_t mp_machine_hw_i2c_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    MI2C_CHECK_PARAMS();
    i2c_dev_t *dev = &self->dev;
    mp_int_t ret = -1;
    mp_uint_t timeout = self->timeout;
    uint8_t mode = dev->config.mode;

    return aos_hal_i2c_init(dev);
}

mp_int_t mp_machine_hw_i2c_deinit(mp_obj_base_t *self_in)
{
    MI2C_CHECK_PARAMS();
    i2c_dev_t *dev = &self->dev;
    return aos_hal_i2c_finalize(dev);
}

mp_int_t mp_machine_hw_i2c_transfer(mp_obj_base_t *self_in, uint16_t addr, size_t n, mp_machine_i2c_buf_t *bufs,
                                    unsigned int flags)
{
    MI2C_CHECK_PARAMS();
    i2c_dev_t *dev = &self->dev;
    mp_int_t ret = -1;
    mp_uint_t timeout = self->timeout;
    uint8_t mode = dev->config.mode;

    int data_len = 0;
    for (; n--; ++bufs) {
        if (flags & MP_MACHINE_I2C_FLAG_READ) {
            ret = aos_hal_i2c_mem_read(dev, dev->config.dev_addr, addr, n, bufs->buf, bufs->len, timeout);
        } else {
            if (bufs->len != 0) {
                ret = aos_hal_i2c_mem_write(dev, dev->config.dev_addr, addr, n, bufs->buf, bufs->len, timeout);
            }
        }

        if (ret != 0) {
            LOGE(LOG_TAG, "mp_machine_hw_i2c_transfer fail, ret = %d\n", ret);
            goto fail;
        }

        data_len += bufs->len;
    }
    return data_len;

fail:
    return ret;
}

mp_int_t mp_machine_hw_i2c_write(mp_obj_base_t *self_in, const uint8_t *src, size_t len)
{
    MI2C_CHECK_PARAMS();
    i2c_dev_t *dev = &self->dev;
    mp_int_t ret = -1;
    mp_uint_t timeout = self->timeout;
    uint8_t mode = dev->config.mode;

    if (mode == I2C_MODE_MASTER) {
        ret = aos_hal_i2c_master_send(dev, dev->config.dev_addr, src, len, timeout);
    } else {
        ret = aos_hal_i2c_slave_send(dev, src, len, timeout);
    }

    if(ret != 0) {
        return ret;
    }

    return len;
}

mp_int_t mp_machine_hw_i2c_read(mp_obj_base_t *self_in, uint8_t *dest, size_t len, bool nack)
{
    MI2C_CHECK_PARAMS();
    i2c_dev_t *dev = &self->dev;
    mp_int_t ret = -1;
    mp_uint_t timeout = self->timeout;
    uint8_t mode = dev->config.mode;

    if (mode == I2C_MODE_MASTER) {
        ret = aos_hal_i2c_master_recv(dev, dev->config.dev_addr, dest, len, timeout);
    } else {
        ret = aos_hal_i2c_slave_recv(dev, dest, len, timeout);
    }

    if (ret != 0) {
        return ret;
    }

    return len;
}

/******************************************************************************/
// MicroPython bindings for machine API

STATIC void machine_hw_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    MI2C_CHECK_PARAMS();
    i2c_dev_t *dev = &self->dev;

    mp_printf(print,
              "I2C(port=%u, scl=%u, sda=%u, freq=%u, addr_width=%u, "
              "dev_addr=%u, mode=%u)",
              self->port, self->scl, self->sda, dev->config.freq, dev->config.address_width, dev->config.dev_addr,
              dev->config.mode);
}

STATIC mp_int_t machine_hw_i2c_init(machine_hw_i2c_obj_t *self, bool first_init)
{
    i2c_dev_t *dev = &self->dev;
    mp_int_t ret = -1;
    if (!first_init) {
        ret = aos_hal_i2c_finalize(dev);
        if (ret != 0) {
            LOGE(LOG_TAG, "aos_hal_i2c_finalize fail, ret = %d\n", ret);
        }
    }

    ret = aos_hal_i2c_init(dev);
    if (ret != 0) {
        LOGE(LOG_TAG, "aos_hal_i2c_finalize fail, ret = %d\n", ret);
    }
    return ret;
}

mp_obj_t machine_hw_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args)
{
    // Parse args
    enum {
        ARG_id,
        ARG_scl,
        ARG_sda,
        ARG_freq,
        ARG_timeout,
        ARG_mode,
        ARG_addr,
        ARG_addrsize
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = I2C_BUS_BIT_RATES_400K } },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = I2C_DEFAULT_TIMEOUT_US } },
        { MP_QSTR_mode, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = I2C_MODE_MASTER } },
        { MP_QSTR_addr, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = 0x00 } },
        { MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = I2C_HAL_ADDRESS_WIDTH_7BIT } },

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
    mp_int_t freq = args[ARG_freq].u_int;

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
    .init = mp_machine_hw_i2c_init,
    .deinit = mp_machine_hw_i2c_deinit,
    .read = mp_machine_hw_i2c_read,
    .write = mp_machine_hw_i2c_write,
    .transfer = mp_machine_hw_i2c_transfer,
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