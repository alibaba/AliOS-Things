#include <unistd.h>
#include "py/mpconfig.h"
#include "py/runtime.h"

#include <k_api.h>
#include "aos/hal/uart.h"

/*
 * Core UART functions to implement for a port
 */

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    unsigned char c = 0;
    uint32_t recv_size = 0;
    int ret = -1;
    uart_dev_t uart_stdio;

    memset(&uart_stdio, 0, sizeof(uart_stdio));
    uart_stdio.port = 0;

    ret = hal_uart_recv_II(&uart_stdio, &c, 1, &recv_size, 0xffffffff);
    if ((ret != 0) || (recv_size != 1)) {
        return -1;
    }

    return c;
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    uart_dev_t uart_stdio;

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

mp_uint_t mp_hal_ticks_cpu(void) {
    return krhino_sys_tick_get();
}

void mp_hal_delay_us(mp_uint_t us) {
    sys_time_t t0 = krhino_sys_tick_get(), t1, dt;
    uint64_t dtick = us * RHINO_CONFIG_TICKS_PER_SECOND / 1000000L;
    while (1) {
        t1 = krhino_sys_tick_get();
        dt = t1 - t0;
        if (dt >= dtick) {
            break;
        }
        mp_handle_pending();
    }
}

void mp_hal_delay_ms(mp_uint_t ms) {
    sys_time_t t0 = krhino_sys_tick_get(), t1, dt;
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

