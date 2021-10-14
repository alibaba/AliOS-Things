#ifndef INCLUDED_MPHALPORT_H
#define INCLUDED_MPHALPORT_H

#include "shared/runtime/interrupt_char.h"
#include "py/ringbuf.h"
#include "py/mpconfig.h"

typedef mp_uint_t cpu_stack_t;

extern ringbuf_t stdin_ringbuf;

extern void mp_hal_set_interrupt_char(int c);

// This macro is used to implement PEP 475 to retry specified syscalls on EINTR
#define MP_HAL_RETRY_SYSCALL(ret, syscall, raise) \
    {                                             \
        for (;;) {                                \
            MP_THREAD_GIL_EXIT();                 \
            ret = syscall;                        \
            MP_THREAD_GIL_ENTER();                \
            if (ret == -1) {                      \
                int err = errno;                  \
                if (err == MP_EINTR) {            \
                    mp_handle_pending(true);      \
                    continue;                     \
                }                                 \
                raise;                            \
            }                                     \
            break;                                \
        }                                         \
    }

#define RAISE_ERRNO(err_flag, error_val) \
    {                                    \
        if (err_flag == -1) {            \
            mp_raise_OSError(error_val); \
        }                                \
    }

#define mp_hal_delay_us_fast(us) mp_hal_delay_us(us)

#include "aos_hal_gpio.h"
#include "py/obj.h"

#ifdef CONFIG_GPIO_NUM
#define PY_GPIO_NUM_MAX CONFIG_GPIO_NUM
#else
#define PY_GPIO_NUM_MAX 64
#endif

#define MP_HAL_PIN_FMT   "%u"
#define mp_hal_pin_obj_t mp_uint_t

extern gpio_config_t mphal_gpio_config_obj_t[PY_GPIO_NUM_MAX];

mp_hal_pin_obj_t machine_pin_get_id(mp_obj_t pin_in);
gpio_config_t mp_hal_pin_config_get(mp_hal_pin_obj_t pin_obj);
mp_int_t mp_hal_pin_low(mp_hal_pin_obj_t pin_obj);
mp_int_t mp_hal_pin_high(mp_hal_pin_obj_t pin_obj);
mp_int_t mp_hal_pin_config_set(mp_hal_pin_obj_t pin_obj, gpio_config_t config);

#define mp_hal_get_pin_obj(o)    machine_pin_get_id(o)
#define mp_hal_pin_name(p)       (p)
#define mp_hal_pin_input(p)      mp_hal_pin_config_set((p), INPUT_PULL_UP)
#define mp_hal_pin_output(p)     mp_hal_pin_config_set((p), OUTPUT_PUSH_PULL)
#define mp_hal_pin_open_drain(p) mp_hal_pin_config_set((p), OUTPUT_OPEN_DRAIN_NO_PULL)
#define mp_hal_pin_od_low(p)     mp_hal_pin_low(p)
#define mp_hal_pin_od_high(p)    mp_hal_pin_high(p)
#define mp_hal_pin_write(p, v)   ((v) ? mp_hal_pin_high(p) : mp_hal_pin_low(p))

#endif  // INCLUDED_MPHALPORT_H
