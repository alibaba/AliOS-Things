#include <stdint.h>
#include <stdbool.h>
#include <aos/kernel.h>
#include <cli_console.h>
#include <aos/cli.h>
#include <sys/socket.h>
#include <lwip/apps/telnetserver.h>
#include <aos/hal/uart.h>

int telnet_console_write(const void *buf, size_t len, void *privata_data)
{
    char *ptr = (char *)buf;
    if (buf == NULL) {
        return 0;
    }
    ptr[len] = 0;
    TelnetWriteString((char *)buf);

    return len;
}

int telnet_console_read(void *buf, size_t len, void *privata_data)
{
    return 0;
}

static int telnet_console_init(void *private_data)
{
    return 0;
}

static int telnet_console_deinit(void *private_data)
{
    return 0;
}

static device_console telnet_console = {
    .name = "telnet-console",
    .write = telnet_console_write,
    .read = telnet_console_read,
    .init = telnet_console_init,
    .deinit = telnet_console_deinit
};

cli_console cli_telnet_console = {
    .i_list = {0},
    .name = "cli-telnet",
    .dev_console = &telnet_console,
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

int32_t telnet_console_create(int argc, char **argv)
{
    int32_t ret;
    ret = cli_console_init(&cli_telnet_console);
    if (ret != 0) {
        printf("Error: Failed to init telnet console: %d\r\n", ret);
        return -1;
    }
    telnetserver_start(&cli_telnet_console);
    printf("success to create telnet cli thread: %d\r\n", ret);
    return 0;
}

ALIOS_CLI_CMD_REGISTER(telnet_console_create, telnetcli, Console Telnet Create)
