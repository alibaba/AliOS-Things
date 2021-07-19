#include <unistd.h>
#include "py/mpconfig.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/mpstate.h"
#include "py/mphal.h"
#include "py/ringbuf.h"

#include "aos_hal_uart.h"
#include "board_config.h"
#include "mphalport.h"

STATIC uint8_t stdin_ringbuf_array[260];
ringbuf_t stdin_ringbuf = {stdin_ringbuf_array, sizeof(stdin_ringbuf_array), 0, 0};

static uart_dev_t uart_stdio = {0};

extern int hal_uart_rx_sem_take(int uartid, int timeout);
extern int hal_uart_rx_sem_give(int port);

/*
 * Core UART functions to implement for a port
 */
void mp_hal_uart_init(void) {
    uart_stdio.port = MP_REPL_UART_PORT;
    uart_stdio.config.baud_rate = MP_REPL_UART_BAUDRATE;
    uart_stdio.config.data_width = DATA_WIDTH_8BIT;
    uart_stdio.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_stdio.config.mode = MODE_TX_RX;
    uart_stdio.config.parity = NO_PARITY;
    uart_stdio.config.stop_bits = STOP_BITS_1;
    aos_hal_uart_init(&uart_stdio);
}


// Receive single character
int mp_hal_stdin_rx_chr(void) {
    uint8_t c = 0;
    uint32_t recv_size = 0;
    int ret = -1;

    // memset(&uart_stdio, 0, sizeof(uart_stdio));
    // uart_stdio.port = MP_REPL_UART_PORT;

    // try to check whether we have receive uart input
    ret = aos_hal_uart_recv_II(&uart_stdio, &c, 1, &recv_size, 200);
    if (ret == 0 && recv_size == 1) {
        return c;
    }
    return ret;

    // MICROPY_EVENT_POLL_HOOK

    // // wait till we get any uart input or wakeup
    // ret = hal_uart_rx_sem_take(uart_stdio.port, HAL_WAIT_FOREVER);
    // return (-MP_EAGAIN);
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    aos_msleep(1);
    // memset(&uart_stdio, 0, sizeof(uart_stdio));
    // uart_stdio.port = MP_REPL_UART_PORT;
    if ((len > 0) && (str != NULL)) {
        aos_hal_uart_send(&uart_stdio, str, len, 5000);
    }

    // Only release the GIL if many characters are being sent
    // bool release_gil = len > 20;
    // if (release_gil) {
    //     MP_THREAD_GIL_EXIT();
    // }

    // // memset(&uart_stdio, 0, sizeof(uart_stdio));
    // // uart_stdio.port = MP_REPL_UART_PORT;
    // if ((len > 0) && (str != NULL)) {
    //     aos_hal_uart_send(&uart_stdio, str, len, 5000);
    // }

    // if (release_gil) {
    //     MP_THREAD_GIL_ENTER();
    // }
    // mp_uos_dupterm_tx_strn(str, len);
}

mp_uint_t mp_hal_ticks_us(void) {
    // haas600 and haas510 system not support us, get ticks is ms
    return  aos_now_ms();
}

mp_uint_t mp_hal_ticks_ms(void) {
    return aos_now_ms();
}

uint64_t mp_hal_time_ns(void) {
    // haas600 and haas510 system not support us, get ticks is ms
    return aos_now_ms();
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return ql_rtos_get_systicks();
}

void mp_hal_delay_us(mp_uint_t us) {
    // haas600 and haas510 system not support delay us only use delay ms
    mp_uint_t ms = (us < 1000) ? 1 : (us + 600) / 1000; 
    mp_hal_delay_ms(ms);
}

void mp_hal_delay_ms(mp_uint_t ms) {
    mp_uint_t num = 0;
    while(1) {
        MICROPY_EVENT_POLL_HOOK;
        ql_rtos_task_sleep_ms(1);
        num += 1;
        if(num >= ms) {
            break;
        } 
    }
    // ql_rtos_task_sleep_ms(ms);
}

// Wake up the main task if it is sleeping
void mp_hal_wake_main_task_from_isr(void) {
    // aos_hal_uart_rx_sem_give(uart_stdio.port);
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    if ((poll_flags & MP_STREAM_POLL_RD) && stdin_ringbuf.iget != stdin_ringbuf.iput) {
        ret |= MP_STREAM_POLL_RD;
    }
    return ret;
}

/*******************************************************************************/
// GPIO
/*******************************************************************************/

gpio_config_t mphal_gpio_config_obj_t[PY_GPIO_NUM_MAX] = {0};

int32_t mp_hal_pin_config_set(mp_hal_pin_obj_t pin_obj, gpio_config_t cfg) {
    gpio_dev_t dev = {0};
    dev.port = pin_obj;
    dev.config = cfg;

    int32_t ret = aos_hal_gpio_init(&dev);
    if (0 != ret) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "pin index out range"));
        return ret;
    }

    mphal_gpio_config_obj_t[pin_obj] = cfg;
    return ret;
}

gpio_config_t mp_hal_pin_config_get(mp_hal_pin_obj_t pin_obj) {
    if(pin_obj < 0 || pin_obj > PY_GPIO_NUM_MAX) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "pin index out range"));
        return 0;
    }
    return mphal_gpio_config_obj_t[pin_obj];
}

int32_t mp_hal_pin_high(mp_hal_pin_obj_t pin_obj) {
    gpio_dev_t dev = {0};
    dev.port = pin_obj;
    dev.config = mp_hal_pin_config_get(pin_obj);

    int32_t ret = aos_hal_gpio_output_high(&dev);
    if (0 != ret) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "mp_hal_pin_high fail"));
    }
    return ret;
}

int32_t mp_hal_pin_low(mp_hal_pin_obj_t pin_obj) {
    gpio_dev_t dev = {0};
    dev.port = pin_obj;
    dev.config = mp_hal_pin_config_get(pin_obj);

    int32_t ret = aos_hal_gpio_output_low(&dev);
    if (0 != ret) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "mp_hal_pin_low fail"));
    }
    return ret;
}

int32_t mp_hal_pin_read(mp_hal_pin_obj_t pin_obj) {
    gpio_dev_t dev = {0};
    dev.port = pin_obj;
    dev.config = mp_hal_pin_config_get(pin_obj);

    uint32_t value = -1;
    int32_t ret = aos_hal_gpio_input_get(&dev, &value);
    if (ret < 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "mp_hal_pin_read fail"));
        return ret;
    }
    return value;
}

/*******************************************************************************/
// MAC address
/*******************************************************************************/
#define MP_HAL_UNIQUE_ID_ADDRESS (0xDEADBEAF)

// Generate a random locally administered MAC address (LAA)
void mp_hal_generate_laa_mac(int idx, uint8_t buf[6]) {
    uint8_t *id = (uint8_t *)MP_HAL_UNIQUE_ID_ADDRESS;
    buf[0] = 0x02; // LAA range
    buf[1] = (id[11] << 4) | (id[10] & 0xf);
    buf[2] = (id[9] << 4) | (id[8] & 0xf);
    buf[3] = (id[7] << 4) | (id[6] & 0xf);
    buf[4] = id[2];
    buf[5] = (id[0] << 2) | idx;
}

// A board can override this if needed
MP_WEAK void mp_hal_get_mac(int idx, uint8_t buf[6]) {
    mp_hal_generate_laa_mac(idx, buf);
}

void mp_hal_get_mac_ascii(int idx, size_t chr_off, size_t chr_len, char *dest) {
    static const char hexchr[16] = "0123456789ABCDEF";
    uint8_t mac[6];
    mp_hal_get_mac(idx, mac);
    for (; chr_len; ++chr_off, --chr_len) {
        *dest++ = hexchr[mac[chr_off >> 1] >> (4 * (1 - (chr_off & 1))) & 0xf];
    }
}
