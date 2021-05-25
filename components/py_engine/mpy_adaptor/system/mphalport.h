#ifndef INCLUDED_MPHALPORT_H
#define INCLUDED_MPHALPORT_H

#include "py/ringbuf.h"
#include "lib/utils/interrupt_char.h"

extern ringbuf_t stdin_ringbuf;

extern void mp_hal_set_interrupt_char(int c);


// needed for machine.I2C
#define mp_hal_delay_us_fast(us) mp_hal_delay_us(us)
#define mp_hal_pin_od_low(pin)
#define mp_hal_pin_od_high(pin)
#define mp_hal_pin_open_drain(p)


// needed for soft machine.SPI
#define mp_hal_pin_output(p)
#define mp_hal_pin_input(p)
#define mp_hal_pin_name(p)
#define mp_hal_pin_high(p)

#include "py/obj.h"
#define MP_HAL_PIN_FMT "%u"



// This macro is used to implement PEP 475 to retry specified syscalls on EINTR
#define MP_HAL_RETRY_SYSCALL(ret, syscall, raise) { \
        for (;;) { \
            MP_THREAD_GIL_EXIT(); \
            ret = syscall; \
            MP_THREAD_GIL_ENTER(); \
            if (ret == -1) { \
                int err = errno; \
                if (err == EINTR) { \
                    mp_handle_pending(true); \
                    continue; \
                } \
                raise; \
            } \
            break; \
        } \
}

#define RAISE_ERRNO(err_flag, error_val) \
    { if (err_flag == -1) \
      { mp_raise_OSError(error_val); } }

#endif // INCLUDED_MPHALPORT_H