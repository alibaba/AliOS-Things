#include <stdint.h>
#include <string.h>
#include <aos/kernel.h>
#include <cli_console.h>
#include <aos/hal/uart.h>

int32_t g_cli_direct_read = 0;

/* uart_input_read depends on mcu*/
__attribute__((weak)) int uart_input_read()
{
    return 1;
}

int uart_console_init(void *private_data);
int uart_console_deinit(void *private_data);
int uart_console_write(const void *buf, size_t len, void *privata_data);
int uart_console_read(void *buf, size_t len, void *privata_data);

static device_console uart_console = {
    .name   = "uart-console",
    .fd     = -1,
    .write  = uart_console_write,
    .read   = uart_console_read,
    .init   = uart_console_init,
    .deinit = uart_console_deinit
};

int uart_console_write(const void *buf, size_t len, void *privata_data)
{
    uart_dev_t uart_stdio;
    size_t i;
    const char *tmp = (char *)buf;

    if (buf == NULL) {
        return 0;
    }

    memset(&uart_stdio, 0, sizeof(uart_stdio));
    uart_stdio.port = 0;

    for (i = 0; i < len; i++) {
        if (*tmp == '\n') {
            hal_uart_send(&uart_stdio, (void *)"\r", 1, AOS_WAIT_FOREVER);
        }
        hal_uart_send(&uart_stdio, (void *)tmp, 1, AOS_WAIT_FOREVER);
        tmp++;
    }

    return len;
}

int uart_console_read(void *buf, size_t len, void *privata_data)
{
    int ret = -1;

    uart_dev_t uart_stdio;
    char *inbuf = (char *)buf;

    uint32_t recv_size = 0;
	unsigned char ch   = 0;

    if (buf == NULL) {
        return 0;
    }
    memset(&uart_stdio, 0, sizeof(uart_dev_t));
    uart_stdio.port = 0;

    if ( g_cli_direct_read == 0 ) {
        ret = hal_uart_recv_II(&uart_stdio, inbuf, 1, &recv_size, HAL_WAIT_FOREVER);
        if ((ret == 0) && (recv_size == 1)) {
            return recv_size;
        } else {
            return 0;
        }
    } else {
        do {ch = uart_input_read();}while(ch == 0);
        *inbuf = ch;
        return 1;
    }
}

int uart_console_init(void *private_data)
{
    return 0;
}

int uart_console_deinit(void *private_data)
{
    return 0;
}

cli_console cli_uart_console = {
    .i_list = {0},
    .name = "ash",
    .dev_console = &uart_console,
    .init_flag = 0,
    .exit_flag = 0,
    .alive = 1,
    .private_data = NULL,
    .cli_tag = {0},
    .cli_tag_len = 0,
    .task_list = {0},
    .finsh_callback = NULL,
    .start_callback = NULL,
};
