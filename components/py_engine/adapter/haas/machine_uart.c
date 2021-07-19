/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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
#include "py/mperrno.h"
#include "modmachine.h"

#include "ulog/ulog.h"
#include "aos_hal_uart.h"

#define LOG_TAG "machine_uart"

enum {
  UART_NUM_0,
  UART_NUM_1,
  UART_NUM_2,
  UART_NUM_3,
  UART_NUM_MAX
};

enum {
    e_DATA_WIDTH_5BIT = 5,
    e_DATA_WIDTH_6BIT,
    e_DATA_WIDTH_7BIT,
    e_DATA_WIDTH_8BIT,
    e_DATA_WIDTH_9BIT
};

enum {
    e_STOP_BITS_1 = 1,
    e_STOP_BITS_2
};

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    uint8_t uart_num;
    bool uart_inited;
    uart_dev_t dev;
    uint16_t timeout;       // timeout waiting for first char (in ms)
    uint16_t timeout_char;  // timeout waiting between chars (in ms)
} machine_uart_obj_t;

STATIC const char *_parity_name[]   = {"None", "Odd", "Even"};
STATIC const char *_flowctrl_name[] = {"Disable", "CTS", "RTS", "CTS_RTS"};
STATIC const char *_mode_name[]     = {"Tx", "Rx", "Tx_Rx"};
STATIC const uint8_t _bits[]        = {e_DATA_WIDTH_5BIT, e_DATA_WIDTH_6BIT, e_DATA_WIDTH_7BIT, e_DATA_WIDTH_8BIT, e_DATA_WIDTH_9BIT};

const mp_obj_type_t machine_uart_type;

/******************************************************************************/
// MicroPython bindings for UART

STATIC void machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "UART(%u, baudrate=%u, bits=%u, parity=%s, stop=%u, flow_ctrl=%s, mode=%s, timeout=%u, timeout_char=%u)",
        self->uart_num, self->dev.config.baud_rate, _bits[self->dev.config.data_width], 
        _parity_name[self->dev.config.parity], self->dev.config.stop_bits, 
        _flowctrl_name[self->dev.config.flow_control], _mode_name[self->dev.config.mode],
        self->timeout, self->timeout_char);
}

STATIC void machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_tx, ARG_rx, ARG_rts, ARG_cts, ARG_txbuf, ARG_rxbuf, ARG_timeout, ARG_timeout_char, ARG_invert };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 115200} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = e_DATA_WIDTH_8BIT} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_int = NO_PARITY} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = e_STOP_BITS_1} },
        { MP_QSTR_tx, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_rx, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_rts, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_cts, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_txbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_rxbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_invert, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uart_dev_t *dev = &self->dev;

    // wait for all data to be transmitted before changing settings
    // TODO

    // set baudrate
    int32_t baudrate = args[ARG_baudrate].u_int;
    if (baudrate <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("baud rate error"));
    } else {
        dev->config.baud_rate = baudrate;
    }

    // set flow_control
    uint32_t rts = args[ARG_rts].u_int;
    uint32_t cts = args[ARG_cts].u_int;
    if(rts && cts) {
      dev->config.flow_control = FLOW_CONTROL_CTS_RTS;
    } else if(cts) {
      dev->config.flow_control = FLOW_CONTROL_CTS;
    } else if(rts) {
      dev->config.flow_control = FLOW_CONTROL_RTS;
    } else {
      dev->config.flow_control = FLOW_CONTROL_DISABLED;
    }
    
    // set data bits
    uint8_t data_width = args[ARG_bits].u_int;
    switch(data_width) {
        case e_DATA_WIDTH_5BIT:
            dev->config.data_width = DATA_WIDTH_5BIT;
            break;

        case e_DATA_WIDTH_6BIT:
            dev->config.data_width = DATA_WIDTH_6BIT;
            break;

        case e_DATA_WIDTH_7BIT:
            dev->config.data_width = DATA_WIDTH_7BIT;
            break;

        case e_DATA_WIDTH_8BIT:
            dev->config.data_width = DATA_WIDTH_8BIT;
            break;

        case e_DATA_WIDTH_9BIT:
            dev->config.data_width = DATA_WIDTH_9BIT;
            break;
        
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid data bits"));
    }

    // set parity
    if (args[ARG_parity].u_obj != MP_OBJ_NULL) {
        if (args[ARG_parity].u_obj == mp_const_none) {
            dev->config.parity = NO_PARITY;
        } else {
            mp_int_t parity = mp_obj_get_int(args[ARG_parity].u_obj);
            if (parity & 1) {
                dev->config.parity = ODD_PARITY;
            } else {
                dev->config.parity = EVEN_PARITY;
            }
        }
    }

    // set stop bits
    uint8_t stop_bits = args[ARG_stop].u_int;
    switch(stop_bits) {
        case e_STOP_BITS_1:
          dev->config.stop_bits = STOP_BITS_1;
          break;

        case e_STOP_BITS_2:
          dev->config.stop_bits = STOP_BITS_2;
          break;

        default:
          mp_raise_ValueError(MP_ERROR_TEXT("invalid stop bits"));
    }

    // set timeout
    self->timeout = args[ARG_timeout].u_int;

    // set timeout_char
    // make sure it is at least as long as a whole character (13 bits to be safe)
    self->timeout_char = args[ARG_timeout_char].u_int;
    uint32_t min_timeout_char = 13000 / baudrate + 1;
    if (self->timeout_char < min_timeout_char) {
        self->timeout_char = min_timeout_char;
    }
}

STATIC mp_obj_t machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get uart id
    mp_int_t uart_num = mp_obj_get_int(args[0]);
    if (uart_num < 0 || uart_num >= UART_NUM_MAX) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) does not exist"), uart_num);
    }

    // Attempts to use UART0 from Python has resulted in all sorts of fun errors.
    // FIXME: UART0 is disabled for now.
    if (uart_num == UART_NUM_0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) is disabled (dedicated to REPL)"), uart_num);
    }

    // create instance
    machine_uart_obj_t *self = m_new_obj(machine_uart_obj_t);
    self->base.type = &machine_uart_type;
    self->dev.port = uart_num;
    self->dev.config.baud_rate = 115200;
    self->dev.config.data_width = DATA_WIDTH_8BIT;
    self->dev.config.parity = NO_PARITY;
    self->dev.config.stop_bits = STOP_BITS_1;
    self->dev.config.flow_control = FLOW_CONTROL_DISABLED;
    self->dev.config.mode = MODE_TX_RX;
    self->dev.priv = NULL;
    self->timeout = 0;
    self->timeout_char = 0;
    self->uart_inited = false;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_uart_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_uart_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_uart_init_obj, 1, machine_uart_init);

STATIC mp_obj_t machine_uart_deinit(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if(self->uart_inited == true) {
        aos_hal_uart_finalize(&self->dev);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_deinit_obj, machine_uart_deinit);

STATIC mp_obj_t machine_uart_any(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_raise_NotImplementedError(MP_ERROR_TEXT("attenuation not supported"));
    return MP_OBJ_NEW_SMALL_INT(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_any_obj, machine_uart_any);

STATIC mp_obj_t machine_uart_sendbreak(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_raise_NotImplementedError(MP_ERROR_TEXT("attenuation not supported"));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_sendbreak_obj, machine_uart_sendbreak);

STATIC const mp_rom_map_elem_t machine_uart_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_uart_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_uart_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&machine_uart_any_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendbreak), MP_ROM_PTR(&machine_uart_sendbreak_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_uart_locals_dict, machine_uart_locals_dict_table);

STATIC mp_int_t uart_init(machine_uart_obj_t *self) {
    // init uart if not
    if(self->uart_inited == false) {
        int status = aos_hal_uart_init(&self->dev);
        if(status != 0) {
            LOGE(LOG_TAG, "Failed to init uart, status = %d\n", status);
            return -status;
        } else {
            self->uart_inited = true;
        }
    }

    return 0;
}

STATIC mp_uint_t machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }

    int status = uart_init(self);
    if(status != 0) {
        return status;
    }

    mp_uint_t bytes_read = 0;
    MP_THREAD_GIL_EXIT();
    status = aos_hal_uart_recv_II(&self->dev, buf_in, size, &bytes_read, self->timeout);
    MP_THREAD_GIL_ENTER();

    if (status != 0) {
        LOGW(LOG_TAG, "uart read failed with status=%d", status);
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    return bytes_read;
}

STATIC mp_uint_t machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // init uart if not
    int status = uart_init(self);
    if(status != 0) {
        return status;
    }

    MP_THREAD_GIL_EXIT();
    status = aos_hal_uart_send(&self->dev, buf_in, size, self->timeout);
    MP_THREAD_GIL_ENTER();

    if (status != 0) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    // return number of bytes written
    return size;
}

STATIC mp_uint_t machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = self_in;
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        size_t rxbufsize = 0;
        // uart_get_buffered_data_len(self->uart_num, &rxbufsize);
        if ((flags & MP_STREAM_POLL_RD) && rxbufsize > 0) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && 1) { // FIXME: uart_tx_any_room(self->uart_num)
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

STATIC const mp_stream_p_t uart_stream_p = {
    .read = machine_uart_read,
    .write = machine_uart_write,
    .ioctl = machine_uart_ioctl,
    .is_text = false,
};

const mp_obj_type_t machine_uart_type = {
    { &mp_type_type },
    .name = MP_QSTR_UART,
    .print = machine_uart_print,
    .make_new = machine_uart_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &uart_stream_p,
    .locals_dict = (mp_obj_dict_t *)&machine_uart_locals_dict,
};
