#include <stdint.h>
#include <string.h>
#include <aos/kernel.h>
#include <cli_console.h>

#ifdef AOS_KERNEL_MODE
#include <aos/hal/uart.h>
#endif

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
#ifdef AOS_KERNEL_MODE
    extern int hal_uart_print(const char *buf, int size);
    return hal_uart_print(buf, len);
#else
    /* use vfs in app */
    int ret = -1;
    int fd  = uart_console.fd;

    if (fd >= 0) {
        ret = write(fd, buf, len);
    }
    return ret;
#endif
}

int uart_console_read(void *buf, size_t len, void *privata_data)
{
    int ret = -1;
#ifdef AOS_KERNEL_MODE
    uart_dev_t uart_stdio;
    char *inbuf = (char *)buf;

    unsigned int recv_size = 0;
	unsigned char ch       = 0;

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
#else
    /* use vfs in app */
    int fd  = uart_console.fd;

    if (fd >= 0) {
        ret = read(fd, buf, len);
    }
    return ret;
#endif
}

int uart_console_init(void *private_data)
{
    // uart_console.fd = open("/dev/ttyUART0", 0);
    // if (uart_console.fd < 0) {
    //     printf("%s : %d ret = %d\r\n", __func__, __LINE__, uart_console.fd);
    // } else {
    //     printf("%s open device ok.\r\n", __func__);
    // }
    return 1;
}

int uart_console_deinit(void *private_data)
{
    return 1;
}

cli_console cli_uart_console = {
    .i_list = {0},
    .name = "cli-uart",
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
