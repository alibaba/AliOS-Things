#include <unistd.h>
#include "py/mpconfig.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/mpstate.h"
#include "py/mphal.h"
#include "py/ringbuf.h"

#include <k_api.h>
#include "aos/hal/uart.h"

STATIC uint8_t stdin_ringbuf_array[260];
ringbuf_t stdin_ringbuf = {stdin_ringbuf_array, sizeof(stdin_ringbuf_array), 0, 0};

static uart_dev_t uart_stdio = {0};

extern int hal_uart_rx_sem_take(int uartid, int timeout);
extern int hal_uart_rx_sem_give(int port);

/*
 * Core UART functions to implement for a port
 */

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    uint8_t c = 0;
    uint32_t recv_size = 0;
    int ret = -1;

    memset(&uart_stdio, 0, sizeof(uart_stdio));
    uart_stdio.port = 0;

    // try to check whether we have receive uart input
    ret = hal_uart_recv_II(&uart_stdio, &c, 1, &recv_size, 0);
    if (ret == 0 && recv_size == 1) {
        return c;
    }

    MICROPY_EVENT_POLL_HOOK

    // wait till we get any uart input or wakeup
    ret = hal_uart_rx_sem_take(uart_stdio.port, HAL_WAIT_FOREVER);
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    memset(&uart_stdio, 0, sizeof(uart_stdio));
    uart_stdio.port = 0;
    if ((len > 0) && (str != NULL)) {
        hal_uart_send(&uart_stdio, str, len, 5000);
    }
}

mp_uint_t mp_hal_ticks_us(void) {
    return krhino_ticks_to_ms(krhino_sys_tick_get()) * 1000;
}

mp_uint_t mp_hal_ticks_ms(void) {
    return krhino_ticks_to_ms(krhino_sys_tick_get());
}

uint64_t mp_hal_time_ns(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t ns = tv.tv_sec * 1000000000ULL;
    ns += (uint64_t)tv.tv_usec * 1000ULL;
    return ns;
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return krhino_sys_tick_get();
}

void mp_hal_delay_us(mp_uint_t us) {
    tick_t t0 = krhino_sys_tick_get(), t1, dt;
    uint64_t dtick = us * RHINO_CONFIG_TICKS_PER_SECOND / 1000000L;
    while (1) {
        t1 = krhino_sys_tick_get();
        dt = t1 - t0;
        if (dt >= dtick) {
            break;
        }
        mp_handle_pending(false);
    }
}

void mp_hal_delay_ms(mp_uint_t ms) {
    tick_t t0 = krhino_sys_tick_get(), t1, dt;
    uint64_t dtick = ms * RHINO_CONFIG_TICKS_PER_SECOND / 1000L;
    while (1) {
        t1 = krhino_sys_tick_get();
        dt = t1 - t0;
        if (dt >= dtick) {
            break;
        }
        MICROPY_EVENT_POLL_HOOK;
        krhino_task_sleep(1);
    }
}

// Wake up the main task if it is sleeping
void mp_hal_wake_main_task_from_isr(void) {
    hal_uart_rx_sem_give(uart_stdio.port);
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    if ((poll_flags & MP_STREAM_POLL_RD) && stdin_ringbuf.iget != stdin_ringbuf.iput) {
        ret |= MP_STREAM_POLL_RD;
    }
    return ret;
}